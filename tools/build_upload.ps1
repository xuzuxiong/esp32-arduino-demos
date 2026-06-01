# DECQ：命令行编译 / 烧录（比 Arduino IDE 点按钮通常更省事；第二次起多为增量编译会更快）
# 用法（在 DECQ 目录下 PowerShell 执行）:
#   .\build_upload.ps1
#   .\build_upload.ps1 -Port COM6
#   .\build_upload.ps1 -CompileOnly
#
# 首次运行若未安装 arduino-cli，会自动下载到本目录 tools\arduino-cli\

param(
  [string] $Port = "COM6",
  [switch] $CompileOnly
)

$ErrorActionPreference = "Stop"
$SketchDir = Join-Path (Split-Path $PSScriptRoot -Parent) "firmware\DECQ"
$ArduinoUser = Join-Path $env:LOCALAPPDATA "Arduino15"
$ToolsDir = Join-Path $SketchDir "tools\arduino-cli"
$CliExe = Join-Path $ToolsDir "arduino-cli.exe"

function Get-ArduinoCli {
  $fromPath = Get-Command arduino-cli -ErrorAction SilentlyContinue
  if ($fromPath) { return $fromPath.Source }
  if (Test-Path $CliExe) { return $CliExe }
  New-Item -ItemType Directory -Force -Path $ToolsDir | Out-Null
  $ver = "1.1.1"
  $zip = Join-Path $env:TEMP "arduino-cli-$ver.zip"
  $url = "https://github.com/arduino/arduino-cli/releases/download/v$ver/arduino-cli_${ver}_Windows_64bit.zip"
  Write-Host "正在下载 arduino-cli $ver 到 tools ..."
  [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
  Invoke-WebRequest -Uri $url -OutFile $zip -UseBasicParsing
  Expand-Archive -Path $zip -DestinationPath $ToolsDir -Force
  if (-not (Test-Path $CliExe)) { throw "arduino-cli 解压失败: $CliExe" }
  return $CliExe
}

$cli = Get-ArduinoCli
& $cli config set directories.user $ArduinoUser | Out-Null

# ESP32 Dev Module，16MB Flash，大 APP 分区（固件约 1.5MB，必须用大分区）
$fqbn = "esp32:esp32:esp32:PartitionScheme=app3M_fat9M_16MB,CPUFreq=240,FlashMode=qio,FlashFreq=80,FlashSize=16M,UploadSpeed=921600"

Write-Host "编译: $SketchDir"
& $cli compile --fqbn $fqbn $SketchDir
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

if ($CompileOnly) {
  Write-Host "仅编译，未烧录。"
  exit 0
}

Write-Host "烧录端口: $Port（按住 BOOT 再插 USB 若无法进入下载模式可再试）"
& $cli upload -p $Port --fqbn $fqbn $SketchDir
exit $LASTEXITCODE
