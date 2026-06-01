# Release v0.1.0 — 首次公开开源整理

**Date:** 2026-06-01  
**Tag:** `v0.1.0`

## Summary

首次将 DECQ ESP32 固件示例整理为可公开的开源仓库，包含完整文档、MIT 许可证与脱敏配置模板。

## What's included

- **firmware/DECQ/** — Arduino 主固件
  - I2S (PCM1808) + FFT 低频能量音频事件检测
  - CH9329 UART HID 键盘/鼠标/滚轮
  - SSD1306 OLED + 6 键菜单 + NVS
  - 可选 WiFiManager / MQTT / HTTP OTA
- **docs/** — hardware, firmware, protocol, audio-detection, FAQ
- **examples/** — config template, serial commands, sample log
- **tools/build_upload.ps1** — Windows arduino-cli 编译烧录
- GitHub Issue / PR 模板
- `OPENAI_CODEX_OSS_APPLICATION_NOTES.md`

## Security / sanitization

- Removed production MQTT server IP and credentials from source
- Configuration moved to `config.example.h` (placeholders only)
- `mqtt_web_service` backend **not** included in this release

## Known limitations

- Arduino only (no PlatformIO / ESP-IDF yet)
- No automated tests
- Schematics / PCB not published
- Early-stage project — reference implementation for learning

## Quick start

```powershell
git clone https://github.com/xuzuxiong/esp32-arduino-demos.git
cd esp32-arduino-demos
cp firmware/DECQ/config.example.h firmware/DECQ/config.h
.\tools\build_upload.ps1 -Port COM6
```

See [README.md](README.md) and [CHANGELOG.md](CHANGELOG.md) for details.

## Full changelog

See [CHANGELOG.md](CHANGELOG.md).
