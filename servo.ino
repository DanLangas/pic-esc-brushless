#include <Servo.h>
Servo myservo;  // create servo object to control a servo
 
void setup() {
  pinMode(10, OUTPUT);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
}
void loop() {
  digitalWrite(10, HIGH);
  myservo.write(60);                  // sets the servo position according to the scaled value
  delay(500);                           // waits for the servo to get there
  digitalWrite(10, LOW);
  myservo.write(120);                  // sets the servo position according to the scaled value
  delay(500);                           // waits for the servo to get there
}
