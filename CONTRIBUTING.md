# 贡献指南 / Contributing

感谢你对 **esp32-arduino-demos** 的关注。本项目处于早期阶段，欢迎提交 Issue 和 Pull Request。

## 开始之前

1. 阅读 [README.md](README.md) 与 [docs/](docs/) 下的文档。
2. 确认改动不包含真实 WiFi 密码、MQTT 凭据、服务器地址或客户信息。
3. 本地修改 `config.h` 时**不要提交**；仅更新 `config.example.h` 中的占位符。

## 开发环境

- **Arduino IDE 2.x** 或 **arduino-cli**（见 `tools/build_upload.ps1`）
- 板型：ESP32 Dev Module，16MB Flash，`app3M_fat9M_16MB` 分区
- 依赖库见 [docs/firmware.md](docs/firmware.md)

## 提交 Pull Request

1. Fork 本仓库并创建特性分支：`git checkout -b feature/your-topic`
2. 保持改动范围小、聚焦单一问题或功能。
3. 若修改协议或引脚定义，请同步更新 `docs/` 中对应文档。
4. 在 PR 描述中说明：改了什么、为什么改、如何验证。

## 代码风格

- 与现有固件风格保持一致（C++ / Arduino）。
- 新增宏或引脚请集中到对应 `.h` 文件并写注释。
- 避免引入与 ESP32 示例无关的大型依赖。

## 报告 Bug

请使用 [Bug Report 模板](.github/ISSUE_TEMPLATE/bug_report.md)，并提供：

- 硬件版本、ESP32 模块型号
- Arduino / 库版本
- 复现步骤与 Serial 日志（脱敏后）

## 功能建议

请使用 [Feature Request 模板](.github/ISSUE_TEMPLATE/feature_request.md)，说明使用场景与期望行为。

## 许可证

贡献代码即表示同意以 [MIT License](LICENSE) 发布。
