// Define step and direction pins
const int stepPin = 13;
const int dirPin = 12;

// Define the steps per revolution for your stepper motor
const int stepsPerRevolution = 200;  // Adjust this value according to your motor specifications

// UART buffer
const int BUFFER_SIZE = 50; // Define a buffer size for incoming serial data
char inputBuffer[BUFFER_SIZE]; // Buffer to store the incoming serial data
int bufferIndex = 0; // Index to keep track of the buffer position


void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  // Set the step and direction pins as outputs
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);

  // Set the initial direction
  digitalWrite(dirPin, HIGH);  // Set direction to clockwise
  digitalWrite(stepPin, HIGH);

  Serial.begin(115200);
  Serial.print ("Test de motor y sensor de proximidad.");
}

#define CLOCKWISE       1
#define ANTICLOCKWISE   0

void loop() {
  if (Serial.available() > 0) { // Check if data is available to read
    char command = Serial.read(); // Read the incoming command
    Serial.print("Commando recibido: ");
    Serial.println(command);
    if (command == 'a') {
      stepMotor(CLOCKWISE,36,1);
    }
    if (command == 'b') {
      stepMotor(ANTICLOCKWISE,36,1);
    }

  } 
}

// arg direction 0=anticlockise 1=clockwise
void stepMotor (int direction, int angle, int speed) {
  digitalWrite(dirPin, direction);
  int steps = (stepsPerRevolution * angle) / 360;
  Serial.printf("StepMotor: dir=%d, angle=%d, speed=%d, steps=%d\r\n", direction, angle, speed, steps);
// Rotate the motor in one direction
  for (int i = 0; i < steps; i++) {
    // Generate a step pulse
    digitalWrite(stepPin, HIGH);
    //digitalWrite(LED_BUILTIN, HIGH);
    delayMicroseconds(1000/speed);  // Adjust delay for desired speed
    digitalWrite(stepPin, LOW);
    //digitalWrite(LED_BUILTIN, LOW);
    delayMicroseconds(1000/speed);  // Adjust delay for desired speed
  }
}
