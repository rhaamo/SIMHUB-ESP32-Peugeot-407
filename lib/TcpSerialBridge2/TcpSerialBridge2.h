#pragma once

#include <Arduino.h>
#if USE_HARDCODED_CREDENTIALS
#else
// captive portal ip is always 192.168.4.1
#include <ESPAsyncWebServer.h>
#endif
#include <FullLoopbackStream.h>


#if DEBUG_BRIDGE
// all the logs, more memory usage
#define _ESPASYNC_WIFIMGR_LOGLEVEL_    4
#else
// no logs, less memory usage
#define _ESPASYNC_WIFIMGR_LOGLEVEL_    0
#endif


#ifdef ESP32
  #include <esp_wifi.h>
  #include <WiFi.h>
  #include <WiFiClient.h>
  #include <AsyncTCP.h>
  #include <WiFiMulti.h>
  WiFiMulti wifiMulti;


#else

  #include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
  //needed for library
  #include <ESPAsyncDNSServer.h>
  #include <EspAsyncTCP.h>

  // From v1.1.1
  #include <ESP8266WiFiMulti.h>
  ESP8266WiFiMulti wifiMulti;
  #define ESP_getChipId()   (ESP.getChipId())

#endif

#define WIFICHECK_INTERVAL    1000L

// a list to hold all clients
static std::vector<AsyncClient*> clients;


#if USE_HARDCODED_CREDENTIALS
String Router_SSID = WIFI_SSID;
String Router_Pass = WIFI_PASSWORD;
#include <ESPAsync_WiFiManager_Debug.h>
#else
// ASSUME Arduino ESP 2.0+
#include "FS.h"
#include <LittleFS.h>
FS* filesystem =      &LittleFS;
#define FileFS        LittleFS
#define FS_Name       "LittleFS"

String Router_SSID;
String Router_Pass;

typedef struct
{
  char wifi_ssid[32];
  char wifi_pw[64];
}  WiFi_Credentials;

typedef struct
{
  WiFi_Credentials  WiFi_Creds [1];
  uint16_t checksum;
} WM_Config;

WM_Config WM_config;

// Stores whether ESP has WiFi credentials saved from previous session
bool initialConfig = false;

// Where in the filesystem we store credentials
#define CONFIG_FILENAME           F("/wifi_cred.dat")

IPAddress stationIP   = IPAddress(0, 0, 0, 0);
IPAddress gatewayIP   = IPAddress(192, 168, 2, 1);
IPAddress netMask     = IPAddress(255, 255, 255, 0);
IPAddress APStaticIP  = IPAddress(192, 168, 100, 1);
IPAddress APStaticGW  = IPAddress(192, 168, 100, 1);
IPAddress APStaticSN  = IPAddress(255, 255, 255, 0);
#include <ESPAsync_WiFiManager.h>
WiFi_STA_IPConfig WM_STA_IPconfig;
#endif


///////////////////////////////////////////

uint8_t connectMultiWiFi()
{
#ifdef ESP32
  #define WIFI_MULTI_1ST_CONNECT_WAITING_MS           800L
#else
  #define WIFI_MULTI_1ST_CONNECT_WAITING_MS           2200L
#endif
  #define WIFI_MULTI_CONNECT_WAITING_MS               500L

  uint8_t status;

  LOGERROR(F("ConnectMultiWiFi with :"));

  if ( (Router_SSID != "") && (Router_Pass != "") )
  {
    LOGERROR3(F("* Flash-stored Router_SSID = "), Router_SSID, F(", Router_Pass = "), Router_Pass );
    LOGERROR3(F("* Add SSID = "), Router_SSID, F(", PW = "), Router_Pass );
    wifiMulti.addAP(Router_SSID.c_str(), Router_Pass.c_str());
  }

  LOGERROR(F("Connecting MultiWifi..."));

  int i = 0;
  status = wifiMulti.run();
  delay(WIFI_MULTI_1ST_CONNECT_WAITING_MS);

  while ( ( i++ < 20 ) && ( status != WL_CONNECTED ) )
  {
    status = WiFi.status();

    if ( status == WL_CONNECTED )
      break;
    else
      delay(WIFI_MULTI_CONNECT_WAITING_MS);
  }

  if ( status == WL_CONNECTED )
  {
    LOGERROR1(F("WiFi connected after time: "), i);
    LOGERROR3(F("SSID:"), WiFi.SSID(), F(",RSSI="), WiFi.RSSI());
    LOGERROR3(F("Channel:"), WiFi.channel(), F(",IP address:"), WiFi.localIP() );
  }
  else
  {
    LOGERROR(F("WiFi not connected"));

#if ESP8266
    ESP.reset();
#else
    ESP.restart();
#endif
  }

  return status;
}

void check_WiFi()
{
  if ( (WiFi.status() != WL_CONNECTED) )
  {
#if DEBUG_BRIDGE
    Serial.println(F("\nWiFi lost. Call connectMultiWiFi in loop"));
#endif
    connectMultiWiFi();
  }
}

void check_status()
{
  static ulong checkstatus_timeout = 0;
  static ulong checkwifi_timeout = 0;
  static ulong current_millis;
  current_millis = millis();

  // Check WiFi every WIFICHECK_INTERVAL (1) seconds.
  if ((current_millis > checkwifi_timeout) || (checkwifi_timeout == 0))
  {
    check_WiFi();
    checkwifi_timeout = current_millis + WIFICHECK_INTERVAL;
  }
}

#if USE_HARDCODED_CREDENTIALS
#else
int calcChecksum(uint8_t* address, uint16_t sizeToCalc)
{
  uint16_t checkSum = 0;

  for (uint16_t index = 0; index < sizeToCalc; index++)
  {
    checkSum += * ( ( (byte*) address ) + index);
  }

  return checkSum;
}

bool loadConfigData()
{
  File file = FileFS.open(CONFIG_FILENAME, "r");
  LOGERROR(F("LoadWiFiCfgFile "));

  memset((void *) &WM_config,       0, sizeof(WM_config));
  memset((void *) &WM_STA_IPconfig, 0, sizeof(WM_STA_IPconfig));

  if (file)
  {
    file.readBytes((char *) &WM_config,   sizeof(WM_config));
    file.readBytes((char *) &WM_STA_IPconfig, sizeof(WM_STA_IPconfig));

    file.close();
    LOGERROR(F("OK"));

    if ( WM_config.checksum != calcChecksum( (uint8_t*) &WM_config, sizeof(WM_config) - sizeof(WM_config.checksum) ) )
    {
      LOGERROR(F("WM_config checksum wrong"));
      return false;
    }
    return true;
  }
  else
  {
    LOGERROR(F("failed"));
    return false;
  }
}

void saveConfigData()
{
  File file = FileFS.open(CONFIG_FILENAME, "w");
  LOGERROR(F("SaveWiFiCfgFile "));

  if (file)
  {
    WM_config.checksum = calcChecksum( (uint8_t*) &WM_config, sizeof(WM_config) - sizeof(WM_config.checksum) );

    file.write((uint8_t*) &WM_config, sizeof(WM_config));
    file.write((uint8_t*) &WM_STA_IPconfig, sizeof(WM_STA_IPconfig));
    file.close();
    LOGERROR(F("OK"));
  }
  else
  {
    LOGERROR(F("failed"));
  }
}
#endif

static void handleError(void* arg, AsyncClient* client, int8_t error) {
#if DEBUG_BRIDGE
	Serial.printf("\n connection error %s from client %s \n", client->errorToString(error), client->remoteIP().toString().c_str());
#endif
}

static void handleDisconnect(void* arg, AsyncClient* client) {
#if DEBUG_BRIDGE
	Serial.printf("\n client %s disconnected \n", client->remoteIP().toString().c_str());
#endif
    std::vector<AsyncClient*>::iterator it = clients.begin();
    while(it != clients.end()) {
        if((*it) == client) {
            it = clients.erase(it);
            break;
        }
        else ++it;
    }
}

static void handleTimeOut(void* arg, AsyncClient* client, uint32_t time) {
#if DEBUG_BRIDGE
	Serial.printf("\n client ACK timeout ip: %s \n", client->remoteIP().toString().c_str());
#endif
}

class TcpSerialBridge2
{
public:
  TcpSerialBridge2(uint16_t tcpPort) : server(tcpPort) {}


  void begin(unsigned long baud) {
    // NO OP;
  }

  void setup(FullLoopbackStream *outgoingStream, FullLoopbackStream *incomingStream) {
#if DEBUG_BRIDGE
    Serial.begin(115200);
    delay(200);
#endif

    this->outgoingStream = outgoingStream;
    this->incomingStream = incomingStream;
    unsigned long startedAt = millis();

#if USE_HARDCODED_CREDENTIALS
#else
    // Format FileFS if not yet
#ifdef ESP32
    if (!FileFS.begin(true))
#else
    if (!FileFS.begin())
#endif
    {
#ifdef ESP8266
      FileFS.format();
#endif

      if (!FileFS.begin())
      {
        // prevents debug info from the library to hide err message.
        delay(100);
#if DEBUG_BRIDGE
        Serial.println(F("LittleFS failed!"));
#endif

        // hold the ESP hostage without doing anything else
        while (true) { delay(1); }
      }
    }


    WM_STA_IPconfig._sta_static_ip   = stationIP;
    WM_STA_IPconfig._sta_static_gw   = gatewayIP;
    WM_STA_IPconfig._sta_static_sn   = netMask;
    AsyncWebServer webServer(80);

#if ( CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32C3 ) // maybe S3 too?
    // does AsyncDNSServer not work with these boards?
    //  let me know
    ESPAsync_WiFiManager ESPAsync_wifiManager(&webServer, NULL, "AutoConnectAP");
#else
    AsyncDNSServer dnsServer;
    ESPAsync_WiFiManager ESPAsync_wifiManager(&webServer, &dnsServer, "AutoConnectAP");
#endif
    ESPAsync_wifiManager.setDebugOutput(DEBUG_BRIDGE);

    Router_SSID = ESPAsync_wifiManager.WiFi_SSID();
    Router_Pass = ESPAsync_wifiManager.WiFi_Pass();
    bool configDataLoaded = false;
#endif


    if ( (Router_SSID != "") && (Router_Pass != "") )
    {
      LOGERROR3(F("* Add SSID = "), Router_SSID, F(", PW = "), Router_Pass);
      wifiMulti.addAP(Router_SSID.c_str(), Router_Pass.c_str());
#if USE_HARDCODED_CREDENTIALS
#else
      ESPAsync_wifiManager.setConfigPortalTimeout(120); //If no access point name has been previously entered disable timeout.
#endif
#if DEBUG_BRIDGE
      Serial.println(F("Got ESP Self-Stored Credentials. Timeout 120s for Config Portal"));
#endif
    }


#if USE_HARDCODED_CREDENTIALS
#else
    if (loadConfigData())
    {
      configDataLoaded = true;

      ESPAsync_wifiManager.setConfigPortalTimeout(120); //If no access point name has been previously entered disable timeout.
#if DEBUG_BRIDGE
      Serial.println(F("Got stored Credentials. Timeout 120s for Config Portal"));
#endif
    }
    else
    {
      // Enter Config Portal only if no stored SSID on  file
#if DEBUG_BRIDGE
      Serial.println(F("Open Config Portal without Timeout: No stored Credentials."));
#endif
      initialConfig = true;
    }
#endif

#if USE_HARDCODED_CREDENTIALS
    startedAt = millis();
    connectMultiWiFi();
#else
    if (initialConfig)
    {
      String chipID = String(ESP_getChipId(), HEX);
      chipID.toUpperCase();

      // SSID for Config Portal
      String AP_SSID = "ESP_" + chipID + "-SH";

#if DEBUG_BRIDGE
      Serial.println(F("We haven't got any access point credentials, so get them now"));
      Serial.print(F("Starting configuration portal @ "));
      Serial.print(F("192.168.4.1"));
      Serial.print(F(", SSID = "));
      Serial.print(AP_SSID);
#endif
      // Starts an access point
      if ( !ESPAsync_wifiManager.startConfigPortal(AP_SSID.c_str(), NULL) )
      {
#if DEBUG_BRIDGE
        Serial.println(F("Not connected to WiFi but continuing anyway."));
#endif
      }
      else
      {
#if DEBUG_BRIDGE
        Serial.println(F("WiFi connected...yay :)"));
#endif
      }

      memset(&WM_config, 0, sizeof(WM_config));

      uint8_t i = 0;
      strcpy(WM_config.WiFi_Creds[i].wifi_ssid, ESPAsync_wifiManager.getSSID(i).c_str());
      strcpy(WM_config.WiFi_Creds[i].wifi_pw, ESPAsync_wifiManager.getPW(i).c_str());
      // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
      if ( (String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= 8) )
      {
        LOGERROR3(F("* Add SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw );
        wifiMulti.addAP(WM_config.WiFi_Creds[i].wifi_ssid, WM_config.WiFi_Creds[i].wifi_pw);
      }
      ESPAsync_wifiManager.getSTAStaticIPConfig(WM_STA_IPconfig);
      saveConfigData();
    }
    else
    {
      wifiMulti.addAP(Router_SSID.c_str(), Router_Pass.c_str());
    }

    startedAt = millis();

    if (!initialConfig)
    {
      // Load stored data, the addAP ready for MultiWiFi reconnection
      if (!configDataLoaded) {
        loadConfigData();
      }

      uint8_t i = 0;
      // Don't permit NULL SSID and password len < MIN_AP_PASSWORD_SIZE (8)
      if ( (String(WM_config.WiFi_Creds[i].wifi_ssid) != "") && (strlen(WM_config.WiFi_Creds[i].wifi_pw) >= 8) )
      {
        LOGERROR3(F("* Add SSID = "), WM_config.WiFi_Creds[i].wifi_ssid, F(", PW = "), WM_config.WiFi_Creds[i].wifi_pw );
        wifiMulti.addAP(WM_config.WiFi_Creds[i].wifi_ssid, WM_config.WiFi_Creds[i].wifi_pw);
      }

      if ( WiFi.status() != WL_CONNECTED )
      {
  #if DEBUG_BRIDGE
        Serial.println(F("ConnectMultiWiFi in setup"));
  #endif
        connectMultiWiFi();
      }
    }
#endif

#if DEBUG_BRIDGE
    Serial.print(F("After waiting "));
    Serial.print((float) (millis() - startedAt) / 1000L);
    Serial.print(F(" secs more in setup(), connection result is "));
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.print(F("connected. Local IP: "));
      Serial.println(WiFi.localIP());
    }
    else
    {
      Serial.print(F("Wifi Status: "));
      Serial.println(WiFi.status());
    }
#endif

    server.setNoDelay(true);
    server.onClient([this](void *arg, AsyncClient* client){
#if DEBUG_BRIDGE
      Serial.printf("\n new client has been connected to server, ip: %s", client->remoteIP().toString().c_str());
#endif

      // add to list
      clients.push_back(client);
      
      // register events
#if DEBUG_BRIDGE
      client->onAck([&](void* arg, AsyncClient* client, size_t len, uint32_t time){ Serial.printf("\n ack: %d %d\n", len, time); }, NULL);
#endif
      client->onData([&](void* arg, AsyncClient* client, void *data, size_t len){ this->handleData(arg, client, data, len); }, NULL);
      client->onError(&handleError, NULL);
      client->onDisconnect(&handleDisconnect, NULL);
      client->onTimeout(&handleTimeOut, NULL);
    }, &server);
    server.begin();
  }
  
  void loop() {
    // put your main code here, to run repeatedly
    check_status();
    this->flush();
  }

  void flush() {
    // if there is data available in the wifi stream, it's meant
    //  to go from Serial port to TCP client
    size_t availableLength = this->outgoingStream->available();
    if (availableLength)
    {
#if DEBUG_BRIDGE
    Serial.printf("flushing with this much data: %d \n", availableLength);
#endif
      if (clients.size() == 0) {
        // no clients to flush to
        return;
      }

      // read the available data from the stream, and put in in the buffer
      char sbuf[availableLength];
      this->outgoingStream->readBytes(sbuf, availableLength);

      // if TCP client is available
      AsyncClient* client = clients.front();
      if (client->connected())
      {
        // send data to client
        size_t total = client->write(sbuf, availableLength);
#if DEBUG_BRIDGE
      Serial.printf("\n ---> data sent to client %s: %d bytes \n", client->remoteIP().toString().c_str(), total);
      // printing all bytes sent as individual bytes in hex
      for (size_t i = 0; i < availableLength; i++) {
        Serial.printf("%02X ", sbuf[i]);
      }
      Serial.println(" ");
#endif
      }
    }
  }

private:
  void handleData(void* arg, AsyncClient* client, void *data, size_t len) {
#if DEBUG_BRIDGE
	  Serial.printf("\n <--- data received from client %s \n", client->remoteIP().toString().c_str());
    // this line may cause problems with the c++ linker. If you need deep debugging the actual data received, and it compiles fine if you
    //  remove the commenting below, then you're good.
    //Serial.write((uint8_t*)data, len);
    //Serial.println(" ");
#endif
    const uint8_t *castData = (uint8_t*)data;
    this->incomingStream->write(castData, (size_t)len);
    // printing all bytes received as individual bytes
    for (size_t i = 0; i < len; i++) {
      Serial.printf("%02X ", castData[i]);
    }
    Serial.println(" ");
  }

  AsyncServer server;
  FullLoopbackStream *incomingStream;
  FullLoopbackStream *outgoingStream;
};

