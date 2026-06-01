from flask import Blueprint, request, jsonify, flash, redirect, url_for, render_template
from app.models.device import Device
from app.extensions import db, mqtt
from datetime import datetime

#kj_handler_bp = Blueprint('kj_handler', __name__)

#@kj_handler_bp.route('/device_control/<device_id>', methods=['GET', 'POST'])
#def device_control(device_id):
    # 检查设备是否存在
  #  device = Device.query.filter_by(device_id=device_id).first()
   # if not device:
    #    flash('设备不存在', 'danger')
    #    return redirect(url_for('dashboard'))

    # 处理设备控制逻辑...

   # return render_template('kj_device_control.html', device=device)