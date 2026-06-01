# /home/lighthouse/mqtt_web_service/app/routes.py
from app.models.activation_models import ActivationCode, DeviceActivationRecord
from flask import request, make_response, render_template
import csv
from io import StringIO
from flask import (
    render_template, request, redirect, url_for,
    flash, session, jsonify, current_app
)
from werkzeug.security import check_password_hash, generate_password_hash
from datetime import datetime
from app.models.user import User
from app.models.device import Device
from app.extensions import db
from flask import Blueprint
import logging
from openpyxl import Workbook
from openpyxl.styles import Font, Alignment
from flask_login import login_required, current_user

logging.basicConfig(level=logging.DEBUG)
print("routes.py 已加载！")

bp = Blueprint('main', __name__)

def register_routes(app):
    app.register_blueprint(bp)

# ================= 登录 =================
@bp.route('/login', methods=['GET', 'POST'])
def login():
    error = None
    if request.method == 'POST':
        phone = request.form.get('phone')
        password = request.form.get('password')

        user = User.query.filter_by(phone=phone).first()
        if not user or not check_password_hash(user.password, password):
            flash('手机号或密码错误', 'error')
            return redirect(url_for('main.login'))

        session['user_id'] = user.id
        session['is_admin'] = user.is_admin

        if user.is_admin:
            flash('管理员登录成功', 'success')
            return redirect(url_for('main.admin_dashboard'))
        else:
            flash('普通用户登录成功', 'success')
            return redirect(url_for('main.dashboard'))

    return render_template('index.html', error=error)

# ================= 注册（重点改造） =================
@bp.route('/register', methods=['GET', 'POST'])
def register():
    """
    FREEZE_MODE=true 时：
    - 默认禁止注册
    - 允许：
        1) ?invite=OFFICIAL_INVITE   （官方销售入口）
        2) ?k=FREEZE_BYPASS_KEY      （内部强权限）
    """

    invite = (request.args.get('invite') or request.form.get('invite') or '').strip()
    key = (request.args.get('k') or request.form.get('k') or '').strip()

    if current_app.config.get('FREEZE_MODE', False):
        bypass = (current_app.config.get('FREEZE_BYPASS_KEY') or '').strip()
        official_invite = (current_app.config.get('OFFICIAL_INVITE') or '').strip()

        allowed = False
        if bypass and key == bypass:
            allowed = True
        elif official_invite and invite == official_invite:
            allowed = True

        if not allowed:
            return render_template(
                'register.html',
                error=f"平台已冻结：停止新增注册（{current_app.config.get('FREEZE_CUTOFF_DATE', '2025-12-25')} 起）。请使用官方注册链接。",
                invite=invite,
                k=key
            ), 403

    if request.method == 'POST':
        phone = request.form['phone']
        password = request.form['password']
        confirm_password = request.form['confirm_password']
        security_question = request.form['security_question']
        security_answer = request.form['security_answer']

        if len(phone) != 11 or not phone.isdigit():
            return render_template('register.html', error="请输入有效的手机号码", invite=invite, k=key)
        if len(password) < 6:
            return render_template('register.html', error="密码长度必须大于6位", invite=invite, k=key)
        if password != confirm_password:
            return render_template('register.html', error="两次密码输入不一致", invite=invite, k=key)
        if User.query.filter_by(phone=phone).first():
            return render_template('register.html', error="手机号已注册", invite=invite, k=key)

        try:
            user = User(
                phone=phone,
                password=generate_password_hash(password),
                security_question=security_question,
                security_answer=security_answer
            )
            db.session.add(user)
            db.session.commit()
            flash('注册成功，请登录', 'success')
            return redirect(url_for('main.login'))
        except Exception as e:
            db.session.rollback()
            logging.exception("注册失败")
            return render_template('register.html', error="注册失败，请稍后重试", invite=invite, k=key)

    return render_template('register.html', invite=invite, k=key)

# ================= 重置密码 =================
@bp.route('/reset_password', methods=['GET', 'POST'])
def reset_password():
    if request.method == 'POST':
        phone = request.form['phone']
        user = User.query.filter_by(phone=phone).first()
        if user:
            return render_template('reset_password_question.html',
                                   phone=phone,
                                   question=user.security_question)
        return render_template('reset_password.html', error="手机号不存在")
    return render_template('reset_password.html')

@bp.route('/reset_password_answer', methods=['POST'])
def reset_password_answer():
    phone = request.form['phone']
    security_answer = request.form['security_answer']
    user = User.query.filter_by(phone=phone).first()
    if user and user.security_answer == security_answer:
        return render_template('reset_password_form.html', phone=phone)
    return render_template('reset_password_question.html',
                           phone=phone,
                           question=user.security_question,
                           error="密保答案错误")

# ================= 用户面板 =================
@bp.route('/dashboard')
def dashboard():
    user_id = session.get('user_id')
    if not user_id:
        return redirect(url_for('main.login'))

    user = User.query.get(user_id)
    if not user:
        session.pop('user_id', None)
        return redirect(url_for('main.login'))

    devices = Device.query.filter_by(owner_id=user_id).all()
    return render_template('control.html',
                           devices=devices,
                           can_activate=user.can_activate_device)

# ================= 其它 =================
@bp.route('/')
def index():
    return redirect(url_for('main.login'))

@bp.route('/logout')
def logout():
    session.pop('user_id', None)
    flash('你已成功登出', 'success')
    return redirect(url_for('main.login'))

@bp.route('/help')
def help():
    return render_template('help.html')

@bp.route('/admin_dashboard')
def admin_dashboard():
    if 'user_id' not in session:
        return redirect(url_for('main.login'))

    current_user = User.query.get(session['user_id'])
    if not current_user or not current_user.is_super_admin:
        flash('无权限访问', 'error')
        return redirect(url_for('main.dashboard'))

    users = User.query.all()
    devices = Device.query.all()
    stats = {
        'total_users': len(users),
        'total_devices': len(devices),
        'active_devices': Device.query.filter_by(status='active').count(),
        'inactive_devices': Device.query.filter_by(status='inactive').count()
    }

    return render_template('admin_dashboard.html',
                           users=users,
                           devices=devices,
                           stats=stats,
                           current_user=current_user)
