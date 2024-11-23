// leds/buzzer pinout
#define D_LED1 27
#define D_LED2 28
#define D_LED3 38
#define D_LED4 42
#define D_LED5 43
#define D_LED6 44
#define D_RGBLED_R 13
#define D_RGBLED_G 11
#define D_RGBLED_B 12
#define LEDS_COUNT 9

const int led_order[] = { D_LED1, D_LED2, D_LED3, D_LED4, D_LED5, D_LED6, D_RGBLED_R, D_RGBLED_G, D_RGBLED_B };

void leds_setup() {
  for (int i = 0; i < LEDS_COUNT; i++) {
    pinMode(led_order[i], OUTPUT);
    digitalWrite(led_order[i], LOW);
  }
}

void leds_test () {
  for (int i = 0; i < LEDS_COUNT; i++) {
    digitalWrite(led_order[i], HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);                         // wait for a second
    digitalWrite(led_order[i], LOW);  // turn the LED off by making the voltage LOW
    delay(1);                           // wait for a second
  }
}