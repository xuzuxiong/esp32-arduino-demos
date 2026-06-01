import os
from dotenv import load_dotenv

load_dotenv()

class Config:
    SECRET_KEY = os.getenv('SECRET_KEY')
    SQLALCHEMY_DATABASE_URI = os.getenv('SQLALCHEMY_DATABASE_URI')
    SQLALCHEMY_TRACK_MODIFICATIONS = os.getenv('SQLALCHEMY_TRACK_MODIFICATIONS', 'False').lower() == 'true'
    MQTT_BROKER_URL = os.getenv('MQTT_BROKER_URL')
    MQTT_BROKER_PORT = int(os.getenv('MQTT_BROKER_PORT', 1883))
    MQTT_USERNAME = os.getenv('MQTT_USERNAME')
    MQTT_PASSWORD = os.getenv('MQTT_PASSWORD')
    broker_url = os.getenv('CELERY_BROKER_URL')
    MQTT_KEEPALIVE = 60
    FREEZE_MODE = os.getenv('FREEZE_MODE', 'false').lower() == 'true'
    FREEZE_CUTOFF_DATE = os.getenv('FREEZE_CUTOFF_DATE', '2099-01-01')
    FREEZE_BYPASS_KEY = os.getenv('FREEZE_BYPASS_KEY', '')
    OFFICIAL_INVITE = os.getenv('OFFICIAL_INVITE', '')
