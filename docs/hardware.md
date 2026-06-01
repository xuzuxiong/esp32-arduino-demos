# 硬件说明 / Hardware

本文档描述 DECQ 示例固件所针对的硬件连接与注意事项。

## 模块清单

| 模块 | 型号 / 说明 | 作用 |
|------|-------------|------|
| 主控 | ESP32（Dev Module，16MB Flash 推荐） | 运行 Arduino 固件 |
| ADC | PCM1808（I2S 从模式） | 48 kHz 音频采样 |
| HID 桥 | CH9329（UART） | USB 键盘 / 鼠标 HID |
| 显示 | SSD1306 128×32（I2C，地址 0x3C） | 状态与菜单 |
| 输入 | 6 路物理按键（上拉输入） | 本地菜单控制 |
| 指示 | GPIO 输出（Audio / WiFi 等） | 状态 LED 或继电器 |

> 完整 BOM 与接线原理图见 [hardware/BOM.md](../hardware/BOM.md)、[hardware/wiring-schematic.md](../hardware/wiring-schematic.md)。

## 引脚连接（与当前固件一致）

### I2S → PCM1808

| ESP32 GPIO | PCM1808 / I2S | 说明 |
|------------|---------------|------|
| GPIO 26 | BCK / SCK | 位时钟 |
| GPIO 25 | LRCK / WS | 字选择 |
| GPIO 32 | DOUT / SD | 数据输入（ESP32 RX） |

固件定义见 `firmware/DECQ/fish_sound_detection.h`：

```cpp
#define I2S_WS 25
#define I2S_SD 32
#define I2S_SCK 26
#define SAMPLE_RATE 48000
```

### UART → CH9329

| ESP32 GPIO | CH9329 | 说明 |
|------------|--------|------|
| GPIO 16 | RX | ESP32 接收（接 CH9329 TX） |
| GPIO 17 | TX | ESP32 发送（接 CH9329 RX） |
| GND | GND | 共地 |
| 3.3V / 5V | VCC | 按模块规格供电 |

波特率：**115200 8N1**（`scan_controller.h` 中 `BAUD_RATE`）。

### I2C → OLED

| ESP32 GPIO | SSD1306 | 说明 |
|------------|---------|------|
| GPIO 21 | SDA | I2C 数据 |
| GPIO 22 | SCL | I2C 时钟 |
| 3.3V | VCC | |
| GND | GND | |

### 按键与指示 GPIO

| GPIO | 宏名 | 方向 | 说明 |
|------|------|------|------|
| 36, 35, 23, 5, 34, 39 | `buttonPins[]` | 输入 | 6 路按键 |
| 15 | `MUT_OUT` | 输出 | 静音 / 输出控制 |
| 19 | `MUT_UP` | 输出 | |
| 18 | `MUT_DN` | 输出 | |
| 4 | `Audio` | 输出 | 音频事件指示 |
| 2 | `Wifi_AA` | 输出 | WiFi 状态指示（连上为 LOW） |

引脚定义见 `firmware/DECQ/keypad.h`。

## 供电注意事项

1. **ESP32 与 CH9329 共地**，电平匹配（多数 CH9329 模块支持 3.3V TTL）。
2. **I2S 布线尽量短**，PCM1808 模拟端远离数字开关噪声源。
3. PCM1808 需按数据手册提供正确的主时钟 / 模式引脚配置（硬件设计时确认）。
4. 大电流负载（继电器、功放）不要直接从 ESP32 3.3V 引脚取电。
5. USB 供电调试时，注意总电流与地线反弹。

## 信号链路概览

```
麦克风/线路 ──► PCM1808 ──I2S──► ESP32 ──UART──► CH9329 ──USB──► 主机(PC)
                     ▲              │
                     │              ├── I2C ──► OLED
                     │              └── GPIO ──► 按键 / LED
```

## 屏幕坐标系（CH9329 绝对鼠标）

固件默认逻辑分辨率 **1920×1080**，绝对坐标映射到 CH9329 的 4096 级坐标：

```cpp
chipX = (x * 4096) / 1920;
chipY = (y * 4096) / 1080;
```

若你的目标主机分辨率不同，请修改 `scan_controller.h` 中的 `SCREEN_W` / `SCREEN_H` 及相关宏。

## 相关文档

- [固件结构](firmware.md)
- [串口 / HID / MQTT 协议](protocol.md)
- [音频检测原理](audio-detection.md)
