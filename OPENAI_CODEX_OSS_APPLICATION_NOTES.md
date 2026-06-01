# OpenAI Codex for Open Source — 申请材料草稿

> 维护者填表用。仓库 **v0.1.2** 已脱敏上传：固件 + 可选 Flask MQTT 服务端 + 占位配置。  
> 表单：https://openai.com/form/codex-for-oss/

---

## 表单字段 — 直接复制（英文，各 ≤500 字符）

### Why does this repository qualify?

```
Primary maintainer of an MIT ESP32 stack: Arduino firmware (PCM1808 I2S FFT + CH9329 HID) plus optional sanitized Flask MQTT web backend. Includes BOM/wiring docs, minimal examples, placeholder configs, and server deployment guide. Early-stage but end-to-end reproducible for embedded developers.
```

### How will you use API credits?

```
Keep docs/code in sync; expand minimal examples; add tests; triage issues/FAQ; review PRs for leaked secrets. Codex helps maintain this teaching-oriented firmware+server reference.
```

### Anything else we should know?

```
New public repo, honest early-stage scope. All secrets replaced with placeholders (.env.example, config.example.h). Release v0.1.2 on GitHub. Not claiming large star count.
```

---

## 项目状态摘要

| 项 | 说明 |
|----|------|
| 仓库 | https://github.com/xuzuxiong/esp32-arduino-demos |
| 版本 | v0.1.2 |
| 许可证 | MIT |
| 固件 | `firmware/DECQ/`，无硬编码密码/OTA 域名 |
| 服务端 | `server/mqtt_web_service/`，无 `.env`/SQL 备份/真实电话 |
| 配置策略 | 占位符默认可用，备注说明上线前替换 |

---

## 声明

学习与研究用途；无 WARRANTY。
