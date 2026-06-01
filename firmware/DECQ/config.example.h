#ifndef DECQ_CONFIG_H
#define DECQ_CONFIG_H

// Copy this file to config.h and fill in your own values:
//   cp config.example.h config.h
//
// config.h is listed in .gitignore and will not be committed.

// MQTT broker (optional — only used when network mode is enabled in firmware)
#define MQTT_SERVER "192.168.1.100"
#define MQTT_PORT 1883
#define MQTT_USER "YOUR_MQTT_USERNAME"
#define MQTT_PASSWORD "YOUR_MQTT_PASSWORD"

// WiFiManager captive portal AP name (device creates this SSID when WiFi is not configured)
#define WIFI_PORTAL_SSID "DECQ-Setup"

// OTA firmware download URL placeholder (override via MQTT control command)
#define OTA_FIRMWARE_URL "http://192.168.1.100/firmware/decq.bin"

#endif
