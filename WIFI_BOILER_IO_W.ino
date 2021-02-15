#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define OUT_PIN_RELAY 5                                         // RELAY OUT D1 on board
#define LED 2                                                   // WIFI INDICATOR on board
int POWER_SUPPLY;                                               // Gloabal INT ON/OFF
#define WLAN_SSID       "WI-FISHKA"                             // Название точки WIFI
#define WLAN_PASS       "**********"                             // Ваш пароль
#define AIO_SERVER      "io.adafruit.com"                       // Сервер IP/URL
#define AIO_SERVERPORT  1883                                    // Порт 1883, для SSL 8883
#define AIO_USERNAME    "******"                               // Ваш догин
#define AIO_KEY         "aio_*****************************"      // Ваш ключ
#define UPDATE_TIME_INDICATOR 15000                             // Update Indicator R/G
int indicator_status;                                           // Gloabal INT indicator status
unsigned long timing;
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");
Adafruit_MQTT_Publish indicatroRG = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/indicator");
void MQTT_connect();
// Начало говнокода
void setup() {
  pinMode(OUT_PIN_RELAY, OUTPUT);
  pinMode(LED, OUTPUT);
  Serial.begin(115200); delay(100);
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); WIFI_CONNECT_WAIT(); Serial.print(".");
  }
  Serial.println(); Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  mqtt.subscribe(&onoffbutton); // подписка на кнопку
  digitalWrite(OUT_PIN_RELAY, LOW); digitalWrite(LED, LOW);
  delay(5000);
}
void loop() {
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription; // Подписка
  while ((subscription = mqtt.readSubscription(1000))) {
    // Check if its the onoff button feed
    if (subscription == &onoffbutton) {
      Serial.print(F("On-Off button: "));
      Serial.println((char *)onoffbutton.lastread);
      if (strcmp((char *)onoffbutton.lastread, "ON") == 0) {
        POWER_SUPPLY = 1;
        Serial.println(F("MQTT = Button ON"));
        delay(200);
        update_indicator();
      }
      else if (strcmp((char *)onoffbutton.lastread, "OFF") == 0) {
        POWER_SUPPLY = 0;
        Serial.println(F("MQTT = Button OFF"));
        delay(200);
        update_indicator();
      }
    }
  }
  if (POWER_SUPPLY == 1) {
    digitalWrite(OUT_PIN_RELAY, HIGH);
  }
  if (POWER_SUPPLY == 0) {
    digitalWrite(OUT_PIN_RELAY, LOW);
  }
  if (millis() - timing > UPDATE_TIME_INDICATOR) { // Публик
    timing = millis();
    update_indicator();
  }
  if (! mqtt.ping()) {
    mqtt.disconnect();
  }
}
void update_indicator()
{
  if (POWER_SUPPLY == 1) {
    indicator_status = 0;
  }
  if (POWER_SUPPLY == 0) {
    indicator_status = 1;
  }
  if (!indicatroRG.publish(indicator_status)) {
    Serial.println(F("Update indicator R/G - FAIL"));
  }
}
void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 15 seconds...");
    mqtt.disconnect();
    MQTT_CONNECT_RETR();
    delay(15000);  // wait 15 seconds
    retries--;
    if (retries == 0) {
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
  MQTT_CONNECT_OK();
}
void MQTT_CONNECT_OK()
{
  digitalWrite(LED, LOW); delay(40);
  digitalWrite(LED, HIGH); delay(40);
  digitalWrite(LED, LOW); delay(40);
  digitalWrite(LED, HIGH); delay(40);
  digitalWrite(LED, LOW); delay(40);
  digitalWrite(LED, LOW); delay(40);
  digitalWrite(LED, HIGH); delay(40);
  digitalWrite(LED, LOW); delay(40);
  digitalWrite(LED, HIGH); delay(40);
  digitalWrite(LED, LOW);
}
void MQTT_CONNECT_RETR()
{
  digitalWrite(LED, LOW); delay(100);
  digitalWrite(LED, HIGH); delay(100);
  digitalWrite(LED, LOW); delay(100);
  digitalWrite(LED, HIGH); delay(100);
  digitalWrite(LED, LOW);
}
void WIFI_CONNECT_WAIT()
{
  digitalWrite(LED, LOW); delay(300);
  digitalWrite(LED, HIGH); delay(300);
  digitalWrite(LED, LOW); delay(300);
  digitalWrite(LED, HIGH); delay(300);
  digitalWrite(LED, LOW);
}
