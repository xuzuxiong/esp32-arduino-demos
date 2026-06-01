# Examples / 示例

独立 Arduino sketch，可从主固件 DECQ 中拆分学习，依赖更少。

| 目录 | 说明 | 依赖 |
|------|------|------|
| [ch9329_mouse/](ch9329_mouse/) | CH9329 绝对移动 + 左键单击 | 无额外库 |
| [i2s_energy_detect/](i2s_energy_detect/) | PCM1808 I2S + FFT 低频能量检测 | arduinoFFT |

## 编译

```bash
arduino-cli compile --fqbn esp32:esp32:esp32 examples/ch9329_mouse
arduino-cli compile --fqbn esp32:esp32:esp32 examples/i2s_energy_detect
```

CI 会在 push/PR 时自动编译上述示例。

## 配置类示例（非 sketch）

- [basic_config.example.h](../examples/basic_config.example.h)
- [serial_commands.md](../examples/serial_commands.md)
- [sample_output.txt](../examples/sample_output.txt)
