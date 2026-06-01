import logging
import paho.mqtt.client as mqtt
import queue
import threading
from flask import Flask

# 初始化日志记录器
logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.INFO)

# 初始化 MQTT 客户端
mqtt_client = mqtt.Client()

# 消息队列
message_queue = queue.Queue()

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        logger.info("MQTT 发布者已成功连接到服务器")
        # 发布者不需要订阅主题，注释掉订阅逻辑
        # client.subscribe('devices/+/status')
    else:
        logger.error(f"MQTT 发布者连接失败，返回码: {rc}")

# 设置连接回调
mqtt_client.on_connect = on_connect

def publish_worker():
    while True:
        try:
            device_id, action, value, qos, retain = message_queue.get()
            publish_device_control_internal(device_id, action, value, qos, retain)
            message_queue.task_done()
        except Exception as e:
            logger.error(f"消息队列处理出错: {str(e)}")

# 启动消息队列处理线程
threading.Thread(target=publish_worker, daemon=True).start()

def setup_mqtt_publisher(app: Flask):
    """初始化 MQTT 发布者并连接到服务器"""
    max_retries = 5
    retries = 0
    while retries < max_retries:
        try:
            # 设置 MQTT 用户名和密码（如果有）
            if app.config.get('MQTT_USERNAME') and app.config.get('MQTT_PASSWORD'):
                mqtt_client.username_pw_set(
                    app.config['MQTT_USERNAME'], 
                    app.config['MQTT_PASSWORD']
                )

            # 只在第一次连接时初始化 mqtt_client
            if not mqtt_client.is_connected():
                mqtt_client.connect(
                    app.config['MQTT_BROKER_URL'],
                    app.config['MQTT_BROKER_PORT'],
                    app.config['MQTT_KEEPALIVE']
                )
                mqtt_client.loop_start()  # 启动 MQTT 客户端的消息循环
                logger.info("MQTT 发布者已连接到服务器并开始循环")
                break
        except Exception as e:
            retries += 1
            logger.error(f"MQTT 发布者连接失败（第 {retries} 次重试）: {str(e)}")
            if retries < max_retries:
                time.sleep(2)  # 等待 2 秒后重试
    else:
        logger.error("达到最大重试次数，无法连接到 MQTT 服务器")

    return mqtt_client

def publish_device_control(device_id, action, value=None, qos=0, retain=False):
    """将消息加入队列等待发布"""
    message_queue.put((device_id, action, value, qos, retain))

def publish_device_control_internal(device_id, action, value=None, qos=0, retain=False):
    """发布设备控制命令到 MQTT 主题"""
    try:
        topic = f'devices/{device_id}/control'
        if value:
            message = f'{action}/{value}'
        else:
            message = action
        result = mqtt_client.publish(topic, message, qos=qos, retain=retain)
        if result.rc == mqtt.MQTT_ERR_SUCCESS:
            logger.info(f"已通过 MQTT 发布消息: Topic: {topic}, Message: {message}, QoS: {qos}, Retain: {retain}")
        else:
            logger.error(f"MQTT 消息发布失败，返回码: {result.rc}")
    except Exception as e:
        logger.error(f"发布 MQTT 消息时发生错误: {str(e)}")