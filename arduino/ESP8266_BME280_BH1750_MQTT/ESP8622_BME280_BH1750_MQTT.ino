#include <NTPClient.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <BH1750.h>
#include <PubSubClient.h>
#include <PubSubClientTools.h>

#include <Thread.h>             // https://github.com/ivanseidel/ArduinoThread
#include <ThreadController.h>

#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10

#define SEALEVELPRESSURE_HPA 1013.25

BH1750 lightMeter;

const char* ssid     = "HOT_HOT";
const char* password = "87K8UJJH";
const char* MQTT_SERVER = "192.168.1.161";

Adafruit_BME280 bme;

unsigned long delayTime = 1000;

// UDP Client
WiFiClient espClient;
PubSubClient client(MQTT_SERVER, 1883, espClient);
PubSubClientTools mqtt(client);

ThreadController threadControl = ThreadController();
Thread thread = Thread();

// Wifi NTP UDP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "ntp.ua.pt");

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
  Serial.println();
  delay(100);

  // BH1750
  //Wire.begin(D5,D6);
  lightMeter.begin();
  Serial.println("BH1750 Test");
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

  // Start NTP Client
  timeClient.begin();

  // Connect to MQTT
  String s = "";
  Serial.print(s+"Connecting to MQTT: "+MQTT_SERVER+" ... ");
  if (client.connect("ESP8266Client")) {
    Serial.println("connected");
    //mqtt.subscribe("test/inTopic1", topic1_subscriber);
    //mqtt.subscribe("test/inTopic2", topic2_subscriber);
  } else {
    Serial.println(s+"failed, rc="+client.state());
  }

  // Enable Thread
  thread.onRun(publisher);
  thread.setInterval(1000);
  threadControl.add(&thread);
}

void loop() {
  client.loop();
  threadControl.run();
}

void publisher() {
  // JSON Buffer
  StaticJsonBuffer<128> jsonBuffer;

  // Update NTP client
  timeClient.update();
  unsigned long ts = timeClient.getEpochTime();

  // Read BME280 values
  float t = bme.readTemperature();
  float h = bme.readHumidity();
  float p = bme.readPressure() / 100.0F;

  //Serial.print("Temperature: ");
  //Serial.print(t);
  //Serial.print("C Humidity: ");
  //Serial.print(h);
  //Serial.print("% Pressure: ");
  //Serial.print(p);
  //Serial.print("hpa");

  // Read BH1750 
  unsigned int l = lightMeter.readLightLevel();
  //l = floor(((l - 1.0)/(54612.0 - 1.0))*100.0);
  //Serial.print(" Light: ");
  //Serial.print(l);
  //Serial.print("%");
  //Serial.print(" TS: ");
  //Serial.print(ts);
  //Serial.print("ms -> ");
  //Serial.println(timeClient.getFormattedTime());

  // Publish MQTT Packets
  // Temperature
  String txt = "";
  JsonObject& root = jsonBuffer.createObject();
  root["name"] = "Temperature";
  root["type"] = "line";
  JsonObject& data = root.createNestedObject("data");
  data["value"] = t;
  root["ts"] = ts;

  root.printTo(txt);
  Serial.println(txt);
  mqtt.publish("students/temperature", txt);

  // Humidity
  root["name"] = "Humidity";
  root["type"] = "value";
  data["value"] = h;
  
  txt = "";
  root.printTo(txt);
  Serial.println(txt);
  mqtt.publish("students/humidity", txt);

  // Pressure
  root["name"] = "Pressure";
  root["type"] = "line";
  data["value"] = p;
  
  txt = "";
  root.printTo(txt);
  Serial.println(txt);
  mqtt.publish("students/pressure", txt);

  // Light
  root["name"] = "Light";
  root["type"] = "gauge";
  data["value"] = l;
  root["min"] = 0;
  root["max"] = 100;

  txt = "";
  root.printTo(txt);
  Serial.println(txt);
  mqtt.publish("students/light", txt);
}
