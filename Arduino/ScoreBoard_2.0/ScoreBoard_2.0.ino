/*
  ScoreBoard
  
  Based on a stripped down version of the LED example from the ArduinoBLE examples
*/

#include <ArduinoBLE.h> //http://librarymanager/All#ArduinoBLE_IoT
#include <math.h>

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

#define check_side_change() {if ((scoreSideLeft+scoreSideRight)%7==0 && (scoreSideLeft+scoreSideRight>0)) changeSideCall();}

BLEService scoreService("45340637-dea7-48d7-9262-5f392e4317c6");
BLEUnsignedIntCharacteristic commandCharacteristic("81044b7b-7fb3-41e1-9127-a8730afd24ff", BLERead | BLEWrite);

//int sideTeamA;
int scoreSideLeft;
int scoreSideRight;
int LedsON = HIGH;
int soundOn = HIGH;

// the setup function runs once when you press reset or power the board
void setup() {
  leds_setup();       // 4 x 7-segments 
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
  //scoreService.addCharacteristic(teamBScoreCharacteristic);

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
  
  Serial.print ("Peripheral address: ");
  Serial.println(BLE.address());
  Serial.println("Setup finished.");
  
  scoreSideLeft = 88;
  scoreSideRight = 88;
  LedsON = HIGH;
  scoreSet();
  //buzzerStartMelody();
  buzzerClick();
  LedsON = LOW;
  scoreSideLeft = 0;
  scoreSideRight = 0;
  scoreSet();
}

int state = 0;  // 0 - disconnected ; 1 - connected

void loop() {
  // listen for BLE peripherals to connect:
  static int note = 400;
  int analogValue;
  float voltage;
  float charge;
  int bat_ready;
  BLE.poll();

  if (Serial.available() > 0) { // Check if data is available to read
    char command = Serial.read(); // Read the incoming command
    if (command == ' ' || command == '\n' || command == '\r') return;
    Serial.print("Commando recibido: ");
    Serial.println(command);
    switch (command)
    {
      case 'a':
        //buzzerPlayMelodyEndGame();
        playNote(note, 1000);
        note = note + 50;
        Serial.print("Freq: ");
        Serial.println(note);
        break;    
      case 'b':
        playNote(note, 1000);
        note = note - 50;
        Serial.print("Freq: ");
        Serial.println(note);
        break;
      case 'c': 
        {
          const int pwmPin = 18; // Choose any PWM pin
          const int sinResolution = 256; // Number of points on the sine wave
          const float sinMax = 255.0; // Maximum value of the sine wave
          int dutyCycle;
          for (int i = 0; i<1200; i++) {
            dutyCycle = (sin(i * 2 * PI / sinResolution) + 1) * sinMax / 2;
            analogWrite(pwmPin, dutyCycle);
            delay(5); // Adjust delay for desired frequency
          }
        }
        break;  
      case 'd':
        analogValue = analog_read();
        voltage = analogToVoltage(analogValue);
        charge = voltageToCharge(voltage);
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
      default: 
        break;
    }
  }  
  
}

void changeSideCall(void) {
  buzzerDucks1();
  //sideTeamA = !sideTeamA;
  int temp = scoreSideLeft;
  scoreSideLeft = scoreSideRight;
  scoreSideRight = temp;
  scoreSet();
  buzzerDucks2();
};

void blePeripheralConnectHandler(BLEDevice central) {
  // central connected event handler
  Serial.print("*Connected event, central: ");
  Serial.println(central.address());
  buzzerBLEconnected();
  state = 1; 
  Serial.println(F("State: 1 - Connected "));
 
}

void blePeripheralDisconnectHandler(BLEDevice central) {
  // central disconnected event handler
  Serial.print("*Disconnected event, central: ");
  Serial.println(central.address());
  state = 0; 
  Serial.println(F("State: 0 - Disocnnected"));
  buzzerBLEdisconnected();
}

void commandCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  
    buzzerClick();
    int value = commandCharacteristic.value();
    int command = value & 0xFF;
    int data = (value >> 8 ) & 0xFF;
    Serial.println("BLE Command received: "+ String(command));
    Serial.println("BLE data: "+ String(data));
    switch (command) {
    case CMD_SIDE_A_INC:
      if (scoreSideLeft < 99) scoreSideLeft++;
      LedsON = HIGH;
      break;
    case CMD_SIDE_A_DEC:
      if (scoreSideLeft > 0) scoreSideLeft--;
      LedsON = HIGH;
      break;
    case CMD_SIDE_B_INC:
      if (scoreSideRight < 99) scoreSideRight++;
      LedsON = HIGH;
      break;
    case CMD_SIDE_B_DEC:
      if (scoreSideRight > 0) scoreSideRight--;
      LedsON = HIGH;
      break;
    case CMD_SIDE_A_SET:
      scoreSideLeft = data;
      LedsON = HIGH;
      break;
    case CMD_SIDE_B_SET:
      scoreSideRight = data;
      LedsON = HIGH;
      break;          
    case CMD_RESET_COUNTERS:
      scoreSideLeft = 0;
      scoreSideRight = 0;
      break;
    case CMD_DISPLAY_ON:
      LedsON = HIGH;
      break;
    case CMD_DISPLAY_OFF:
      LedsON = LOW;
      break;
    case CMD_DISPLAY_ON_OFF:
      LedsON = !LedsON;
      break;
    case CMD_CHANGE_SIDES:                 
      changeSideCall();
      break;
    case CMD_SOUND_ON_OFF:
      soundOn = not (soundOn);
      if (soundOn == LOW) buzzerSoundOff();
      if (soundOn == HIGH) buzzerSoundOn();      
    default:
      break;
    }
    scoreSet();
    Serial.println("New score -> Team A: " + String(scoreSideLeft) + "   Team B: "+ String(scoreSideRight));
    if (command==CMD_SIDE_A_INC || command==CMD_SIDE_B_INC) check_side_change();
}

void scoreSet() {
  set_score(scoreSideLeft, scoreSideRight);
  Serial.println("* Team A: " + String(scoreSideLeft) + "   Team B: "+ String(scoreSideRight));
}