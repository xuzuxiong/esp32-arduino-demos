# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

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

[0.1.0]: https://github.com/xuzuxiong/esp32-arduino-demos/releases/tag/v0.1.0
