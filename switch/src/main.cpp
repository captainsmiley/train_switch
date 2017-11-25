#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include "tgesp.h"
#include <TickerScheduler.h>
#define TG_VERSION 6

#define STA_WIFI_ATTEMPT 1

tgesp esp;

TickerScheduler ts(4);

unsigned long int t_g;

const char* ssid = "TN_24GHz_C587A5";
const char* password = "68E84C8ED7";

int loop_counter = 0;

IPAddress local_IP(192,168,5,22);
IPAddress gateway(192,168,5,9);
IPAddress subnet(255,255,255,0);

// Controller wifi
static char ssid_c[] = "ESPWiFi_TrainController";
static char password_c[] = "connectme";

// AP wifi
static char ssid_ap[] = "ESPWiFi_TrainSwitch";
static char password_ap[] = "connectme";
// Hölö wifi
static char ssid_st_h[] = "TeliaGateway9C-97-26-49-11-55";
static char password_st_h[] = "179A1021DD";

// Södertälje wifi
static char ssid_st_s[] = "TP-LINK_28D578";
static char password_st_s[] = "50044801";

// hack wifi
const char* ssid_x = "TN_24GHz_C587A5";
const char* password_x = "68E84C8ED7";

char msg = '\0';
uint8_t pos = 0;


void listen_for_clients()
{
	esp.listen_for_clients();
}


void check_OTA()
{
  ArduinoOTA.handle();
}

void print_info()
{
  t_g++;
  Serial.print("TSV2 ");Serial.print(t_g);Serial.print(" IP: ");
  Serial.println(WiFi.localIP());
}

void app()
{
}

void setup()
{
  Serial.begin(115200);
  WiFi.hostname("train_switch");
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(local_IP, gateway, subnet);
	String ssid_temp = ssid_ap + String(esp.cmds.get_id());

  WiFi.softAP(ssid_temp.c_str(),password_ap);
  delay(1000);


	WiFi.begin(ssid_c, password_c);
	uint8_t attempt = 0;
	while (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println("Connection to STA Failed!");
		attempt++;
		delay(5000);
		if (attempt >= STA_WIFI_ATTEMPT)
		{
			WiFi.mode(WIFI_AP);
			break;
			//WiFi.softAP(ssid_ap,password_ap);
		}

	}


  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  esp.setup();
	ts.add(3,60,listen_for_clients);
  ts.add(0, 200, check_OTA);
  ts.add(1,1000,print_info);
  ts.add(2,100,app);

}

void print_wl_status()
{
  switch(WiFi.status())
  {
    case WL_CONNECTED:
    Serial.println("wl_connected");
    break;
    default:
    Serial.println("Can't handle status");
    break;
  }
}

void loop()
{
  ts.update();
}
