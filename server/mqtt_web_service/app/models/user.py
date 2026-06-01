# app/models/user.py
from app.extensions import db  # 从 extensions 导入 db
class User(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    phone = db.Column(db.String(11), unique=True, nullable=False)
    password = db.Column(db.String(128), nullable=False)
    security_question = db.Column(db.String(128), nullable=False)
    security_answer = db.Column(db.String(128), nullable=False)
    is_admin = db.Column(db.Boolean, default=False)
    is_super_admin = db.Column(db.Boolean, default=False)  # 超级管理员权限
    can_activate_device = db.Column(db.Boolean, default=False)  # 用户是否有激活权限字段
    activation_permission_start = db.Column(db.DateTime)  #激活权限开启时间

    def __repr__(self):
        return f'<User {self.phone}>'
    @property
    def role(self):
        """返回用户角色描述"""
        if self.is_super_admin:
            return "超级管理员"
        elif self.can_activate_device:  # ✅ 使用现有字段
            return "激活码管理员"
        else:
            return "普通用户"
    @property
    def activation_permission(self) -> bool:  # ✅ 修改属性名
        """模板访问权限的接口"""
        return bool(self.can_activate_device)  # ✅ 正确关联原字段