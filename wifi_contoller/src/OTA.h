#ifndef APP_OTA_TTG
#define APP_OTA_TTG
#include <ESP8266WiFi.h>
#include <ESP8266WiFi.h>

#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#define OTA_UPDATE_RATE_MS 100

class OTA
{
public:
  OTA();
  virtual ~OTA();

  const static uint8_t update_rate = OTA_UPDATE_RATE_MS;
  void update();
  void setup();


};


#endif
