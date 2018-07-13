#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include "DHT.h"

const char* ssid     = "demoIT";
const char* password = "";

// DHT22
#define DHTTYPE DHT22
#define DHTPIN  5
DHT dht(DHTPIN, DHTTYPE);

unsigned long delayTime = 800;

// UDP Client
WiFiUDP udp;
unsigned int localUdpPort = 4210;
const char* ip = "192.168.1.1";
unsigned int port = 8888;

void setup() {
  Serial.begin(115200);
  delay(100);

  // DHT22
  dht.begin();
  Serial.println("DHT22 Test");
  Serial.println();
  delay(100);

  // WiFi Connect
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rWorking to connect");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("NodeMCU(ESP8266)");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start UDP Connection
  udp.begin(localUdpPort);
}


void loop() {
  // DHT22 read values
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  }
  else {
    // JSON Buffer
    StaticJsonBuffer<100> jsonBuffer;
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print("C Humidity: ");
    Serial.print(h);
    Serial.println("%");

    // Publish UDP Packets
    // Temperature
    JsonObject& root = jsonBuffer.createObject();
    root["type"] = "pub";
    root["topic"] = "temperature";
    root["value"] = t; 

    udp.beginPacket(ip, port);
    root.printTo(udp);
    udp.println();
    udp.endPacket();

    // Humidity
    root["topic"] = "humidity";
    root["value"] = h; 

    udp.beginPacket(ip, port);
    root.printTo(udp);
    udp.println();
    udp.endPacket();
  }

  delay(delayTime);
}
