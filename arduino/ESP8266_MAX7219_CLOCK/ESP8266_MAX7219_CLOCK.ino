#include <SPI.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <TimeLib.h>
#include <Timezone.h>

int pinCS = D8;
int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays   = 1;
char time_value[20];

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

unsigned int wait = 70;
unsigned int spacer = 1;
unsigned int width  = 5 + spacer;
const char* ssid = "Sunset Hackathon";
const char* password = "sunset2019";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pt.pool.ntp.org");

TimeChangeRule dstRule = {"WEST", Last, Sun, Mar, 2, +60};
TimeChangeRule stdRule = {"WET",  Last, Sun, Oct, 2, 0};
Timezone tz(dstRule, stdRule);

void setup() {
  Serial.begin(115200);
  delay(100);
  
  WiFi.begin(ssid,password);

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

  timeClient.begin();
  
  matrix.setIntensity(15);      // Use a value between 0 and 15 for brightness
  matrix.setRotation(0, 1);    // The first display is position upside down
  matrix.setRotation(1, 1);    // The first display is position upside down
  matrix.setRotation(2, 1);    // The first display is position upside down
  matrix.setRotation(3, 1);    // The first display is position upside down
}

void loop() {
  matrix.fillScreen(LOW);

  // Update NTP client
  timeClient.update();
  unsigned long ts = tz.toLocal(timeClient.getEpochTime());
  char buff[16];
  sprintf(buff, "%.2d:%.2d:%.2d", hour(ts), minute(ts), second(ts));
  String time = String(buff);
  time.trim();
  Serial.println(time);
  time.toCharArray(time_value, 10); 
  matrix.drawChar(2,1, time_value[0], HIGH,LOW,1); // H
  matrix.drawChar(8,1, time_value[1], HIGH,LOW,1); // HH
  matrix.drawChar(14,1,time_value[2], HIGH,LOW,1); // HH:
  matrix.drawChar(20,1,time_value[3], HIGH,LOW,1); // HH:M
  matrix.drawChar(26,1,time_value[4], HIGH,LOW,1); // HH:MM
  matrix.write(); // Send bitmap to display
  delay(500);

  matrix.fillScreen(LOW);
  matrix.drawChar(2,1, time_value[0], HIGH,LOW,1); // H
  matrix.drawChar(8,1, time_value[1], HIGH,LOW,1); // HH
  matrix.drawChar(14,1,' ', HIGH,LOW,1);                   // HH:
  matrix.drawChar(20,1,time_value[3], HIGH,LOW,1); // HH:M
  matrix.drawChar(26,1,time_value[4], HIGH,LOW,1); // HH:MM
  matrix.write();
  delay(500);
}
