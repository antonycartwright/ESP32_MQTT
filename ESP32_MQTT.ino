#include <WiFi.h>
#include <MQTT.h>
#include <SimpleDHT.h>

const char ssid[] = "fgfg";
const char pass[] = "fgfg";

WiFiClient net;
MQTTClient client;

int pinDHT11 = 17;
SimpleDHT11 dht11(pinDHT11);

unsigned long lastMillis = 0;

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect("client", "user", "pass")) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);

  client.begin("iot.reyax.com", net);

  connect();
}

void loop() {

  client.loop();
  delay(10);

  if (!client.connected()) {
    connect();
  }

  if (millis() - lastMillis > 15000) {
    lastMillis = millis();

    byte temperature = 0;
    byte humidity = 0;
    int err = SimpleDHTErrSuccess;
    if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
      Serial.print("Read DHT11 failed, err=");
      Serial.print(SimpleDHTErrCode(err));
      Serial.print(",");
      Serial.println(SimpleDHTErrDuration(err));
      return;
    }

    Serial.print((int)temperature);
    Serial.print(" *C, ");
    Serial.print((int)humidity);
    Serial.println(" H");

    client.publish("/temp", String(temperature));
    client.publish("/hum", String(humidity));
  }
}
