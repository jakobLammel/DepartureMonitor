#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <esp8266httpclient.h>
#include <ArduinoJson.h>
#include <credentials.h>

const bool debugMode = true;

/* ===  wifi connection settings  ===  */
const char* ssid = WIFI_SSID;
const char* pwd = WIFI_PASSWD;
const long interval = 500;          
const long reqConnectNum = 60; // number of intervals to wait for connection

/* ===  rbl connection settings  === */
const char* apiKey = API_KEY;
const char* rblCode = RBL;


void setup() {
  Serial.begin(9600);
}

void connectToNetwork(const char* ssidA, const char* pwdA) {
  WiFi.begin(ssidA, pwdA);
  if (debugMode) {Serial.println("Connecting to ");}

  int i = 0;
  while (WiFi.status() != WL_CONNECTED && i < reqConnectNum) {
    delay(interval);
    if (debugMode) {Serial.print(".");}
    i++;
  }
}

void showMissingNetwork() {
  // to be implemented, display a missing network symbol on screen.
}

void showInvalidRequest() {
  // to be implemented, display a missing network symbol on screen.
}

void showNightTime() {
  // to be implemented, display a missing network symbol on screen.
}

void loop() {
  connectToNetwork(ssid, pwd);

  if (WiFi.status() != WL_CONNECTED) {
    if (debugMode) {Serial.println("no network connection");}
    showMissingNetwork();
  } else
  {
    if (debugMode) {Serial.println("connected");}
    char requestURL[255];
    snprintf(requestURL, sizeof(requestURL), "http://www.wienerlinien.at/ogd_realtime/monitor?rbl=%s&sender=%s", rblCode, apiKey);
    if (debugMode) {Serial.println(requestURL);}

    WiFiClient client;
    HTTPClient http;
    http.begin(client, requestURL);
    int httpCode = http.GET();

    if (httpCode <= 0) {
      if (debugMode) {Serial.println("http Code not OK");}
      showInvalidRequest();
    } else
    {
      if (debugMode) {Serial.println(httpCode);}

      Stream& response = http.getStream();

      DynamicJsonDocument doc(16384);

      deserializeJson(doc, response);

      if (doc["data"]["monitors"][0].isNull()) {
        if (debugMode) {Serial.println("no more trains");}
        showNightTime();
      } else
      {
        Serial.println(doc["data"]["monitors"][0]["lines"][0]["departures"]["departure"][0]["departureTime"]["countdown"].as<int>());
      }
    }
  http.end();
  WiFi.disconnect();
  }
  
}