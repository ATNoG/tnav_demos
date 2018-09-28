#include <brzo_i2c.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <SSD1306Brzo.h>
#include <TimeLib.h>
#include <Timezone.h>  

// Include custom images
#include "images.h"

// Include messages in EN/PT
#include "msg_pt.h" 

#define SSID "demoIT"
#define PASSWORD ""

SSD1306Brzo display(0x3c, D3, D5);

// UDP Client
WiFiUDP udp;
unsigned int localUdpPort = 4210;
const char* ip = "192.168.1.1";
int port = 8888;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

// Data variables
float t = 0.0;
float h = 0.0;
float p = 0.0;
unsigned int l = 0;
time_t ts = 0;

TimeChangeRule dstRule = {"WEST", Last, Sun, Mar, 2, +60};
TimeChangeRule stdRule = {"WET",  Last, Sun, Oct, 2, 0};
Timezone tz(dstRule, stdRule);

void setup() {
  Serial.begin(115200);
  delay(10);
  
  // Initialize the SSD1306 OLED display
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();

  // Connect to WiFi
  Serial.println();
  WiFi.begin(SSID, PASSWORD);
  Serial.print("\n\r \n\rWorking to connect");

  // Wait for connection
  int progress = 0;
  while (WiFi.status() != WL_CONNECTED) {
    display.clear();
    Serial.print(".");
    if(progress < 99) {
      progress = (progress+3) % 99;
    }
    // draw the progress bar
    display.drawProgressBar(0, 32, 120, 10, progress);
    // draw the percentage as String
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64,  3, connect_msg);
    display.drawString(64, 15, String(progress) + "%");
    display.display();
    delay(100);
  }
  
  progress = 100;
  display.clear();
  display.drawProgressBar(0, 32, 120, 10, progress);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64,  3, connect_msg);
  display.drawString(64, 15, String(progress) + "%");
  display.display();
  delay(120);

  // Print Network Information
  Serial.println("");
  Serial.println("NodeMCU(ESP8266)");
  Serial.print("Connected to ");
  Serial.println(SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0,  3, board_msg);
  display.drawString(0, 15, connected_msg + String(SSID));
  display.drawString(0, 27, ip_msg + WiFi.localIP().toString());
  display.display();
  delay(3000);

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

  // Subscribe Light
  root["topic"] = "light";
  udp.beginPacket(ip, port);
  root.printTo(udp);
  udp.println();
  udp.endPacket();

  // Subscribe Temperature
  root["topic"] = "temperature";
  udp.beginPacket(ip, port);
  root.printTo(udp);
  udp.println();
  udp.endPacket();

  // Subscribe Pressure
  root["topic"] = "pressure";
  udp.beginPacket(ip, port);
  root.printTo(udp);
  udp.println();
  udp.endPacket();
}

void loop() {
  char m[4];
  char buff[20];
  strcpy(m, monthShortStr(month(ts)));
  sprintf(buff, "%.2d %s %d %.2d:%.2d:%.2d", day(ts), m, year(ts), hour(ts), minute(ts), second(ts));
  
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0,  0, buff);
  display.drawString(0, 11, temp_msg     + String(t)+"ºC");
  display.drawString(0, 22, pressure_msg + String(p)+"hPa");
  display.drawString(0, 33, humidity_msg + String(h)+"%");
  display.drawString(0, 44, light_msg    + String(l)+"%");
  display.drawXbm(96, 32, sun_width, sun_height, sun_bits);
  display.display();
  
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
    ts = tz.toLocal(root["ts"]);
    
    if(strcmp(topic,"humidity") == 0) {
      h = root["value"];
    } else if(strcmp(topic,"light") == 0) {
      l = root["value"];
    } else if(strcmp(topic,"temperature") == 0) {
      t = root["value"];
    } else {
      p = root["value"];
    }

    root.printTo(Serial);
    Serial.println();
  }
  
  delay(10);
}
