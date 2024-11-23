#define SIDE_LEFT   0
#define SIDE_RIGHT  1

// Pin definitions
const int dataPin = 15;  // SDI: Serial Data Input
const int clockPin = 17; // CLK: Clock Input
const int latchPin_left = 18;  // LE: Latch Enable 
const int enablePin_left = 19; // !OE: Output Enable
const int latchPin_right = 10; // LE/: Latch Enable
const int enablePin_right = 9;  // !OE: Output Enable

// Digit codes for 7-segment displays 
uint8_t digit_code[11] = {
  //  XGFEDCBA
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b00000000  // OFF
};

void sendDataToSTP16CP05(int display, int data_i) {
    int latchPin = (display == 0) ? latchPin_left : latchPin_right;
    int enablePin = (display == 0) ? enablePin_left : enablePin_right;

    // enable chip
    digitalWrite(enablePin, HIGH);
    
    // send 16-bit data
    for (int i = 15; i >= 0; i--) {
        digitalWrite(dataPin, (data_i >> i) & 0x01);
        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
    }

    // latch the content to the output
    digitalWrite(latchPin, HIGH);
    digitalWrite(latchPin, LOW);

    // disable the chip
    digitalWrite(enablePin, LOW);
}

void led_driver_turn_all_off() {
    sendDataToSTP16CP05(0, 0x0000);
    sendDataToSTP16CP05(1, 0x0000);
}
void led_driver_setup() {
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(latchPin_left, OUTPUT);
    pinMode(latchPin_right, OUTPUT);
    pinMode(enablePin_left, OUTPUT);
    pinMode(enablePin_right, OUTPUT);

    digitalWrite(dataPin, LOW);
    digitalWrite(latchPin_left, LOW);
    digitalWrite(latchPin_right, LOW);
    digitalWrite(enablePin_left, LOW);
    digitalWrite(enablePin_right, LOW);
    digitalWrite(clockPin, LOW);

    led_driver_turn_all_off();
}

void set_score_side(int side, int score) {
    if (score > 99) score = 99; // Limit score to 99
    if (score < 0) score = 0; // Limit score to 99

    if (LedsON == LOW) {   // Blank the score on that side
      led_driver_turn_all_off();
    } else {
      uint8_t unit = digit_code[score % 10];
      uint8_t dec = digit_code[score / 10];
      if (dec == digit_code[0]) dec = digit_code[10];
      sendDataToSTP16CP05(side, (unit << 8) | dec);
    }
}

void set_score(int side_left_score, int side_right_score) {
  set_score_side(SIDE_LEFT, side_left_score);
  set_score_side(SIDE_RIGHT, side_right_score);
}

void led_driver_test() {
    int score;
    for (int i = 0; i < 10; i++) {
        score = i * 10 + i;
        set_score(score, score);
        delay(500);
    }
    led_driver_turn_all_off();
}

void led_driver_test2() {
    for (int i = 0; i < 16; i++) {
        uint16_t ledData = 1 << i; // Set one bit high
        sendDataToSTP16CP05(0, ledData);
        sendDataToSTP16CP05(1, ledData);
        delay(400); // Wait 200ms
    }
    led_driver_turn_all_off();
}
