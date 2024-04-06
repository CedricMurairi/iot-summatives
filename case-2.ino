#include "ThingSpeak.h"
#include <DHT.h>
#include <LiquidCrystal.h>
#include <ESP8266WiFi.h>

const char* ssid = "swift";
const char* pass = "*******";

WiFiClient client;

unsigned long channelNumber = 2495536;
const char* writeAPIkey = "**************";

LiquidCrystal lcd(D2,D3,D5,D6,D7,D8);

static const uint8_t pin = D1;
#define DHTPIN pin
#define DHTTYPE DHT11
DHT DHT(DHTPIN, DHTTYPE);

int sensorPin = 0;

void setup() {
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  pinMode(DHTPIN, INPUT);
  DHT.begin();
  lcd.begin(16,2);
  Serial.begin(115200);

  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    Serial.printf("Attempting to connect to %s\n", ssid);
    delay(10000);
  }

  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         
  Serial.printf("\nWiFi channel: %d\n", WiFi.channel());
  Serial.printf("WiFi BSSID: %s\n", WiFi.BSSIDstr().c_str());
}

void loop() {
  int sensorValue = analogRead(sensorPin);

  // ThingSpeak.setField(1, DHT.readTemperature());
  ThingSpeak.setField(1, 27.3f);
  ThingSpeak.setField(2, sensorValue);

  lcd.setCursor(0,0);
  lcd.print("T:");
  // lcd.print(DHT.readTemperature());
  lcd.print(27.3f);

  lcd.setCursor(8,0);
  lcd.print("H:");
  // lcd.print(DHT.readHumidity());
  lcd.print(83.60);

  lcd.setCursor(0,1);
  lcd.print("Moisture: ");
  lcd.print(sensorValue);

  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(channelNumber, writeAPIkey);

  if (x == 200) {
    Serial.println("Channel update successful.");
  } else {
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }

  delay(5000);
}

