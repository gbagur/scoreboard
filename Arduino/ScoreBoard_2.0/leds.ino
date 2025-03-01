#include "leds.h"

const int led_order[] = { D_LED1, D_LED2, D_LED3, D_LED4, D_LED5, D_LED6, D_RGBLED_R, D_RGBLED_G, D_RGBLED_B };

void leds_setup() {
  for (int i = 0; i < LEDS_COUNT; i++) {
    pinMode(led_order[i], OUTPUT);
    digitalWrite(led_order[i], LOW);
  }
}

void rgbled(int color) {
  digitalWrite(D_RGBLED_R, color & RGBLED_RED);
  digitalWrite(D_RGBLED_G, color & RGBLED_GREEN);
  digitalWrite(D_RGBLED_B, color & RGBLED_BLUE);
}


void leds_test () {
  for (int i = 0; i < LEDS_COUNT; i++) {
    digitalWrite(led_order[i], HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);                         // wait for a second
    digitalWrite(led_order[i], LOW);  // turn the LED off by making the voltage LOW
    delay(1);                           // wait for a second
  }
}