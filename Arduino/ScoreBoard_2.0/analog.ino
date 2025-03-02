#define ANALOG_PIN  35
#define ADC_RESOLUTION 1024 
#define VREF 2.0 // Reference voltage in volts

const float v_low = 9;   // lowest battery voltage
static float v_high = 12.4; // highest battery voltage 

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

void update_battery_indication() {
  int analogValue = analog_read();
  float voltage = analogToVoltage(analogValue);
  float charge = voltageToCharge(voltage);
  if (charge < 35) rgbled(RGBLED_RED);
  else if (charge < 50) rgbled(RGBLED_YELLOW);
  else rgbled(RGBLED_GREEN);
}
