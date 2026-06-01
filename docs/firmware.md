# 固件说明 / Firmware

## 概述

主固件位于 **`firmware/DECQ/`**，工程名为 `DECQ.ino`（Arduino 要求文件夹名与 `.ino` 同名）。

当前仓库 **仅支持 Arduino 框架**（ESP32 Arduino Core），未提供 PlatformIO / ESP-IDF 工程模板。

## 目录结构

```
firmware/DECQ/
├── DECQ.ino              # 入口：setup / loop
├── fish_sound_detection.* # I2S 采集 + FFT 低频能量检测
├── scan_controller.*      # CH9329 UART HID 协议
├── keypad.*               # 物理按键 debounce / 长按
├── menu.*                 # OLED 菜单与业务逻辑
├── TimerManager.*         # 定时器与自动化状态机
├── oled_display.*         # SSD1306 显示
├── wifi_mqtt.*            # WiFiManager + MQTT（可选）
├── OTAUpgrade.*           # HTTP OTA
├── Parser.*               # 串口坐标命令解析
├── esp32_id_utils.*       # 设备 ID 生成
├── config.example.h       # MQTT / OTA 配置模板
└── build_upload.ps1       # Windows 下 arduino-cli 编译烧录
```

## 主要模块

| 模块 | 文件 | 已实现功能 |
|------|------|------------|
| 音频检测 | `fish_sound_detection.*` | I2S 48kHz 采样、Hamming 窗 FFT、低频能量阈值、事件 debounce |
| HID 控制 | `scan_controller.*` | CH9329 帧格式：键盘、绝对/相对鼠标、滚轮 |
| 人机界面 | `menu.*`, `keypad.*`, `oled_display.*` | 多页菜单、NVS 参数保存 |
| 网络（可选） | `wifi_mqtt.*` | WiFiManager 配网、MQTT 状态同步与控制 |
| OTA（可选） | `OTAUpgrade.*` | HTTP 下载固件、OLED 进度 |

## 依赖库（Arduino Library Manager）

安装 **ESP32 Arduino Core 2.x** 后，还需：

| 库 | 用途 |
|----|------|
| [arduinoFFT](https://github.com/kosme/arduinoFFT) | FFT 分析 |
| [U8g2](https://github.com/olikraus/u8g2) | OLED |
| [PubSubClient](https://github.com/knolleary/pubsubclient) | MQTT |
| [WiFiManager](https://github.com/tzapu/WiFiManager) |  captive portal 配网 |
| [ArduinoJson](https://github.com/bblanchon/ArduinoJson) | JSON 序列化 |
| [CRC32](https://github.com/bakercp/CRC32) | 设备 ID |

## 板型与分区

`build_upload.ps1` 使用的 FQBN：

```
esp32:esp32:esp32:PartitionScheme=app3M_fat9M_16MB,CPUFreq=240,FlashMode=qio,FlashFreq=80,FlashSize=16M,UploadSpeed=921600
```

固件体积较大（约 1.5MB+），**必须使用 16MB Flash 与大 APP 分区**。

## 配置

默认使用 `config.example.h` 占位符（`YOUR_*` / `192.168.1.100`）。可选：创建 `config.h` 覆盖本地值（已 gitignore）。

## 编译与烧录

### 方式 A：PowerShell + arduino-cli（推荐）

在仓库根目录：

```powershell
.\tools\build_upload.ps1
.\tools\build_upload.ps1 -Port COM6
.\tools\build_upload.ps1 -CompileOnly
```

或在 `firmware/DECQ/` 下直接运行同目录的 `build_upload.ps1`。

### 方式 B：Arduino IDE

1. 打开 `firmware/DECQ/DECQ.ino`
2. 选择板型：**ESP32 Dev Module**
3. Partition Scheme：**App 3MB / FAT 9MB (16MB Flash)**
4. 安装上述依赖库
5. 编译并上传

## 首次运行

1. 上电后 Serial **115200** 会打印 `Device ID: CQxxxxxx`
2. OLED 显示启动动画
3. 若未开启网络模式（`variableValue_WLZT` 默认 false），设备以本地菜单模式运行
4. 开启网络模式后，首次连接会进入 **DECQ-Setup** 配网热点

## NVS 命名空间

运行时参数保存在 NVS 命名空间 `testNamespace`（见 `DECQ.ino`）。恢复出厂可擦除 Flash 或清除该命名空间。

## 计划功能（尚未实现）

- PlatformIO / ESP-IDF 移植
- 单元测试与 CI 编译
- 可配置引脚表（无需改源码）
- 独立的最小 CH9329 / I2S 示例 sketch

## 相关文档

- [硬件接线](hardware.md)
- [协议说明](protocol.md)
- [音频检测](audio-detection.md)
- [FAQ](faq.md)
