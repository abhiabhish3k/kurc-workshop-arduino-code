#include <Servo.h>

// ==================================================
// SERVO CONFIGURATION
// ==================================================

// Arduino pin connected to the servo signal wire
const int SERVO_PIN = 9;

// Create a Servo object
Servo myServo;


// ==================================================
// SETUP
// ==================================================

void setup() {

  // Attach the servo to the selected Arduino pin
  myServo.attach(SERVO_PIN);

  // Move servo to the starting position
  // Change 90 to any value between 0 and 180
  myServo.write(90);

  delay(1000);
}


// ==================================================
// MAIN LOOP
// ==================================================

void loop() {

  // ------------------------------------------------
  // Move servo from 0° to 180°
  // ------------------------------------------------

  for (int angle = 0; angle <= 180; angle++) {

    myServo.write(angle);

    // Small delay controls how fast the servo moves
    delay(15);
  }


  // ------------------------------------------------
  // Move servo from 180° back to 0°
  // ------------------------------------------------

  for (int angle = 180; angle >= 0; angle--) {

    myServo.write(angle);

    // Increase this value for slower movement
    delay(15);
  }
}
