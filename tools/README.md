# Tools / 工具

## build_upload.ps1

Windows 下使用 [arduino-cli](https://arduino.github.io/arduino-cli/) 编译并烧录 `firmware/DECQ/`。

```powershell
# 在仓库根目录
.\tools\build_upload.ps1
.\tools\build_upload.ps1 -Port COM6
.\tools\build_upload.ps1 -CompileOnly
```

首次运行会自动下载 arduino-cli 到 `firmware/DECQ/tools/arduino-cli/`（已在 `.gitignore` 中忽略）。

`firmware/DECQ/build_upload.ps1` 为同脚本的本地副本，可在 sketch 目录直接运行。

## 计划添加

- [ ] Linux / macOS 编译脚本
- [ ] GitHub Actions CI（仅 compile）
- [ ] 串口协议测试脚本
