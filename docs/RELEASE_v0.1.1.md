# Release v0.1.1 — OSS readiness improvements

**Date:** 2026-06-01  
**Tag:** `v0.1.1`

## Summary

Security sanitization, CI compile checks, hardware documentation, and minimal examples to improve open-source and Codex for OSS application readiness.

## Fixed

- Removed hardcoded personal OTA domain; use `OTA_FIRMWARE_URL` from config
- MQTT `upgrade` supports optional HTTP URL in control value

## Added

- GitHub Actions workflow: compile DECQ + `examples/ch9329_mouse` + `examples/i2s_energy_detect`
- `firmware/DECQ/libraries.txt`
- `hardware/BOM.md`, `hardware/wiring-schematic.md`, `hardware/block-diagram.md`
- Minimal Arduino examples under `examples/`
- Updated `OPENAI_CODEX_OSS_APPLICATION_NOTES.md` with form-ready text

## Upgrade

```powershell
git pull
cp firmware/DECQ/config.example.h firmware/DECQ/config.h   # if not done yet
```

See [CHANGELOG.md](CHANGELOG.md) for full history.
