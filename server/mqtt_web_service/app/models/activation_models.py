# app/models/activation_models.py
from datetime import datetime
from app.extensions import db
from sqlalchemy import ForeignKey, UniqueConstraint, Index
from sqlalchemy.orm import relationship


class ActivationCode(db.Model):
    """激活码主表，关联用户表（通过 user_id 外键）"""
    __tablename__ = 'activation_codes'
    
    id = db.Column(db.Integer, primary_key=True)
    user_id = db.Column(db.Integer, ForeignKey('user.id'), nullable=False)  # 关联 User 表的 id
    code = db.Column(db.String(32), unique=True, nullable=False, index=True)  # 激活码（32位唯一字符串）
    created_at = db.Column(db.DateTime, default=datetime.utcnow)  # 创建时间
    expires_at = db.Column(db.DateTime)  # 过期时间（可选）
    is_revoked = db.Column(db.Boolean, default=False)  # 是否失效
    
    # 关联关系：一个用户可生成多个激活码
    user = relationship('User', backref='generated_activation_codes')

class DeviceActivationRecord(db.Model):
    """设备激活记录表，关联激活码表和设备表"""
    __tablename__ = 'device_activation_records'
    
    id = db.Column(db.Integer, primary_key=True)
    activation_code_id = db.Column(db.Integer, ForeignKey('activation_codes.id'), nullable=False)  # 关联激活码 ID
    device_id = db.Column(db.String(50), ForeignKey('device.device_id'), nullable=False)  # 关联 Device 表的 device_id（字符串类型，与原表一致）
    activated_at = db.Column(db.DateTime, default=datetime.utcnow)  # 激活时间
    
    # 唯一约束：同一个激活码只能激活同一设备一次
    __table_args__ = (
        UniqueConstraint('activation_code_id', 'device_id', name='uq_activation_code_device'),
    )
    
    # 索引优化查询
    Index('idx_activated_at', 'activated_at')
    
    # 关联关系
    activation_code = relationship('ActivationCode', backref='activation_records')
   # device = relationship('Device', backref='activation_records')  # 设备表反向关联激活记录