# app/models.py
from .extensions import db  # 从 extensions 导入 db 实例
from .extensions import db, login_manager  # 从 extensions 导入 login_manager
from flask_login import UserMixin
class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    phone = db.Column(db.String(11), unique=True, nullable=False)
    password = db.Column(db.String(128), nullable=False)
    security_question = db.Column(db.String(128), nullable=False)
    security_answer = db.Column(db.String(128), nullable=False)
    is_admin = db.Column(db.Boolean, default=False)

    def __repr__(self):
        return f'<User {self.phone}>'

class Device(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    device_id = db.Column(db.String(50), unique=True, nullable=False)
    is_online = db.Column(db.Boolean, nullable=False, default=False)
    last_seen = db.Column(db.DateTime, nullable=True)
    mode = db.Column(db.String(50), nullable=True)
    is_running = db.Column(db.Boolean, default=True)
    timer = db.Column(db.Integer, nullable=True)
    remaining_time = db.Column(db.Integer, nullable=True)
    owner_id = db.Column(db.Integer, db.ForeignKey('user.id'))
    connected_since = db.Column(db.DateTime, nullable=True)

    user = db.relationship('User', backref='devices')

    def __repr__(self):
        return f'<Device {self.device_id}>'
@login_manager.user_loader
def load_user(user_id):
    return User.query.options(
        db.load_only(User.id, User.phone, User.can_activate_device)
    ).get(int(user_id))