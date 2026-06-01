#include "esp32_id_utils.h"

#include <CRC32.h>

uint64_t chipId = 0;
String deviceID;

void getESP32MacAddress(char* macStr, size_t bufferSize) {
  uint8_t mac[6];
  WiFi.macAddress(mac);
  snprintf(
      macStr,
      bufferSize,
      "%02X:%02X:%02X:%02X:%02X:%02X",
      mac[0],
      mac[1],
      mac[2],
      mac[3],
      mac[4],
      mac[5]);
}

uint64_t getESP32ChipId() {
  return ESP.getEfuseMac();
}

String generateUniqueDeviceID() {
  const uint64_t localChipId = ESP.getEfuseMac();

  CRC32 crc;
  crc.update(reinterpret_cast<const uint8_t*>(&localChipId), sizeof(localChipId));
  const uint32_t hashValue = crc.finalize();

  String hexHash = String(hashValue, HEX);
  hexHash.toUpperCase();
  while (hexHash.length() < 8) {
    hexHash = "0" + hexHash;
  }

  return "CQ" + hexHash.substring(0, 6);
}
