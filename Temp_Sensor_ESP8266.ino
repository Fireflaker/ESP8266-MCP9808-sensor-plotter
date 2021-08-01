#include "ESP8266WiFi.h"
#include <Wire.h>
#include <Adafruit_MCP9808.h>
#include "Settings.h"

//uint8_t newMACAddress[] = {0xB8, 0x27, 0xEB, 0xFB, 0xAE, 0xF7};
uint8_t newMACAddress[] = {0x00, 0x09, 0xBF, 0x7C, 0x7B, 0x7A};

// Wifi Client
WiFiClient client;

// Create MCP9808 temperature sensor object
Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();

// Wifi settings
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

// ThingSpeak settings
const int channelID = TS_CHANNEL_ID;
String writeAPIKey = TS_WRITE_KEY;
const char* server = "api.thingspeak.com";

// Stop for 20 seconds at the end of each loop
// Free Thingsspeak accounts have a 15 second rate limit
const int postingInterval = 15 * 100; //150ms, push server and it seems to respond smh

float LightsensorValue = 666;
int MotionSensorValue = 555;

void setup() {
  // Start Serial
  Serial.begin(115200);
  Serial.println("");
  Serial.println("#define TS_WRITE_KEY  -AKJWOC6MV0TFTEMZ-");
  Serial.println("Sensor channel 2 #define TS_CHANNEL_ID 1461051");
  Serial.println("if (!tempsensor.begin(0x19)) {");
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("Allen Wu, uploaded August 1 2021 night, after fundrasing painting day, of -cactus in MoMA-. Might have future.");
  Serial.println("https://github.com/Fireflaker/ESP8266-MCP9808-sensor-plotter");
  Serial.println("Last version, on Git, has bug! although unexpected. Temperature does not update on ThingSpeak. - Reason: need delay before temperature read! Solution: Never goto sleep! -");
  Serial.println("This MAC takes a very long time to connect to Wifi");
  Serial.println("MCP9808 temperature unchanged: https://forums.adafruit.com/viewtopic.php?f=19&t=86088");
  Serial.println("");

  // Set the pins used for I2C on your ESP8266 for the temp sensor
  Wire.pins(12, 14); //D4=SDA, D5=SCL on ESP8266

  // Start the sensor
  if (!tempsensor.begin(0x19)) {
    //  A2 A1 A0 address
    //  0  0  0   0x18  this is the default address
    //  0  0  1   0x19
    //  0  1  0   0x1A
    //  0  1  1   0x1B
    //  1  0  0   0x1C
    //  1  0  1   0x1D
    //  1  1  0   0x1E
    //  1  1  1   0x1F
    Serial.println("Couldn't find MCP9808!");
    while (1);
  }
  WiFi.mode(WIFI_STA);

  Serial.print("[OLD] ESP8266 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  // For Soft Access Point (AP) Mode
  //wifi_set_macaddr(SOFTAP_IF, &newMACAddress[0]);
  // For Station Mode
  wifi_set_macaddr(STATION_IF, &newMACAddress[0]);

  Serial.print("[NEW] ESP8266 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());


  // Start Wifi and wait until we're connected
  WiFi.begin(ssid, password);
  WiFi.begin(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }


  tempsensor.wake();
  tempsensor.setResolution(3); // sets the resolution mode of reading, the modes are defined in the table bellow:
  // Mode Resolution SampleTime
  //  0    0.5°C       30 ms
  //  1    0.25°C      65 ms
  //  2    0.125°C     130 ms
  //  3    0.0625°C    250 ms

  pinMode(A0, INPUT);    // sets the A0 as input for LightSensor
  pinMode(13, INPUT);    // sets the 13-D7 as input for MotionSens1
  //pinMode(15, INPUT);    // sets the 15-D8 as input for MotionSens2

}

void loop() {

  // Wake up MSP9808 - power consumption ~200 mikro Ampere - no more!!

  float tempValue = tempsensor.readTempC();

  //Start Managing motion sensor inputs

  LightsensorValue = (analogRead(A0));
  MotionSensorValue = (analogRead(13)); //13-D7 as input for MotionSens1

  //Parse data for neither triggered, 1 then 2, 2 then 1, with delay in ms.
  //If one triggers and the other times out at 5000ms, just print 125000


  // Measure Signal Strength (RSSI) of Wi-Fi connection
  long rssi = WiFi.RSSI();

  Serial.print("Deg. C: ");
  Serial.print(tempValue);
  Serial.print("\tRSSI: ");
  Serial.print(rssi);
  Serial.print("\tMotion: ");
  Serial.print(MotionSensorValue);
  Serial.print("\tLightADC: ");
  Serial.println(LightsensorValue);


  if (client.connect(server, 80)) {
    reportToThingspeak(tempValue, rssi, LightsensorValue);
  }

  client.stop();
  delay(postingInterval);
}

void reportToThingspeak(float tempValue, long rssi, float LightsensorValue) {
  String body = "field1=";
  body += String(rssi);
  body += "&field2=";
  body += String(tempValue, 3); // Two decimal places
  body += "&field3=";
  body += String(LightsensorValue); // Two decimal places
  body += "&field4=";
  body += String(MotionSensorValue); // Two decimal places
  body += "&field5=";
  body += String(millis() / 1000); // Two decimal places

  client.println("POST /update HTTP/1.1");
  client.println("Host: api.thingspeak.com");
  client.println("User-Agent: ESP8266 (nothans)/1.0");
  client.println("Connection: close");
  client.println("X-THINGSPEAKAPIKEY: " + writeAPIKey);
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.println("Content-Length: " + String(body.length()));
  client.println("");
  client.print(body);
}
