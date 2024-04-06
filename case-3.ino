#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <LiquidCrystal.h>
#include <ThingSpeak.h>

#define WLAN_SSID       "swift"
#define WLAN_PASS       "********"

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "CedricMurairiM"
#define AIO_KEY         "**********************"

#define LIGHT_SENSOR_PIN A0
#define LED_PIN          D0
#define LCD_RS_PIN       D1
#define LCD_EN_PIN       D2
#define LCD_D4_PIN       D4
#define LCD_D5_PIN       D5
#define LCD_D6_PIN       D6
#define LCD_D7_PIN       D7

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe switchButton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/toggle-light");

unsigned long myChannelNumber = 2498813;
const char * myWriteAPIKey = "5XGEEMJ3CUBEE54F";

LiquidCrystal lcd(LCD_RS_PIN, LCD_EN_PIN, LCD_D4_PIN, LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN);

void connectWiFi();
void connectMQTT();
void setupLCD();
void processMQTTMessages();

void setup() {
  Serial.begin(115200);
  connectWiFi();
  ThingSpeak.begin(client);
  pinMode(LED_PIN, OUTPUT);
  setupLCD();
  mqtt.subscribe(&switchButton);
}

void loop() {
  connectMQTT();
  processMQTTMessages();
  int lightIntensity = analogRead(LIGHT_SENSOR_PIN);
  lcd.setCursor(7, 0);
  lcd.print(" ");
  lcd.setCursor(7, 0);
  lcd.print(lightIntensity);
  lcd.print(" lux");
  if (!mqtt.publish(AIO_USERNAME "/feeds/light-intensity", String(lightIntensity).c_str())) {
    Serial.println("Failed to publish light intensity value!");
  }
  ThingSpeak.writeField(myChannelNumber, 1, lightIntensity, myWriteAPIKey);
  delay(6000);
}

void connectWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("WiFi Connected!");
}

void connectMQTT() {
  if (mqtt.connected()) {
    return;
  }
  int8_t ret;
  while ((ret = mqtt.connect()) != 0) {
    mqtt.disconnect();
    delay(3000);
  }
}

void processMQTTMessages() {
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &switchButton) {
      int switchState = atoi((char *)switchButton.lastread);
      Serial.print("Received MQTT message: ");
      Serial.println(switchState);
      digitalWrite(LED_PIN, switchState == 1 ? HIGH : LOW);
    }
  }
}

void setupLCD() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Light:");
}
