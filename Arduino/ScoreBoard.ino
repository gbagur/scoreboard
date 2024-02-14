/*
  BLEAdvertise

  This example creates a BLE peripheral which advertises itself as Artemis.
  Central devices may connect with it, but there are no services or 
  characteristics to interact with.

  Based on a stripped down version of the LED example from the ArduinoBLE examples
*/


//#include "led.ino"
//#include "buzzer.ino"
#include <ArduinoBLE.h> //http://librarymanager/All#ArduinoBLE_IoT

BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214"); // Bluetooth® Low Energy LED Service
BLEByteCharacteristic switchCharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLEWrite);

BLEService scoreService("45340637-dea7-48d7-9262-5f392e4317c6");
BLEUnsignedIntCharacteristic teamBScoreCharacteristic("81044b7b-7fb3-41e1-9127-a8730afd24ff", BLERead | BLEWrite);
BLEUnsignedIntCharacteristic teamAScoreCharacteristic("6f61f273-a5c5-4c6a-8744-8ef84a1484c4", BLERead | BLEWrite);

int sideTeamA;
int sideTeamB;
int scoreTeamA;
int scoreTeamB;

// the setup function runs once when you press reset or power the board
void setup() {

  sideTeamA = 0;
  sideTeamB = 0;
  scoreTeamA = 0;
  scoreTeamB = 1;

  // initialize digital pin LED_BUILTIN as an output.
  segmentInit();
  buzzerInit();
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  while (!Serial);
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName("Scoreboard");
  BLE.setAdvertisedService(scoreService);
  BLE.setAdvertisedService(ledService);

  // add the characteristic to the service
  ledService.addCharacteristic(switchCharacteristic);
  scoreService.addCharacteristic(teamAScoreCharacteristic);
  scoreService.addCharacteristic(teamBScoreCharacteristic);


  // add service
  BLE.addService(ledService);
  BLE.addService(scoreService);

  // set the initial value for the characeristic:
  switchCharacteristic.writeValue(0);
  
  teamAScoreCharacteristic.writeValue(0);
  teamBScoreCharacteristic.writeValue(0);

  // start advertising
  BLE.advertise();
  BLE.address();
  Serial.print ("Peripheral address: ");
  Serial.println(BLE.address());
  Serial.println("Setup finished.");
  scoreSet(sideTeamA,scoreTeamA);
  scoreSet(sideTeamB,scoreTeamB);
  buzzerPlayMelodyEndGame();

}

void loop() {
  // listen for BLE peripherals to connect:
  static int br=0;

  //digitSet(0, 1);
  BLEDevice central = BLE.central();
  // if a central is connected to peripheral:
  if (Serial.available() > 0) { // Check if data is available to read
    char command = Serial.read(); // Read the incoming command
    Serial.print("Commando recibido: ");
    Serial.println(command);
    if (command == 'a') {
      digitSet(0, 1);
      buzzerPlayMelodyEndGame();
    }
    if (command == 'b') {
      digitSet(0, 2);
      //analogWrite(buzzerPin, 400);
      buzzerChangeSide();
    }
    if (command == 'c') {
      digitSet(0, 3);
      buzzerRichMan();
      //analogWrite(buzzerPin, 0);
      
    }
  }  
  
  if (central) {
    //digitSet(0, 3);
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
    Serial.print("Device Name: ");
    Serial.println(central.deviceName());
    // while the central is still connected to peripheral...
    while (central.connected()) {
      //digitSet(0, 4);
      //digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
      //delay(100);                      // wait for a second
      //digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
      //delay(100);                      // wait for a second
      if (switchCharacteristic.written()) {
        if (switchCharacteristic.value()) {   // any value other than 0
          Serial.println("LED on");
          digitalWrite(LED_BUILTIN, HIGH);         // will turn the LED on
        } else {                              // a 0 value
          Serial.println(F("LED off"));
          digitalWrite(LED_BUILTIN, LOW);          // will turn the LED off
        }
        Serial.println("BLE: Build in LED data received");
      }
      if (teamAScoreCharacteristic.written()) {
        buzzerClick();
        scoreTeamA += teamAScoreCharacteristic.value()
        scoreSet(sideTeamA,scoreTeamA);
        Serial.println("BLE: team A score received");
      }
      if (teamBScoreCharacteristic.written()) {
        buzzerClick();
        scoreTeamB += teamBScoreCharacteristic.value()
        scoreSet(sideTeamB,scoreTeamB);
        Serial.println("BLE: team B score received");
        Serial.print("New score -> Team A: " + String(scoreTeamA) + "   Team B: "+ String(scoreTeamB));
      }
      if ((scoreTeamA+scoreTeamB)%7=0) changeSideCall();

    }
    //digitSet(0, 5);
    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}

changeSideCall() {
  buzzerDucks();
  sideTeamA = !sideTeamA;
  sideTeamB = !sideTeamB;
  scoreSet(sideTeamA,scoreTeamA);
  scoreSet(sideTeamB,scoreTeamB);
  buzzerDucks();
};



