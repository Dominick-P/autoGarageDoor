#include "Ultrasonic.h"
#include <IRremote.h>

// Ultrasonic sensors
Ultrasonic inGarageSensor(27, 14);
Ultrasonic outerGarageSensor(0, 4);

// IR receiver
const int IRPin = 16;
IRrecv irrecv(IRPin);
decode_results results;

// LED pins
const int ledRed = 25;   // Red LED for 'closed' status
const int ledGreen = 26; // Green LED for 'open' status

// Garage door status
bool isDoorOpen = false;

// Timing constants
const long openDoorTime = 2 * 60 * 100; // 12 seconds
unsigned long doorOpenedTime = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  // Initialize IR receiver
  irrecv.enableIRIn();

  // Initialize LED pins
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);

  // Check the initial status of the garage door
  if (outerGarageSensor.read() <= 20) {
    // Garage door is closed
    isDoorOpen = false;
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledGreen, LOW);
  } else {
    // Garage door is open
    isDoorOpen = true;
    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen, HIGH);
  }
}

void loop() {
  int distanceIG = inGarageSensor.read();
  int distanceOG = outerGarageSensor.read();

  // Check if IR command received
  if (irrecv.decode(&results)) {
    Serial.println(results.value);

    if (results.value == 0xffffffff) {
      Serial.println("IR open command received!");
      openGarageDoor();
    }
    irrecv.resume(); // Receive the next value
  }

  // Check to close the door after 2 minutes if the car is still inside
  if (isDoorOpen && millis() - doorOpenedTime > openDoorTime && distanceIG < 150) {
    closeGarageDoor();
  }

  delay(1000);
}

void openGarageDoor() {
  Serial.println("Opening garage door...");

  isDoorOpen = true;
  doorOpenedTime = millis();

  // Update LED status
  digitalWrite(ledRed, LOW);
  digitalWrite(ledGreen, HIGH);

  // Add code to physically open the door
}

void closeGarageDoor() {
  if (outerGarageSensor.read() > 300) { // Check if outer sensor detects approaching car
    Serial.println("Closing garage door...");
    isDoorOpen = false;

    // Update LED status
    digitalWrite(ledRed, HIGH);
    digitalWrite(ledGreen, LOW);

    // Add code to physically close the door
  } else {
    Serial.println("Closing aborted due to approaching vehicle");
  }
}
