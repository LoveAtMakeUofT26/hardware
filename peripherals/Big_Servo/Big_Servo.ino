#include <ESP32Servo.h>

Servo myservo;  // Create servo object

void setup() {
  // Attach servo to pin 9 and define min/max pulse width for accuracy
  myservo.attach(48, 600, 2300);
  myservo.write(0);  // Start at 0 degrees
}

void loop() {
  myservo.write(60);  // Move to 60 degrees
  delay(500);
  myservo.write(0);   // Return to start position
  delay(500);
}
