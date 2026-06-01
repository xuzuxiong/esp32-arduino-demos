# OpenAI Codex for Open Source — 申请材料草稿

> 本文档供维护者填写 OpenAI Codex for Open Source 申请时使用。内容基于项目**真实早期状态**，不夸大影响力。

---

## 1. 项目简介

**esp32-arduino-demos** 是一个面向嵌入式开发者的开源参考项目，核心示例 **DECQ** 演示如何在 ESP32（Arduino 框架）上集成：

- **PCM1808** I2S 音频采集
- **FFT + 低频能量** 声音事件检测
- **CH9329** UART → USB HID 键鼠控制
- SSD1306 OLED 菜单、物理按键、NVS 参数保存
- 可选 WiFiManager / MQTT 远程状态同步

仓库地址：https://github.com/xuzuxiong/esp32-arduino-demos

许可证：**MIT**

---

## 2. Primary Maintainer 角色

我是该仓库的 **Primary Maintainer**，负责：

- 固件架构与引脚定义的维护
- 文档（中英文 README、`docs/`）与开源合规（脱敏、LICENSE）
- Issue / PR  triage 与版本发布
- 将工程经验沉淀为可复现的最小示例，降低 ESP32 + I2S + HID 的学习门槛

---

## 3. 对开源社区的价值

| 价值点 | 说明 |
|--------|------|
| 硬件组合少见 | PCM1808 + CH9329 + ESP32 的完整接线与固件在同一仓库，便于对照学习 |
| 真实工程代码 | 非 Hello World，包含 FFT 检测、HID 帧格式、NVS、可选 MQTT |
| 文档齐全 | hardware / firmware / protocol / audio-detection / FAQ 分离 |
| 可安全复现 | 敏感凭据已替换为占位符，config 与 example 分离 |
| MIT 许可 | 允许社区 fork、改板、集成到其他项目 |

本项目**不**声称已有大规模用户或商业部署；定位为 **early-stage reference implementation**。

---

## 4. Codex 将如何帮助维护

| 维护任务 | Codex 辅助方式 |
|----------|----------------|
| 文档与代码同步 | 引脚 / 协议变更时更新 `docs/` 与 README |
| 脱敏审查 | 扫描 PR 中是否含密码、内网 IP |
| 拆分示例 | 从 DECQ 提取独立 minimal sketch（如纯 CH9329 demo） |
| 测试与 CI | 生成 Parser 单元测试、arduino-cli CI 工作流 |
| Issue 响应 | 根据 FAQ 与日志草稿回复常见问题 |
| 国际化 | 保持中文为主、英文关键词便于 GitHub 搜索 |

---

## 5. 目前项目状态（如实）

- **版本**：v0.1.0（首次公开整理）
- **成熟度**：早期；主固件功能完整但菜单业务逻辑复杂，文档仍在完善
- **支持平台**：仅 Arduino（ESP32 Core），无 PlatformIO / ESP-IDF
- **硬件资料**：原理图 / PCB 尚未公开，仅有引脚表与接线说明
- **服务端**：不包含 MQTT Web 后端；网络功能需用户自建 Broker
- **测试**：无自动化测试，依赖硬件在环验证
- **社区**：新开源仓库，无虚假宣传 star 或下载量

---

## 6. 后续维护计划（6～12 个月）

1. **v0.2.x**：拆分 `examples/ch9329_keyboard/`、`examples/i2s_energy/` 最小 sketch
2. 添加 GitHub Actions **compile-only** CI
3. 补充 KiCad 原理图占位或社区贡献的 BOM
4. 统一 MQTT 下行 JSON 协议文档与固件解析
5. 根据 Issue 迭代 FAQ 与调参指南
6. 评估 PlatformIO 支持（若社区有需求）

---

## 7. 声明

- 本仓库用于**学习与研究** ESP32 音频与 HID 集成。
- 不包含商业授权、客户数据或生产服务器配置。
- 使用者需自行承担部署与安全责任（见 README 免责声明）。

---

*最后更新：2026-06-01*
