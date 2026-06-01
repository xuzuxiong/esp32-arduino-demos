from app.models.user import User
from app.models.activation import ActivationCode, DeviceActivationLog
from app.extensions import db

class ActivationService:
    @staticmethod
    def activate_device(activator_code, user_id, device_id):
        """激活设备并记录日志"""
        # 权限校验
        user = User.query.get(user_id)
        if not user:
            return False, "用户不存在"
        if not user.can_activate:
            return False, "您的激活权限已被管理员禁用"
        
        # 激活码校验
        code = ActivationCode.query.filter_by(code=activator_code).first()
        if not code:
            return False, "无效的激活码"
        if not code.is_active:  # 如果使用了is_active字段
            return False, "激活码已失效"
        
        # 记录激活日志
        log = DeviceActivationLog(
            activation_code_id=code.id,
            user_id=user_id,
            device_id=device_id
        )
        db.session.add(log)
        db.session.commit()
        
        return True, "设备激活成功"
    
    @staticmethod
    def generate_activation_code(user_id):
        """为用户生成新的激活码"""
        user = User.query.get(user_id)
        if not user:
            return None, "用户不存在"
        if not user.can_activate:
            return None, "您的激活权限已被管理员禁用"
        
        # 检查是否已生成过激活码
        existing_code = ActivationCode.query.filter_by(creator_id=user_id).first()
        if existing_code:
            return existing_code, "已存在激活码"
        
        # 生成新的激活码（通常使用UUID或手机号）
        new_code = ActivationCode(
            code=user.phone,  # 使用手机号作为激活码
            creator_id=user_id,
            is_active=True
        )
        db.session.add(new_code)
        db.session.commit()
        
        return new_code, "激活码生成成功"