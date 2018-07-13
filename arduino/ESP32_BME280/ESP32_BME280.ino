#include <Wire.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SSID "demoIT"
#define PASSWORD ""

#define SEALEVELPRESSURE_HPA 1013.25

Adafruit_BME280 bme;

unsigned long delayTime = 800;

// UDP Client
WiFiUDP udp;
unsigned int localUdpPort = 4210;
const char* ip = "192.168.1.1";
unsigned int port = 8888;

void setup() {
  Serial.begin(115200);
  delay(100);

  // BME 280
  bool status = bme.begin();
  if (!status) {
      Serial.println("Could not find a valid BME280 sensor, check wiring!");
      while (1);
  }
  Serial.println("BME280 Test");
  Serial.println();
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
  Serial.println("NodeMCU(ESP8266)");
  Serial.print("Connected to ");
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start UDP Connection
  udp.begin(localUdpPort);
}

void loop() { 
  // JSON Buffer
  StaticJsonBuffer<100> jsonBuffer;

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
  Serial.println("hpa");

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

  // Pressure
  root["topic"] = "pressure";
  root["value"] = p; 
  
  udp.beginPacket(ip, port);
  root.printTo(udp);
  udp.println();
  udp.endPacket();
  
  delay(delayTime);
}
