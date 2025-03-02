#include <RCSwitch.h>

// Constants
const unsigned long DEBOUNCE_DELAY = 100;          // Minimum time between signals in milliseconds
const int LONG_PRESS_THRESHOLD = 10;              // Number of repeated signals for long press
const int RECEIVER_PIN = 33;                      // Pin number for RF receiver
const long KEY_1_CODE = 16736113;                // RF code for key 1
const long KEY_2_CODE = 16761569;                // RF code for key 2
const long KEY_3_CODE = 16736114;                // RF code for key 3
const long KEY_4_CODE = 16736120;                // RF code for key 4
const int KEY_1_VALUE = 1;                       // Return value for key 1
const int KEY_2_VALUE = 2;                       // Return value for key 2
const int KEY_3_VALUE = 3;                       // Return value for key 3
const int KEY_4_VALUE = 4;                       // Return value for key 4
const int LONG_PRESS_OFFSET = 4;                 // Offset added to key values for long press

// Global objects
RCSwitch mySwitch = RCSwitch();

// Static variables (file scope)
static long lastReceivedValue = 0;
static long currentReceivedValue = 0;
static int repeatCount = 0;

/**
 * Initialize the RC receiver
 */
void rc_setup() {
    pinMode(RECEIVER_PIN, INPUT);
    mySwitch.enableReceive(digitalPinToInterrupt(RECEIVER_PIN));
}

/**
 * Check for RC signals and return the corresponding key code
 * Returns 0 if no valid key press detected
 * Returns 1-4 for short presses
 * Returns 5-8 for long presses
 */
int rc_check() {
    int keyPressed = 0;
    bool isLongPress = false;
    unsigned long currentTime = millis();
    static unsigned long lastCheckTime = 0;
    
    // Debounce check
    if ((currentTime - lastCheckTime) < DEBOUNCE_DELAY) {
        return 0;
    }
    lastCheckTime = currentTime;

    // Check if new signal is available
    if (!mySwitch.available()) {
        // if (lastReceivedValue != 0)
        //     Serial.println("set 0");
        lastReceivedValue = 0;
        return 0;
    }

    // Process new received value
    currentReceivedValue = mySwitch.getReceivedValue();


    mySwitch.resetAvailable();

    // Handle repeated signals
    if (currentReceivedValue == lastReceivedValue) {
        repeatCount++;
        if (repeatCount == LONG_PRESS_THRESHOLD) {
            isLongPress = true;
            repeatCount = 0;
        } else {
            return 0;
        }
    } else {
        repeatCount = 0;
    }
    // Serial.print(repeatCount);
    // Serial.print(" / ");
    // Serial.print(currentReceivedValue );
    // Serial.print(" / ");
    // Serial.print(lastReceivedValue );
    // Serial.print(" / ");
    // Serial.println(isLongPress );
        

    lastReceivedValue = currentReceivedValue;

    // Map received values to key codes
    switch (currentReceivedValue) {
        case KEY_1_CODE:
            keyPressed = KEY_1_VALUE;
            break;
        case KEY_2_CODE:
            keyPressed = KEY_2_VALUE;
            break;
        case KEY_3_CODE:
            keyPressed = KEY_3_VALUE;
            break;
        case KEY_4_CODE:
            keyPressed = KEY_4_VALUE;
            break;
        default:
            keyPressed = 0;
    }

    // Adjust key code for long press
    if (isLongPress && keyPressed > 0) {
        keyPressed += LONG_PRESS_OFFSET;
    }


#ifdef rx_debug
    // Debug output
    Serial.print("Received Decimal: ");
    Serial.print(currentReceivedValue);
    Serial.print(" / Binary: ");
    Serial.print(currentReceivedValue, BIN);
    Serial.print(" / Hexadecimal: 0x");
    Serial.print(currentReceivedValue, HEX);
    Serial.print(" / ");
    Serial.print(mySwitch.getReceivedBitlength());
    Serial.print(" bit Protocol: ");
    Serial.println(mySwitch.getReceivedProtocol());
    Serial.print(" / Key pressed: ");
    Serial.print(keyPressed);
#endif

    return keyPressed;
}