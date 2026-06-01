# 协议说明 / Protocol

本文档描述 DECQ 固件涉及的三类通信：**CH9329 UART HID**、**Serial 文本命令**、**MQTT 控制（可选）**。

## 1. CH9329 UART HID 协议

### 物理层

- 接口：ESP32 `Serial2`，GPIO16(RX) / GPIO17(TX)
- 波特率：115200，8N1

### 帧格式（通用）

| 字段 | 说明 |
|------|------|
| 帧头 | `0x57 0xAB` |
| 地址 | 通常 `0x00` |
| 命令码 | 见下表 |
| 长度 | 后续数据字节数 |
| 数据 | 载荷 |
| 校验 | 帧头至数据末尾的 **累加和**（低 8 位） |

### 常用命令码（与固件实现一致）

| 命令码 | 功能 | 实现函数 |
|--------|------|----------|
| `0x02` | 键盘按键 | `sendKey()` / `releaseKey()` |
| `0x04` | 绝对鼠标 | `sendAbsMouse()` |
| `0x05` | 相对鼠标 / 按键 | `clickLeftPress()` 等 |

### 键盘按键示例

按下键码 `0x1E`（数字 1）：

```
57 AB 00 02 08 00 00 1E 00 00 00 00 00 [SUM]
```

释放所有键：

```
57 AB 00 02 08 00 00 00 00 00 00 00 00 [SUM]
```

键码表见 `scan_controller.h` 中 `KEY_*` 常量（USB HID 用法页映射）。

### 绝对鼠标示例

移动到逻辑坐标 (800, 180)（1920×1080 基准）：

1. 转换为芯片坐标：`chipX = x * 4096 / 1920`
2. 帧：`57 AB 00 04 07 02 00 [X_L] [X_H] [Y_L] [Y_H] 00 [SUM]`

### 鼠标按键

- 左键按下：`... 05 01 01 ...`
- 右键按下：`... 05 01 02 ...`
- 释放：`... 05 01 00 ...`

## 2. Serial 文本命令（Parser）

固件通过 `Parser.cpp` 解析形如 **`CMD,x,y`** 的文本行（逗号或空格分隔）：

| 命令 | 格式 | 含义 |
|------|------|------|
| `MON` | `MON,884,154` | 解析为坐标指令（业务层使用） |
| `WALK` | `WALK,1588,348` | 移动类指令 |
| `LOOT` | `LOOT,1028,147` | 拾取类指令 |
| `NONE` | `NONE` | 空操作 |

解析成功后写入 `Parsed` 结构：

```cpp
struct Parsed {
  CommandType type;  // CMD_MON / CMD_WALK / CMD_LOOT / CMD_NONE
  int x, y;
};
```

> 具体坐标如何驱动 HID 动作，由 `menu.cpp` / `TimerManager.cpp` 中的业务逻辑调用 `sendAbsMouse()` 等完成。

## 3. MQTT 协议（可选，需开启网络模式）

### 连接

- Broker / 端口 / 用户名 / 密码：见 `config.example.h`
- Client ID：`DECQ:{deviceID}`
- 设备 ID 格式：`CQ` + 6 位十六进制（基于芯片 MAC 的 CRC32）

### 主题

| 方向 | 主题 | 说明 |
|------|------|------|
| 上行 | `devices/{device_id}/status` | 心跳、JSON 状态、mode/paused 等 |
| 下行 | `devices/{device_id}/control` | 远程控制 |

### 上行载荷示例

简单心跳：

```
online
running
paused
mode/auto
```

JSON 状态同步（节选字段）：

```json
{
  "online": true,
  "running": true,
  "menu": 0,
  "mode": "auto",
  "uptime_sec": 1234,
  "return": 0,
  "dd_fishing_keys": [26, 27, 28, 29, 30, 31]
}
```

完整字段见 `wifi_mqtt.cpp` 中 `publishDeviceStateSync()`。

### 下行控制格式

支持两种形式：

1. **简单命令**：`pause`、`resume`、`upgrade`
2. **键值对**：`action/value`，例如 `dd_all_keys_press_time/100`
3. **JSON 对象**：`{"dd_bait_key": 33, "enabled": true}`

固件在 `mqttCallback()` 中解析并写入 NVS，随后 `publishDeviceStateSync()` 回传状态。

### OTA

- 命令 `upgrade` 或带 URL 的控制字段会触发 `OTAUpgrade::upgradeFromURL()`
- 默认 URL 见 `OTA_FIRMWARE_URL`（`config.example.h`）

## 4. 示例指令速查

| 场景 | 指令 / 动作 |
|------|-------------|
| 左键单击 | `clickLeftPress()` + `clickLeftRelease()` |
| 移动到 (100, 200) | `sendAbsMouse(100, 200)` |
| 发送按键 A | `sendKey(KEY_A)` |
| 解析坐标 | `parse_cmd_xy("MON,100,200", out)` |
| MQTT 暂停 | publish 到 control：`pause` |

更多示例见 [examples/serial_commands.md](../examples/serial_commands.md)。

## 免责声明

CH9329 协议细节以芯片官方文档为准；本仓库实现来自工程实践，仅供学习参考。MQTT 部分需自建 Broker，本仓库**不包含**服务端实现。
