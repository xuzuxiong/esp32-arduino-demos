#ifndef WIFI_MQTT_H
#define WIFI_MQTT_H

#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiManager.h>

extern int DYA;
extern byte data[];

extern bool isWiFiConnected;
extern bool isMQTTConnected;

extern const char* mqttServer;
extern const int mqttPort;
extern const char* mqttUser;
extern const char* mqttPassword;

extern WiFiClient espClient;
extern PubSubClient mqttClient;

extern bool WIFI_OK;
extern String msgTopic;
extern String msgPayload;

void connectToWiFi();
void disconnectFromWiFi();
void connectToMQTT();
void disconnectFromMQTT();
void networkMaintain();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void publishDeviceStateSync();

#endif
