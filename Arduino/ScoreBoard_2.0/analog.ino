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
  charge = 29.41 * voltage - 264.68;  // convert Voltage to Battery charge 
  if (charge < 0) charge = 0;
  if (charge > 100) charge = 100;
  return charge;
}


// float voltage_read () {
//   return 0.15407854984894259818731117824773 * analog_read()/;

// }