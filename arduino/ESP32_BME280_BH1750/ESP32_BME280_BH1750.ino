#include <NTPClient.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <BH1750.h>

#define SSID "TheOffice"
#define PASSWORD "8006002030"

#define SEALEVELPRESSURE_HPA 1013.25

BH1750 lightMeter;
Adafruit_BME280 bme;

unsigned long delayTime = 1000;

// UDP Client
WiFiUDP udp;
unsigned int localUdpPort = 4210;
const char* ip = "192.168.1.125";
unsigned int port = 8888;

// Wifi NTP UDP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "192.168.1.125");

void setup() {
  Serial.begin(115200);
  delay(100);
  
  // BME280
  bool status = bme.begin();
  if (!status) {
      Serial.println("Could not find a valid BME280 sensor, check wiring!");
      while (1);
  }
  Serial.println("BME280 Test");
  delay(100);

  // BH1750
  //Wire.begin(D5,D6);
  lightMeter.begin();
  Serial.println("BH1750 Test");
  delay(100);

  // WiFi Connect
  WiFi.begin(SSID, PASSWORD);
  Serial.print("\n\r \n\rWorking to connect");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("NodeMCU(ESP32)");
  Serial.print("Connected to ");
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start UDP Connection
  udp.begin(localUdpPort);

  // Start NTP Client
  timeClient.begin();
}

void loop() {
  // JSON Buffer
  JsonDocument doc;

  // Update NTP client
  timeClient.update();
  unsigned long ts = timeClient.getEpochTime();

  // Read BME280 values
  float t = bme.readTemperature();
  float h = bme.readHumidity();
  float p = bme.readPressure() / 100.0F;

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print("C Humidity: ");
  Serial.print(h);
  Serial.print("% Pressure: ");
  Serial.print(p);
  Serial.print("hpa");

  // Read BH1750 
  unsigned int l = lightMeter.readLightLevel();
  l = floor(((l - 1.0)/(54612.0 - 1.0))*100.0);
  Serial.print(" Light: ");
  Serial.print(l);
  Serial.print("%");
  Serial.print(" TS: ");
  Serial.print(ts);
  Serial.print("ms -> ");
  Serial.println(timeClient.getFormattedTime());

  // Publish UDP Packets
  // Temperature
  //JsovnObject& root = doc.createObject();
  doc["type"] = "pub";
  doc["topic"] = "temperature";
  doc["value"] = t;
  doc["ts"] = ts;
  
  udp.beginPacket(ip, port);
  //root.printTo(udp);
  serializeJson(doc, udp);
  udp.println();
  udp.endPacket();

  // Humidity
  doc["topic"] = "humidity";
  doc["value"] = h; 
  
  udp.beginPacket(ip, port);
  //root.printTo(udp);
  serializeJson(doc, udp);
  udp.println();
  udp.endPacket();

  // Pressure
  doc["topic"] = "pressure";
  doc["value"] = p; 
  
  udp.beginPacket(ip, port);
  //root.printTo(udp);
  serializeJson(doc, udp);
  udp.println();
  udp.endPacket();

  // Light
  doc["topic"] = "light";
  doc["value"] = l; 
  
  udp.beginPacket(ip, port);
  //root.printTo(udp);
  serializeJson(doc, udp);
  udp.println();
  udp.endPacket();
  
  delay(delayTime);
}
