

// TODO: maybe change structures into normal classes and include there also functions lup() and ldown()
// and other utility functions



/************************************************/
/*          Definition of Servo's Pulses        */
/************************************************/

#define SERVOMIN  125 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  575 // this is the 'maximum' pulse length count (out of 4096)


/*************************************************/
/*          Legs structures definitions          */
/*************************************************/

struct cameraServo {
  unsigned int pcaPosition;
  const char* leg;
  int MIN;
  int mid;
  int MAX;
  float current_value;
  float target_value;
};

struct legServoXY {
  unsigned int pcaPosition;
  const char* leg;
  int front;
  int def;
  int side;
  float current_value;
  float target_value;
};

struct legServoZ {
  unsigned int pcaPosition;
  const char* leg;
  int MIN;
  int floor_height;
  int mid;
  int mid_backup;
  int MAX;
  float current_value;
  float target_value;
};


/******************************************/
/*          Legs initialisations          */
/******************************************/
/*
  Note:
  - Initialization values depend on:
      - The physical mounting of the servos
      - Factory and construction minor differences
*/

// Robot's head
struct cameraServo topXY = {
    .pcaPosition = 8,
    .leg = "Top Camera XY",
    .MIN = 0,
    .mid = 100,
    .MAX = 150,
    .current_value = 100,
    .target_value = 100
};

// Front Right (Horizontal - XY plane)
struct legServoXY FRXY = {
    .pcaPosition = 5,
    .leg = "Front Right Leg XY",
    .front = 100,
    .def = 170,
    .side = 190,
    .current_value = 170.0f,
    .target_value = 170.0f
};

// Front Left (Horizontal - XY plane)
struct legServoXY FLXY = {
    .pcaPosition = 7,
    .leg = "Front Left Leg XY",
    .front = 150,
    .def = 80,
    .side = 50,
    .current_value = 80.0f,
    .target_value = 80.0f
};

// Back Right (Horizontal - XY plane)
struct legServoXY BRXY = {
    .pcaPosition = 4,
    .leg = "Back Right Leg XY",
    .front = 110,
    .def = 45,
    .side = 18,
    .current_value = 45.0f,
    .target_value = 45.0f
};

// Back Left (Horizontal - XY plane)
struct legServoXY BLXY = {
    .pcaPosition = 6,
    .leg = "Back Left Leg XY",
    .front = 90,
    .def = 170,
    .side = 190,
    .current_value = 170.0f,
    .target_value = 170.0f
};


// Front Right (Vertical - Z plane)
struct legServoZ FRZ = {
    .pcaPosition = 1,
    .leg = "Front Right Leg Z",
    .MIN = 60,
    .floor_height = 110,
    .mid = ((200 + 60) / 2) + 20,
    .mid_backup = ((200 + 60) / 2) + 20,
    .MAX = 200,
    .current_value = ((200 + 60) / 2) + 20.0f,
    .target_value = ((200 + 60) / 2) + 20.0f
};

// Front Left (Vertical - Z plane)
struct legServoZ FLZ = {
    .pcaPosition = 3,
    .leg = "Front Left Leg Z",
    .MIN = 150,
    .floor_height = 100,
    .mid = (150 / 2) - 20,
    .mid_backup = (150 / 2) - 20,
    .MAX = 0,
    .current_value = (150 / 2) - 20.0f,
    .target_value = (150 / 2) - 20.0f
};

// Back Right (Vertical - Z plane)
struct legServoZ BRZ = {
    .pcaPosition = 0,
    .leg = "Back Right Leg Z",
    .MIN = 210,
    .floor_height = 180,
    .mid = ((210 + 70) / 2) - 20,
    .mid_backup = ((210 + 70) / 2) - 20,
    .MAX = 70,
    .current_value = ((210 + 70) / 2) - 20.0f,
    .target_value = ((210 + 70) / 2) - 20.0f
};

// Back Left (Vertical - Z plane)
struct legServoZ BLZ = {
    .pcaPosition = 2,
    .leg = "Back Left Leg Z",
    .MIN = 90,
    .floor_height = 140,
    .mid = ((210 + 90) / 2) + 25,
    .mid_backup = ((210 + 90) / 2) + 25,
    .MAX = 210,
    .current_value = ((210 + 90) / 2) + 25.0f,
    .target_value = ((210 + 90) / 2) + 25.0f
};



/**************************************************/
/*          Legs assignment into arrays           */
/**************************************************/

legServoXY* legXY[] = {&FRXY, &FLXY, &BRXY, &BLXY};
legServoZ* legZ[]   = {&FRZ, &FLZ, &BRZ, &BLZ};



/****************************************/
/*          Utility functions           */
/****************************************/

/*
 * Short-named function to simply change value (e.g. in the gait array)
 * 
 * ldown() jde k MAX, protoze natahuju nohu dolu.
 */
// some servos move leg up with increasing values, some with decreasing. It is pain.
// LUP jde k MIN, protoze zvedam nohu nahoru.
// TODO, pridat tu fci do structury a volat jen FRZ.up(20)
int lup(struct legServoZ servo, int value)
{
    if(servo.MIN > servo.MAX) 
    {
        if(servo.mid + value > servo.MIN)
            return servo.MIN;
        return servo.mid + value;
    }
    
    if(servo.MIN < servo.MAX) 
    {
        if(servo.mid - value < servo.MIN)
            return servo.MIN;
        return servo.mid - value;
    }
}


/*
 * Short-named function to simply change value (e.g. in the gait array)
 * 
 * ldown() jde k MAX, protoze natahuju nohu dolu.
 */
int ldown(struct legServoZ servo, int value)
{
    if(servo.MIN > servo.MAX) 
    {
        if(servo.mid - value < servo.MAX)
            return servo.MAX;
        return servo.mid - value;
    }
    
    if(servo.MIN < servo.MAX) 
    {
        if(servo.mid + value > servo.MAX)
            return servo.MAX;
        return servo.mid + value;
    }
}

/*
 *
 */
int angleToPulse(int angle){
   int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX); // map angle of 0 to 180 to Servo min and Servo max
//    Serial.print("Angle: "); Serial.print(angle);
//    Serial.print(" pulse: "); Serial.println(pulse);
   return pulse;
}

/*
 *
 */
// Exponential smoothing
// Warning: There is a problem, that it goes to target asymptotically and never actually reach it.
// TODO: Need update to jump to target if close enough.
float smooth(float target, float prev) {
    return (target * 0.2f) + (prev * 0.8f);
}

// Motion progress for more predictable timing
float easeInOut(float t) {
    return t * t * (3 - 2 * t);  // Smoothstep
}

