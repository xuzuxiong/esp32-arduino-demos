# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.2] - 2026-06-01

### Added

- **server/mqtt_web_service/** — sanitized Flask MQTT web backend (optional)
- `docs/server.md`, `server/README.md`, `.env.example`
- Removed DB backups, internal deploy docs, and contact secrets from server tree

## [0.1.1] - 2026-06-01

### Fixed

- Remove hardcoded personal OTA URL; use `OTA_FIRMWARE_URL` from config
- MQTT `upgrade` action supports optional HTTP URL in value

### Added

- GitHub Actions compile workflow for DECQ + minimal examples
- `firmware/DECQ/libraries.txt` dependency list
- Hardware BOM, wiring schematic, block diagram under `hardware/`
- Minimal examples: `examples/ch9329_mouse/`, `examples/i2s_energy_detect/`
- `examples/README.md`

## [0.1.0] - 2026-06-01

### Added

- Initial public release of **DECQ** firmware example under `firmware/DECQ/`.
- I2S audio capture via PCM1808 and low-frequency energy / FFT-based event detection.
- CH9329 UART HID driver: keyboard, absolute/relative mouse, scroll wheel.
- SSD1306 OLED menu, physical keypad input, NVS parameter persistence.
- Optional WiFiManager + MQTT remote control and OTA upgrade hooks.
- Documentation under `docs/`, example configs under `examples/`.
- MIT License, contributing guide, security policy, and GitHub issue/PR templates.

### Notes

- This is an **early-stage** reference project for learning and hardware bring-up.
- Network features are **optional** and disabled by default until enabled in device settings.
- No production MQTT backend is bundled in this repository.

> **v0.1.2** 起包含可选 Flask MQTT 服务端（`server/mqtt_web_service/`，已脱敏）。

[0.1.2]: https://github.com/xuzuxiong/esp32-arduino-demos/releases/tag/v0.1.2
[0.1.1]: https://github.com/xuzuxiong/esp32-arduino-demos/releases/tag/v0.1.1
[0.1.0]: https://github.com/xuzuxiong/esp32-arduino-demos/releases/tag/v0.1.0
