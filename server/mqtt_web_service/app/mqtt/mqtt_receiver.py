import logging
import paho.mqtt.client as mqtt
from app.models.device import Device
from app.extensions import db
from datetime import datetime, timedelta
from apscheduler.schedulers.background import BackgroundScheduler
from sqlalchemy.exc import IntegrityError
from flask import current_app

# 初始化日志记录器（默认 INFO，建议根据环境配置开关）
logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.INFO)

# 初始化 MQTT 客户端
mqtt_client = mqtt.Client()

# MQTT 连接成功回调
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        # logger.info("MQTT 客户端已成功连接到服务器")
        client.subscribe('devices/+/status')
    else:
        # logger.error(f"MQTT 客户端连接失败，返回码: {rc}")
        pass

# 消息回调封装，传入 app 实例
def on_message(app):
    def inner(client, userdata, message):
        topic = message.topic
        payload = message.payload.decode()
        # logger.info(f"收到 MQTT 消息: Topic: {topic}, Payload: {payload}")

        if topic.startswith("devices/") and topic.endswith("/status"):
            device_id = topic.split('/')[1]
            status = payload
            with app.app_context():
                update_device_status(device_id, status)
    return inner

# 更新设备状态
def update_device_status(device_id, status):
    device = Device.query.filter_by(device_id=device_id).first()
    if device:
        if status == "online":
            if not device.is_online:
                device.connected_since = datetime.now()
            device.is_online = True
        elif status == "offline":
            device.is_online = False
            device.connected_since = None
        elif status == "paused":
            device.is_running = False
        elif status == "running":
            device.is_running = True
        elif status.startswith("mode/"):
            device.mode = status.split('/')[1]

        device.last_seen = datetime.now()
        try:
            db.session.commit()
            # logger.info(f"更新设备 {device_id} 状态: {status}")
        except Exception as e:
            db.session.rollback()
            # logger.error(f"更新设备 {device_id} 状态时发生错误: {str(e)}")
    else:
        # 新设备接入
        if status == "online":
            new_device = Device(
                device_id=device_id,
                is_online=True,
                connected_since=datetime.now(),
                last_seen=datetime.now()
            )
            try:
                db.session.add(new_device)
                db.session.commit()
                # logger.info(f"新设备已插入数据库: {device_id}")
            except IntegrityError:
                db.session.rollback()
                # logger.warning(f"设备 {device_id} 已存在于数据库中，无需重复插入。")
            except Exception as e:
                db.session.rollback()
                # logger.error(f"插入设备 {device_id} 时发生错误: {str(e)}")

# 定期检查设备状态（掉线处理）
def check_device_status():
    try:
        with current_app.app_context():
            devices = Device.query.all()
            now = datetime.now()
            for device in devices:
                if device.last_seen and (now - device.last_seen) > timedelta(minutes=2):
                    device.is_online = False
                    device.connected_since = None
                    try:
                        db.session.commit()
                        # logger.info(f"设备 {device.device_id} 超过 2 分钟未活动，标记为离线")
                    except Exception as e:
                        db.session.rollback()
                        # logger.error(f"更新设备 {device.device_id} 状态为离线时出错: {str(e)}")
    except Exception as e:
        # logger.error(f"定时检查设备状态时发生异常: {str(e)}")
        pass

# 启动后台定时任务
scheduler = BackgroundScheduler()
scheduler.add_job(check_device_status, 'interval', minutes=1)
scheduler.start()

# 设置连接回调
mqtt_client.on_connect = on_connect

# 封装为 Flask 初始化函数
def setup_mqtt_receiver(app):
    try:
        if app.config.get('MQTT_USERNAME') and app.config.get('MQTT_PASSWORD'):
            mqtt_client.username_pw_set(app.config['MQTT_USERNAME'], app.config['MQTT_PASSWORD'])

        mqtt_client.on_message = on_message(app)

        if not mqtt_client.is_connected():
            mqtt_client.connect(
                app.config['MQTT_BROKER_URL'],
                app.config['MQTT_BROKER_PORT'],
                app.config['MQTT_KEEPALIVE']
            )
            mqtt_client.loop_start()
            # logger.info("MQTT 客户端已连接到服务器并开始循环")
    except Exception as e:
        # logger.error(f"MQTT 客户端连接失败: {str(e)}")
        pass

    return mqtt_client
