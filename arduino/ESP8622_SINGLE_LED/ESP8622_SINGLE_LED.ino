#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>

const char* ssid     = "demoIT";
const char* password = "";

unsigned long delayTime = 10;
unsigned int ledPin = D6;
unsigned int humidity = 70;

// UDP Client
WiFiUDP udp;
unsigned int localUdpPort = 5000;
const char* ip = "192.168.0.1";
int port = 8888;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

void setup() {
  Serial.begin(115200);
  delay(10);

  // LEDs Starts OFF
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // Connect to WiFi
  Serial.println();
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rWorking to connect");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print Network Information
  Serial.println("");
  Serial.println("NodeMCU(ESP8266)");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start UDP Socket
  udp.begin(localUdpPort);

  // JSON Encoder
  const size_t bufferSize = JSON_OBJECT_SIZE(2);
  DynamicJsonBuffer jsonBuffer(bufferSize);

  // Subscribe Humidity
  JsonObject& root = jsonBuffer.createObject();
  root["type"] = "sub";
  root["topic"] = "humidity";

  udp.beginPacket(ip, port);
  root.printTo(udp);
  udp.println();
  udp.endPacket();
}

void loop() {
  // Receive UDP Package
  int packetSize = udp.parsePacket();
  
  if (packetSize)
  {
    // Read UDP Package
    udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    
    // JSON Parser
    const size_t bufferSize = JSON_OBJECT_SIZE(3) + 40;
    DynamicJsonBuffer jsonBuffer(bufferSize);
    JsonObject& root = jsonBuffer.parseObject(packetBuffer);
    const char* type = root["type"]; 
    const char* topic = root["topic"];
    int value = root["value"];

    root.printTo(Serial);
    Serial.println();

    if(value > humidity) {
      digitalWrite(ledPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
    }
  }
  delay(delayTime);
}
