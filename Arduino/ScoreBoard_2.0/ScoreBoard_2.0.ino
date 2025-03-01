/*
  ScoreBoard
  
*/

#include <ArduinoBLE.h> //http://librarymanager/All#ArduinoBLE_IoT
#include <math.h>
#include "leds.h"

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

BLEService scoreService("45340637-dea7-48d7-9262-5f392e4317c6");
BLEUnsignedIntCharacteristic commandCharacteristic("81044b7b-7fb3-41e1-9127-a8730afd24ff", BLERead | BLEWrite);

int scoreSideLeft;
int scoreSideRight;
int LedsON = true;
int soundOn = true;
bool changeSideEnabled = false;

// the setup function runs once when you press reset or power the board
void setup() {
  leds_setup();       // 4 x 7-segments 
  rgbled(RGBLED_BLUE);
  power_setup();
  led_driver_setup();
  buzzerInit();

  Serial.begin(115200);
  while (!Serial);

  // begin initialization
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Scoreboard");
  BLE.setAdvertisedService(scoreService);
  
  // add the characteristic to the service
  scoreService.addCharacteristic(commandCharacteristic);

  // add service
  BLE.addService(scoreService);

  // set the initial value for the characeristic:
  commandCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();
  BLE.address();
  
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);  
  commandCharacteristic.setEventHandler(BLEWritten, commandCharacteristicWritten);
  
    Serial.print("Peripheral address: ");
    Serial.println(BLE.address());
    Serial.println("Setup finished.");
  
  
  rc_setup();

  scoreSideLeft = 88;
  scoreSideRight = 88;
  LedsON = true;
  scoreSet();
  buzzerClick();
  LedsON = false;
  scoreSideLeft = 0;
  scoreSideRight = 0;
  scoreSet();
  rgbled(RGBLED_RED);
}

int state = 0;  // 0 - disconnected ; 1 - connected

void loop() {
  static int note = 400;
  int analogValue;
  float voltage;
  float charge;
  int bat_ready;

  int rc_key_pressed = rc_check();
  if (rc_key_pressed) {
    #ifdef DEBUG
      Serial.print("RC Key pressed: ");
      Serial.println(rc_key_pressed);
    #endif
    buzzerClick();
  }
  
  switch (rc_key_pressed) {
    case 1:
      if (scoreSideLeft < 99) scoreSideLeft++;
      LedsON = true;
      break;
    case 3:
      if (scoreSideLeft > 0) scoreSideLeft--;
      LedsON = true;
      break;
    case 2:
      if (scoreSideRight < 99) scoreSideRight++;
      LedsON = true;
      break;
    case 4:
      if (scoreSideRight > 0) scoreSideRight--;
      LedsON = true;
      break;
    case 7:
      changeSideEnabled = false;
      scoreSideLeft = 0;
      scoreSideRight = 0;
    case 8:
      changeSideEnabled = true;
      scoreSideLeft = 0;
      scoreSideRight = 0;
    default: 
      break;
  }
  if (rc_key_pressed != 0) {
    scoreSet();
    #ifdef DEBUG
      Serial.println("New score -> Team A: " + String(scoreSideLeft) + "   Team B: "+ String(scoreSideRight));
    #endif
  }
  if (rc_key_pressed > 0 && rc_key_pressed < 5) {
    check_side_change();
  }

  BLE.poll();
  if (Serial.available() > 0) { // Check if data is available to read
    char command = Serial.read(); // Read the incoming command
    if (command == ' ' || command == '\n' || command == '\r') return;
    #ifdef DEBUG
      Serial.print("Commando recibido: ");
      Serial.println(command);
    #endif
    switch (command)
    {
      case 'a':
        playNote(note, 1000);
        note = note + 50;
        #ifdef DEBUG
          Serial.print("Freq: ");
          Serial.println(note);
        #endif
        break;    
      case 'b':
        playNote(note, 1000);
        note = note - 50;
        #ifdef DEBUG
          Serial.print("Freq: ");
          Serial.println(note);
        #endif
        break;
      case 'c': 
        {
          const int pwmPin = 18;
          const int sinResolution = 256;
          const float sinMax = 255.0;
          int dutyCycle;
          for (int i = 0; i<1200; i++) {
            dutyCycle = (sin(i * 2 * PI / sinResolution) + 1) * sinMax / 2;
            analogWrite(pwmPin, dutyCycle);
            delay(5);
          }
        }
        break;  
      case 'd':
        analogValue = analog_read();
        voltage = analogToVoltage(analogValue);
        charge = voltageToCharge(voltage);
        #ifdef DEBUG
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
        #endif
        break;
      default: 
        break;
    scoreSet();
    #ifdef DEBUG
      Serial.println("New score -> Team A: " + String(scoreSideLeft) + "   Team B: "+ String(scoreSideRight));
    #endif
    }
  }  
}

void changeSideCall(void) {
  buzzerDucks1();
  int temp = scoreSideLeft;
  scoreSideLeft = scoreSideRight;
  scoreSideRight = temp;
  scoreSet();
  buzzerDucks2();
};

void blePeripheralConnectHandler(BLEDevice central) {
  #ifdef DEBUG
    Serial.print("*Connected event, central: ");
    Serial.println(central.address());
  #endif
  buzzerBLEconnected();
  state = 1; 
  #ifdef DEBUG
    Serial.println(F("State: 1 - Connected "));
  #endif
}

void blePeripheralDisconnectHandler(BLEDevice central) {
  #ifdef DEBUG
    Serial.print("*Disconnected event, central: ");
    Serial.println(central.address());
  #endif
  state = 0; 
  #ifdef DEBUG
    Serial.println(F("State: 0 - Disocnnected"));
  #endif
  buzzerBLEdisconnected();
}

void commandCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
    buzzerClick();
    int value = commandCharacteristic.value();
    int command = value & 0xFF;
    int data = (value >> 8 ) & 0xFF;
    #ifdef DEBUG
      Serial.println("BLE Command received: "+ String(command));
      Serial.println("BLE data: "+ String(data));
    #endif
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
      soundOn = not (soundOn);
      if (!soundOn) buzzerSoundOff();
      if (soundOn) buzzerSoundOn();      
      break;
    default:
      break;
    }
    scoreSet();
    #ifdef DEBUG
      Serial.println("New score -> Team A: " + String(scoreSideLeft) + "   Team B: "+ String(scoreSideRight));
    #endif
    if (command==CMD_SIDE_A_INC || command==CMD_SIDE_B_INC) check_side_change();
}

void scoreSet() {
  set_score(scoreSideLeft, scoreSideRight);
  #ifdef DEBUG
    Serial.println("* Team A: " + String(scoreSideLeft) + "   Team B: "+ String(scoreSideRight));
  #endif
}

void check_side_change() {
    if (changeSideEnabled) {
        if ((scoreSideLeft+scoreSideRight)%7==0 && (scoreSideLeft+scoreSideRight>0)) 
        changeSideCall();
    }
}