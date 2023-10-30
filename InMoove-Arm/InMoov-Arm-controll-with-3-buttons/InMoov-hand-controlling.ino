#include <PCA9685.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x41);

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
// Watch video V1 to understand the two lines below: http://youtu.be/y8X9X10Tn1k
#define SERVOMIN  125 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  575 // this is the 'maximum' pulse length count (out of 4096)

// our servo # counter
uint8_t servonum = 0;

enum SERVOS {
  PALEC = 3,
  UKAZOVAK = 0,
  PROSTREDNIK = 2,
  PRSTENIK = 4,
  MALIK = 1,
  ZAPESTI = 5,
};

enum FINGER_RANGES {
  PALEC_MIN = 105,
  PALEC_MAX = 220,

  UKAZOVAK_MIN = 115,
  UKAZOVAK_MAX = -10,

  PROSTREDNIK_MIN = 95,
  PROSTREDNIK_MAX = 215,

  PRSTENIK_MIN = 100,
  PRSTENIK_MAX = 220,

  MALIK_MIN = 95,
  MALIK_MAX = 215,

  ZAPESTI_MIN = -30,
  ZAPESTI_STRED = 0,
  ZAPESTI_MAX = 80
};

int current_PALEC_angle = PALEC_MIN;
int current_UKAZOVAK_angle = UKAZOVAK_MIN;
int current_PROSTREDNIK_angle = PROSTREDNIK_MIN;
int current_PRSTENIK_angle = PRSTENIK_MIN;
int current_MALIK_angle = MALIK_MIN;
int current_ZAPESTI_angle = ZAPESTI_STRED;

// constants won't change. They're used here to set pin numbers:
const int buttonUpPin = 2;  // the number of the pushbutton pin
const int buttonDownPin = 7;  // the number of the pushbutton pin
const int buttonChangePin = 4;  // the number of the pushbutton pin

// variables will change:
int buttonChangeState = 0;  // variable for reading the pushbutton status
int buttonUpState = 0;  // variable for reading the pushbutton status
int buttonDownState = 0;  // variable for reading the pushbutton status

unsigned int active_servo = 0;
bool servo_change_enabled = true;

void setup() {
  Serial.begin(9600);
  Serial.println("16 channel Servo test!");

  pwm.begin();
  
  pwm.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  // initialize the pushbutton pin as an input:
  pinMode(buttonUpPin, INPUT);
  pinMode(buttonDownPin, INPUT);
  pinMode(buttonChangePin, INPUT);
  //yield();
}

int changeActiveServo() {
  active_servo += 1;
  if(active_servo > 5) {
    active_servo = 0;
  }
}

const char* get_active_finger(int active_servo)
{
  const char* current_finger = "";
  switch(active_servo)
  {
    case 0:
      current_finger = "PALEC";
      break;
    case 1:
      current_finger = "UKAZOVAK";
      break;
    case 2:
      current_finger = "PROSTREDNIK";
      break;
    case 3:
      current_finger = "PRSTENIK";
      break;
    case 4:
      current_finger = "MALIK";
      break;
    case 5:
      current_finger = "ZAPESTI";
      break;
  }

  return current_finger;
}

int update_active_finger(int active_servo, int angle_increment)
{
  int active_finger_angle = 0;
  switch(active_servo)
  {
    case 0:
      current_PALEC_angle += angle_increment;
      if (current_PALEC_angle < PALEC_MIN)
      {
        current_PALEC_angle = PALEC_MIN;
      } 
      else if (current_PALEC_angle > PALEC_MAX)
      {
        current_PALEC_angle = PALEC_MAX;
      }
      active_finger_angle = current_PALEC_angle;
      pwm.setPWM(PALEC, 0, angleToPulse(current_PALEC_angle));
      break;
    case 1: //Pozor, ukazovacek ma uhly naopak!
      current_UKAZOVAK_angle += angle_increment;
      if (current_UKAZOVAK_angle > UKAZOVAK_MIN)
      {
        current_UKAZOVAK_angle = UKAZOVAK_MIN;
      } 
      else if (current_UKAZOVAK_angle < UKAZOVAK_MAX)
      {
        current_UKAZOVAK_angle = UKAZOVAK_MAX;
      }
      active_finger_angle = current_UKAZOVAK_angle;
      pwm.setPWM(UKAZOVAK, 0, angleToPulse(current_UKAZOVAK_angle));
      break;
    case 2:
      current_PROSTREDNIK_angle += angle_increment;
      if (current_PROSTREDNIK_angle < PROSTREDNIK_MIN)
      {
        current_PROSTREDNIK_angle = PROSTREDNIK_MIN;
      } 
      else if (current_PROSTREDNIK_angle > PROSTREDNIK_MAX)
      {
        current_PROSTREDNIK_angle = PROSTREDNIK_MAX;
      }
      active_finger_angle = current_PROSTREDNIK_angle;
      pwm.setPWM(PROSTREDNIK, 0, angleToPulse(current_PROSTREDNIK_angle));
      break;
    case 3:
      current_PRSTENIK_angle += angle_increment;
      if (current_PRSTENIK_angle < PRSTENIK_MIN)
      {
        current_PRSTENIK_angle = PRSTENIK_MIN;
      } 
      else if (current_PRSTENIK_angle > PRSTENIK_MAX)
      {
        current_PRSTENIK_angle = PRSTENIK_MAX;
      }
      active_finger_angle = current_PRSTENIK_angle;
      pwm.setPWM(PRSTENIK, 0, angleToPulse(current_PRSTENIK_angle));
      break;
    case 4:
      current_MALIK_angle += angle_increment;
      if (current_MALIK_angle < MALIK_MIN)
      {
        current_MALIK_angle = MALIK_MIN;
      } 
      else if (current_MALIK_angle > MALIK_MAX)
      {
        current_MALIK_angle = MALIK_MAX;
      }
      active_finger_angle = current_MALIK_angle;
      pwm.setPWM(MALIK, 0, angleToPulse(current_MALIK_angle));
      break;
    case 5:
      current_ZAPESTI_angle += angle_increment;
      if (current_ZAPESTI_angle < ZAPESTI_MIN)
      {
        current_ZAPESTI_angle = ZAPESTI_MIN;
      } 
      else if (current_ZAPESTI_angle > ZAPESTI_MAX)
      {
        current_ZAPESTI_angle = ZAPESTI_MAX;
      }
      active_finger_angle = current_ZAPESTI_angle;
      pwm.setPWM(ZAPESTI, 0, angleToPulse(current_ZAPESTI_angle));
      break;
  }

  return active_finger_angle;
}

bool first_run = true;

// the code inside loop() has been updated by Robojax
void loop() {

  //watch video for details: https://youtu.be/bal2STaoQ1M
  //for(int i=0; i<16; i++)
  //{
  //  for(int angle = 0; angle < 181; angle += 10){
  //    delay(50);
  //      pwm.setPWM(i, 0, angleToPulse(angle));
  //      // see YouTube video for details (robojax) 
  //  }
  //}

  if(first_run)
  {
    set_default_hand_gesture();
    first_run = false;
  }

  // read the state of the pushbutton value:
  buttonChangeState = digitalRead(buttonChangePin);
  buttonUpState = digitalRead(buttonDownPin);
  buttonDownState = digitalRead(buttonUpPin);

  // Prepnuti aktivniho serva (check if the pushbutton is pressed. If it is, the buttonState is HIGH)
  if (buttonChangeState == HIGH) {
    Serial.print("Change Button is Pressed");
    Serial.println();
    if(servo_change_enabled) {
      changeActiveServo();
      Serial.print("New active servo: "); Serial.print(active_servo);
      Serial.println();
      servo_change_enabled = false;
    }
  } else {
    //Serial.print("Not pressed");
    //Serial.println();
    servo_change_enabled = true;
  }


  // Ovladani aktivniho serva
  if(buttonUpState == HIGH)
  {
    int active_finger_angle = update_active_finger(active_servo, 5);
    
    Serial.print("[UP] Angle of :"); 
    Serial.print(get_active_finger(active_servo)); 
    Serial.print(" is "); 
    Serial.print(active_finger_angle);
    Serial.println();
  }
  else if(buttonDownState == HIGH)
  {
    int active_finger_angle = update_active_finger(active_servo, -5);
    
    Serial.print("[DOWN] Angle of ");
    Serial.print(get_active_finger(active_servo));
    Serial.print(" is ");
    Serial.print(active_finger_angle);
    Serial.println();
  }
  else
  {
    //Serial.print("No movement of "); Serial.print(active_servo);
    //Serial.println();
  }

  // robojax PCA9865 16 channel Servo control
  delay(50);// wait for 1 second
}

/*
 * @param: int delay_value - Urcuje rychlost pohybu
*/
int set_default_hand_gesture()
{
  pwm.setPWM(PALEC, 0, angleToPulse(PALEC_MIN));
  pwm.setPWM(UKAZOVAK, 0, angleToPulse(UKAZOVAK_MIN));
  pwm.setPWM(PROSTREDNIK, 0, angleToPulse(PROSTREDNIK_MIN));
  pwm.setPWM(PRSTENIK, 0, angleToPulse(PRSTENIK_MIN));
  pwm.setPWM(MALIK, 0, angleToPulse(MALIK_MIN));
  pwm.setPWM(ZAPESTI, 0, angleToPulse(ZAPESTI_STRED));
}

/*
/* angleToPulse(int ang)
 * @brief gets angle in degree and returns the pulse width
 * @param "ang" is integer represending angle from 0 to 180
 * @return returns integer pulse width
 * Usage to use 65 degree: angleToPulse(65);
 * Written by Ahmad Shamshiri on Sep 17, 2019. 
 * in Ajax, Ontario, Canada
 * www.Robojax.com 
 */ 
int angleToPulse(int ang){
   int pulse = map(ang,0, 180, SERVOMIN,SERVOMAX);// map angle of 0 to 180 to Servo min and Servo max 
   Serial.print("Angle: ");Serial.print(ang);
   Serial.print(" pulse: ");Serial.println(pulse);
   return pulse;
}
 