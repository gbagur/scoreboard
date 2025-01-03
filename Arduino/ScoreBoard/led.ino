#define __ASSERT_USE_STDERR

#include <assert.h>


int seg[4][7] = { {14, 25, 24, 35, 4, 8, 10},
                  {27, 28, 32, 12, 13, 23, 22},
                  {0, 1, 2, 45, 41, 43, 42},
                  {16, 33, 34, 11, 29, 31, 17}
                };

#define serial_data_pin     29
#define serial_clock_pin    11

int digit_matrix[11][7] = {
        {1, 1, 1, 1, 1, 1, 0}, // 0
        {0, 1, 1, 0, 0, 0, 0}, // 1
        {1, 1, 0, 1, 1, 0, 1}, // 2
        {1, 1, 1, 1, 0, 0, 1}, // 3
        {0, 1, 1, 0, 0, 1, 1}, // 4
        {1, 0, 1, 1, 0, 1, 1}, // 5
        {1, 0, 1, 1, 1, 1, 1}, // 6
        {1, 1, 1, 0, 0, 0, 0}, // 7
        {1, 1, 1, 1, 1, 1, 1}, // 8
        {1, 1, 1, 1, 0, 1, 1},  // 9
        {0, 0, 0, 0, 0, 0, 0}  // 10 = CLEAN
    };

void segmentInit(void) {
  for (int j=0;j<2;j++){
    for (int i=0;i<7;i++){
      pinMode(seg[j][i], INPUT);
      digitalWrite(seg[j][i], LOW);
    }
  }
}

void openDrainSet(int pin, int value) {
  //digitalWrite(pin, LOW);
  if (value == 1) pinMode(pin, OUTPUT);
  if (value == 0) pinMode(pin, INPUT);
}

void segmentSet(int segment_pin, int value) {
  openDrainSet(segment_pin, OUTPUT);
}

void digitSet(int digit, int number) {
  if (digit <0 || digit >4) return;
  if (digit<3) {
    for (int segment_nr=0;segment_nr<7;segment_nr++) {
      segmentSet(seg[digit][segment_nr], digit_matrix[number][segment_nr]);
    }
  }
  else if (digit==3) {
    //shift register
    for (int bit=0;bit<8;bit++) {
      openDrainSet(serial_data_pin, bit);
      openDrainSet(serial_clock_pin, 0);
      delay(100);       
      openDrainSet(serial_clock_pin, 1);
      delay(100);       
    }    
  }
}

void digitOff(int digit) {digitSet(digit, 10); }

void segmentSetOnly(int digit, int segment) {
  digitOff(digit);
  segmentSet(seg[digit][segment], HIGH);
}

void scoreSetSide(int side, int score) {
  int MIN_VALUE = 0;
  int MAX_VALUE = 99;
 
  if (LedsON == LOW) {   // Blank the score on that side
    digitSet(0 + side*2, 10);
    digitSet(1 + side*2, 10);
    return;
  }
  if (score < MIN_VALUE || score > MAX_VALUE) return; 
  if (side < 0 || side > 1) return;

  int unit = score % 10; // Extracting the unit by taking the remainder when divided by 10
  int decena = score / 10; // Extracting the decena by integer division by 10
  digitSet(0 + side*2, unit);
  digitSet(1 + side*2, decena);
}

void scoreSet(void) {
  scoreSetSide(1,scoreSideLeft);
  scoreSetSide(0,scoreSideRight);
}

/// EXAMPLES for test ////

void FullCountDownExample() {
  // Blink countdown
  static float wait_time = 1;
  wait_time = 500;
    for (int i=0;i<11;i++){
      for (int digit=0;digit<4;digit++){
        digitSet(digit, i);
        delay(wait_time); 
      }
    }
}

void digitCountDownExample(int digit) {
  // Blink countdown
  static float wait_time = 1;
  wait_time = 400;
    int n = 2;
    for (int i=0;i<11;i++){
      for (int j=0;j<n;j++){
        digitSet(digit, i);
        delay(wait_time/2/n); 
        // digitOff(digit);
        // delay(wait_time/2/n); 
      }
    }
}

void digitNibbleExample2(int digit) {
  // Circle nibble slowing down
  static float wait_time = 1;
  static int a = 0;
  wait_time = 1000;
  
    a = !a;
    for (int i=0;i<6;i++){
      segmentSetOnly(digit, i);
      delay(wait_time); 
    }
  delay(wait_time);   
  
}

void digitNibbleExample3(void) {
  // Circle nibble slowing down
  static float wait_time = 1;
  static int a = 0;
  wait_time = 200;
    for (int i=0;i<7;i++){
      segmentSetOnly(0, i);
      segmentSetOnly(1, i);
      segmentSetOnly(2, i);
      segmentSetOnly(3, i);
      delay(wait_time); 
    }
}

void digitNibbleExample(int digit) {
  // Circle nibble slowing down
  static float wait_time = 1;
  static int a = 0;
  wait_time = 1;
  do {
    a = !a;
    for (int i=0;i<6;i++){
      segmentSetOnly(digit, i);
      delay(wait_time); 
    }
    wait_time = wait_time * 1.4;
  }  while (wait_time<250); 
}

// handle diagnostic informations given by assertion and abort program execution:
void __assert(const char *__func, const char *__file, int __lineno, const char *__sexp) {
    // transmit diagnostic informations through serial link. 
    Serial.println(__func);
    Serial.println(__file);
    Serial.println(__lineno, DEC);
    Serial.println(__sexp);
    Serial.flush();
    // abort program execution.
    abort();
}
