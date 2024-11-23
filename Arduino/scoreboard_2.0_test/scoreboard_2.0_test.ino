/*
  Scoreboard 2.0 - test software
*/

void setup() {
  leds_setup();
  power_setup();
  led_driver_setup();
  //leds_test();
  //led_driver_test();
}

// the loop function runs over and over again forever
void loop() {
  leds_test();
  led_driver_test2();
  led_driver_test();
}

