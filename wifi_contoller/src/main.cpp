#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <TickerScheduler.h>


TickerScheduler ts(4);

unsigned long int t_g;

const char* ssid = "TN_24GHz_C587A5";
const char* password = "68E84C8ED7";

int loop_counter = 0;

IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

// AP wifi
static char ssid_ap[] = "ESPWiFi_TrainController";
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


extern "C" {
#include<user_interface.h>
}
struct station_info *stat_info;
struct ip_addr *IPaddress;
IPAddress address;
void find_clients()
{
  Serial.print(" Clients:\r\n");
  stat_info = wifi_softap_get_station_info();

  while (stat_info != NULL)
  {
    IPaddress = &stat_info->ip;
    address = IPaddress->addr;
    Serial.print("\t");
    Serial.print(address);
    Serial.print("\r\n");
    stat_info = STAILQ_NEXT(stat_info, next);
  }
}
// Initialize the client library
WiFiClient client;
void send_info_to_clients()
{
  Serial.println("Starting to send info to clinets");
  stat_info = wifi_softap_get_station_info();
  while (stat_info != NULL)
  {
    IPaddress = &stat_info->ip;
    address = IPaddress->addr;
    if (client.connect(address, 80)) {
      Serial.print("connected to ");Serial.println(address);
      // Make a HTTP request:
      client.println("GET /controll?"+String(msg)+" HTTP/1.0");
      client.println();
    }
    else Serial.println("Faild to send info to " +String(address)  );
    client.stop();
    stat_info = STAILQ_NEXT(stat_info, next);
  }

}

void check_OTA()
{
  ArduinoOTA.handle();
}

void print_info()
{
  t_g++;
  Serial.print("R ");Serial.print(t_g);Serial.print(" IP: ");
  Serial.println(WiFi.localIP());
}

void app()
{
  if (Serial.available() > 0) {
    char c = Serial.read();
    if(c=='#' && msg != '\0')
    {
       Serial.println(msg);
       send_info_to_clients();
    }
    else msg = c;
  }
}

void setup()
{
  Serial.begin(115200);
  WiFi.hostname("train_switcher");
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid_ap,password_ap);
  delay(1000);
  //WiFi.begin(ssid_st_s, password_st_s);

  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid_x, password_x);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
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
  ts.add(0, 200, check_OTA);
  ts.add(1,1000,print_info);
  ts.add(2,100,app);
  //ts.add(3,5000,find_clients);

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