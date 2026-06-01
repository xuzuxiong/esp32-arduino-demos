
#ifndef OTAUpgrade_h
#define OTAUpgrade_h

#include <ArduinoOTA.h>
#include <HTTPClient.h>

class OTAUpgrade {
public:
  // 构造函数和析构函数
  OTAUpgrade();
  ~OTAUpgrade();

  // 初始化 OTA 升级
  void begin();
  // 处理 OTA 更新
  void handle();
  // 从 URL 升级固件
  void upgradeFromURL(const String& firmwareURL);
  // 获取固件 URL
  String getFirmwareURL();

private:
  String firmwareURL;  // 固件的 URL 地址
};
extern OTAUpgrade otaUpgrade;  // 声明外部变量
#endif