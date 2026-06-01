#/home/lighthouse/mqtt_web_service/app/models/device.py
from app.extensions import db
from sqlalchemy.dialects.postgresql import JSONB
from datetime import datetime

# 按键映射表（名称到索引的映射，与前端完全一致）
key_mapping = {
    'A': 0, 'B': 1, 'C': 2, 'D': 3, 'E': 4, 'F': 5, 'G': 6, 'H': 7, 'I': 8, 'J': 9,
    'K': 10, 'L': 11, 'M': 12, 'N': 13, 'O': 14, 'P': 15, 'Q': 16, 'R': 17, 'S': 18,
    'T': 19, 'U': 20, 'V': 21, 'W': 22, 'X': 23, 'Y': 24, 'Z': 25, '1': 26, '2': 27,
    '3': 28, '4': 29, '5': 30, '6': 31, '7': 32, '8': 33, '9': 34, '0': 35,
    '回车键': 36, 'Esc键': 37, '退格键': 38, 'Tab键': 39, '空格键': 40, 
    '-（减号键）': 41, '=（等号键）': 42, '[（左方括号键）': 43, 
    ']（右方括号键）': 44, '\\（反斜杠键）': 45, ';（分号键）': 46,  # 注意反斜杠键名用双反斜杠转义
    '\'（单引号键）': 47, '~（波浪号键）': 48, ',（逗号键）': 49,
    '.（句号键）': 50, '/（斜杠键）': 51, 'Caps Lock键': 52, 'F1键': 53, 
    'F2键': 54, 'F3键': 55, 'F4键': 56, 'F5键': 57, 'F6键': 58, 
    'F7键': 59, 'F8键': 60, 'F9键': 61, 'F10键': 62, 'F11键': 63, 
    'F12键': 64, 'Print Screen键': 65, 'Scroll Lock键': 66, 
    'Pause/Break键': 67, 'Insert键': 68, 'Home键': 69, 
    'Page Up键': 70, 'Delete键': 71, 'End键': 72, 'Page Down键': 73, 
    '右箭头键': 74, '左箭头键': 75, '下箭头键': 76, '上箭头键': 77, 
    'Num Lock键': 78, '数字键盘 /（除号键）': 79, '数字键盘 *（乘号键）': 80, 
    '数字键盘 -（减号键）': 81, '数字键盘 +（加号键）': 82, '数字键盘回车键': 83, 
    '数字键盘 7': 84, '数字键盘 8': 85, '数字键盘 9': 86, '数字键盘 4': 87, 
    '数字键盘 5': 88, '数字键盘 6': 89, '数字键盘 1': 90, '数字键盘 2': 91, 
    '数字键盘 3': 92, '数字键盘 0': 93, '数字键盘 .（小数点键）': 94,
    '左Ctrl键': 95, '右Ctrl键': 96, '左Shift键': 97, '右Shift键': 98, 
    '左Alt键': 99, '右Alt键': 100, '左Windows键': 101, '右Windows键': 102
}

# 反向按键映射表（索引到名称的映射）
reverse_key_mapping = {v: k for k, v in key_mapping.items()}

class Device(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    device_id = db.Column(db.String(50), unique=True, nullable=False)  # 旧字段（不变）
    is_online = db.Column(db.Boolean, nullable=False, default=False)     # 旧字段（不变）
    last_seen = db.Column(db.DateTime, nullable=True)                   # 旧字段（不变）
    mode = db.Column(db.String(50), nullable=True)                      # 旧字段（不变）
    is_running = db.Column(db.Boolean, default=True)                    # 旧字段（不变）
    timer = db.Column(db.Integer, nullable=True, default=0)             # 旧字段（不变）
    ear_time = db.Column(db.Integer, nullable=True, default=0)          # 旧字段（不变）
    fishing_a_time = db.Column(db.Integer, default=0)                   # 旧字段（不变）
    fishing_b_time = db.Column(db.Integer, default=0)                   # 旧字段（不变）
    fishing_c_time = db.Column(db.Integer, default=0)                   # 旧字段（不变）
    fishing_d_time = db.Column(db.Integer, default=0)                   # 旧字段（不变）
    fishing_e_time = db.Column(db.Integer, default=0)                   # 旧字段（不变）
    fishing_f_time = db.Column(db.Integer, default=0)                   # 旧字段（不变）
    monster_a_time = db.Column(db.Integer, default=0)                   # 旧字段（不变）
    monster_b_time = db.Column(db.Integer, default=0)                   # 旧字段（不变）
    monster_c_time = db.Column(db.Integer, default=0)                   # 旧字段（不变）
    monster_d_time = db.Column(db.Integer, default=0)                   # 旧字段（不变）
    monster_e_time = db.Column(db.Integer, default=0)                   # 旧字段（不变）
    monster_f_time = db.Column(db.Integer, default=0)                   # 旧字段（不变）
    monster_g_time = db.Column(db.Integer, default=0)                   # 旧字段（不变）
    monster_h_time = db.Column(db.Integer, default=0)                   # 旧字段（不变）
    monster_i_time = db.Column(db.Integer, default=0)                   # 旧字段（不变）
    monster_j_time = db.Column(db.Integer, default=0)                   # 旧字段（不变）
    remaining_time = db.Column(db.Integer, nullable=True)               # 旧字段（不变）
    owner_id = db.Column(db.Integer, db.ForeignKey('user.id'))          # 旧字段（不变）
    connected_since = db.Column(db.DateTime, nullable=True)             # 旧字段（不变）
    remark = db.Column(db.String(255))                                 # 旧字段（不变）
    status = db.Column(db.String(50), default='idle')                   # 新增状态字段用于统计数据

    return_seconds = db.Column(db.Integer, default=0)  # 新增倒计时字段
    activated = db.Column(db.Boolean, default=False)  # 是否已激活
    activation_time = db.Column(db.DateTime)          # 激活时间
    activation_account = db.Column(db.String(20))     # 激活账号
    
    
    # 添加关系和级联删除配置
    activation_records = db.relationship(
        'DeviceActivationRecord', 
        backref='device', 
        lazy=True,
        cascade="all, delete-orphan"  # 关键配置：级联删除
    )
    # 关联用户（旧字段，不变）
    user = db.relationship('User', backref='devices')

    # ================== 新增DD设备专属字段（不影响旧数据）==================
    # 按键设置（JSONB存储索引列表，默认值对应前端下拉选单初始值）
    dd_fishing_keys = db.Column(
        JSONB, 
        default=[key_mapping['1'], key_mapping['2'], key_mapping['3'], key_mapping['4'], key_mapping['5'], key_mapping['6']]
    )
    dd_monster_keys = db.Column(
        JSONB, 
        default=[key_mapping['1'], key_mapping['2'], key_mapping['3'], key_mapping['4'], key_mapping['5']]
    )
    dd_buf_keys = db.Column(
        JSONB, 
        default=[key_mapping['6'], key_mapping['7'], key_mapping['8'], key_mapping['9'], key_mapping['0']]
    )
        # 新增：上饵按键设置（单个整数索引，默认值为33）
    dd_bait_key = db.Column(
        db.Integer, 
        default=41  # 默认值对应按键映射表中的 '-'
    )
    dd_aura_keys = db.Column(
        JSONB, 
        default=[
            key_mapping['-（减号键）'],
            key_mapping['=（等号键）'],
            key_mapping['\\（反斜杠键）']  # 注意双反斜杠转义
        ]
    )
    dd_direction_keys = db.Column(
        JSONB, 
        default=[
            key_mapping['上箭头键'],
            key_mapping['下箭头键'],
            key_mapping['左箭头键'],
            key_mapping['右箭头键']
        ]
    )
    dd_return_key = db.Column(
        db.Integer, 
        default=key_mapping[']（右方括号键）']  # 存储索引
    )

    # 时间设置（JSONB存储时间列表，默认值对应前端初始值）
    dd_fishing_times = db.Column(
        JSONB, 
        default=[99999, 99999, 99999, 99999, 99999, 99999]
    )
    dd_monster_times = db.Column(
        JSONB, 
        default=[1, 2, 3, 15, 10]
    )
    dd_buf_times = db.Column(
        JSONB, 
        default=[20, 31, 1800, 3600, 1]
    )
    dd_aura_times = db.Column(
        JSONB, 
        default=[99999, 99999, 99999]
    )
    dd_direction_times = db.Column(
        JSONB, 
        default=[99999, 99999, 99999, 99999]
    )

    # 回城设置（新增字段，默认值对应前端）
    dd_return_press_time_ms = db.Column(
        db.Integer, 
        default=50
    )
    dd_return_interval_min = db.Column(
        db.Integer, 
        default=240
    )

    # 其他新增设置
    dd_all_keys_press_time = db.Column(
        db.Integer,
        default=100
    )
    dd_key_selection = db.Column(
        JSONB,
        default=[key_mapping['1'], key_mapping['2'], key_mapping['3'], key_mapping['4'], key_mapping['5'], key_mapping['6']]
    )
    dd_fish_monster_mode = db.Column(
        db.Boolean,
        default=True
    )

    # 记录最后更新时间（自动更新）
    last_updated = db.Column(
        db.DateTime, 
        default=datetime.utcnow, 
        onupdate=datetime.utcnow
    )

    # ================== 辅助方法（兼容新旧字段）==================
    def get_dd_key_name(self, key_index):
        """将DD设备的按键索引转换为名称"""
        return reverse_key_mapping.get(key_index, f"未知键({key_index})")
    
    def get_dd_fishing_key_names(self):
        """获取DD设备的钓鱼按键名称列表"""
        return [self.get_dd_key_name(idx) for idx in self.dd_fishing_keys]
    
    # ... 其他DD设备专属的辅助方法（如get_dd_monster_key_names等）...

    def __repr__(self):
        return f'<Device {self.device_id}>'