#include <WiFi.h>

const char* ssid = "IOT";
const char* password = "123456789";

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("ESP32 Wi-Fi Test");
  Serial.println("----------------");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("Wi-Fi connected!");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    Serial.print("Signal Strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");

  } else {

    Serial.println("Wi-Fi connection FAILED!");

    Serial.print("Wi-Fi status code: ");
    Serial.println(WiFi.status());

    Serial.println();
    Serial.println("Check:");
    Serial.println("1. Wi-Fi name");
    Serial.println("2. Wi-Fi password");
    Serial.println("3. Wi-Fi is 2.4 GHz");
    Serial.println("4. ESP32 is within Wi-Fi range");
  }
}

void loop() {
}