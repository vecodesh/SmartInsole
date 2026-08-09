#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  delay(1000);

  sensors.begin();

  Serial.println();
  Serial.println("DS18B20 Temperature Test");
  Serial.println("------------------------");
}

void loop() {
  sensors.requestTemperatures();

  float temperatureC = sensors.getTempCByIndex(0);

  if (temperatureC == DEVICE_DISCONNECTED_C) {
    Serial.println("Sensor disconnected!");
  } 
  else {
    Serial.print("Temperature: ");
    Serial.print(temperatureC, 2);
    Serial.println(" °C");
  }

  delay(1000);
}