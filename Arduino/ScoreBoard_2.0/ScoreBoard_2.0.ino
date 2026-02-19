/*
  ScoreBoard
  
  Production/Debug Versioning:
  - Define DEBUG_SERIAL to enable Serial monitoring and commands.
  - Undefine DEBUG_SERIAL for Production (avoids blocking on startup).
*/

#include <ArduinoBLE.h> // http://librarymanager/All#ArduinoBLE_IoT
#include <math.h>
#include "leds.h"

// --- CONFIGURATION ---
// Comment this line out for PRODUCTION mode
// #define DEBUG_SERIAL 

#ifdef DEBUG_SERIAL
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
#endif

// button in
#define BUTTON_IN           8

// List of commands
#define CMD_SIDE_A_INC      1
#define CMD_SIDE_A_DEC      2
#define CMD_SIDE_B_INC      3
#define CMD_SIDE_B_DEC      4
#define CMD_SIDE_A_SET      5
#define CMD_SIDE_B_SET      6
#define CMD_DISPLAY_ON      7
#define CMD_DISPLAY_OFF     8
#define CMD_RESET_COUNTERS  10
#define CMD_DISPLAY_ON_OFF  11
#define CMD_CHANGE_SIDES    12
#define CMD_SOUND_ON_OFF    13
#define CMD_SOUND_ON        14
#define CMD_SOUND_OFF       15

#define SLEEP_TIME          (5 * 60 * 1000)
#define ALARM_TIME          (20 * 60 * 1000)

// BLE Services and Characteristics
BLEService scoreService("45340637-dea7-48d7-9262-5f392e4317c6");
BLEUnsignedIntCharacteristic commandCharacteristic("81044b7b-7fb3-41e1-9127-a8730afd24ff", BLERead | BLEWrite);

// Battery Service (Standard BLE)
BLEService batteryService("180F");
BLEUnsignedCharCharacteristic batteryLevelChar("2A19", BLERead | BLENotify);

int game_mode = 1;  // 1 - volleyball 2-tennis
#define LAST_GAME_MODE        2
#define GAME_MODE_VOLLEYBALL  1
#define GAME_MODE_TENNIS      2
#define TENNIS_SCORE_ADVANTAGE 100

int scoreSideLeft;
int scoreSideRight;
int setSideLeft = 0;
int setSideRight = 0;
int LedsON = true;
int soundOn = true;
bool changeSideEnabled = false;
bool debug_battery_level = false;
unsigned long currentTime;
unsigned long lastUseTime;
int state = 0;  // 0 - disconnected ; 1 - connected
bool alarm = false;

// Function Prototypes
void scoreSet();
void check_side_change();
void blePeripheralConnectHandler(BLEDevice central);
void blePeripheralDisconnectHandler(BLEDevice central);
void commandCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic);
void updateBLEBattery();

void setup() {
  pinMode(BUTTON_IN, INPUT);
  debug_battery_level = (digitalRead(BUTTON_IN) == 0);
  
  leds_setup();       // 4 x 7-segments 
  rgbled(RGBLED_BLUE);
  power_setup();
  led_driver_setup();
  buzzerInit();

#ifdef DEBUG_SERIAL
  Serial.begin(115200);
  // This blocks the code until a serial monitor is opened. 
  // Great for debugging, bad for production/battery use.
  while (!Serial); 
#endif

  // begin initialization
  if (!BLE.begin()) {
    DEBUG_PRINTLN("Starting BLE failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Scoreboard");
  BLE.setAdvertisedService(scoreService);
  
  // add the characteristic to the service
  scoreService.addCharacteristic(commandCharacteristic);
  batteryService.addCharacteristic(batteryLevelChar);

  // add services
  BLE.addService(scoreService);
  BLE.addService(batteryService);

  // set the initial value for the characteristics:
  commandCharacteristic.writeValue(0);
  batteryLevelChar.writeValue(100);

  // start advertising
  BLE.advertise();
  
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);  
  commandCharacteristic.setEventHandler(BLEWritten, commandCharacteristicWritten);
  
  DEBUG_PRINT("Peripheral address: ");
  DEBUG_PRINTLN(BLE.address());
  DEBUG_PRINTLN("Setup finished.");

  scoreSideLeft = 88;
  scoreSideRight = 88;
  LedsON = true;
  scoreSet();
  buzzerClick();
  LedsON = false;
  scoreSideLeft = 0;
  scoreSideRight = 0;
  scoreSet();
  currentTime = millis();
  lastUseTime = currentTime;
}

void loop() {
  static int button_in_pre;
  int button_in;
  static int note = 400;
  static unsigned long lastCheckTime = 0;
  static int debug_led_state = HIGH;

  currentTime = millis();

  // Power indication update and BLE Battery Update
  if ((currentTime - lastCheckTime) > 1000) {
    updateBLEBattery();
    lastCheckTime = currentTime;
    if (debug_battery_level) {
      LedsON = true;
      update_battery_indication();
      digitalWrite(D_LED4, isBatteryReady());
      return;
    }

  }

  if (LedsON == true && (currentTime - lastUseTime) > SLEEP_TIME) {
    LedsON = false;
    scoreSet();
    buzzerClick();
  }

  // Button in
  button_in = digitalRead(BUTTON_IN);
  if (button_in == LOW && button_in_pre == HIGH) {
    DEBUG_PRINTLN("Button pressed.");
    lastUseTime = currentTime;
    game_mode = game_mode + 1;
    if (game_mode > LAST_GAME_MODE) game_mode = 1;
    digitalWrite(D_LED1, HIGH);
    if (game_mode == 2) digitalWrite(D_LED2, HIGH);
    if (game_mode == 3) digitalWrite(D_LED3, HIGH);
    buzzerClick();
    delay(700);
    digitalWrite(D_LED1, LOW);
    digitalWrite(D_LED2, LOW);
    digitalWrite(D_LED3, LOW);
  }
  button_in_pre = button_in;


  
  BLE.poll();

#ifdef DEBUG_SERIAL
  if (Serial.available() > 0) {
    char command = Serial.read();
    if (command == ' ' || command == '\n' || command == '\r') return;
    
    DEBUG_PRINT("Commando recibido: ");
    DEBUG_PRINTLN(command);

    switch (command) {
      case 'a':
        playNote(note, 1000);
        note = note + 50;
        DEBUG_PRINT("Freq: ");
        DEBUG_PRINTLN(note);
        break;    
      case 'b':
        playNote(note, 1000);
        note = note - 50;
        DEBUG_PRINT("Freq: ");
        DEBUG_PRINTLN(note);
        break;
      case 'd': {
        int analogValue = analog_read();
        float voltage = analogToVoltage(analogValue);
        float charge = voltageToCharge(voltage);
        Serial.print("Analog value on pin 35: ");
        Serial.println(analogValue);
        Serial.print("Voltage: ");
        Serial.print(voltage);
        Serial.println(" V");
        Serial.print("Charge: ");
        Serial.print(charge);
        Serial.println(" %");
        Serial.print("Charge complete: ");
        Serial.println(isBatteryReady());
        break;
      }
      default: 
        break;
    }
  }
#endif
}

void updateBLEBattery() {
  // Read battery status using existing hardware abstraction functions
  int analogValue = analog_read();
  float voltage = analogToVoltage(analogValue);
  int charge = (int)voltageToCharge(voltage);
  
  // Constrain to 0-100 range for BLE standard compliance
  if (charge > 100) charge = 100;
  if (charge < 0) charge = 0;
  
  batteryLevelChar.writeValue((uint8_t)charge);
}

void changeSideCall(void) {
  buzzerDucks1();
  int temp = scoreSideLeft;
  scoreSideLeft = scoreSideRight;
  scoreSideRight = temp;
  scoreSet();
  buzzerDucks2();
}

void blePeripheralConnectHandler(BLEDevice central) {
  DEBUG_PRINT("*Connected event, central: ");
  DEBUG_PRINTLN(central.address());
  
  buzzerBLEconnected();
  state = 1; 
  DEBUG_PRINTLN(F("State: 1 - Connected "));
}

void blePeripheralDisconnectHandler(BLEDevice central) {
  DEBUG_PRINT("*Disconnected event, central: ");
  DEBUG_PRINTLN(central.address());
  
  state = 0; 
  DEBUG_PRINTLN(F("State: 0 - Disconnected"));
  buzzerBLEdisconnected();
}

void commandCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  lastUseTime = currentTime;
  buzzerClick();
  int value = commandCharacteristic.value();
  int command = value & 0xFF;
  int data = (value >> 8) & 0xFF;

  DEBUG_PRINT("BLE Command received: ");
  DEBUG_PRINTLN(command);
  DEBUG_PRINT("BLE data: ");
  DEBUG_PRINTLN(data);

  switch (command) {
    case CMD_SIDE_A_INC:
      if (scoreSideLeft < 99) scoreSideLeft++;
      LedsON = true;
      break;
    case CMD_SIDE_A_DEC:
      if (scoreSideLeft > 0) scoreSideLeft--;
      LedsON = true;
      break;
    case CMD_SIDE_B_INC:
      if (scoreSideRight < 99) scoreSideRight++;
      LedsON = true;
      break;
    case CMD_SIDE_B_DEC:
      if (scoreSideRight > 0) scoreSideRight--;
      LedsON = true;
      break;
    case CMD_SIDE_A_SET:
      scoreSideLeft = data;
      LedsON = true;
      break;
    case CMD_SIDE_B_SET:
      scoreSideRight = data;
      LedsON = true;
      break;          
    case CMD_RESET_COUNTERS:
      scoreSideLeft = 0;
      scoreSideRight = 0;
      break;
    case CMD_DISPLAY_ON:
      LedsON = true;
      break;
    case CMD_DISPLAY_OFF:
      LedsON = false;
      break;
    case CMD_DISPLAY_ON_OFF:
      LedsON = !LedsON;
      break;
    case CMD_CHANGE_SIDES:                 
      changeSideCall();
      break;
    case CMD_SOUND_ON:
      soundOn = true;
      buzzerSoundOn();      
      break;
    case CMD_SOUND_OFF:
      soundOn = false;
      buzzerSoundOff();
      break;
    case CMD_SOUND_ON_OFF:
      soundOn = !soundOn;
      if (!soundOn) buzzerSoundOff();
      else buzzerSoundOn();      
      break;
    default:
      break;
  }
  scoreSet();

  DEBUG_PRINT("New score -> Team A: ");
  DEBUG_PRINT(scoreSideLeft);
  DEBUG_PRINT("   Team B: ");
  DEBUG_PRINTLN(scoreSideRight);

  if (command == CMD_SIDE_A_INC || command == CMD_SIDE_B_INC) check_side_change();
}

void scoreSet() {
  set_score(scoreSideLeft, scoreSideRight);
  DEBUG_PRINT("* Team A: ");
  DEBUG_PRINT(scoreSideLeft);
  DEBUG_PRINT("   Team B: ");
  DEBUG_PRINTLN(scoreSideRight);
}

void check_side_change() {
  if (changeSideEnabled && (game_mode == GAME_MODE_VOLLEYBALL)) {
    if ((scoreSideLeft + scoreSideRight) % 7 == 0 && (scoreSideLeft + scoreSideRight > 0)) 
      changeSideCall();
  }
}

void increase_tennis_score(int * score) {
  if (*score == 0) *score = 15;
  else if (*score == 15) *score = 30;
  else if (*score == 30) *score = 40;
  else if (*score == 40) *score = TENNIS_SCORE_ADVANTAGE;
}

void decrease_tennis_score(int * score) {
  if (*score == 15) *score = 0;
  else if (*score == 30) *score = 15;
  else if (*score == 40) *score = 30;
  else if (*score == TENNIS_SCORE_ADVANTAGE) *score = 40;
}