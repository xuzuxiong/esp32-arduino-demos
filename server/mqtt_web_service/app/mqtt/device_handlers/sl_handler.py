from flask import Blueprint, request, jsonify, flash, redirect, url_for, render_template, current_app
from app.models.device import Device
from app.extensions import db
from datetime import datetime
from app.mqtt.mqtt_publisher import publish_device_control

sl_handler_bp = Blueprint('sl_handler', __name__, url_prefix='/sl')

# 按键类型配置（名称: (字段名, 长度, 默认值, 友好名称)）
KEY_TYPE_CONFIG = {
    'fishing': ('dd_fishing_keys', 6, [26, 27, 28, 29, 30, 31], '钓鱼'),
    'monster': ('dd_monster_keys', 5, [26, 27, 28, 29, 30], '打怪'),
    'buf': ('dd_buf_keys', 5, [31, 32, 33, 34, 35], '增益'),
    'aura': ('dd_aura_keys', 3, [41, 42, 45], '光环'),
    'direction': ('dd_direction_keys', 4, [77, 76, 75, 74], '方向'),
    'return': ('dd_return_key', 1, [44], '回城'),
    'bait': ('dd_bait_key', 1, [33], '上饵')  # 新增上饵按键配置
}

# 时间设置配置（名称: (字段名, 长度, 默认值)）
TIME_TYPE_CONFIG = {
    'fishing': ('dd_fishing_times', 6, [1, 90, 210, 270, 240, 850]),
    'monster': ('dd_monster_times', 5, [1, 2, 3, 15, 10]),
    'buf': ('dd_buf_times', 5, [20, 31, 1800, 3600, 1]),
    'aura': ('dd_aura_times', 3, [99999, 99999, 99999]),
    'direction': ('dd_direction_times', 4, [99999, 99999, 99999, 99999]),
}

# 按键映射表
KEY_MAPPING = {
    'A': 0, 'B': 1, 'C': 2, 'D': 3, 'E': 4, 'F': 5, 'G': 6, 'H': 7, 'I': 8, 'J': 9,
    'K': 10, 'L': 11, 'M': 12, 'N': 13, 'O': 14, 'P': 15, 'Q': 16, 'R': 17, 'S': 18,
    'T': 19, 'U': 20, 'V': 21, 'W': 22, 'X': 23, 'Y': 24, 'Z': 25, '1': 26, '2': 27,
    '3': 28, '4': 29, '5': 30, '6': 31, '7': 32, '8': 33, '9': 34, '0': 35,
    '回车键': 36, 'Esc键': 37, '退格键': 38, 'Tab键': 39, '空格键': 40, '-（减号键）': 41,
    '=（等号键）': 42, '[（左方括号键）': 43, ']（右方括号键）': 44, '\\（反斜杠键）': 45,
    ';（分号键）': 46, '\'（单引号键）': 47, '~（波浪号键）': 48, ',（逗号键）': 49,
    '.（句号键）': 50, '/（斜杠键）': 51, 'Caps Lock键': 52, 'F1键': 53, 'F2键': 54,
    'F3键': 55, 'F4键': 56, 'F5键': 57, 'F6键': 58, 'F7键': 59, 'F8键': 60,
    'F9键': 61, 'F10键': 62, 'F11键': 63, 'F12键': 64, 'Print Screen键': 65,
    'Scroll Lock键': 66, 'Pause/Break键': 67, 'Insert键': 68, 'Home键': 69,
    'Page Up键': 70, 'Delete键': 71, 'End键': 72, 'Page Down键': 73, '右箭头键': 74,
    '左箭头键': 75, '下箭头键': 76, '上箭头键': 77, 'Num Lock键': 78, '数字键盘 /（除号键）': 79,
    '数字键盘 *（乘号键）': 80, '数字键盘 -（减号键）': 81, '数字键盘 +（加号键）': 82,
    '数字键盘回车键': 83, '数字键盘 7': 84, '数字键盘 8': 85, '数字键盘 9': 86,
    '数字键盘 4': 87, '数字键盘 5': 88, '数字键盘 6': 89, '数字键盘 1': 90,
    '数字键盘 2': 91, '数字键盘 3': 92, '数字键盘 0': 93, '数字键盘 .（小数点键）': 94,
    '左Ctrl键': 95, '右Ctrl键': 96, '左Shift键': 97, '右Shift键': 98, '左Alt键': 99,
    '右Alt键': 100, '左Windows键': 101, '右Windows键': 102
}
REVERSE_KEY_MAPPING = {v: k for k, v in KEY_MAPPING.items()}

def save_setting(device, field, value, is_list=False):
    """通用设置保存方法，支持列表和单个值"""
    try:
        if is_list:
            # 获取字段的预期长度
            expected_length = None
            
            # 先从TIME_TYPE_CONFIG中查找
            for cfg_field, (cfg_name, cfg_length, _) in TIME_TYPE_CONFIG.items():
                if cfg_name == field:
                    expected_length = cfg_length
                    break
            
            # 如果没找到，再从KEY_TYPE_CONFIG中查找
            if expected_length is None:
                for cfg_field, (cfg_name, cfg_length, _, _) in KEY_TYPE_CONFIG.items():
                    if cfg_name == field:
                        expected_length = cfg_length
                        break
            
            # 如果还是没找到，尝试从当前设备数据获取
            if expected_length is None:
                current_value = getattr(device, field, [])
                if isinstance(current_value, list):
                    expected_length = len(current_value)
            
            # 如果仍无法确定长度，记录错误并返回
            if expected_length is None:
                current_app.logger.error(f"无法确定列表字段 {field} 的长度")
                return False
            
            # 确保值是列表类型
            if not isinstance(value, list):
                value = [value]
            
            # 调整列表长度
            if len(value) < expected_length:
                # 填充默认值
                default_value = 0  # 默认为0
                value = value + [default_value] * (expected_length - len(value))
                current_app.logger.warning(f"列表长度不足，已用默认值填充: {field}")
            elif len(value) > expected_length:
                # 截断列表
                value = value[:expected_length]
                current_app.logger.warning(f"列表长度超过预期，已截断: {field}")
            
            current_app.logger.info(f"设置列表字段 {field} 的值为: {value}")
            setattr(device, field, value)
        else:
            # 处理单个值
            current_app.logger.info(f"设置单个字段 {field} 的值为: {value}")
            setattr(device, field, value)
        
        db.session.commit()
        publish_device_control(device.device_id, field, str(value))
        current_app.logger.info(f"设置 {field} 成功: {value}")
        return True
    except Exception as e:
        db.session.rollback()
        current_app.logger.error(f"设置 {field} 失败: {str(e)}", exc_info=True)
        return False

@sl_handler_bp.route('/device_control/<device_id>', methods=['GET', 'POST'])
def device_control(device_id):
    current_app.logger.info(f"进入 device_control 函数，设备 ID: {device_id}")
    device = Device.query.filter_by(device_id=device_id).first()

    if not device:
        flash('设备不存在', 'danger')
        return redirect(url_for('index'))

    # 初始化设备数据（确保所有字段存在且格式正确）
    initialize_device_data(device)

    if request.method == 'POST':
        action = request.form.get('action')
        current_app.logger.info(f"接收到POST请求，action: {action}，表单数据: {request.form}")

        try:
            if action.startswith('set_'):
                # 设置时间参数
                time_type = action.split('_')[1]
                if time_type in TIME_TYPE_CONFIG:
                    field, length, _ = TIME_TYPE_CONFIG[time_type]
                    values = []
                    
                    if time_type == 'direction':  # 特殊处理方向键时间
                        direction_names = ['up', 'down', 'left', 'right']
                        for i in range(length):
                            key = f"{direction_names[i]}_direction_time"
                            value = request.form.get(key)
                            values.append(int(value) if value and value.isdigit() else 0)
                            current_app.logger.info(f"方向键 {direction_names[i]} 的时间值: {values[-1]}")
                    else:  # 处理其他时间设置
                        for i in range(length):
                            key = f"{time_type}_{i+1}_time"
                            value = request.form.get(key)
                            values.append(int(value) if value and value.isdigit() else 0)
                    
                    if save_setting(device, field, values, is_list=True):
                        flash(f"{time_type}时间设置成功", "success")
                    else:
                        flash(f"{time_type}时间设置失败", "danger")
                
                elif time_type == 'bait':  # 处理上饵按键设置
                    bait_key = request.form.get('bait_key')
                    if bait_key and bait_key.isdigit():
                        if save_setting(device, 'dd_bait_key', int(bait_key)):
                            flash(f"上饵按键设置成功", "success")
                            current_app.logger.info(f"上饵按键设置为: {bait_key}")
                        else:
                            flash(f"上饵按键设置失败", "danger")
                    else:
                        current_app.logger.warning(f"上饵按键值无效: {bait_key}")
                        flash(f"上饵按键值无效", "danger")
            
            elif action == 'save_return_settings':
                # 保存回城设置
                return_key = request.form.get('return_key')
                press_time = request.form.get('return_press_time_ms')
                interval = request.form.get('return_interval_min')
                
                if return_key and return_key.isdigit():
                    save_setting(device, 'dd_return_key', int(return_key))
                
                if press_time and press_time.isdigit():
                    save_setting(device, 'dd_return_press_time_ms', int(press_time))
                
                if interval and interval.isdigit():
                    save_setting(device, 'dd_return_interval_min', int(interval))

            elif action in ['switch_mode', 'pause_resume', 'switch_fish_monster']:
                # 切换模式/状态
                if action == 'switch_mode':
                    device.mode = 'auto' if device.mode != 'auto' else 'manual'
                elif action == 'pause_resume':
                    device.is_running = not device.is_running
                elif action == 'switch_fish_monster':
                    device.dd_fish_monster_mode = not device.dd_fish_monster_mode
                
                db.session.commit()
                
                # 发布MQTT消息
                action_to_attribute = {
                    'switch_mode': 'mode',
                    'pause_resume': 'is_running',
                    'switch_fish_monster': 'dd_fish_monster_mode'
                }
                attribute = action_to_attribute.get(action)
                if attribute:
                    publish_device_control(device.device_id, attribute, str(getattr(device, attribute)))
                flash(f"操作成功: {action}", "success")

            elif action == 'upgrade_device':
                # 发布升级指令到MQTT
                try:
                    publish_device_control(device.device_id, 'upgrade', '1')
                    flash("设备升级指令已发送", "success")
                    current_app.logger.info(f"设备 {device_id} 升级指令已发布")
                except Exception as e:
                    flash(f"升级失败: {str(e)}", "danger")
                    current_app.logger.error(f"设备 {device_id} 升级失败: {str(e)}")

            else:
                # 处理未知动作
                current_app.logger.warning(f"未知动作: {action}")
                flash(f"未知动作: {action}", "warning")

        except Exception as e:
            db.session.rollback()
            flash(f"操作失败: {str(e)}", "danger")
            current_app.logger.error(f"设备 {device_id} 操作失败，错误: {str(e)}", exc_info=True)

        return redirect(url_for('sl_handler.device_control', device_id=device_id))

    # 计算在线时长
    online_duration = "设备未连接"
    if device.is_online and device.connected_since:
        delta = datetime.now() - device.connected_since
        hours, remainder = divmod(delta.total_seconds(), 3600)
        minutes, _ = divmod(remainder, 60)
        online_duration = f"{int(hours)} 小时 {int(minutes)} 分钟"

    current_app.logger.info(f"渲染模板，设备数据: {device_to_dict(device)}")
    return render_template(
        'sl_device_control.html',
        device=device,
        online_duration=online_duration,
        key_mapping=KEY_MAPPING,
        reverse_key_mapping=REVERSE_KEY_MAPPING
    )

def initialize_device_data(device):
    """确保设备数据完整并符合预期格式"""
    # 初始化列表字段
    for key_type, (field, length, default, _) in KEY_TYPE_CONFIG.items():
        current = getattr(device, field, None)
        
        if key_type == 'return' or key_type == 'bait':  # 回城键和上饵键是单个值
            if current is None:
                setattr(device, field, default[0])
                current_app.logger.info(f"初始化 {field} 为默认值: {default[0]}")
        else:  # 其他按键是列表
            if not isinstance(current, list) or len(current) != length:
                setattr(device, field, default.copy())
                current_app.logger.info(f"初始化 {field} 为默认值: {default}")

    # 初始化时间设置字段
    for time_type, (field, length, default) in TIME_TYPE_CONFIG.items():
        current = getattr(device, field, None)
        if not isinstance(current, list) or len(current) != length:
            setattr(device, field, default.copy())
            current_app.logger.info(f"初始化 {field} 为默认值: {default}")

    # 初始化其他状态字段
    if device.mode is None:
        device.mode = 'manual'
    if device.is_running is None:
        device.is_running = False
    if device.dd_fish_monster_mode is None:
        device.dd_fish_monster_mode = False

    db.session.commit()

def device_to_dict(device):
    """将设备对象转换为字典（不包含敏感信息）"""
    data = {}
    for key, value in device.__dict__.items():
        if not key.startswith('_'):
            data[key] = value
    return data

@sl_handler_bp.route('/set_key_setting', methods=['POST'])
def set_key_setting():
    data = request.get_json()
    current_app.logger.info(f"接收到按键设置请求: {data}")
    
    device_id = data.get('device_id')
    key_type = data.get('type')
    index = data.get('index')
    key_index = data.get('keyIndex')

    # 验证必要参数
    if not all([device_id, key_type, index, key_index]):
        current_app.logger.error(f"按键设置失败: 缺少参数, {data}")
        return jsonify({'status': 'error', 'message': '缺少必要参数'}), 400

    # 将字符串类型的索引转换为整数
    try:
        index = int(index)
        key_index = int(key_index)
    except ValueError:
        current_app.logger.error(f"按键设置失败: 无效索引或按键值, index={index}, keyIndex={key_index}")
        return jsonify({'status': 'error', 'message': '无效索引或按键值'}), 400

    device = Device.query.filter_by(device_id=device_id).first()
    if not device:
        return jsonify({'status': 'error', 'message': '设备不存在'}), 404

    # 验证按键类型
    if key_type not in KEY_TYPE_CONFIG:
        current_app.logger.error(f"按键设置失败: 无效按键类型, {key_type}")
        return jsonify({'status': 'error', 'message': '无效按键类型'}), 400

    field, max_length, default, friendly_name = KEY_TYPE_CONFIG[key_type]

    # 验证索引范围
    if key_type != 'return' and key_type != 'bait' and (index < 0 or index >= max_length):
        current_app.logger.error(f"按键设置失败: 索引超出范围, {index}")
        return jsonify({'status': 'error', 'message': f'索引超出范围 (0-{max_length-1})'}), 400

    # 验证按键值
    if key_index not in REVERSE_KEY_MAPPING:
        current_app.logger.error(f"按键设置失败: 无效按键值, {key_index}")
        return jsonify({'status': 'error', 'message': '无效按键值'}), 400

    try:
        if key_type == 'return' or key_type == 'bait':
            # 处理回城键和上饵键（单个值）
            setattr(device, field, key_index)
        else:
            # 处理列表类型按键
            key_list = getattr(device, field, [])
            if not isinstance(key_list, list) or len(key_list) != max_length:
                key_list = default.copy()  # 重置为默认值
            
            # 创建列表副本并修改
            new_list = key_list.copy()
            new_list[index] = key_index
            setattr(device, field, new_list)
        
        db.session.commit()
        publish_device_control(device_id, field, {
            'index': index,
            'key_index': key_index,
            'key_name': REVERSE_KEY_MAPPING[key_index]
        })
        
        current_app.logger.info(f"按键设置成功: {friendly_name}索引{index}设置为{REVERSE_KEY_MAPPING[key_index]}")
        return jsonify({
            'status': 'success',
            'message': f'{friendly_name}按键设置成功',
            'keyName': REVERSE_KEY_MAPPING[key_index]
        })


    except Exception as e:
        db.session.rollback()
        current_app.logger.error(f"按键设置失败: {str(e)}", exc_info=True)
        return jsonify({'status': 'error', 'message': '服务器内部错误'}), 500