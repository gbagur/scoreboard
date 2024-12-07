#define ANALOG_PIN  35
#define ADC_RESOLUTION 1024 
#define VREF 2.0 // Reference voltage in volts

void analog_setup() {
  pinMode(ANALOG_PIN, INPUT);
}

int analog_read () {
  int a = analogRead(ANALOG_PIN);
  return a;
}

float analogToVoltage(int raw_value) {
  return (raw_value * 0.01344);  // convert ADC pin voltage to Battery voltage 
}

float voltageToCharge(float voltage) {
  float charge;
  float v_low = 9;
  static float v_high = 12.4;
  float m;
  m = 100 / (v_high - v_low);
  charge = m * (voltage - v_low);
  if (charge < 0) charge = 0;
  if (charge > 100) {
    Serial.println("High_voltage_updated:");
    Serial.print("Old v_high: ");
    Serial.print(v_high);
    Serial.println(" V");
    v_high = voltage;
    Serial.print("New v_high: ");
    Serial.print(v_high);
    Serial.println(" V");
    charge = 100;
  }
  return charge;
}
