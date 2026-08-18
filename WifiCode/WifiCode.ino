#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// =====================================================
// Wi-Fi
// =====================================================

const char* ssid = "IOT";
const char* password = "123456789";

// =====================================================
// MQTT
// =====================================================

// Laptop IP address
const char* mqtt_server = "10.211.51.109";

const int mqtt_port = 1883;

const char* mqtt_topic = "smart-insole/sensors";

// =====================================================
// FSR
// =====================================================

#define FSR_PIN 34

const float VCC = 3.3;
const float R_FIXED = 10000.0;

// =====================================================
// DS18B20
// =====================================================

#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// =====================================================
// MQTT
// =====================================================

WiFiClient espClient;
PubSubClient client(espClient);


// =====================================================
// Wi-Fi connection
// =====================================================

void setup_wifi() {

  Serial.print("Connecting to Wi-Fi");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 30) {

    delay(500);
    Serial.print(".");
    attempts++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {

    Serial.println("Wi-Fi connected!");

    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.localIP());

    Serial.print("Signal Strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");

  } else {

    Serial.println("Wi-Fi connection FAILED!");
  }
}


// =====================================================
// MQTT connection
// =====================================================

void reconnect_mqtt() {

  while (!client.connected()) {

    Serial.print("Connecting to MQTT...");

    String clientID = "ESP32-SmartInsole-";

    clientID += String((uint32_t)ESP.getEfuseMac(), HEX);

    if (client.connect(clientID.c_str())) {

      Serial.println("connected!");

    } else {

      Serial.print("FAILED, state=");
      Serial.println(client.state());

      delay(2000);
    }
  }
}


// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  delay(1000);

  // FSR configuration
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  // DS18B20 configuration
  sensors.begin();

  // Wi-Fi
  setup_wifi();

  // MQTT
  client.setServer(mqtt_server, mqtt_port);

  Serial.println();
  Serial.println("====================================");
  Serial.println(" SMART INSOLE MQTT SENSOR SYSTEM");
  Serial.println("====================================");
}


// =====================================================
// LOOP
// =====================================================

void loop() {

  // Make sure MQTT is connected
  if (!client.connected()) {

    reconnect_mqtt();
  }

  client.loop();


  // ===================================================
  // READ FSR
  // ===================================================

  long sum = 0;

  for (int i = 0; i < 16; i++) {

    sum += analogRead(FSR_PIN);

    delayMicroseconds(200);
  }

  int raw = sum / 16;

  float voltage = (raw / 4095.0) * VCC;

  float r_fsr;

  if (voltage < 0.05) {

    r_fsr = 9999999;

  }
  else if (voltage > 3.25) {

    r_fsr = 50;

  }
  else {

    r_fsr = R_FIXED * (VCC / voltage - 1.0);
  }


  // ===================================================
  // READ TEMPERATURE
  // ===================================================

  sensors.requestTemperatures();

  float temperatureC = sensors.getTempCByIndex(0);


  // ===================================================
  // DISPLAY SENSOR DATA
  // ===================================================

  Serial.println("------------------------------------");

  Serial.print("FSR Raw        : ");
  Serial.println(raw);

  Serial.print("FSR Voltage    : ");
  Serial.print(voltage, 3);
  Serial.println(" V");

  Serial.print("FSR Resistance : ");

  if (r_fsr >= 9999999) {

    Serial.println("OPEN");

  } else {

    Serial.print(r_fsr, 0);
    Serial.println(" ohm");
  }

  Serial.print("Temperature    : ");

  if (temperatureC == DEVICE_DISCONNECTED_C) {

    Serial.println("Sensor disconnected!");

  } else {

    Serial.print(temperatureC, 2);
    Serial.println(" °C");
  }


  // ===================================================
  // CREATE JSON MESSAGE
  // ===================================================

  String json = "{";

  json += "\"pressure_raw\":";
  json += String(raw);

  json += ",\"pressure_voltage\":";
  json += String(voltage, 3);

  json += ",\"fsr_resistance\":";

  if (r_fsr >= 9999999) {

    json += "null";

  } else {

    json += String(r_fsr, 0);
  }

  json += ",\"temperature\":";

  if (temperatureC == DEVICE_DISCONNECTED_C) {

    json += "null";

  } else {

    json += String(temperatureC, 2);
  }

  json += "}";


  // ===================================================
  // PUBLISH TO MQTT
  // ===================================================

  if (client.publish(mqtt_topic, json.c_str())) {

    Serial.println("MQTT Published:");
    Serial.println(json);

  } else {

    Serial.println("MQTT Publish FAILED!");
  }


  delay(2000);
}