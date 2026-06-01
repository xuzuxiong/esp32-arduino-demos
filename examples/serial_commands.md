# 串口 / HID / MQTT 命令示例

本文档仅包含**示例值**，不含真实凭据。

## CH9329 HID（固件内部 API）

```cpp
#include "scan_controller.h"

initSerial();                    // Serial2 @ 115200, GPIO16/17

sendAbsMouse(800, 180);          // 绝对鼠标移动
clickLeftPress();
clickLeftRelease();              // 左键单击
scrollWheelUp();                 // 滚轮上滚

sendKey(KEY_A);                  // 按下 A
releaseKey(KEY_A);               // 释放 A
releaseAllKeys();                // 释放全部
```

## 文本坐标命令（Parser）

输入格式（UART 或业务层字符串）：

```
MON,884,154
WALK,1588,348
LOOT,1028,147
NONE
```

C++ 解析：

```cpp
Parsed out;
char line[] = "MON,100,200";
if (parse_cmd_xy(line, out)) {
  // out.type == CMD_MON, out.x == 100, out.y == 200
}
```

## MQTT 控制主题（可选）

设备 ID 示例：`CQ1A2B3C`（实际由 MAC 哈希生成）

**订阅（设备端）：** `devices/CQ1A2B3C/control`

**发布（控制端）示例：**

```
pause
resume
mode/auto
dd_all_keys_press_time/100
upgrade
```

**JSON 控制示例：**

```json
{
  "enabled": true,
  "dd_bait_key": 33,
  "dd_return_key": 50
}
```

**状态主题：** `devices/CQ1A2B3C/status`

设备会周期性发布 `online` 及 JSON 状态（见 `docs/protocol.md`）。

## Serial 调试输出（115200）

上电后典型日志见 [sample_output.txt](sample_output.txt)。
