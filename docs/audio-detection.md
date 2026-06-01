# 音频检测说明 / Audio Detection

## 概述

DECQ 固件通过 **I2S 从 PCM1808** 采集音频，使用 **FFT + 低频能量分析** 检测突发声音事件，可用于触发 GPIO 指示或后续自动化逻辑。

> **已实现**：I2S 采集、Hamming 窗 FFT、低频 bin 能量、滑动历史中位数基线、阈值判定、600ms debounce。  
> **计划 / 可扩展**：多频段分类、MFCC、云端推理、VAD 状态机等（尚未在仓库中实现）。

## 信号链路

```
音频源 → PCM1808 (24-bit I2S) → ESP32 I2S DMA → 去 DC → FFT → 低频能量 → 阈值比较 → 事件
```

## 采样参数

| 参数 | 值 | 定义位置 |
|------|-----|----------|
| 采样率 | 48000 Hz | `SAMPLE_RATE` |
| 每帧样本数 | 1024 | `SAMPLES` |
| 位宽 | 32-bit 容器（有效 24-bit 左对齐） | `setupI2S()` |
| I2S 模式 | Master RX, Standard I2S | `fish_sound_detection.cpp` |

## 处理流程

1. **`readI2SData()`**  
   从 I2S 读取 1024 个样本，右移 8 位对齐，计算 DC offset 并归一化到 `vReal[]`。

2. **FFT**  
   - 窗函数：Hamming  
   - `FFT.compute(FFT_FORWARD)`  
   - `complexToMagnitude()` 得到幅度谱

3. **低频能量 `computeLowFreqEnergy()`**  
   对 bin **1～49**（排除 DC）的幅度平方求和：

   ```cpp
   lowFreqEnergy += vReal[i] * vReal[i];
   ```

4. **基线 `computeBaselineEnergy()`**  
   维护长度 50 的 `energyHistory[]`，取**中位数**作为动态基线，适应环境噪声。

5. **触发条件 `detectFishSound()`**  
   同时满足：
   - `energyRatio = lowFreqEnergy / (baseline + 0.01) >= ENERGY_THRESHOLD (8.0)` **或**
   - `lowFreqEnergy >= ABSOLUTE_THRESHOLD (4.0)`
   - `MIN_VOLUME (0.5) <= lowFreqEnergy <= MAX_VOLUME (50.0)`
   - `lowFreqEnergy > 10.0`
   - 检测功能已启用（`setFishSoundDetectionEnabled(true)`）
   - 距上次触发 ≥ 600ms

6. **事件输出**  
   - 设置内部 pending 标志（`consumeFishSoundEvent()` 读取）  
   - 翻转 `Audio` GPIO 电平作为指示

## 关键宏（可 tuning）

```cpp
#define ENERGY_THRESHOLD 8.0
#define ABSOLUTE_THRESHOLD 4.0
#define MIN_VOLUME 0.5
#define MAX_VOLUME 50.0
#define HISTORY_SIZE 50
```

调参建议：

- 环境噪声大 → 提高 `ENERGY_THRESHOLD` 或 `ABSOLUTE_THRESHOLD`
- 漏检弱信号 → 降低阈值或增大 `HISTORY_SIZE` 平滑基线
- 误触发多 → 增大 debounce 时间或缩小有效能量上限

## 启用 / 禁用

```cpp
setFishSoundDetectionEnabled(true);   // 允许触发
if (consumeFishSoundEvent()) { /* 处理一次事件 */ }
```

主循环在 `DECQ.ino` 的 `loop()` 中始终调用 `detectFishSound()`；是否产生有效事件由上述开关控制。

## 与 FFT 库

使用 [arduinoFFT](https://github.com/kosme/arduinoFFT)（模板类型 `ArduinoFFT<float>`）。

## 未实现 / 路线图

| 方向 | 状态 |
|------|------|
| 多频段能量（中高频） | 计划 |
| 纯时域 RMS / 峰值检测（无 FFT） | 可扩展示例 |
| 谱减法降噪 | 计划 |
| 事件类型分类（ML） | 计划 |
| 音频流上传 | 未计划 |

欢迎通过 Issue 讨论你需要的检测场景，便于后续添加**独立的最小示例**而不影响主固件复杂度。

## 相关文件

- `firmware/DECQ/fish_sound_detection.h`
- `firmware/DECQ/fish_sound_detection.cpp`
- `firmware/DECQ/DECQ.ino`（`detectFishSound()` 调用）
