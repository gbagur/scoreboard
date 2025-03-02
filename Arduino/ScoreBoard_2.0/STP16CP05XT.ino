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
uint8_t digit_code[13] = {
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
    0b00000000,  // 10 OFF
    0b01110111,  // 11 'A'
    0b01011110   // 12 'd'
};

#define LITERAL_A 11
#define LITERAL_D 12

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

uint32_t score2binary(uint16_t side_left_score, uint16_t side_right_score) {
    uint8_t unit_left;
    uint8_t dec_left;
    uint8_t unit_right;
    uint8_t dec_right;
    if (side_left_score > 100) side_left_score = 101; // Limit score 
    if (side_left_score < 0) side_left_score = 0; // Limit score 
    if (side_right_score > 100) side_right_score = 101; // Limit score 
    if (side_right_score < 0) side_right_score = 0; // Limit score
    if (side_left_score == 100) {
      unit_left = digit_code[LITERAL_D];
      dec_left = digit_code[LITERAL_A];      
    } else {
      unit_left = digit_code[side_left_score % 10];
      dec_left = digit_code[side_left_score / 10];
      if (dec_left == digit_code[0]) dec_left = digit_code[10];  // if decen = 0 then turn it off (10)
    }
    if (side_right_score == 100) {
      unit_right = digit_code[LITERAL_D];
      dec_right = digit_code[LITERAL_A];    
    } else {
      unit_right = digit_code[side_right_score % 10];
      dec_right = digit_code[side_right_score / 10];
      if (dec_right == digit_code[0]) dec_right = digit_code[10];
    }
    //Serial.println("scores ---> " + String(dec_left) + String(unit_left) + " " + String(dec_right) + String(unit_right));
    return (unit_right << 24) | (dec_right << 16) | (unit_left << 8) | (dec_left);
}

void score_draw (uint32_t binary) {
  // enable chip
  digitalWrite(enablePin_left, HIGH);
  digitalWrite(enablePin_right, HIGH);
  
  // send 16-bit data
  for (int i = 31; i >= 0; i--) {
      digitalWrite(dataPin, (binary >> i) & 0x01);
      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
  }

  // latch the content to the output
  digitalWrite(latchPin_left, HIGH);
  digitalWrite(latchPin_right, HIGH);  
  digitalWrite(latchPin_left, LOW);
  digitalWrite(latchPin_right, LOW);

  // disable the chip
  digitalWrite(enablePin_left, LOW);
  digitalWrite(enablePin_right, LOW);
}

void set_score(uint16_t side_left_score, uint16_t side_right_score) {
  uint32_t score_binary = score2binary(side_left_score, side_right_score);
  //Serial.println("*score_binary: " + String(score_binary));
  if (LedsON == LOW) {
      led_driver_turn_all_off();
  } else {
    score_draw(score_binary);
  }
}

void led_driver_turn_all_off() {
  score_draw(0);
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
