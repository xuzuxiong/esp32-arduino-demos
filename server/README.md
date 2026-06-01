# MQTT Web Service / Flask 服务端

可选的 **MQTT + Web 控制后端**，与 `firmware/DECQ/` 固件配合使用。  
本目录为**参考实现**，部署前请替换 `.env`、品牌文案与商业模板内容。

## 功能

- Flask Web 控制台（多机型 device handler：`/cq`、`/dd`、`/dy` 等）
- MQTT 订阅 `devices/+/status`、发布 `devices/{id}/control`
- PostgreSQL 设备/用户/激活码模型（SQLAlchemy + Alembic）
- 可选 Celery + Redis 后台任务
- REST API（`app/api.py`）供前端轮询设备状态

## 不包含（已脱敏删除）

- `.env` 生产凭据
- 数据库 SQL 备份、用户 CSV、设备 ID 列表
- 内部部署文档中的真实路径与密码

## 快速开始（开发）

```bash
cd server/mqtt_web_service
python -m venv venv
source venv/bin/activate   # Windows: venv\Scripts\activate
pip install -r requirements.txt
python run.py
```

> 备注：无 `.env` 时自动加载 `.env.example` 占位符。生产部署时再创建 `.env` 填入真实数据库与 MQTT 凭据。

### PostgreSQL

```bash
createdb mqtt_service_db
export FLASK_APP=run.py
flask db upgrade
python run.py
```

浏览器访问 `http://127.0.0.1:8000`

### MQTT

确保 `.env` 中 Broker 与固件 `config.h` 一致。设备上线主题：

```
devices/{device_id}/status  →  online / JSON
devices/{device_id}/control ←  pause / resume / dd_* ...
```

协议详见 [docs/protocol.md](../../docs/protocol.md) 与 [docs/server.md](../../docs/server.md)。

## 生产部署（概要）

```bash
./start_project.sh
```

建议使用 Nginx 反向代理 + Gunicorn + systemd。详见 [docs/server.md](../../docs/server.md)。

## 目录结构

```
server/mqtt_web_service/
├── app/                 # Flask 应用
│   ├── mqtt/            # MQTT 收发、device handlers
│   ├── models/          # SQLAlchemy 模型
│   └── routes.py        # 登录/注册/管理
├── frontend/templates/  # Jinja2 控制页
├── migrations/          # Alembic
├── requirements.txt
├── .env.example
└── run.py
```

## 自定义说明

- `frontend/templates/*` 中可能含**示例商业文案**（分销/代理模态框），开源版已将联系电话替换为 `YOUR_SALES_PHONE`，部署时请自行修改或删除。
- `FREEZE_MODE` 等开关见 `.env.example`。

## License

MIT（与仓库根目录一致）
