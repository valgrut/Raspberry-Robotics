#include <L298NX2.h>

const unsigned int EN_A = 7;
const unsigned int IN1_A = 10;
const unsigned int IN2_A = 8;

const unsigned int IN1_B = 9;
const unsigned int IN2_B = 20;
const unsigned int EN_B = 6;

L298NX2 motors(EN_A, IN1_A, IN2_A, EN_B, IN1_B, IN2_B);


// Pin 20 = rotace do prava

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600); //115200

  while (!Serial) {
    //do nothing
  }

  // initialize digital pin LED_BUILTIN as an output.
  //ledcAttach(servo1, 12000, 8);  // 12 kHz PWM, 8-bit resolution
  //pinMode(LED_BUILTIN, OUTPUT);
  //pinMode(21, OUTPUT);

  //pinMode(servo1, OUTPUT);

  // Set initial speed for both motors
  motors.setSpeed(100);
  //motors.setSpeedA(0);
  //motors.setSpeedB(10);
}

// A = chuze - funguje 
// B = rotace

void walk()
{
  //digitalWrite(LED_BUILTIN, HIGH);
  motors.forwardA();
  delay(1000);
  motors.backwardA();
  delay(1000);
  //digitalWrite(LED_BUILTIN, LOW);
}

// the loop function runs over and over again forever
void loop() {
  motors.setSpeed(100);

  for (int i = 0; i < 5; i++)
    walk();
  
  motors.forwardB();
  delay(1000);
  motors.stopB();
}
