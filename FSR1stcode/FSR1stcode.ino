#define FSR_PIN 34              // GPIO34

const float VCC     = 3.3;      // ESP32 3.3 V
const float R_FIXED = 10000.0;  // Your 10 kΩ resistor

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);               // 0–4095
  analogSetAttenuation(ADC_11db);         // Full 0–3.3 V range

  Serial.println("FSR Test - 10k resistor");
  Serial.println("Raw ADC | Voltage (V) | Approx R_FSR (ohm)");
  Serial.println("-------------------------------------------");
}

void loop() {
  // Average 16 samples to reduce noise
  long sum = 0;
  for (int i = 0; i < 16; i++) {
    sum += analogRead(FSR_PIN);
    delayMicroseconds(200);
  }
  int raw = sum / 16;

  float voltage = (raw / 4095.0) * VCC;

  // Calculate approximate FSR resistance
  float r_fsr;
  if (voltage < 0.05) {
    r_fsr = 9999999;          // almost open circuit
  } else if (voltage > 3.25) {
    r_fsr = 50;               // very low resistance (high force / saturated)
  } else {
    r_fsr = R_FIXED * (VCC / voltage - 1.0);
  }

  // Print results
  Serial.print(raw);
  Serial.print("\t\t");
  Serial.print(voltage, 3);
  Serial.print("\t\t");
  Serial.println((long)r_fsr);

  delay(150);   // sampling rate ≈ 6–7 Hz (change if needed)
}