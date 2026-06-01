from flask import Blueprint, request, jsonify, flash, redirect, url_for, render_template, session, current_app
from app.models.device import Device
from app.extensions import db
from datetime import datetime
from app.mqtt.mqtt_publisher import publish_device_control

dy_handler_bp = Blueprint('dy_handler', __name__)

# 处理时间设置
def save_time_setting(device, field, value):
    if value <= 0:
        raise ValueError(f"{field} 的值无效，必须为正数")
    setattr(device, field, value)
    db.session.commit()
    publish_device_control(device.device_id, field, value)
    flash(f"{field} 设置成功：{value} 秒", "success")

@dy_handler_bp.route('/device_control/<device_id>', methods=['GET', 'POST'])
def device_control(device_id):
    current_app.logger.info(f"进入 device_control 函数，设备 ID: {device_id}")
    device = Device.query.filter_by(device_id=device_id).first()

    if not device:
        flash('设备不存在', 'danger')
        return redirect(url_for('dashboard'))

    if request.method == 'POST':  
        action = request.form.get('action')

        try:
            # 钓鱼时间设置
            if action == 'set_fishing_time':
                for fish in ['a', 'b', 'c', 'd', 'e', 'f']:
                    time_value = request.form.get(f'fishing_{fish}_time')
                    if time_value and time_value.isdigit():
                        save_time_setting(device, f'fishing_{fish}_time', int(time_value))
                    elif time_value:
                        flash(f"钓鱼 {fish} 时间输入无效，请输入有效的数字", "danger")

            # 打怪时间设置
            elif action == 'set_monster_time':
                for monster in ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j']:
                    time_value = request.form.get(f'monster_{monster}_time')
                    if time_value and time_value.isdigit():
                        save_time_setting(device, f'monster_{monster}_time', int(time_value))
                    elif time_value:
                        flash(f"打怪 {monster} 时间输入无效，请输入有效的数字", "danger")

            # 定时器 & 挂耳时间设置
            elif action == 'set_timer_ear':
                timer = request.form.get('timer')
                ear_time = request.form.get('ear_time')

                if timer and timer.isdigit():
                    save_time_setting(device, 'timer', int(timer) * 60)  # 转换为秒
                elif timer:
                    flash("定时器输入无效，请输入有效的数字", "danger")

                if ear_time and ear_time.isdigit():
                    save_time_setting(device, 'ear_time', int(ear_time) * 60)  # 转换为秒
                elif ear_time:
                    flash("挂耳时间输入无效，请输入有效的数字", "danger")

            # 切换模式
            elif action == 'switch_mode':
                device.mode = '钓鱼' if device.mode != '钓鱼' else '打怪'
                db.session.commit()
                publish_device_control(device.device_id, 'mode', device.mode)
                flash(f"模式已切换为 {device.mode}", "success")

            # 暂停/继续
            elif action == 'pause_resume':
                if not device.is_online:
                    flash(f'设备 {device.device_id} 离线，无法进行暂停/继续操作', 'danger')
                else:
                    device.is_running = not device.is_running
                    db.session.commit()
                    publish_device_control(device.device_id, 'resume' if device.is_running else 'pause')
                    flash(f"设备 {device.device_id} {'继续运行' if device.is_running else '已暂停'}", "success")

            # 升级设备
            elif action == 'upgrade_device':
                current_app.logger.info(f"设备 {device.device_id} 正在升级")
                publish_device_control(device.device_id, 'upgrade')
                flash(f"设备 {device.device_id} 正在升级", "success")

            else:
                flash('无效的操作', 'danger')

        except ValueError as ve:
            flash(f'操作失败: {ve}', 'danger')
            current_app.logger.error(f'设备控制错误: {ve}')
        except Exception as e:
            flash(f'操作失败: {e}', 'danger')
            current_app.logger.error(f'设备控制错误: {e}')

        # 处理完 POST 请求后，重定向到当前页面的 GET 请求
        return redirect(url_for('dy_handler.device_control', device_id=device_id))

    # 计算在线时长
    if device.is_online and device.connected_since:
        delta = datetime.now() - device.connected_since
        hours, remainder = divmod(delta.total_seconds(), 3600)
        minutes, _ = divmod(remainder, 60)
        online_duration = f"{int(hours)} 小时 {int(minutes)} 分钟"
    else:
        online_duration = "设备未连接"

    return render_template('device_control.html', device=device, online_duration=online_duration)