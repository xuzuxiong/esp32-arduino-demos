#include "wifi_mqtt.h"
#if __has_include("config.h")
#include "config.h"
#else
#include "config.example.h"
#endif
#include "esp32_id_utils.h"
#include <ArduinoJson.h>
#include <Preferences.h>
#include "menu.h"
#include "TimerManager.h"
#include "OTAUpgrade.h"

extern Preferences nvs;
extern OTAUpgrade otaUpgrade;

bool isWiFiConnected = false;
bool isMQTTConnected = false;
bool WIFI_OK = false;

const char* mqttServer = MQTT_SERVER;
const int mqttPort = MQTT_PORT;
const char* mqttUser = MQTT_USER;
const char* mqttPassword = MQTT_PASSWORD;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void publishDeviceStateSync();

static String normalizeAction(String action) {
  action.trim();
  if (action.startsWith("dd_")) action = action.substring(3);
  return action;
}

static void putU8(const char* nvsKey, uint8_t& ref, int value) {
  if (value < 0 || value > 255) return;
  ref = (uint8_t)value;
  nvs.putInt(nvsKey, ref);
}

static void putUInt(const char* nvsKey, unsigned int& ref, unsigned long value) {
  ref = (unsigned int)value;
  nvs.putInt(nvsKey, (int32_t)ref);
}

static void putULong(const char* nvsKey, unsigned long& ref, unsigned long value) {
  ref = value;
  nvs.putUInt(nvsKey, (uint32_t)ref);
}

static bool parseBoolValue(String valueStr, bool defaultValue = false) {
  valueStr.trim();
  valueStr.toLowerCase();
  if (valueStr == "1" || valueStr == "true" || valueStr == "on" || valueStr == "yes") return true;
  if (valueStr == "0" || valueStr == "false" || valueStr == "off" || valueStr == "no") return false;
  return defaultValue;
}

static int extractLongValues(const String& source, long* values, int maxCount) {
  int count = 0;
  bool inNumber = false;
  long current = 0;
  long sign = 1;

  for (unsigned int i = 0; i < source.length(); ++i) {
    const char c = source[i];
    if (c == '-' && !inNumber) {
      inNumber = true;
      current = 0;
      sign = -1;
      continue;
    }
    if (c >= '0' && c <= '9') {
      if (!inNumber) {
        inNumber = true;
        current = 0;
        sign = 1;
      }
      current = current * 10 + (c - '0');
      continue;
    }
    if (inNumber) {
      if (count < maxCount) values[count++] = current * sign;
      inNumber = false;
      current = 0;
      sign = 1;
    }
  }

  if (inNumber && count < maxCount) values[count++] = current * sign;
  return count;
}

static bool applyListOrIndexedU8(String valueStr,
                                 uint8_t* refs[],
                                 const char* const nvsKeys[],
                                 int count) {
  long values[16] = {0};
  const int valueCount = extractLongValues(valueStr, values, 16);
  if (valueStr.indexOf('{') >= 0 && valueCount >= 2) {
    const int index = (int)values[0];
    if (index >= 0 && index < count) {
      putU8(nvsKeys[index], *refs[index], (int)values[1]);
      return true;
    }
  }
  if (valueCount >= count) {
    for (int i = 0; i < count; ++i) {
      putU8(nvsKeys[i], *refs[i], (int)values[i]);
    }
    return true;
  }
  return false;
}

static bool applyListOrIndexedUInt(String valueStr,
                                   unsigned int* refs[],
                                   const char* const nvsKeys[],
                                   int count) {
  long values[16] = {0};
  const int valueCount = extractLongValues(valueStr, values, 16);
  if (valueStr.indexOf('{') >= 0 && valueCount >= 2) {
    const int index = (int)values[0];
    if (index >= 0 && index < count) {
      putUInt(nvsKeys[index], *refs[index], (unsigned long)values[1]);
      return true;
    }
  }
  if (valueCount >= count) {
    for (int i = 0; i < count; ++i) {
      putUInt(nvsKeys[i], *refs[i], (unsigned long)values[i]);
    }
    return true;
  }
  return false;
}

static bool applyOneControl(String action, String valueStr) {
  action = normalizeAction(action);
  valueStr.trim();
  const int iv = valueStr.length() ? valueStr.toInt() : 0;
  const unsigned long ulv = (unsigned long)valueStr.toInt();

  uint8_t* fishingKeys[] = { &DY1, &DY2, &DY3, &DY4, &DY5, &DY6 };
  const char* const fishingKeyNames[] = { "DY1A", "DY2A", "DY3A", "DY4A", "DY5A", "DY6A" };
  unsigned int* fishingTimes[] = { &variableValue_AJ1, &variableValue_AJ2, &variableValue_AJ3, &variableValue_AJ4, &variableValue_AJ5, &variableValue_AJ6 };
  const char* const fishingTimeNames[] = { "AJ1", "AJ2", "AJ3", "AJ4", "AJ5", "AJ6" };

  uint8_t* monsterKeys[] = { &JN1, &JN2, &JN3, &JN4, &JN5 };
  const char* const monsterKeyNames[] = { "JN1A", "JN2A", "JN3A", "JN4A", "JN5A" };
  unsigned int* monsterTimes[] = { &variableValue_JN1, &variableValue_JN2, &variableValue_JN3, &variableValue_JN4, &variableValue_JN5 };
  const char* const monsterTimeNames[] = { "JN1", "JN2", "JN3", "JN4", "JN5" };

  uint8_t* bufKeys[] = { &BUF1, &BUF2, &BUF3, &BUF4, &BUF5 };
  const char* const bufKeyNames[] = { "BUF1A", "BUF2A", "BUF3A", "BUF4A", "BUF5A" };
  unsigned int* bufTimes[] = { &variableValue_BF1, &variableValue_BF2, &variableValue_BF3, &variableValue_BF4, &variableValue_BF5 };
  const char* const bufTimeNames[] = { "BF1", "BF2", "BF3", "BF4", "BF5" };

  uint8_t* auraKeys[] = { &GH1, &GH2, &GH3 };
  const char* const auraKeyNames[] = { "GH1A", "GH2A", "GH3A" };
  unsigned int* auraTimes[] = { &variableValue_GH1, &variableValue_GH2, &variableValue_GH3 };
  const char* const auraTimeNames[] = { "GH1", "GH2", "GH3" };

  uint8_t* directionKeys[] = { &S, &X, &Z, &Y };
  const char* const directionKeyNames[] = { "SA", "XA", "ZA", "YA" };
  unsigned int* directionTimes[] = { &variableValue_S, &variableValue_X, &variableValue_Z, &variableValue_Y };
  const char* const directionTimeNames[] = { "S", "X", "Z", "Y" };

  if (action == "fishing_keys" || action == "key_selection") {
    return applyListOrIndexedU8(valueStr, fishingKeys, fishingKeyNames, 6);
  }
  if (action == "monster_keys") {
    return applyListOrIndexedU8(valueStr, monsterKeys, monsterKeyNames, 5);
  }
  if (action == "buf_keys") {
    return applyListOrIndexedU8(valueStr, bufKeys, bufKeyNames, 5);
  }
  if (action == "aura_keys") {
    return applyListOrIndexedU8(valueStr, auraKeys, auraKeyNames, 3);
  }
  if (action == "direction_keys") {
    return applyListOrIndexedU8(valueStr, directionKeys, directionKeyNames, 4);
  }

  if (action == "fishing_times") {
    return applyListOrIndexedUInt(valueStr, fishingTimes, fishingTimeNames, 6);
  }
  if (action == "monster_times") {
    return applyListOrIndexedUInt(valueStr, monsterTimes, monsterTimeNames, 5);
  }
  if (action == "buf_times") {
    return applyListOrIndexedUInt(valueStr, bufTimes, bufTimeNames, 5);
  }
  if (action == "aura_times") {
    return applyListOrIndexedUInt(valueStr, auraTimes, auraTimeNames, 3);
  }
  if (action == "direction_times") {
    return applyListOrIndexedUInt(valueStr, directionTimes, directionTimeNames, 4);
  }

  if (action == "DY1A") { putU8("DY1A", DY1, iv); return true; }
  if (action == "DY2A") { putU8("DY2A", DY2, iv); return true; }
  if (action == "DY3A") { putU8("DY3A", DY3, iv); return true; }
  if (action == "DY4A") { putU8("DY4A", DY4, iv); return true; }
  if (action == "DY5A") { putU8("DY5A", DY5, iv); return true; }
  if (action == "DY6A") { putU8("DY6A", DY6, iv); return true; }
  if (action == "GR1A") { putU8("GR1A", GR1, iv); return true; }
  if (action == "JN1A") { putU8("JN1A", JN1, iv); return true; }
  if (action == "JN2A") { putU8("JN2A", JN2, iv); return true; }
  if (action == "JN3A") { putU8("JN3A", JN3, iv); return true; }
  if (action == "JN4A") { putU8("JN4A", JN4, iv); return true; }
  if (action == "JN5A") { putU8("JN5A", JN5, iv); return true; }
  if (action == "BUF1A") { putU8("BUF1A", BUF1, iv); return true; }
  if (action == "BUF2A") { putU8("BUF2A", BUF2, iv); return true; }
  if (action == "BUF3A") { putU8("BUF3A", BUF3, iv); return true; }
  if (action == "BUF4A") { putU8("BUF4A", BUF4, iv); return true; }
  if (action == "BUF5A") { putU8("BUF5A", BUF5, iv); return true; }
  if (action == "GH1A") { putU8("GH1A", GH1, iv); return true; }
  if (action == "GH2A") { putU8("GH2A", GH2, iv); return true; }
  if (action == "GH3A") { putU8("GH3A", GH3, iv); return true; }
  if (action == "SA") { putU8("SA", S, iv); return true; }
  if (action == "XA") { putU8("XA", X, iv); return true; }
  if (action == "ZA") { putU8("ZA", Z, iv); return true; }
  if (action == "YA") { putU8("YA", Y, iv); return true; }
  if (action == "HCA") { putU8("HCA", HC, iv); return true; }
  if (action == "return_key") { putU8("HCA", HC, iv); return true; }
  if (action == "bait_key") { putU8("GR1A", GR1, iv); return true; }

  if (action == "AJ1") { putUInt("AJ1", variableValue_AJ1, ulv); return true; }
  if (action == "AJ2") { putUInt("AJ2", variableValue_AJ2, ulv); return true; }
  if (action == "AJ3") { putUInt("AJ3", variableValue_AJ3, ulv); return true; }
  if (action == "AJ4") { putUInt("AJ4", variableValue_AJ4, ulv); return true; }
  if (action == "AJ5") { putUInt("AJ5", variableValue_AJ5, ulv); return true; }
  if (action == "AJ6") { putUInt("AJ6", variableValue_AJ6, ulv); return true; }
  if (action == "JN1") { putUInt("JN1", variableValue_JN1, ulv); return true; }
  if (action == "JN2") { putUInt("JN2", variableValue_JN2, ulv); return true; }
  if (action == "JN3") { putUInt("JN3", variableValue_JN3, ulv); return true; }
  if (action == "JN4") { putUInt("JN4", variableValue_JN4, ulv); return true; }
  if (action == "JN5") { putUInt("JN5", variableValue_JN5, ulv); return true; }
  if (action == "BF1") { putUInt("BF1", variableValue_BF1, ulv); return true; }
  if (action == "BF2") { putUInt("BF2", variableValue_BF2, ulv); return true; }
  if (action == "BF3") { putUInt("BF3", variableValue_BF3, ulv); return true; }
  if (action == "BF4") { putUInt("BF4", variableValue_BF4, ulv); return true; }
  if (action == "BF5") { putUInt("BF5", variableValue_BF5, ulv); return true; }
  if (action == "GH1") { putUInt("GH1", variableValue_GH1, ulv); return true; }
  if (action == "GH2") { putUInt("GH2", variableValue_GH2, ulv); return true; }
  if (action == "GH3") { putUInt("GH3", variableValue_GH3, ulv); return true; }
  if (action == "S") { putUInt("S", variableValue_S, ulv); return true; }
  if (action == "X") { putUInt("X", variableValue_X, ulv); return true; }
  if (action == "Z") { putUInt("Z", variableValue_Z, ulv); return true; }
  if (action == "Y") { putUInt("Y", variableValue_Y, ulv); return true; }
  if (action == "HC" || action == "variableValue_HC" || action == "return_interval_min") {
    putUInt("HC", variableValue_HC, ulv);
    return true;
  }
  if (action == "AX" || action == "all_keys_press_time") {
    putUInt("AX", variableValue_AX, ulv);
    return true;
  }
  if (action == "return_press_time_ms") {
    putULong("HC_A", variableValue_HC_A, ulv);
    return true;
  }
  if (action == "MS") {
    variableValue_MS = (iv != 0);
    nvs.putBool("MS", variableValue_MS);
    return true;
  }
  if (action == "DGFS") {
    variableValue_DGFS = (iv != 0);
    nvs.putBool("DGFS", variableValue_DGFS);
    return true;
  }

  if (action == "timer" || action == "fishing_a_time" || action == "monster_b_time" || action == "ear_time") {
    variableValue_HC_B = ulv * 1000UL;
    variableValue_HC_D = millis();
    nvs.putUInt("HC_B", (uint32_t)variableValue_HC_B);
    nvs.putUInt("HC_D", (uint32_t)variableValue_HC_D);
    return true;
  }

  if (action == "is_running") {
    isPaused = !parseBoolValue(valueStr, true);
    nvs.putBool("isPaused", isPaused);
    return true;
  }

  if (action == "fish_monster_mode") {
    Menu = parseBoolValue(valueStr, true) ? 0 : 1;
    nvs.putChar("MU", (char)Menu);
    return true;
  }

  if (action == "upgrade") {
    otaUpgrade.upgradeFromURL("http://dy.xuzuxiong.com/upgrade.bin");
    return true;
  }

  if (action == "mode") {
    String lowerValue = valueStr;
    lowerValue.toLowerCase();
    if (lowerValue == "auto") {
      variableValue_MS = true;
      nvs.putBool("MS", variableValue_MS);
    } else if (lowerValue == "manual") {
      variableValue_MS = false;
      nvs.putBool("MS", variableValue_MS);
    } else if (valueStr.indexOf("打怪") >= 0) {
      Menu = 1;
      nvs.putChar("MU", (char)Menu);
    } else if (valueStr.indexOf("钓鱼") >= 0) {
      Menu = 0;
      nvs.putChar("MU", (char)Menu);
    } else if (valueStr.indexOf("传奇") >= 0 || lowerValue.indexOf("legend") >= 0) {
      Menu = 4;
      nvs.putChar("MU", (char)Menu);
    }
    return true;
  }

  return false;
}

static void applySimpleOrWhole(const String& message) {
  String msg = message;
  msg.trim();
  if (msg.length() == 0) return;

  if (msg == "pause") {
    isPaused = true;
    nvs.putBool("isPaused", true);
    return;
  }
  if (msg == "resume") {
    isPaused = false;
    nvs.putBool("isPaused", false);
    return;
  }
  if (msg == "upgrade") {
    otaUpgrade.upgradeFromURL("http://dy.xuzuxiong.com/upgrade.bin");
    return;
  }

  int slash = msg.indexOf('/');
  if (slash > 0) {
    String action = msg.substring(0, slash);
    String value = msg.substring(slash + 1);
    applyOneControl(action, value);
  }
}

void connectToWiFi() {
  if (isWiFiConnected) return;

  WiFiManager wifiManager;
  if (wifiManager.autoConnect()) {
    Serial.println("WiFi connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    isWiFiConnected = true;
    WIFI_OK = true;
    return;
  }

  if (wifiManager.startConfigPortal(WIFI_PORTAL_SSID)) {
    while (!WiFi.isConnected()) {
      delay(500);
    }
    Serial.println("WiFi configured");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    isWiFiConnected = true;
    WIFI_OK = true;
    return;
  }

  Serial.println("WiFi config failed");
  WIFI_OK = false;
}

void disconnectFromWiFi() {
  if (!isWiFiConnected) return;
  WiFi.disconnect();
  isWiFiConnected = false;
  Serial.println("WiFi disconnected");
}

void connectToMQTT() {
  if (!isWiFiConnected) return;

  mqttClient.setBufferSize(4096);
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setKeepAlive(60);
  mqttClient.setSocketTimeout(15);

  int retryCount = 0;
  while (!mqttClient.connected() && retryCount < 3) {
    Serial.println("Connecting to MQTT...");
    if (mqttClient.connect(("DECQ:" + deviceID).c_str(),
                           mqttUser,
                           mqttPassword,
                           ("devices/" + deviceID + "/status").c_str(),
                           0,
                           false,
                           "offline")) {
      mqttClient.subscribe(("devices/" + deviceID + "/control").c_str());
      isMQTTConnected = true;
      publishDeviceStateSync();
      Serial.println("MQTT connected");
      return;
    }
    ++retryCount;
    delay(3000);
  }

  isMQTTConnected = false;
  Serial.println("MQTT connect failed");
}

void disconnectFromMQTT() {
  if (!isMQTTConnected) return;
  mqttClient.disconnect();
  isMQTTConnected = false;
  Serial.println("MQTT disconnected");
}

void publishDeviceStateSync() {
  if (!mqttClient.connected()) return;

  mqttClient.setBufferSize(4096);

  StaticJsonDocument<2048> doc;
  doc["online"] = true;
  doc["running"] = !isPaused;
  doc["is_running"] = !isPaused;
  doc["menu"] = (int)(unsigned char)Menu;
  doc["menu_name"] = (Menu == 0) ? "fish" : (Menu == 1) ? "monster" : (Menu == 4) ? "legend" : "other";
  doc["mode"] = variableValue_MS ? "auto" : "manual";
  doc["fight_mode"] = variableValue_MS ? "auto" : "manual";
  doc["dd_fish_monster_mode"] = (Menu == 0);
  doc["timer_enabled"] = enabled;
  doc["dd_all_keys_press_time"] = variableValue_AX;
  doc["dd_bait_key"] = GR1;
  doc["dd_return_key"] = HC;
  doc["dd_return_press_time_ms"] = variableValue_HC_A ? variableValue_HC_A : 50;
  doc["dd_return_interval_min"] = variableValue_HC;

  const unsigned long sec = millis() / 1000UL;
  doc["uptime_sec"] = sec;
  doc["runtime_sec"] = sec;

  unsigned long remSec = 0;
  if (enabled && Menu == 0 && variableValue_HC_B > 0 && variableValue_HC_D > 0) {
    const unsigned long elapsed = millis() - variableValue_HC_D;
    if (elapsed < variableValue_HC_B) remSec = (variableValue_HC_B - elapsed) / 1000UL;
  }
  doc["return"] = remSec;
  doc["timer_remaining_sec"] = remSec;

  JsonArray fishingTimes = doc.createNestedArray("dd_fishing_times");
  fishingTimes.add(variableValue_AJ1); fishingTimes.add(variableValue_AJ2); fishingTimes.add(variableValue_AJ3);
  fishingTimes.add(variableValue_AJ4); fishingTimes.add(variableValue_AJ5); fishingTimes.add(variableValue_AJ6);
  JsonArray monsterTimes = doc.createNestedArray("dd_monster_times");
  monsterTimes.add(variableValue_JN1); monsterTimes.add(variableValue_JN2); monsterTimes.add(variableValue_JN3);
  monsterTimes.add(variableValue_JN4); monsterTimes.add(variableValue_JN5);
  JsonArray bufTimes = doc.createNestedArray("dd_buf_times");
  bufTimes.add(variableValue_BF1); bufTimes.add(variableValue_BF2); bufTimes.add(variableValue_BF3);
  bufTimes.add(variableValue_BF4); bufTimes.add(variableValue_BF5);
  JsonArray auraTimes = doc.createNestedArray("dd_aura_times");
  auraTimes.add(variableValue_GH1); auraTimes.add(variableValue_GH2); auraTimes.add(variableValue_GH3);
  JsonArray directionTimes = doc.createNestedArray("dd_direction_times");
  directionTimes.add(variableValue_S); directionTimes.add(variableValue_X); directionTimes.add(variableValue_Z); directionTimes.add(variableValue_Y);

  JsonArray fishingKeyList = doc.createNestedArray("dd_fishing_keys");
  fishingKeyList.add(DY1); fishingKeyList.add(DY2); fishingKeyList.add(DY3); fishingKeyList.add(DY4); fishingKeyList.add(DY5); fishingKeyList.add(DY6);
  JsonArray monsterKeyList = doc.createNestedArray("dd_monster_keys");
  monsterKeyList.add(JN1); monsterKeyList.add(JN2); monsterKeyList.add(JN3); monsterKeyList.add(JN4); monsterKeyList.add(JN5);
  JsonArray bufKeyList = doc.createNestedArray("dd_buf_keys");
  bufKeyList.add(BUF1); bufKeyList.add(BUF2); bufKeyList.add(BUF3); bufKeyList.add(BUF4); bufKeyList.add(BUF5);
  JsonArray auraKeyList = doc.createNestedArray("dd_aura_keys");
  auraKeyList.add(GH1); auraKeyList.add(GH2); auraKeyList.add(GH3);
  JsonArray directionKeyList = doc.createNestedArray("dd_direction_keys");
  directionKeyList.add(S); directionKeyList.add(X); directionKeyList.add(Z); directionKeyList.add(Y);

  char payload[2048];
  const size_t len = serializeJson(doc, payload, sizeof(payload));
  if (len == 0 || len >= sizeof(payload)) return;

  const String topic = String("devices/") + deviceID + "/status";
  mqttClient.publish(topic.c_str(), payload, false);
  mqttClient.publish(topic.c_str(), !isPaused ? "running" : "paused", false);
  mqttClient.publish(topic.c_str(), (String("mode/") + (variableValue_MS ? "auto" : "manual")).c_str(), false);

  Serial.print("[MQTT] sync ");
  Serial.println(payload);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  message.reserve(length + 8);
  for (unsigned int i = 0; i < length; ++i) {
    message += (char)payload[i];
  }
  message.trim();

  const String topicStr = String("devices/") + deviceID + "/control";
  if (String(topic) != topicStr) {
    publishDeviceStateSync();
    return;
  }

  Serial.print("[MQTT] control ");
  Serial.println(message);

  if (message.startsWith("{")) {
    StaticJsonDocument<2048> doc;
    DeserializationError err = deserializeJson(doc, message);
    if (!err) {
      JsonObject root = doc.as<JsonObject>();
      for (JsonPair kv : root) {
        String key = String(kv.key().c_str());
        key.trim();
        String value;
        if (kv.value().is<JsonObject>() || kv.value().is<JsonArray>()) {
          serializeJson(kv.value(), value);
        } else if (kv.value().is<bool>()) {
          value = kv.value().as<bool>() ? "1" : "0";
        } else if (kv.value().is<int>() || kv.value().is<long>()) {
          value = String(kv.value().as<long>());
        } else if (kv.value().is<const char*>()) {
          value = String(kv.value().as<const char*>());
        } else {
          value = kv.value().as<String>();
        }
        applyOneControl(key, value);
      }
    }
  } else {
    applySimpleOrWhole(message);
  }

  publishDeviceStateSync();
}

int DYA = 0;
byte data[64] = {0};
String msgTopic;
String msgPayload;

void networkMaintain() {
  if (WiFi.status() != WL_CONNECTED) {
    isWiFiConnected = false;
    isMQTTConnected = false;
    return;
  }

  isWiFiConnected = true;
  if (!mqttClient.connected()) {
    isMQTTConnected = false;
  }
  if (!isMQTTConnected) {
    connectToMQTT();
  }
  if (mqttClient.connected()) {
    isMQTTConnected = true;
    mqttClient.loop();
  }
}
