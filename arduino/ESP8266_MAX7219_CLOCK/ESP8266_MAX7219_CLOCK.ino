#include <SPI.h>
#include <Wire.h>
#include <BH1750.h>
#include <ESP8266WiFi.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Timezone.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

#define AMBIENT_LOW 20
#define AMBIENT_HIGH 3000
#define INTENSITY_LOW 0
#define INTENSITY_HIGH 15

int pinCS = D8;
int numberOfHorizontalDisplays = 4;
int numberOfVerticalDisplays   = 1;
char time_value[20];

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

unsigned int wait = 70;
unsigned int spacer = 1;
unsigned int width  = 5 + spacer;
const char* ssid = "TheOffice";
const char* password = "8006002030";

float m = float(INTENSITY_LOW - INTENSITY_HIGH) / float(AMBIENT_LOW - AMBIENT_HIGH),
b = float(INTENSITY_LOW - m) * AMBIENT_LOW;

BH1750 lightMeter;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pt.pool.ntp.org");

TimeChangeRule dstRule = {"WEST", Last, Sun, Mar, 2, +60};
TimeChangeRule stdRule = {"WET",  Last, Sun, Oct, 2, 0};
Timezone tz(dstRule, stdRule);

void setup() {
  Serial.begin(115200);
  delay(100);

  // BH1750
  Wire.begin();
  lightMeter.begin();
  Serial.println("BH1750 Test");
  Serial.println();
  delay(100);
  
  WiFi.begin(ssid,password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("NodeMCU(ESP8266)");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  
  matrix.setIntensity(6);      // Use a value between 0 and 15 for brightness
  matrix.setRotation(0, 1);    // The first display is position upside down
  matrix.setRotation(1, 1);    // The first display is position upside down
  matrix.setRotation(2, 1);    // The first display is position upside down
  matrix.setRotation(3, 1);    // The first display is position upside down
}

unsigned int lux_2_intensity(float lux) {
  unsigned int rv = 0;
  
  if(lux < AMBIENT_LOW) {
    rv = 0;
  } else if (lux > AMBIENT_HIGH) {
    rv = 15;
  } else {
    float res = lux*m+b;
    rv = (unsigned int) round(res);
  }

  return rv;
}

void loop() {
  unsigned long StartTime = millis();
  timeClient.update();
  unsigned long ts = tz.toLocal(timeClient.getEpochTime());
  char buff[16];
  sprintf(buff, "%.2d:%.2d:%.2d", hour(ts), minute(ts), second(ts));
  String time = String(buff);
  time.trim();
  time.toCharArray(time_value, 10);
  Serial.print("Time = ");
  Serial.println(time);
  matrix.setIntensity(lux_2_intensity(lightMeter.readLightLevel()));
  matrix.fillScreen(LOW);
  matrix.drawChar(2,1, time_value[0], HIGH,LOW,1); // H
  matrix.drawChar(8,1, time_value[1], HIGH,LOW,1); // HH
  matrix.drawChar(14,1,time_value[2], HIGH,LOW,1); // HH:
  matrix.drawChar(20,1,time_value[3], HIGH,LOW,1); // HH:M
  matrix.drawChar(26,1,time_value[4], HIGH,LOW,1); // HH:MM
  matrix.write();
  unsigned long CurrentTime = millis();
  unsigned long ElapsedTime = CurrentTime - StartTime;
  delay(500-ElapsedTime);

  StartTime = millis();
  matrix.fillScreen(LOW);
  matrix.drawChar(2,1, time_value[0], HIGH,LOW,1); // H
  matrix.drawChar(8,1, time_value[1], HIGH,LOW,1); // HH
  matrix.drawChar(14,1,' ',           HIGH,LOW,1); // HH
  matrix.drawChar(20,1,time_value[3], HIGH,LOW,1); // HH M
  matrix.drawChar(26,1,time_value[4], HIGH,LOW,1); // HH MM
  matrix.write();
  CurrentTime = millis();
  ElapsedTime = CurrentTime - StartTime;
  delay(500-ElapsedTime);
}
