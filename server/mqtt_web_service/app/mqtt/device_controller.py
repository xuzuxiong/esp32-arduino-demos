from flask import Blueprint, current_app, request, jsonify, flash, redirect, url_for, session, render_template
from app.models.device import Device
from app.extensions import db
from app.mqtt.device_handlers.dy_handler import dy_handler_bp
from app.models.user import User
from datetime import datetime
from app.models.activation_models import ActivationCode, DeviceActivationRecord
import logging

device_controller_bp = Blueprint('device_controller', __name__)

# ================== 管理员面板路由（修正重复定义）==================
@device_controller_bp.route('/admin/dashboard', methods=['GET'])
def admin_dashboard():
    if 'user_id' not in session:
        flash('请先登录', 'error')
        return redirect(url_for('main.login'))
    
    current_user = User.query.get(session['user_id'])
    if not current_user.is_admin:
        flash('无权限访问管理员面板', 'error')
        return redirect(url_for('main.dashboard'))
    
    users = User.query.all()
    devices = Device.query.all()
    return render_template('admin_dashboard.html', users=users, devices=devices)

# ================== 删除设备路由 ==================
@device_controller_bp.route('/delete_device/<device_id>', methods=['DELETE'])
def delete_device(device_id):
    try:
        device = Device.query.filter_by(device_id=device_id).first_or_404()
        db.session.delete(device)
        db.session.commit()
        current_app.logger.info(f"设备 {device_id} 已删除")
        return jsonify({'status': 'success'})
    except Exception as e:
        db.session.rollback()
        current_app.logger.error(f"删除设备失败: {str(e)}")
        return jsonify({'status': 'error', 'message': str(e)}), 500

# ================== 保存备注路由 ==================
@device_controller_bp.route('/save_remark/<device_id>', methods=['POST'])
def save_remark(device_id):
    data = request.get_json()
    remark = data.get('remark')

    device = Device.query.filter_by(device_id=device_id).first_or_404()
    device.remark = remark
    
    try:
        db.session.commit()
        return jsonify({'message': '备注保存成功'})
    except Exception as e:
        current_app.logger.error(f"保存备注失败: {str(e)}")
        return handle_db_error(e, '保存备注时发生错误: {}')

@device_controller_bp.route('/add_device', methods=['POST'])
def add_device():
    # 冻结模式：默认禁止新增设备绑定（仍允许删除/解绑）。
    # 仅允许管理员（后台人员）在冻结期间操作绑定（如确有必要）。
    if current_app.config.get('FREEZE_MODE', False) and not session.get('is_admin'):
        flash('平台已冻结：停止新增设备绑定（仅允许删除/解绑）。后续新用户请使用新平台。', 'error')
        return redirect(url_for('main.dashboard'))

    user_id = session.get('user_id')
    if not user_id:
        flash('请先登录', 'error')
        return redirect(url_for('main.login'))

    device_id = request.form.get('device_id')
    if not device_id:
        flash('请输入设备 ID', 'error')
        return redirect(url_for('main.dashboard'))

    device = Device.query.filter_by(device_id=device_id).first()
    if not device:
        flash('设备未找到，请确保设备已上线后再尝试添加', 'error')
        return redirect(url_for('main.dashboard'))

    if not device.is_online:
        flash('设备不在线，请等待设备上线后再尝试添加', 'error')
        return redirect(url_for('main.dashboard'))

    if device.owner_id and device.owner_id == user_id:
        flash('设备已被添加到你的设备列表中', 'error')
        return redirect(url_for('main.dashboard'))

    if device.owner_id and device.owner_id != user_id:
        flash('设备已绑定给其他用户', 'error')
        return redirect(url_for('main.dashboard'))

    device.owner_id = user_id
    try:
        db.session.commit()
        flash('设备绑定成功！', 'success')
    except Exception as e:
        db.session.rollback()
        current_app.logger.error(f"绑定设备失败: {str(e)}")
        flash(f'绑定设备时发生错误: {str(e)}', 'error')

    return redirect(url_for('main.dashboard'))

# ================== 获取设备列表 ==================
@device_controller_bp.route('/get_devices', methods=['GET'])
def get_devices():
    user_id = session.get('user_id')
    if not user_id:
        return jsonify({'error': '用户未登录，请先登录'}), 403

    try:
        devices = Device.query.filter_by(owner_id=user_id).all()
        device_list = []
        for device in devices:
            device_info = {
                'device_id': device.device_id,
                'is_online': device.is_online,
                'remark': device.remark,
                'activated': device.activated,
                'activation_time': device.activation_time.strftime('%Y-%m-%d %H:%M:%S') if device.activation_time else None
            }
            device_list.append(device_info)
        return jsonify({'devices': device_list})
    except Exception as e:
        current_app.logger.error(f"获取设备列表失败: {str(e)}")
        return jsonify({'error': f'获取设备列表时发生错误: {str(e)}'}), 500

def handle_db_error(e, message):
    db.session.rollback()
    current_app.logger.error(message.format(str(e)))
    return jsonify({'error': message.format(str(e))}), 500

# ================== 设备激活路由 ==================
@device_controller_bp.route('/activate_device', methods=['POST'])
def activate_device():
    data = request.get_json()
    device_id = data.get('device_id')
    activation_code = data.get('activation_code')
    current_app.logger.debug(f"接收到激活请求: device_id={device_id}, activation_code={activation_code}")

    device = Device.query.filter_by(device_id=device_id).first_or_404()
    
    if device.activated:
        return jsonify({
            'message': '设备已激活',
            'device': {
                'device_id': device.device_id,
                'activated': True,
                'activation_time': device.activation_time.strftime('%Y-%m-%d %H:%M:%S')
            }
        }), 200

    code = ActivationCode.query.filter(
        ActivationCode.code == activation_code,
        ActivationCode.is_revoked == False,
    ).first()

    if not code:
        current_app.logger.warning(f"无效激活码: {activation_code}")
        return jsonify({'error': '无效的激活码'}), 403

    try:
        device.activated = True
        device.activation_code_id = code.id
        device.activation_time = datetime.utcnow()
        activation_record = DeviceActivationRecord(
            device_id=device_id,
            activation_code_id=code.id,
            activated_at=datetime.utcnow()
        )
        db.session.add_all([device, activation_record])
        db.session.commit()
        current_app.logger.info(f"设备 {device_id} 激活成功")
        return jsonify({
            'message': '设备激活成功',
            'device': {
                'device_id': device_id,
                'activated': True,
                'activation_time': device.activation_time.strftime('%Y-%m-%d %H:%M:%S')
            }
        }), 200
    except Exception as e:
        db.session.rollback()
        current_app.logger.error(f"设备激活失败: {str(e)}")
        return jsonify({'error': '激活设备时发生数据库错误'}), 500

# ================== 获取激活历史路由 ==================
@device_controller_bp.route('/get_activation_history/<device_id>', methods=['GET'])
def get_activation_history(device_id):
    if 'user_id' not in session:
        return jsonify({'error': '用户未登录'}), 403
    
    device = Device.query.filter_by(
        device_id=device_id,
        owner_id=session['user_id']
    ).first_or_404()
    
    records = DeviceActivationRecord.query.filter_by(device_id=device_id)\
        .join(ActivationCode)\
        .join(User, ActivationCode.user_id == User.id)\
        .order_by(DeviceActivationRecord.activated_at.desc())\
        .all()
    
    history = []
    for record in records:
        history.append({
            'activated_at': record.activated_at.strftime('%Y-%m-%d %H:%M:%S'),
            'activation_code': record.activation_code.code,
            'code_owner': record.activation_code.user.phone,
            'code_owner_id': record.activation_code.user_id
        })
    
    return jsonify({'history': history})
