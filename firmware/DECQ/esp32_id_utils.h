#ifndef ESP32_ID_UTILS_H
#define ESP32_ID_UTILS_H

#include <Arduino.h>
#include <WiFi.h>

extern uint64_t chipId;
extern String deviceID;

void getESP32MacAddress(char* macStr, size_t bufferSize);
String generateUniqueDeviceID();
uint64_t getESP32ChipId();

#endif
