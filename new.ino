#include <servo.h>

Servo speedController;
// ---------- USER SETTINGS ---------- //////////////////////
int flyTime = 60 ; // Flight time, sec //
int delayCutOff = 10; // Delay before shutdown, sec //
int idle = 1000; // Zero gas value //
int fullPower = 2000; // Maximum power //
///////////////////////////////////////////////// ///////////////////////////////
int throttle;

void setup() {
pinMode(A0, INPUT_PULLUP); // Button
input speedController.attach(3,1000,2000); // Signal to the controller from digital output 2, minimum value, maximum value //

if (!digitalRead(A0)) { // If the
calibration() button is pressed when turned on; // Switch to calibration mode
}
}

void loop() {
throttle = idle;
speedController.writeMicroseconds(idle);
delay(1000);

if (!digitalRead(A0)) {
btnHold();
}
}

// ---------- Check if the button is held down ----------
void btnHold() {
int time2go = 3; // How long to hold the button, sec.
float timePast = 0.0; // Time counter

while (!digitalRead(A0)) {
delay(100); // 0.1 sec interval
timePast += 0.1;
if (timePast >= time2go) {
motorStart();
}
}

}

void motorStart() {
while (throttle < fullPower) {
speedController.writeMicroseconds(throttle++);
delay(10);
}

flyingTimer();
}

// ---------- Flight mode ----------
void flyingTimer() {
int timePast = 0;

while(timePast < flyTime) {
delay(1000);
timePast++;
}

alarm(); // Warning
delay(delayCutOff * 1000); // Delay before shutdown
speedController.writeMicroseconds(idle);
}

// ---------- Shutdown warning ----------
void alarm() {
speedController.writeMicroseconds(fullPower * 0.75); //1500 µs//
delay(500);
speedController.writeMicroseconds(fullPower * 0.85); //1700 µs//
delay(500);
speedController.writeMicroseconds(fullPower * 0.75); //1500 µs//
delay(500);
speedController.writeMicroseconds(fullPower * 0.85); //1700 µs//
delay(500);
speedController.writeMicroseconds(fullPower); //2000 µs//
}

// ---------- Calibration of the operating range of the regulator ----------
void calibration() {
digitalWrite(13, HIGH);
speedController.writeMicroseconds(fullPower);
delay(3000);
speedController.writeMicroseconds(idle);
digitalWrite(13, LOW);

while (!digitalRead(A0)) { // If the button is pressed
digitalWrite(13, HIGH); // don't let exit
delay(100); // and blink the LED
digitalWrite(13, LOW);
delay(100);
}

}
