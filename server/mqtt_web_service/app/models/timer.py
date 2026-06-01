# app/models/timer.py
from app.extensions import db

class DeviceTimer(db.Model):
    __tablename__ = 'device_timers'
    id = db.Column(db.Integer, primary_key=True)
    device_id = db.Column(db.String(64), unique=True, nullable=False)
    return_seconds = db.Column(db.Integer, default=0)