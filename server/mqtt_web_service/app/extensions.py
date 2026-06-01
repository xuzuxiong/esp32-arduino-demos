# /home/lighthouse/mqtt_web_service/app/extensions.py
from flask_login import LoginManager
from flask_sqlalchemy import SQLAlchemy
import paho.mqtt.client as mqtt
import os
from dotenv import load_dotenv

# 加载环境变量
load_dotenv()

# 初始化 db 实例
db = SQLAlchemy()
login_manager = LoginManager()  # 关键：定义 login_manager 对象
# 初始化 MQTT 客户端
mqtt_client = mqtt.Client()

# 从环境变量中获取 MQTT 代理的地址和端口
MQTT_BROKER_URL = os.getenv('MQTT_BROKER_URL', 'localhost')
MQTT_BROKER_PORT = int(os.getenv('MQTT_BROKER_PORT', 1883))

# 设置 MQTT 客户端连接成功的回调函数
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print("MQTT 客户端已成功连接到服务器")
    else:
        print(f"MQTT 客户端连接失败，返回码: {rc}")

# 绑定连接成功回调函数
mqtt_client.on_connect = on_connect

# 连接到 MQTT 代理
mqtt_client.connect(MQTT_BROKER_URL, MQTT_BROKER_PORT)

# 启动 MQTT 客户端的网络循环（如果需要异步处理消息）
mqtt_client.loop_start()