#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

#define DHTPIN 5
#define DHTTYPE DHT11
#define SOIL_PIN 34
#define LDR_PIN 35
#define RELAY_PIN 27

const char* ssid = "{wifi_ssid_here}";
const char* password = "{password_here}";
const char* serverUrl = "http://{put_your_ip_here}:3000/api/data";

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(SOIL_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
}

void loop() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();
  int soilValue = analogRead(SOIL_PIN);
  int ldrValue = analogRead(LDR_PIN);

  int soilScore = constrain(map(soilValue, 1000, 3000, 0, 100), 0, 100);
  int tempScore = constrain(map(temp, 25, 40, 0, 100), 0, 100);
  int humScore  = constrain(map(hum, 80, 30, 0, 100), 0, 100);
  int ldrScore  = constrain(map(ldrValue, 0, 3000, 0, 100), 0, 100);

  float finalScore = soilScore * 0.4 + tempScore * 0.2 + humScore * 0.2 + ldrScore * 0.2;

  if (finalScore > 50) {
    digitalWrite(RELAY_PIN, LOW);
  } else {
    digitalWrite(RELAY_PIN, HIGH);
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

String json = "{\"temperature\":\"" + String(temp) + "\",\"humidity\":\"" + String(hum) +
              "\",\"soil\":\"" + String(4095-soilValue) + "\",\"light\":\"" + String(4095-ldrValue) +
              "\",\"score\":\"" + String(finalScore) + "\"}";


    int httpResponseCode = http.POST(json);
    http.end();
  }

  delay(5000);
}
