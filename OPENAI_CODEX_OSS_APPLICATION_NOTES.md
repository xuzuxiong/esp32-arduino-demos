# OpenAI Codex for Open Source — 申请材料草稿

> 本文档供维护者填写 [Codex for OSS 表单](https://openai.com/form/codex-for-oss/) 时使用。内容基于项目**真实状态**，不夸大影响力。  
> 最后更新：**v0.1.2**

---

## 表单字段 — 可直接复制（英文，各 ≤500 字符）

### Why does this repository qualify?

```
I am the primary maintainer of an MIT-licensed ESP32 stack: firmware (PCM1808 I2S FFT + CH9329 HID) plus an optional sanitized Flask MQTT web backend. Includes wiring BOM/schematics, CI, minimal Arduino examples, and protocol docs. Early-stage but end-to-end reproducible for embedded developers.
```

### How will you use API credits for your project?

```
Maintain docs/code sync across hardware pin maps and CH9329 protocol notes; expand minimal examples; add Parser unit tests and CI; triage issues/FAQ; review PRs for leaked credentials; prepare KiCad schematic contributions. Codex reduces repetitive maintainer work so I can focus on clear, safe reference firmware for the embedded community.
```

### Anything else we should know?

```
Early-stage project (new public repo). Full stack: ESP32 firmware + optional Flask MQTT server (sanitized). Not claiming large star count. Secrets removed; configs use placeholders. Releases v0.1.x with CI compile workflow.
```

---

## 1. 项目简介

**esp32-arduino-demos** — DECQ 固件 + 可选 Flask MQTT Web 服务端；ESP32 / PCM1808 I2S / CH9329 HID / OLED / MQTT。

https://github.com/xuzuxiong/esp32-arduino-demos · **MIT**

---

## 2. Primary Maintainer 角色

- 固件架构、引脚表、脱敏与 Release
- 文档（README、`docs/`、`hardware/` BOM/接线图）
- CI、minimal examples、Issue/PR triage

---

## 3. 对开源社区的价值

| 价值点 | v0.1.1 状态 |
|--------|-------------|
| 少见硬件栈 | PCM1808 + CH9329 + ESP32 完整接线 + 代码 |
| 可复现 | CI 编译、libraries.txt、config 模板 |
| 硬件文档 | BOM + wiring-schematic + block-diagram |
| 学习路径 | 主固件 + 2 个 minimal `.ino` 示例 |
| 诚实定位 | early-stage，不虚假宣传 |

---

## 4. Codex 将如何帮助维护

文档/引脚同步 · PR 脱敏审查 · 拆示例 · CI/测试 · FAQ/Issue · KiCad 原理图整理

---

## 5. 目前项目状态（如实）

- **版本**：v0.1.2（含 `server/mqtt_web_service/` 脱敏开源）
- **CI**：GitHub Actions 编译 DECQ + examples
- **硬件**：文字/框图接线图 + BOM（KiCad PDF 待贡献）
- **无** 生产数据库备份；MQTT 服务端为参考实现
- **新仓库**，生态影响力仍在积累

---

## 6. 后续计划

KiCad 原理图 · Parser 单元测试 · PlatformIO（按需）· FAQ 迭代

---

## 7. 声明

学习与研究用途；无 WARRANTY；用户自行负责合规与安全。

---

## 预估通过率（维护者自检）

| 维度 | v0.1.0 | v0.1.1 后 |
|------|--------|-----------|
| 仓库规范 | 较好 | 更好（CI + 脱敏完成） |
| 生态影响力 | 弱 | 仍弱（新 repo） |
| 叙事质量 | 中 | 中上（可强调完整 teaching stack） |
| **综合建议** | 可申请 | **建议现在提交表单** |

*OpenAI 滚动审核，无官方通过率；小项目需靠清晰叙事而非 star 数量。*
