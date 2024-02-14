#define __ASSERT_USE_STDERR

#include <assert.h>


int seg[4][7] = { {14, 25, 24, 35, 4, 8, 10},
                  {27, 28, 32, 12, 13, 23, 22},
                  {0, 1, 2, 45, 41, 43, 42},
                  {16, 33, 34, 11, 29, 31, 17}
                };


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

void segmentSet(int segment_pin, int value) {
  if (value == 1) pinMode(segment_pin, OUTPUT);
  if (value == 0) pinMode(segment_pin, INPUT);
}

void digitSet(int digit, int number) {
  if (digit <0 || digit >4) return;
  for (int segment_nr=0;segment_nr<7;segment_nr++) {
    segmentSet(seg[digit][segment_nr], digit_matrix[number][segment_nr]);
  }
}

void digitOff(int digit) {digitSet(digit, 10); }

void segmentSetOnly(int digit, int segment) {
  digitOff(digit);
  segmentSet(seg[digit][segment], HIGH);
}

void scoreSet (int side, int score) {
  int MIN_VALUE = 0;
  int MAX_VALUE = 99;
 
  if (score == 100) {   // Blank the score on that side
    digitSet(0 + side*2, 10);
    digitSet(1 + side*2, 10);
  }
  if (score < MIN_VALUE || score > MAX_VALUE) return; 
  if (side < 0 || side > 1) return;

  int unit = score % 10; // Extracting the unit by taking the remainder when divided by 10
  int decena = score / 10; // Extracting the decena by integer division by 10
  digitSet(0 + side*2, unit);
  digitSet(1 + side*2, decena);
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

void digitNibbleExample2(int digit) {
  // Circle nibble slowing down
  static float wait_time = 1;
  wait_time = 1000;
    for (int i=0;i<7;i++){
      segmentSetOnly(digit, i);
      delay(wait_time); 
    }
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
