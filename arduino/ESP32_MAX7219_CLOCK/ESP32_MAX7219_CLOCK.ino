#include <NTPClient.h>
#include <Wire.h>
#include <WiFi.h>

#define SSID "demoIT"
#define PASSWORD ""

// Wifi NTP UDP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "2.pt.pool.ntp.org");

void setup() {
  Serial.begin(115200);
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

  // Start NTP Client
  timeClient.begin();
}

void loop() {
  // Update NTP client
  timeClient.update();
  unsigned long ts = timeClient.getEpochTime();

  Serial.print(" TS: ");
  Serial.print(ts);
  Serial.print("ms -> ");
  Serial.println(timeClient.getFormattedTime());

  delay(1000);
}
