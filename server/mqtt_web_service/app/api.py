# app/api.py
from flask import Blueprint, jsonify
from app.models import Device, User

# 定义 api 蓝图，URL 前缀可设为 '/api'（可选）
api_bp = Blueprint('api', __name__, url_prefix='/api')

@api_bp.route('/stats', methods=['GET'])
def get_stats():
    # 统计逻辑（根据实际模型字段调整）
    total_devices = Device.query.count()
    online_devices = Device.query.filter(Device.is_online == True).count()
    offline_devices = Device.query.filter(Device.is_online == False).count()
    running_devices = Device.query.filter(Device.status == 'running').count()  # 假设 status 字段存储运行状态
    total_users = User.query.count()

    return jsonify({
        "total_devices": total_devices,
        "online_devices": online_devices,
        "offline_devices": offline_devices,
        "running_devices": running_devices,
        "total_users": total_users
    })