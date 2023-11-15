/*
Author  : Andrea Lombardo
Site    : https://www.lombardoandrea.com
Source  : https://github.com/AndreaLombardo/L298N/

Here you can see how to work in a common configuration.

L298NX2 is not a new version of module or IC, 
but it stands for a double implementation of L298N library.

With L298NX2 is possible to initialize two motors at once.

Speed range go from 0 to 255, default is 100.
Use setSpeed(speed) to change speed for both motors,
setSpeedA(speed) or setSpeedB(speed) for individual changes.

Sometimes at lower speed motors seems not running.
It's normal, may depends by motor and power supply.

Wiring schema in file "L298NX2 - Schema_with_EN_pin.png"
*/

// Include the (new) library
// Library: L298N By Andrea Lombardo
// https://github.com/AndreaLombardo/L298N
#include <L298NX2.h>

// Pin definition
const unsigned int EN_A = 10;
const unsigned int IN1_A = 9;
const unsigned int IN2_A = 8;

const unsigned int IN1_B = 7;
const unsigned int IN2_B = 6;
const unsigned int EN_B = 5;

// Initialize both motors
L298NX2 motors(EN_A, IN1_A, IN2_A, EN_B, IN1_B, IN2_B);

void setup() 
{
  Serial.begin(9600);

  // Wait for Serial Monitor to be opened
  while (!Serial) {
    //do nothing
  }

  // Set initial speed for both motors
  motors.setSpeed(80);
}

void loop() {
  motors.forward();
  delay(3000);
  motors.stop();
  delay(3000);

  motors.setSpeedA(50);
  motors.setSpeedB(400);
  motors.forward();
  delay(3000);
  motors.stop();
  delay(3000);

  printSomeInfo();digitalWrite(IN1_A, HIGH);

  motors.backwardA();
  motors.backwardB();
  delay(3000);
  motors.stop();
  delay(3000);
}

/*
Print some informations in Serial Monitor
*/
void printSomeInfo() {
  Serial.print("Motor A is moving = ");
  Serial.print(motors.isMovingA() ? "YES" : "NO");
  Serial.print(" at speed = ");
  Serial.println(motors.getSpeedA());
  Serial.print("Motor B is moving = ");
  Serial.print(motors.isMovingB() ? "YES" : "NO");
  Serial.print(" at speed = ");
  Serial.println(motors.getSpeedB());
}