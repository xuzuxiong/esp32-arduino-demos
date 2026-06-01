# 常见问题 / FAQ

## 编译与烧录

### Q: Arduino IDE 编译提示分区不够 / sketch too large？

**A:** 选择 **16MB Flash** 与 **App 3MB / FAT 9MB** 分区方案。参考 `tools/build_upload.ps1` 中的 FQBN。

### Q: 找不到 `config.h`？

**A:** 复制 `firmware/DECQ/config.example.h` 为 `config.h`。未复制时编译会自动使用 example 中的占位符。

### Q: 缺少 arduinoFFT / U8g2 等库？

**A:** 在 Arduino Library Manager 搜索安装，见 [firmware.md](firmware.md) 依赖表。

### Q: 无法进入下载模式？

**A:** 按住 **BOOT**，点按 **EN/RST**，再松开 BOOT；或降低 Upload Speed 到 115200。

---

## 串口与调试

### Q: Serial 无输出？

**A:** 确认波特率 **115200**；USB 转串口驱动已安装；部分板卡 USB 口仅用于供电，需外接 UART。

### Q: 中文 OLED 乱码？

**A:** 确保源文件 **UTF-8** 编码；U8g2 字体需支持中文（见 `oled_display.cpp` 所用字体）。

---

## I2S / PCM1808

### Q: I2S 无数据或能量始终为 0？

**A:** 检查：

1. BCK/WS/SD 是否与 `fish_sound_detection.h` 引脚一致
2. PCM1808 供电与 MCLK 配置（硬件）
3. 共地是否可靠
4. Serial 是否打印 `I2S driver install failed` / `I2S pin setup failed`

### Q: 音频检测误触发或从不触发？

**A:** 调整 `ENERGY_THRESHOLD`、`ABSOLUTE_THRESHOLD`、`MIN_VOLUME`；确认已调用 `setFishSoundDetectionEnabled(true)`。详见 [audio-detection.md](audio-detection.md)。

---

## CH9329 / HID

### Q: CH9329 无反应，主机无键鼠输入？

**A:** 检查：

1. TX/RX 是否交叉连接
2. 波特率 115200
3. CH9329 是否已被 USB 主机识别（模块 LED）
4. 用逻辑分析仪查看是否有 `57 AB` 帧头

### Q: 鼠标坐标偏移？

**A:** 修改 `scan_controller.h` 中 `SCREEN_W` / `SCREEN_H` 与实际分辨率一致。

---

## WiFi / MQTT

### Q: 连不上 WiFi？

**A:** 首次使用 WiFiManager 热点 **DECQ-Setup** 配网；或清除 NVS 后重试。网络模式需在设备设置中开启（`variableValue_WLZT`）。

### Q: MQTT 连接失败？

**A:** 检查 `config.h` 中 Broker 地址、端口、用户名密码；Broker 需允许 ESP32 客户端 ID `DECQ:...`。

### Q: 本仓库有 MQTT 服务端吗？

**A:** **没有。** 需自行部署 Mosquitto 等 Broker。协议说明见 [protocol.md](protocol.md)。

---

## OTA

### Q: OTA 下载失败？

**A:** 确认 WiFi 已连接、URL 可 HTTP 访问、固件 bin 与分区匹配；查看 Serial 中 HTTP 错误码。

---

## 其他

### Q: 这个项目成熟吗？

**A:** 处于 **早期开源阶段（v0.1.x）**，主要价值是学习 ESP32 + I2S + CH9329 的完整接线与固件结构，而非开箱即用的商业产品。

### Q: 可以商用吗？

**A:** MIT License 允许商用，但**无 WARRANTY**；请自行完成合规与安全评估。见 [LICENSE](../LICENSE) 与 README 免责声明。

---

仍有疑问？请提交 [GitHub Issue](https://github.com/xuzuxiong/esp32-arduino-demos/issues) 并附上硬件型号与日志（脱敏）。
