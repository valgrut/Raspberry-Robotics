#include <Bluepad32.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "Gaits.h"
#include "BluetoothControllerHandling.h"

/*
*  V2 - Kame - Minimal working example a hrani si + Bluetooth Controll + RTOS
*  
*  Potreboval jsem eliminovat, kde je sakra chyba ohledne brownoutu (Jestli neni v kodu),
*  a navic jsem nerozumel prekomplikovane logice navrhovane od GPT, takze jsem to zkrouhnul na minimum.
*  
*  Pochopil jsem delta time a update gits.
*  
*  Features:
*  - Delta time
*  - Walking gait (static table) is WORKING!!!!! Turn left/right, backward walk as well.
*  - Bluetooth Control
*  - Camera stream
*  - Laser on/off
*  - Camera movement
*  - Source code split into multiple files
*  - [ ] Movement smoothing
*/


// ---------------------------------------------------------------------------------------
// ------------------------------- Definitions -------------------------------------------
// ---------------------------------------------------------------------------------------

const char* ssid = "Tellmywifiloveher";
const char* password = "WeDr1#kY0ur8lo0d";

Adafruit_PWMServoDriver pwmServoDriver = Adafruit_PWMServoDriver(0x40);

bool laserToggleEnabled = true;
bool isLaserOn = false;
#define LASER_PIN 27

unsigned long deltaTime = 0;
unsigned long lastUpdate = 0;

unsigned int movementSpeedGateInterval = 300; // TODO: Could be adjustable from UI / controller.
unsigned int currentGaitLength = 0;
unsigned int currentGaitStep = 0;
unsigned long activeGaitElapsedTime = 0;

// struct MotionControl 
// {
//     MotionState state = IDLE;
//     unsigned long lastTransitionTime = 0;
//     float motionProgress = 0.0f;  // from 0.0 to 1.0
//     unsigned long lastUpdate = 0;  // timestamp of last update    // float deltaTime = 0.0f;   
//     unsigned long startDuration = 300;  // in milliseconds
//     unsigned long stopDuration = 300;  // in milliseconds
// } motion;


// ---------------------------------------------------------------------------------------
// -------------------------------- BT Controller Buttons --------------------------------
// ---------------------------------------------------------------------------------------

#define BUTTON_ARROW_UP    0x01
#define BUTTON_ARROW_DOWN  0x02
#define BUTTON_ARROW_RIGHT 0x04
#define BUTTON_ARROW_LEFT  0x08



// ---------------------------------------------------------------------------------------
// ------------------------------- Legs controll -----------------------------------------
// ---------------------------------------------------------------------------------------

float smoothConst = 0.2f;  // Default: 0.2f
void updateLegXY()
{
  for (int i = 0; i < 4; i++)
  {
    legXY[i]->current_value = (legXY[i]->target_value * smoothConst) + (legXY[i]->current_value * (1-smoothConst));
    pwmServoDriver.setPWM(legXY[i]->pcaPosition, 0, angleToPulse(legXY[i]->current_value));
    //Serial.printf("legXY[%d]->current_value: %d\n", i, legXY[i]->current_value);
  }
}

void updateLegZ()
{
  for (int i = 0; i < 4; i++)
  {
    legZ[i]->current_value = (legZ[i]->target_value * smoothConst) + (legZ[i]->current_value * (1-smoothConst));
    pwmServoDriver.setPWM(legZ[i]->pcaPosition, 0, angleToPulse(legZ[i]->current_value));
  }
}

void updateCameraServo()
{
    topXY.current_value = smooth(topXY.target_value, topXY.current_value);
    pwmServoDriver.setPWM(topXY.pcaPosition, 0, angleToPulse(topXY.current_value));
}


// ================================== Gait processing ==================================

/*
 * processes gait
 * Takes gait and loops over its steps based on the deltaTime.
 * Next step of the gait is updated only when the run time of the current step (activeGaitElapsedTime)
 * is larger than the provided interval, which defines the duration of each step before changing to the next.]
 * 
 * Controlled by global variables !!!
 *
 * TODO: maybe make a class of this and the global variables?
 *
 * gait         gait currently set to process and use
 * gaitInterval duration of each step (before next one is set)
 * gaitLenfth   number of steps of this gate
 * deltaTime    time since previous call of this function
*/
void processGait(const struct GaitStep *gait, unsigned long gaitInterval, int gaitLength, unsigned long deltaTime)
{   
    // if enough time elapses, set new step from gait sequence.
    activeGaitElapsedTime += deltaTime;
    // Serial.printf("gaitElapsedTime: %d\n", activeGaitElapsedTime);

    if (activeGaitElapsedTime >= gaitInterval)
    {
        // Serial.printf("Changing step in gait from %d to %d\n", currentGaitStep, (currentGaitStep + 1) % gaitLength);
        // % => start over after 'gaitLength' iterations.
        currentGaitStep = (currentGaitStep + 1) % gaitLength;

        // Reset counter for tracking when to advance to new step of the gait
        activeGaitElapsedTime -= gaitInterval;

        // Set new targets, this do not move the legs actually.
        // TODO: Pridat, ze kdyz je hodnota minusova, tak ji neupdatovat.
        for (int i = 0; i < 4; i++) 
        {
            legZ[i]->target_value = gait[currentGaitStep].z[i];
            legXY[i]->target_value = gait[currentGaitStep].xy[i];  // or combine x/y later
        }
    }
}



// ---------------------------------------------------------------------------------------
// --------------------------- Movement control ------------------------------------------
// ---------------------------------------------------------------------------------------

bool isMoving = false;
uint8_t buttonPressedID = 0;
enum MOVEMENT_MODES {
    NONE,
    WALK_FORWARD,
    WALK_BACKWARD,
    TURN_RIGHT,
    TURN_LEFT
} moveMode;

void processGamepad(ControllerPtr controller) 
{
    // Change walking mode based on the pressed button.
    if (controller->dpad() == BUTTON_ARROW_UP)
    {
        moveMode = MOVEMENT_MODES::WALK_FORWARD;
        Serial.println("Setting: WALK_FORWARD");
    }
    else if (controller->dpad() == BUTTON_ARROW_DOWN)
    {
        moveMode = MOVEMENT_MODES::WALK_BACKWARD;
        Serial.println("Setting: WALK_BACKWARD");
    }
    else if (controller->dpad() == BUTTON_ARROW_RIGHT)
    {
        moveMode = MOVEMENT_MODES::TURN_RIGHT;
        Serial.println("Setting: TURN_RIGHT");
    } 
    else if (controller->dpad() == BUTTON_ARROW_LEFT)
    {
        moveMode = MOVEMENT_MODES::TURN_LEFT;
        Serial.println("Setting: TURN_LEFT");
    }
    else 
    {
        moveMode = MOVEMENT_MODES::NONE;
        setRestPose();
        //stand_up();
        isMoving = false;
        //Serial.println("Setting: RestPose");
    }


    // Change movement settings with buttons:
    int speedChangeStep = 50;
    if(controller->l1())
    {
        movementSpeedGateInterval += speedChangeStep;
        if(movementSpeedGateInterval > 600)
        {
            movementSpeedGateInterval = 600;
        }
    }
    else if(controller->r1())
    {
        movementSpeedGateInterval -= speedChangeStep;
        if(movementSpeedGateInterval < 100)
        {
            movementSpeedGateInterval = 100;
        }
    }
    

    // Restore default z mid height.
    if(controller->r2())
    {
        BRZ.mid = BRZ.mid_backup;
        BLZ.mid = BLZ.mid_backup;
        FRZ.mid = FRZ.mid_backup;
        FLZ.mid = FLZ.mid_backup;
    }


    // Toggle laser on/off
    if(controller->miscHome() && laserToggleEnabled && ! isLaserOn)
    {
        digitalWrite(LASER_PIN, HIGH);
        laserToggleEnabled = false;
        isLaserOn = true;
    } 
    else if(controller->miscHome() && laserToggleEnabled && isLaserOn)
    {
        digitalWrite(LASER_PIN, LOW);
        laserToggleEnabled = false;
        isLaserOn = false;
    }
    if( ! controller->miscHome() && ! laserToggleEnabled)
    {
        laserToggleEnabled = true;
    }


    // Camera movement - left/right
    int cameraChangeStep = 10;
    if (controller->x())  // left
    {
        topXY.target_value += cameraChangeStep;
        if (topXY.target_value > topXY.MAX)
            topXY.target_value = topXY.MAX;
    }
    else if (controller->b())  // right
    {
        topXY.target_value -= cameraChangeStep;
        if (topXY.target_value < topXY.MIN)
            topXY.target_value = topXY.MIN;
    }


    // Camera movement - up/down
    int zStep = 2;
    if (controller->a())  // down
    {
        BRZ.target_value -= zStep;
        BLZ.target_value += zStep;
        FRZ.target_value -= zStep;
        FLZ.target_value += zStep;

        // set new values as default for all other moves.
        BRZ.mid -= zStep;
        BLZ.mid += zStep;
        FRZ.mid -= zStep;
        FLZ.mid += zStep;
    }
    else if (controller->y())  // up
    {
        BRZ.target_value += zStep;
        BLZ.target_value -= zStep;
        FRZ.target_value += zStep;
        FLZ.target_value -= zStep;
        
        // set new values as default for all other moves.
        BRZ.mid += zStep;
        BLZ.mid -= zStep;
        FRZ.mid += zStep;
        FLZ.mid -= zStep;
    }
}

// based on the pressed (not held) button, 
void loopSelectedMovement()
{
    if(moveMode == MOVEMENT_MODES::WALK_FORWARD)
    {
        int gaitLength = sizeof(basicWalkGate) / sizeof(GaitStep);
        processGait(basicWalkGate, movementSpeedGateInterval, gaitLength, deltaTime);
    }
    else if(moveMode == MOVEMENT_MODES::WALK_BACKWARD)
    {
        int gaitLength = sizeof(backwardWalkGate) / sizeof(GaitStep);
        processGait(backwardWalkGate, movementSpeedGateInterval, gaitLength, deltaTime);
    }
    else if(moveMode == MOVEMENT_MODES::TURN_RIGHT)
    {
        int gaitLength = sizeof(turnRightGait) / sizeof(GaitStep);
        processGait(turnRightGait, movementSpeedGateInterval, gaitLength, deltaTime);
    } 
    else if(moveMode == MOVEMENT_MODES::TURN_LEFT)
    {
        int gaitLength = sizeof(turnLeftGait) / sizeof(GaitStep);
        processGait(turnLeftGait, movementSpeedGateInterval, gaitLength, deltaTime);
    }
    else if(moveMode == MOVEMENT_MODES::NONE)
    {
        // stop moving
    }

}

// ---------------------------------------------------------------------------------------
// ------------------------------- Setup -------------------------------------------------
// ---------------------------------------------------------------------------------------

// Arduino setup function. Runs in CPU 1
void setup()
{
    Serial.begin(115200);

    // Init PCA9685 servo driver
    pwmServoDriver.begin();
    pwmServoDriver.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.enableVirtualDevice(false);

    // Laser setup
    pinMode(LASER_PIN, OUTPUT);
    digitalWrite(LASER_PIN, LOW);

    // set initial position
    setRestPose();
    updateLegXY();
    updateLegZ();
}


// ---------------------------------------------------------------------------------------
// ------------------------------- Loop --------------------------------------------------
// ---------------------------------------------------------------------------------------

void loop()
{
    // Update delta time
    unsigned long currentTime = millis();
    deltaTime = currentTime - lastUpdate; // Elapsed time from last update (since last iteration - time elapsed since this line)
    lastUpdate = currentTime; // We just updated the deltaTime, so we save current Time.

    // Fetch all the controllers' data. Call in main loop.
    bool dataUpdated = BP32.update();  // NOTE: update is held for ~1 second. It does NOT detect the button held down. Just the change.
    if (dataUpdated)  // TODO: movement will be executed by other Task, so main Task can continue in getting input and so it is not blocked.
    {
        processControllers();
    }
    
    loopSelectedMovement();

    // Update servos smoothly
    updateLegXY();
    updateLegZ();
    updateCameraServo();


    //vTaskDelay(1);
    delay(30);
}

