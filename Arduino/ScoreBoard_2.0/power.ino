// power pinout
#define D_8V_ENABLE           3
#define D_3V3_POWER_ENABLE    22
#define D_CHARGE              41

void power_setup() {
  pinMode(D_8V_ENABLE, OUTPUT);
  pinMode(D_3V3_POWER_ENABLE, OUTPUT);
  pinMode(D_CHARGE, INPUT);

  digitalWrite(D_8V_ENABLE, LOW);
  digitalWrite(D_3V3_POWER_ENABLE, HIGH);
}

bool isBatteryReady() {
  return digitalRead(D_CHARGE);
}