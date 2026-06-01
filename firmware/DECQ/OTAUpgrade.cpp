#include "OTAUpgrade.h"
#if __has_include("config.h")
#include "config.h"
#else
#include "config.example.h"
#endif

#include <HTTPClient.h>
#include <Update.h>
#include <WiFi.h>

#include "oled_display.h"

OTAUpgrade otaUpgrade;

OTAUpgrade::OTAUpgrade() {
  firmwareURL = OTA_FIRMWARE_URL;
}

OTAUpgrade::~OTAUpgrade() {}

void OTAUpgrade::begin() {
  ArduinoOTA.begin();
}

void OTAUpgrade::handle() {
  ArduinoOTA.handle();
}

void OTAUpgrade::upgradeFromURL(const String& url) {
  firmwareURL = url;
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    return;
  }

  HTTPClient http;
  http.begin(firmwareURL);
  http.setReuse(false);

  const int httpCode = http.GET();
  if (httpCode != HTTP_CODE_OK) {
    Serial.printf("HTTP request failed: %d\n", httpCode);
    http.end();
    return;
  }

  const int firmwareSize = http.getSize();
  if (firmwareSize <= 0) {
    Serial.println("Invalid firmware size");
    http.end();
    return;
  }

  if (firmwareSize > static_cast<int>(ESP.getFreeSketchSpace())) {
    Serial.println("Firmware too large for free sketch space");
    http.end();
    return;
  }

  if (!Update.begin(firmwareSize)) {
    Serial.println("Update.begin failed");
    http.end();
    return;
  }

  WiFiClient* stream = http.getStreamPtr();
  size_t written = 0;
  unsigned long lastUiUpdate = 0;
  const unsigned long uiInterval = 200;
  uint8_t buffer[512];

  while (http.connected() && written < static_cast<size_t>(firmwareSize)) {
    const size_t available = stream->available();
    if (available == 0) {
      delay(1);
      continue;
    }

    const size_t toRead = available < sizeof(buffer) ? available : sizeof(buffer);
    const int bytesRead = stream->readBytes(buffer, toRead);
    if (bytesRead <= 0) {
      continue;
    }

    const size_t chunkWritten = Update.write(buffer, static_cast<size_t>(bytesRead));
    written += chunkWritten;

    if (millis() - lastUiUpdate >= uiInterval || written >= static_cast<size_t>(firmwareSize)) {
      OLED_ShowUpdateProgress(written, static_cast<size_t>(firmwareSize));
      lastUiUpdate = millis();
    }
  }

  http.end();

  if (!Update.end() || !Update.isFinished()) {
    Serial.println("Firmware update failed");
    return;
  }

  OLED_ShowChineseTitle("固件升级成功", 26);
  delay(500);
  OLED_ShowChineseTitle("设备即将重启", 26);
  delay(500);
  ESP.restart();
}

String OTAUpgrade::getFirmwareURL() {
  return firmwareURL;
}
