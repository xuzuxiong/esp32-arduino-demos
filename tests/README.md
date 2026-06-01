# Tests / 测试

## 当前状态

仓库 **尚未包含** 自动化测试。DECQ 固件主要依赖硬件在环验证。

## 建议的手动测试清单

- [ ] 编译通过（16MB 分区 FQBN）
- [ ] Serial 115200 输出 Device ID
- [ ] I2S 能量值随声音变化
- [ ] CH9329 能在主机产生键鼠事件
- [ ] OLED 菜单可切换并保存 NVS
- [ ] （可选）MQTT 连接与 pause/resume

## 计划

- [ ] `Parser.cpp` 纯逻辑单元测试（Host 侧 gtest / Python）
- [ ] CH9329 帧校验和 golden 测试
- [ ] CI 编译检查

欢迎贡献测试基础设施。
