# OpenAI Codex for OSS Application Notes

> **Maintainer:** xuzuxiong  
> **Repository:** https://github.com/xuzuxiong/esp32-arduino-demos  
> **Release:** v0.1.2  
> **License:** MIT  
> **Form:** https://openai.com/form/codex-for-oss/

This document describes an **early-stage, sanitized, reproducible reference stack**. It does not claim large user counts, stars, downloads, or production deployment scale.

---

## 1. Project Name

**esp32-arduino-demos** — **DECQ Reference Stack**

Public GitHub repository providing MIT-licensed **ESP32 Arduino firmware** plus an **optional sanitized Flask/MQTT web backend**, focused on PCM1808 I2S audio acquisition, FFT-based low-band energy event detection, and CH9329 UART HID keyboard/mouse control.

---

## 2. One-Line Summary

**An early-stage MIT reference for ESP32-WROOM (16 MB Flash): PCM1808 I2S audio at 48 kHz, tunable FFT low-band energy detection, CH9329 UART HID, NVS-backed local settings, optional MQTT remote control/HTTP OTA hooks, and an optional sanitized Flask backend—useful for audio-triggered automation, HID bridge prototyping, and embedded IoT learning where full wiring + protocol notes are rarely published together.**

---

## 3. Project Overview

This is an **MIT-licensed ESP32 Arduino reference stack** (not a commercial product). It consolidates firmware, hardware notes, protocol documentation, minimal examples, and placeholder-based configuration so developers can study or adapt the integration without real credentials in the repository.

| Component | Status |
|-----------|--------|
| PCM1808 I2S audio acquisition | **Implemented** in `firmware/DECQ/` |
| FFT + low-band energy event detection | **Implemented** (Hamming window, adaptive baseline) |
| CH9329 UART HID keyboard/mouse | **Implemented** (UART frame protocol) |
| SSD1306 OLED + 6-button keypad | **Implemented** |
| NVS persistent runtime settings | **Implemented** (`Preferences`, namespace `testNamespace`) |
| WiFiManager + MQTT status/control | **Implemented**, **optional** (enabled only when network mode is on in device settings) |
| HTTP OTA + ArduinoOTA hooks | **Implemented** (placeholder URL; MQTT `upgrade` trigger) |
| Flask/MQTT web backend | **Optional sanitized reference** in `server/mqtt_web_service/` |
| Placeholder configs | **Default** via `config.example.h` and `.env.example` auto-fallback |
| CI on GitHub Actions | **Reference template only** (`tools/ci/compile.yml`), **not enabled by default** |

**Sanitization (v0.1.2):** no committed `.env`, SQL backups, user CSV, real phone numbers, or private deployment credentials. Placeholders include `192.168.1.100`, `YOUR_MQTT_USERNAME`, `YOUR_MQTT_PASSWORD`, `YOUR_SALES_PHONE` in templates.

---

## 4. Implemented Features

### 4.1 ESP32 Firmware (`firmware/DECQ/`)

| Item | Detail | Status |
|------|--------|--------|
| Framework | Arduino IDE / ESP32 Arduino Core 2.x only | **Implemented** |
| Target board | ESP32 Dev Module, **16 MB Flash**, FQBN partition **App 3MB / FAT 9MB** | **Required** |
| Build helper | `tools/build_upload.ps1` (arduino-cli) | **Implemented** |
| Config | `config.example.h` placeholders; optional `config.h` override (gitignored) | **Placeholder default** |

### 4.2 I2S / PCM1808 Audio

| Parameter | Value (from code) |
|-----------|-------------------|
| Sample rate | **48 kHz** |
| FFT size | **1024** samples |
| I2S pins | BCK **GPIO26**, WS **GPIO25**, SD **GPIO32** |
| Processing | 32-bit I2S read, DC offset removal, normalization |

**Status:** **Implemented**

### 4.3 FFT / Energy Event Detection

| Parameter | Value (from `fish_sound_detection.h` / `.cpp`) |
|-----------|--------------------------------------------------|
| Window | **Hamming** |
| Detection band | FFT bins **1–49** (low-band energy sum of magnitude squared) |
| Approx. band (48 kHz / 1024) | ~**47 Hz – ~2.3 kHz** (informative estimate) |
| `ENERGY_THRESHOLD` | **8.0** (ratio vs baseline) |
| `ABSOLUTE_THRESHOLD` | **4.0** |
| `MIN_VOLUME` / `MAX_VOLUME` | **0.5 / 50.0** |
| Baseline | Median of **50**-sample energy history |
| Debounce | **600 ms** between events |
| Additional gate | `lowFreqEnergy > 10.0` before trigger |
| Enable switch | `setFishSoundDetectionEnabled()` (off until application enables) |

**Status:** **Implemented** — not ML classification; energy-threshold detection only.

### 4.4 CH9329 UART HID

| Capability | Detail | Status |
|------------|--------|--------|
| UART | **115200 8N1**, RX **GPIO16**, TX **GPIO17** | **Implemented** |
| Frame format | Header **0x57 0xAB** + checksum | **Implemented** |
| Keyboard | `sendKey()`, `releaseKey()`, `releaseAllKeys()` | **Implemented** |
| Key map | Full HID key constants incl. modifiers (Ctrl/Shift/Alt/Win codes) | **Defined**; chords require app-level sequencing |
| Mouse absolute | `sendAbsMouse()` — logical **1920×1080** mapped to 4096-level coords | **Implemented** |
| Mouse relative | `sendRelMouse()` | **Implemented** |
| Scroll wheel | `scrollWheelUp()` / `scrollWheelDown()` | **Implemented** |
| Left/right click | `clickLeftPress/Release`, `clickRightPress/Release` | **Implemented** |

**Status:** **Implemented** — not a separate driver library; reference implementation in firmware.

### 4.5 Local Display, Buttons, NVS

| Item | Detail | Status |
|------|--------|--------|
| OLED | SSD1306 **128×32**, I2C **0x3C**, SDA/SCL **GPIO21/22** | **Implemented** |
| Keypad | **6** buttons with debounce / long-press logic | **Implemented** |
| NVS | Menu mode, pause flag, network toggle, timing values, key-mapping slots (DY/JN/BUF/GH etc.), saved via `Preferences` | **Implemented** |

### 4.6 MQTT Remote Command / Status (Optional)

Active only when **network mode** is enabled in device settings (`variableValue_WLZT`).

| Topic | Direction | Examples |
|-------|-----------|----------|
| `devices/{device_id}/status` | Device → broker | `online`, `running`, `paused`, JSON state via `publishDeviceStateSync()` |
| `devices/{device_id}/control` | Broker → device | `pause`, `resume`, `upgrade`, JSON fields, `action/value` pairs |

**Status:** **Implemented** in firmware; requires operator-provided MQTT broker and placeholder/real credentials outside the repo.

### 4.7 OTA Update Path

| Path | Detail | Status |
|------|--------|--------|
| Placeholder URL | `OTA_FIRMWARE_URL` in `config.example.h` | **Placeholder** |
| HTTP download OTA | `OTAUpgrade::upgradeFromURL()` with OLED progress | **Implemented** |
| ArduinoOTA | `ArduinoOTA.begin()` / `handle()` when network mode active | **Implemented** (LAN/dev) |
| MQTT trigger | Control payload `upgrade` or `upgrade/<http-url>` | **Implemented** |
| On-device web OTA UI | — | **Not implemented** |

The optional Flask backend may publish MQTT control commands; it is **not** a hosted OTA SaaS.

### 4.8 Optional Flask/MQTT Backend

| Item | Status |
|------|--------|
| Location | `server/mqtt_web_service/` |
| Stack | Flask, SQLAlchemy, Alembic, PostgreSQL, paho-mqtt |
| Sanitization | `.env.example` fallback via `env_loader.py`; no production DB dumps |
| UI templates | Reference pages; some example business copy remains with **YOUR_SALES_PHONE** placeholders |
| Production SaaS | **Not claimed** — reference deployment only |

### 4.9 Documentation, Examples, Hardware Files

| Asset | Status |
|-------|--------|
| `docs/` (hardware, firmware, protocol, server, audio-detection, FAQ) | **Implemented** |
| `hardware/BOM.md`, `wiring-schematic.md`, `block-diagram.md` | **Reference diagrams** (no KiCad Gerber in repo) |
| `examples/ch9329_mouse/` | **Minimal example sketch** |
| `examples/i2s_energy_detect/` | **Minimal example sketch** |
| `examples/basic_config.example.h`, `serial_commands.md` | **Examples / docs** |
| `tools/ci/compile.yml` | **Reference CI template** (not active on GitHub by default) |

### 4.10 Planned (Not in v0.1.2)

- PlatformIO / ESP-IDF ports  
- KiCad schematic / PCB Gerber in repository  
- Automated hardware-in-the-loop tests  
- On-device web OTA portal  
- ML / multi-band audio classification  

---

## 5. Open Source Value

### Problem addressed

Developers integrating **ESP32 + PCM1808 I2S + CH9329 HID + MQTT** often face scattered snippets: I2S timing, FFT thresholds, HID frame formats, topic naming, and sanitization before public release. This repository consolidates those integration details in one **MIT-licensed, clone-and-adapt** package.

### Target developers

- Embedded engineers prototyping audio-triggered USB HID behavior  
- Makers and educators teaching I2S, UART protocols, NVS, and MQTT  
- IoT integrators needing a **reference** firmware ↔ control-plane protocol map  
- OSS maintainers studying placeholder-first public release patterns  

### Why the stack is a useful reference

Public repos rarely document **PCM1808 + CH9329 + ESP32** together with **BOM/wiring**, **MQTT field mapping**, and an **optional sanitized backend**. The project is **early but reproducible**: placeholder configs work out of the box for reading/building; operators replace placeholders before production.

### What we do **not** claim

- No star count, download count, or user-base statistics  
- Not **production-ready**, **industry-leading**, or **widely adopted**  
- No guaranteed time savings — may **help reduce repeated bring-up work** depending on hardware and experience  

---

## 6. Maintainer Role

**Primary maintainer:** xuzuxiong

| Responsibility | Description |
|----------------|-------------|
| Firmware maintenance | DECQ module structure, pin maps, build FQBN notes |
| Documentation | README, `docs/`, hardware references, release notes |
| Release hygiene | Semantic versions (v0.1.x), CHANGELOG |
| Issue / PR triage | GitHub templates, best-effort responses |
| Security hygiene | Placeholder enforcement, secret scanning on PRs |
| Roadmap clarity | Separate **implemented** vs **planned** features |

**Maintenance model:** best-effort open-source maintenance only — **no commercial SLA**.

---

## 7. Current Maturity

### v0.1.2 (early-stage public release)

- Sanitized firmware and optional backend published on GitHub  
- Documentation, BOM/wiring references, and minimal examples included  
- Placeholder configs load by default (`config.example.h`, `.env.example`)  
- Release notes and MIT license present  

### Known Limitations

| Limitation | Notes |
|------------|-------|
| Early-stage scope | Reference implementation, not a mature platform |
| No usage metrics claimed | New public repository |
| CI | **Reference template only** — GitHub Actions **not enabled by default** |
| Testing | **Manual** hardware-in-the-loop checklist; no automated HIL suite |
| Arduino only | **No** PlatformIO / ESP-IDF project in repo |
| Hardware docs | Text/Mermaid/ASCII diagrams — **no** published PCB Gerber |
| Pin mapping | Compatibility depends on matching documented GPIO wiring |
| Application logic | `menu.cpp` / `TimerManager.cpp` contain domain-specific state machines beyond minimal demos |
| Backend | Optional **sanitized reference** — operators must harden for production |
| Server UI copy | Some template pages retain example distributor text with placeholders |

---

## 8. Roadmap

| Phase | Focus |
|-------|-------|
| **v0.2.x** | Extract additional minimal sketches; Parser/host-side tests; optional enable CI template; doc/code sync improvements |
| **v0.3.x** | Community KiCad/PDF schematic contributions; PlatformIO skeleton; clearer unified MQTT JSON control schema |
| **Future** | Configurable pin table without recompile; optional audio-classification extensions (research direction only) |

No fixed delivery dates are committed unless added to repository planning issues.

---

## 9. How Codex Would Help

Codex / API credits would support **open-source maintenance**, not marketing:

| Area | Use |
|------|-----|
| Firmware refactoring | Split teaching modules from DECQ application logic |
| ESP32 RAM / buffer review | Review FFT/I2S buffer usage and suggest safe tuning |
| Documentation sync | Keep pin maps, MQTT tables, and README aligned with code |
| Wiring / protocol docs | Generate consistent diagrams from header definitions |
| Issue triage | Draft replies from FAQ and Serial-log patterns |
| Release notes | Prepare CHANGELOG entries from diffs |
| Example extraction | Maintain minimal CH9329 / I2S sketches |
| CI / build templates | Improve `tools/ci/compile.yml` reference (maintainer-enabled) |
| Compliance scanning | Detect IPs, tokens, phone numbers, personal domains in PRs |
| Test scaffolding | Host-side Parser tests and compile smoke checks |

---

## 10. Application Scenarios

| Scenario | Fit (reference-level) |
|----------|------------------------|
| Audio-triggered automation | Low-band energy event → GPIO/HID action |
| HID bridge prototyping | ESP32 + CH9329 controlling a USB host |
| MQTT-managed embedded device | Optional remote pause/resume/status sync |
| Education / lab exercises | I2S, FFT, UART, NVS, MQTT patterns |
| Maker / hobby projects | Documented wiring + minimal examples |
| Low-cost embedded prototypes | MIT code adaptable with independent security review |

These are **potential uses** of a reference stack, not claims of existing deployments.

---

## 11. Hardware Compatibility

| Component | Requirement | Notes / caveats |
|-----------|-------------|-----------------|
| **ESP32** | Dev Module, **16 MB Flash**, App **3MB** partition | Smaller flash may fail to build (~1.5 MB+ firmware) |
| **PCM1808** | I2S ADC module | GPIO26/25/32; verify module clock/mode wiring |
| **CH9329** | UART HID bridge | GPIO16/17; common GND; 3.3 V TTL typical |
| **SSD1306** | 128×32 I2C OLED | Address 0x3C; GPIO21/22 |
| **Buttons** | 6× inputs | GPIO36/35/23/5/34/39; pull-up |
| **USB host** | PC via CH9329 USB | Separate from ESP32 UART/USB |
| **MQTT broker** | Optional (e.g. Mosquitto) | Not bundled; placeholder `192.168.1.100` |
| **Backend stack** | Optional PostgreSQL + Redis | For `server/mqtt_web_service/` reference only |

See `docs/hardware.md`, `hardware/BOM.md`, `hardware/wiring-schematic.md`.

---

## 12. License and Safety Statement

This project is licensed under the **MIT License**. You may use, modify, and distribute the code under the license terms, provided the copyright notice is included.

- Software is provided **“as is”**, **without warranty**.  
- **Production operators** must replace all placeholders (`YOUR_*`, `192.168.1.100`, example OTA URLs) with their own credentials and infrastructure.  
- **Production deployment** requires independent security review, EMC/hardware validation, and compliance with local regulations.  
- The optional Flask backend is a **sanitized reference**, not a certified or hosted service.

---

## Form Fields (copy/paste, ≤500 characters each)

### A. Why does this repository qualify?

```
I am the primary maintainer of esp32-arduino-demos (MIT). It is an early-stage but reproducible ESP32 Arduino reference: PCM1808 I2S at 48 kHz, FFT low-band energy detection (tunable thresholds), CH9329 UART HID, NVS settings, optional MQTT status/control and HTTP OTA hooks, plus an optional sanitized Flask/MQTT backend. Includes docs, BOM/wiring notes, minimal examples, v0.1.2 release, and placeholder configs—no secrets in repo.
```

### B. How will you use API credits for your project?

```
Codex/API credits will support OSS maintenance: firmware refactoring, ESP32 RAM/buffer review, docs and wiring/protocol sync, issue triage, release notes, minimal example extraction, CI/build template improvements, and PR secret/compliance scanning—keeping implemented features and roadmap clearly separated.
```

### C. Anything else we should know?

```
v0.1.2 is sanitized (no .env, SQL backups, user CSV, or real phone numbers). Placeholders default via config.example.h and .env.example. CI is a reference template only—not enabled by default. Honest early-stage scope; no fake stars, downloads, or adoption claims.
```

---

*Last updated for commit 018b965 / release v0.1.2.*
