#include "fish_sound_detection.h"
#include "scan_controller.h"
#include "oled_display.h"
#include "keypad.h"
#include "menu.h"
#include "wifi_mqtt.h"
#include "esp32_id_utils.h"
#include <ArduinoOTA.h>
#include "OTAUpgrade.h"
#include "TimerManager.h"
#include <esp_system.h>
#include <Preferences.h>
#include <Wire.h>
#include <stdlib.h>

Keypad myKeypad(buttonPins, 6);
Preferences nvs;

unsigned long lastPublishTime = 0;
const unsigned long publishInterval = 30000;
int WLZT_A = 0;

static void loadRuntimeSettings() {
  isPaused = nvs.getBool("isPaused", false);
  variableValue_MS = nvs.getBool("MS", true);
  variableValue_DGFS = nvs.getBool("DGFS", true);
  variableValue_WLZT = nvs.getBool("WLZT", false);
  MUT_YL = nvs.getBool("MUT_YL", true);
  Menu = nvs.getChar("MU", 0);
  enabled = nvs.getBool("enabled", true);

  variableValue_AX = nvs.getInt("AX", 100);

  variableValue_AJ1 = nvs.getInt("AJ1", 1);
  variableValue_AJ2 = nvs.getInt("AJ2", 90);
  variableValue_AJ3 = nvs.getInt("AJ3", 210);
  variableValue_AJ4 = nvs.getInt("AJ4", 270);
  variableValue_AJ5 = nvs.getInt("AJ5", 240);
  variableValue_AJ6 = nvs.getInt("AJ6", 850);

  variableValue_JN1 = nvs.getInt("JN1", 1);
  variableValue_JN2 = nvs.getInt("JN2", 2);
  variableValue_JN3 = nvs.getInt("JN3", 3);
  variableValue_JN4 = nvs.getInt("JN4", 15);
  variableValue_JN5 = nvs.getInt("JN5", 10);

  variableValue_BF1 = nvs.getInt("BF1", 20);
  variableValue_BF2 = nvs.getInt("BF2", 31);
  variableValue_BF3 = nvs.getInt("BF3", 1800);
  variableValue_BF4 = nvs.getInt("BF4", 3600);
  variableValue_BF5 = nvs.getInt("BF5", 1);

  variableValue_GH1 = nvs.getInt("GH1", 99999);
  variableValue_GH2 = nvs.getInt("GH2", 99999);
  variableValue_GH3 = nvs.getInt("GH3", 99999);

  variableValue_S = nvs.getInt("S", 99999);
  variableValue_X = nvs.getInt("X", 99999);
  variableValue_Z = nvs.getInt("Z", 99999);
  variableValue_Y = nvs.getInt("Y", 99999);

  variableValue_HC = nvs.getInt("HC", 240);
  variableValue_HC_A = nvs.getUInt("HC_A", 50);
  variableValue_HC_B = nvs.getUInt("HC_B", 0);
  variableValue_HC_D = nvs.getUInt("HC_D", 0);

  DY1 = nvs.getInt("DY1A", 26);
  DY2 = nvs.getInt("DY2A", 27);
  DY3 = nvs.getInt("DY3A", 28);
  DY4 = nvs.getInt("DY4A", 29);
  DY5 = nvs.getInt("DY5A", 30);
  DY6 = nvs.getInt("DY6A", 31);
  GR1 = nvs.getInt("GR1A", 33);

  JN1 = nvs.getInt("JN1A", 26);
  JN2 = nvs.getInt("JN2A", 27);
  JN3 = nvs.getInt("JN3A", 28);
  JN4 = nvs.getInt("JN4A", 29);
  JN5 = nvs.getInt("JN5A", 30);

  BUF1 = nvs.getInt("BUF1A", 31);
  BUF2 = nvs.getInt("BUF2A", 32);
  BUF3 = nvs.getInt("BUF3A", 33);
  BUF4 = nvs.getInt("BUF4A", 34);
  BUF5 = nvs.getInt("BUF5A", 35);

  GH1 = nvs.getInt("GH1A", 41);
  GH2 = nvs.getInt("GH2A", 42);
  GH3 = nvs.getInt("GH3A", 43);

  S = nvs.getInt("SA", 77);
  X = nvs.getInt("XA", 76);
  Z = nvs.getInt("ZA", 75);
  Y = nvs.getInt("YA", 74);
  HC = nvs.getInt("HCA", 50);
}

static void initPins() {
  pinMode(MUT_OUT, OUTPUT);
  pinMode(MUT_UP, OUTPUT);
  pinMode(MUT_DN, OUTPUT);
  pinMode(Audio, OUTPUT);
  pinMode(Wifi_AA, OUTPUT);

  digitalWrite(Audio, HIGH);
  digitalWrite(Wifi_AA, HIGH);
  digitalWrite(MUT_OUT, HIGH);
  digitalWrite(MUT_UP, HIGH);
  digitalWrite(MUT_DN, HIGH);
}

void setup() {
  Serial.begin(115200);
  initSerial();
  setupI2S();
  initPins();

  randomSeed((uint32_t)esp_random());
  memset(vImag, 0, sizeof(vImag));
  Wire.begin(SDA_PIN, SCL_PIN);

  OLED_Init();
  OLED_Clear();
  OLED_ShowFishingAnimation();

  deviceID = generateUniqueDeviceID();
  Serial.print("Device ID: ");
  Serial.println(deviceID);

  if (!nvs.begin("testNamespace", false)) {
    Serial.println("NVS init failed");
    return;
  }

  loadRuntimeSettings();
  SUJI();

  if (variableValue_WLZT) {
    mqttClient.setCallback(mqttCallback);
    connectToWiFi();
    connectToMQTT();
    otaUpgrade.begin();
  }

  digitalWrite(MUT_OUT, MUT_YL ? HIGH : LOW);
}

void loop() {
  currentMillis = millis();

  detectFishSound();
  myKeypad.update();
  Menu_begin();

  digitalWrite(MUT_OUT, MUT_YL ? HIGH : LOW);

  if (variableValue_WLZT) {
    networkMaintain();

    if (mqttClient.connected() &&
        millis() - lastPublishTime >= (publishInterval + random(0, 5000))) {
      lastPublishTime = millis();
      mqttClient.publish((String("devices/") + deviceID + "/status").c_str(), "online");
      publishDeviceStateSync();
    }

    if (WiFi.status() == WL_CONNECTED) {
      digitalWrite(Wifi_AA, LOW);
    } else {
      digitalWrite(Wifi_AA, HIGH);
    }

    ArduinoOTA.handle();
  } else {
    digitalWrite(Wifi_AA, HIGH);
  }

  SUJI();
  DIAOYU();
  DAGUAI();

  myKeypad.update();
  digitalWrite(Audio, HIGH);
}
