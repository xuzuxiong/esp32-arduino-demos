# esp32-arduino-demos

**ESP32 Arduino 音频检测 + I2S + CH9329 HID 开源示例**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

> 基于 ESP32 / Arduino / PCM1808 (I2S) / CH9329 的可复用固件示例，帮助开发者学习音频事件检测、串口 HID 键鼠控制与可选 MQTT 远程管理。  
> English keywords: `esp32` `arduino` `i2s` `pcm1808` `ch9329` `hid` `audio-detection` `embedded`

---

## 一句话简介

在 ESP32 上采集 I2S 音频、用 FFT 检测低频能量事件，并通过 CH9329 向 USB 主机发送键盘/鼠标指令的开源参考固件（DECQ）。

## 项目用途

- 学习 **ESP32 I2S + PCM1808** 音频采集与 DMA 读取
- 学习 **CH9329 UART 协议** 实现 HID 键盘、绝对/相对鼠标
- 参考 OLED 菜单、按键 debounce、NVS 参数保存的完整结构
- 可选扩展 **WiFiManager + MQTT** 状态同步（需自建 Broker）

> ⚠️ 本项目处于 **早期开源阶段（v0.1.x）**，主要价值是工程参考与学习，并非宣称已大规模商用的成熟产品。

---

## 功能列表

### 已实现（v0.1.0）

| 模块 | 功能 |
|------|------|
| I2S / PCM1808 | 48 kHz 采样，1024 点 FFT，低频能量 + 动态基线检测 |
| CH9329 HID | 键盘、绝对/相对鼠标、滚轮；帧头 `0x57 0xAB` + 校验和 |
| 人机界面 | SSD1306 OLED 128×32，6 路物理按键，NVS 持久化 |
| 串口协议 | `MON,x,y` / `WALK,x,y` / `LOOT,x,y` 文本解析 |
| 网络（可选） | WiFiManager 配网、MQTT 状态 JSON、远程 pause/resume、HTTP OTA 钩子 |
| 工具 | Windows `arduino-cli` 一键编译烧录脚本 |

### 计划功能（未实现）

- PlatformIO / ESP-IDF 工程
- 独立 minimal 示例 sketch（纯 CH9329 / 纯 I2S）
- 自动化测试与 CI 编译
- 公开原理图 / PCB / BOM
- 多频段音频分类、ML 事件识别

---

## 硬件清单

| 器件 | 说明 |
|------|------|
| ESP32 Dev Module | 推荐 16MB Flash |
| PCM1808 | I2S ADC，48 kHz |
| CH9329 | UART → USB HID |
| SSD1306 | I2C 128×32 OLED |
| 按键 ×6 | 上拉输入 |
| 杜邦线 / PCB | 按 [docs/hardware.md](docs/hardware.md) 接线 |

---

## 接线说明

摘要（详见文档）：

| 功能 | ESP32 GPIO |
|------|------------|
| I2S BCK / WS / SD | 26 / 25 / 32 |
| UART CH9329 RX / TX | 16 / 17 |
| I2C OLED SDA / SCL | 21 / 22 |

完整表格：[docs/hardware.md](docs/hardware.md)

---

## 编译环境

| 项目 | 要求 |
|------|------|
| 框架 | **仅 Arduino**（ESP32 Arduino Core 2.x） |
| PlatformIO | ❌ 暂未支持 |
| ESP-IDF | ❌ 暂未支持 |
| OS | Windows（脚本）；macOS/Linux 可用 Arduino IDE 手动编译 |

### 依赖库

arduinoFFT · U8g2 · PubSubClient · WiFiManager · ArduinoJson · CRC32

详见 [docs/firmware.md](docs/firmware.md)

---

## 快速开始

```bash
git clone https://github.com/xuzuxiong/esp32-arduino-demos.git
cd esp32-arduino-demos
```

1. **（推荐）** 复制配置模板：

   ```bash
   cp firmware/DECQ/config.example.h firmware/DECQ/config.h
   # 编辑 config.h，填入本地 MQTT 占位值（若使用网络功能）
   ```

2. **安装依赖库**（Arduino Library Manager）

3. **编译烧录**

   ```powershell
   .\tools\build_upload.ps1 -Port COM6
   ```

   或用 Arduino IDE 打开 `firmware/DECQ/DECQ.ino`，选择 **ESP32 Dev Module** + **App 3MB / FAT 9MB (16MB Flash)**。

4. 打开 Serial Monitor **115200**，记录 `Device ID: CQxxxxxx`

---

## 配置说明

| 文件 | 说明 |
|------|------|
| `firmware/DECQ/config.example.h` | 提交的模板（占位符） |
| `firmware/DECQ/config.h` | 本地配置（**已 gitignore，勿提交真实密码**） |
| `examples/basic_config.example.h` | 与上相同的示例副本 |

主要宏：

```cpp
#define MQTT_SERVER "192.168.1.100"
#define MQTT_PORT 1883
#define MQTT_USER "YOUR_MQTT_USERNAME"
#define MQTT_PASSWORD "YOUR_MQTT_PASSWORD"
#define WIFI_PORTAL_SSID "DECQ-Setup"
#define OTA_FIRMWARE_URL "http://192.168.1.100/firmware/decq.bin"
```

音频阈值见 `firmware/DECQ/fish_sound_detection.h` 中 `ENERGY_THRESHOLD` 等。

---

## 示例输出

```
Device ID: CQ1A2B3C
WiFi connected
192.168.1.50
MQTT connected
[MQTT] sync {"online":true,"running":true,...}
```

更多见 [examples/sample_output.txt](examples/sample_output.txt)

---

## 常见问题

| 问题 | 方向 |
|------|------|
| 编译体积过大 | 使用 16MB + App3M 分区 |
| I2S 无数据 | 检查 26/25/32 接线 |
| CH9329 无反应 | TX/RX 交叉、115200 |
| MQTT 失败 | 检查 config.h、自建 Broker |

完整 FAQ：[docs/faq.md](docs/faq.md)

---

## 项目路线图 Roadmap

| 版本 | 目标 |
|------|------|
| **v0.1.x** | 开源整理、文档、脱敏、DECQ 主固件 |
| v0.2.x | 拆分 minimal examples、CI compile |
| v0.3.x | 硬件原理图 / BOM 社区贡献 |
| 未来 | PlatformIO、Parser 单元测试 |

---

## 仓库结构

```
├── firmware/DECQ/     # 主固件（Arduino sketch）
├── docs/              # 硬件、固件、协议、音频、FAQ
├── examples/          # 配置与命令示例
├── hardware/          # 原理图占位（待贡献）
├── tools/             # build_upload.ps1
├── tests/             # 测试占位
├── LICENSE            # MIT
└── CHANGELOG.md
```

---

## 贡献

阅读 [CONTRIBUTING.md](CONTRIBUTING.md)。Bug / Feature 请用 GitHub Issue 模板。

---

## License

[MIT License](LICENSE) — 可自由使用、修改、商用，但不提供任何担保。

---

## 免责声明

1. 本仓库仅供 **学习与研究** ESP32 音频采集与 HID 控制技术。
2. 固件、文档按 **「原样」** 提供，作者不对直接或间接损失负责。
3. 请遵守当地法律法规与 USB 主机系统的使用条款；不得用于未授权访问或干扰他人系统。
4. 网络、OTA、MQTT 功能需在你 **可控环境** 中部署；勿在公开仓库提交真实密码或 Token。
5. 本项目 **不包含** 生产级安全加固或商业技术支持承诺。

---

## GitHub 展示建议

**Repository description（建议复制到 GitHub Settings）：**

```
ESP32 Arduino demos: I2S/PCM1808 audio FFT detection + CH9329 UART HID keyboard/mouse. Early-stage open reference firmware with docs.
```

**Topics 建议：**

`esp32` `arduino` `i2s` `pcm1808` `ch9329` `hid` `audio-detection` `keyboard` `mouse` `embedded` `iot` `embedded-systems` `uart` `fft`

---

## 相关链接

- [硬件说明](docs/hardware.md)
- [固件说明](docs/firmware.md)
- [协议说明](docs/protocol.md)
- [音频检测](docs/audio-detection.md)
- [OpenAI Codex OSS 申请笔记](OPENAI_CODEX_OSS_APPLICATION_NOTES.md)
