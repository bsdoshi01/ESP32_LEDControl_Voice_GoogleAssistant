#include <WiFi.h>
#include <WiFiClient.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#define ssid ""
#define password ""
#define AIO_SERVER "io.adafruit.com"
#define AIO_SERVERPORT 1883
#define AIO_USERNAME  ""
#define AIO_KEY       ""
#define LEDPIN 16

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);       
Adafruit_MQTT_Subscribe LED_Control = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/LED_Control");

void MQTT_connect() {
  int ret;
  if (mqtt.connected()) {
    return;
  }
  Serial.print("Connecting to MQTT... ");
  while ((ret = mqtt.connect()) != 0) {
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       delay(5000);
  }
  Serial.println("MQTT Connected!");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(100);
  pinMode(LEDPIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);
  
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected");

  mqtt.subscribe(&LED_Control);
}

void loop() {
  // put your main code here, to run repeatedly:
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  while((subscription = mqtt.readSubscription(5000))) {
    if(subscription == &LED_Control) {
      Serial.print(F("Got: "));
      Serial.println((char *)LED_Control.lastread);
      if (!strcmp((char*) LED_Control.lastread, "ON")) {
        digitalWrite(LEDPIN, HIGH);
      } else {
        digitalWrite(LEDPIN, LOW);
      }
    }
  }
}
