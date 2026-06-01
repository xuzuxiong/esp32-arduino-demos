# home/lighthouse/mqtt_web_service/app/__init__.py
import sys
import os
import logging
from dotenv import load_dotenv
from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from flask_migrate import Migrate
from logging.handlers import RotatingFileHandler

from .celery import make_celery
from .extensions import db
from .mqtt.mqtt_receiver import setup_mqtt_receiver
from .mqtt.mqtt_publisher import setup_mqtt_publisher
from app.mqtt.device_handlers.dy_handler import dy_handler_bp
from app.mqtt.device_controller import device_controller_bp
from app.mqtt.device_handlers.dd_handler import dd_handler_bp
from app.mqtt.device_handlers.hm_handler import hm_handler_bp
from app.mqtt.device_handlers.de_handler import de_handler_bp
from app.mqtt.device_handlers.hy_handler import hy_handler_bp
from app.mqtt.device_handlers.zs_handler import zs_handler_bp
from app.mqtt.device_handlers.lf_handler import lf_handler_bp
from app.mqtt.device_handlers.sl_handler import sl_handler_bp
from app.mqtt.device_handlers.xy_handler import xy_handler_bp
from app.mqtt.device_handlers.cq_handler import cq_handler_bp
from app.api import api_bp
from flask_login import LoginManager
from app.models.user import User
from datetime import datetime

# 加载环境变量
load_dotenv()

# 初始化 LoginManager
login_manager = LoginManager()

class Config:
    SECRET_KEY = os.getenv('SECRET_KEY')
    SQLALCHEMY_DATABASE_URI = os.getenv('SQLALCHEMY_DATABASE_URI')
    SQLALCHEMY_TRACK_MODIFICATIONS = str(os.getenv('SQLALCHEMY_TRACK_MODIFICATIONS', 'False')).lower() == 'true'
    MQTT_BROKER_URL = os.getenv('MQTT_BROKER_URL')
    MQTT_BROKER_PORT = int(os.getenv('MQTT_BROKER_PORT', 1883))
    MQTT_USERNAME = os.getenv('MQTT_USERNAME')
    MQTT_PASSWORD = os.getenv('MQTT_PASSWORD')
    MQTT_KEEPALIVE = 60
    CELERY_RESULT_BACKEND = os.getenv('CELERY_RESULT_BACKEND')
    CELERY_BROKER_URL = os.getenv('CELERY_BROKER_URL')
    FREEZE_MODE = str(os.getenv('FREEZE_MODE', 'false')).lower() == 'true'
    FREEZE_CUTOFF_DATE = os.getenv('FREEZE_CUTOFF_DATE', '2099-01-01')
    FREEZE_BYPASS_KEY = os.getenv('FREEZE_BYPASS_KEY', '')
    OFFICIAL_INVITE = os.getenv('OFFICIAL_INVITE', '')

def create_app():
    app = Flask(__name__, template_folder=os.path.join(os.getcwd(), 'frontend', 'templates'))

    # 统一配置加载方式（使用 Config 类）
    app.config.from_object(Config)

    # ===== ✅ 日志配置开始 =====
    log_path = os.path.join(os.getcwd(), 'flask.log')
    log_handler = RotatingFileHandler(log_path, maxBytes=10 * 1024 * 1024, backupCount=5)
    log_handler.setLevel(logging.DEBUG)
    formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
    log_handler.setFormatter(formatter)

    # 添加到 app 的日志处理器
    app.logger.setLevel(logging.DEBUG)
    app.logger.addHandler(log_handler)

    # 同时配置全局 logging（logging.debug 也能输出）
    logging.getLogger().addHandler(log_handler)
    logging.getLogger().setLevel(logging.DEBUG)
    # ===== ✅ 日志配置结束 =====

    # 初始化 login_manager
    login_manager.init_app(app)

    @login_manager.user_loader
    def load_user(user_id):
        return User.query.get(int(user_id))

    # 注册自定义过滤器
    @app.template_filter('datetime')
    def format_datetime(value, format_string='%Y-%m-%d %H:%M'):
        if not value:
            return ""
        return value.strftime(format_string)

    # 注册蓝图
    from .routes import bp
    app.register_blueprint(bp)
    app.register_blueprint(dy_handler_bp, url_prefix='/dy')
    app.register_blueprint(dd_handler_bp, url_prefix='/dd')  # 确保其他设备有前缀
    app.register_blueprint(hm_handler_bp, url_prefix='/hm')  # ✅ 修复后的注册
    app.register_blueprint(de_handler_bp, url_prefix='/de')  # ✅ 修复后的注册
    app.register_blueprint(hy_handler_bp, url_prefix='/hy')  # ✅ 修复后的注册
    app.register_blueprint(zs_handler_bp, url_prefix='/zs')  # ✅ 修复后的注册
    app.register_blueprint(lf_handler_bp, url_prefix='/lf')  # ✅ 修复后的注册
    app.register_blueprint(sl_handler_bp, url_prefix='/sl')  # ✅ 修复后的注册
    app.register_blueprint(xy_handler_bp, url_prefix='/xy')  # ✅ 修复后的注册
    app.register_blueprint(cq_handler_bp, url_prefix='/cq')
    app.register_blueprint(device_controller_bp)
    app.register_blueprint(api_bp)

    @app.context_processor
    def inject_freeze_config():
        return {
            'FREEZE_MODE': app.config.get('FREEZE_MODE', False),
            'FREEZE_CUTOFF_DATE': app.config.get('FREEZE_CUTOFF_DATE', ''),
        }

    # 初始化数据库和迁移
    db.init_app(app)
    migrate = Migrate(app, db)

    # 初始化 Celery
    celery = make_celery(app)

    # 判断是否为数据库命令（如 flask db migrate/upgrade）
    is_db_command = any(arg.startswith('db') for arg in sys.argv)

    # 仅当不是数据库命令且 ENABLE_MQTT 为 True 时，初始化 MQTT
    if not is_db_command and os.getenv('ENABLE_MQTT', 'True').lower() == 'true':
        setup_mqtt_receiver(app)
        setup_mqtt_publisher(app)

    app.logger.info("✅ Flask 应用创建成功，日志系统已启用")

    return app
