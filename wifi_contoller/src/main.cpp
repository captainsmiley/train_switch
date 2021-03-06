#include <Arduino.h>
#include <ESP8266WiFi.h>

#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "OTA.h"

#include <TickerScheduler.h>

#define TG_VERSION 7

#define STA_WIFI_ATTEMPT 1

TickerScheduler ts(4);

unsigned long int t_g;

const char* ssid = "TN_24GHz_C587A5";
const char* password = "68E84C8ED7";

int loop_counter = 0;

IPAddress local_IP(192,168,7,22);
IPAddress gateway(192,168,7,22);
IPAddress subnet(255,255,255,0);

OTA ota;
// AP wifi
static char ssid_ap[] = "ESPWiFi_TrainController";
static char password_ap[] = "connectme";
// Hölö wifi
static char ssid_st_h[] = "TeliaGateway9C-97-26-49-11-55";
static char password_st_h[] = "179A1021DD";

// Södertälje wifi
static char ssid_st_s[] = "TP-LINK_D578";
static char password_st_s[] = "50044801";

// hack wifi
const char* ssid_x = "TN_24GHz_C587A5";
const char* password_x = "68E84C8ED7";

String msg = "";

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

IPAddress baned_clients[10] = {};
uint8_t banned_count = 0;

bool in_banned_list(IPAddress & a)
{
  for (int i=0;i<10;i++)
  {
    if(a == baned_clients[i])
    {
      return true;
    }
  }
  return false;
}

void send_info_to_clients()
{
  if(msg == "")
  {
    return;
  }
  Serial.println("Starting to send info to clinets");
  stat_info = wifi_softap_get_station_info();
  while (stat_info != NULL)
  {
    IPaddress = &stat_info->ip;
    address = IPaddress->addr;

    client.setNoDelay(true);
    client.setTimeout(110);
    Serial.print("Send to: ");Serial.println(address);
    if (!in_banned_list(address))
    {
    if (client.connect(address, 80)) {

      Serial.print("connected to ");Serial.println(address);
      // Make a HTTP request:
      client.println("GET /controll?"+String(msg)+" HTTP/1.0");
      client.println();
      client.flush();
    }
    else
    {
      Serial.print("Faild to send info to ");Serial.println(address);
  //    baned_clients[banned_count] = address;
   //   banned_count++;
    //  if (banned_count >= 10) banned_count = 0;
    }
  }else
  {
    Serial.println("Ip banned");
  }
    client.stop();
    stat_info = STAILQ_NEXT(stat_info, next);
  }

}

void check_OTA()
{
  ota.update();
}

void print_info()
{
  WiFiMode_t wm = WiFi.getMode();
  t_g++;
  Serial.print("R ");Serial.print(t_g);
  Serial.print(" M:");Serial.print(wm);
  Serial.print(" IP: ");
  if (wm==WIFI_AP) Serial.print(WiFi.softAPIP());
  else Serial.print(WiFi.localIP());
  Serial.print(" v:");Serial.println(TG_VERSION);
}

void app()
{
  if (Serial.available() > 0) {
    char c = Serial.read();
    if(c=='#')
    {
       Serial.println(msg);
       send_info_to_clients();
       msg = "";
    }
    else
    {
      msg += c;
    }
  }
}


void setup()
{
  Serial.begin(115200);
  Serial.println("Train switcher wifi controller setup");

  //WiFi.mode(WIFI_AP_STA);
  WiFi.mode(WIFI_AP);
  WiFi.hostname("train_switcher");
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid_ap,password_ap);
  delay(1000);
  //WiFi.begin(ssid_st_s, password_st_s);

//  WiFi.begin(ssid_x, password_x);
  uint8_t attempt = 0;
  //while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    //Serial.println("Connection to STA Failed!");
    //attempt++;
    //delay(5000);
    //if (attempt >= STA_WIFI_ATTEMPT)
    //{
    //  WiFi.mode(WIFI_AP);
     // break;
      //WiFi.softAP(ssid_ap,password_ap);
    //}
    //ESP.restart();
  //}

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ota.setup();
  delay(1000);
  ts.add(0, 100, check_OTA);
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
