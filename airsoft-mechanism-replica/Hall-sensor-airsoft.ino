#include <SparkFun_TB6612.h>
#include <ESP32Servo.h>
// ESP32-C3 Super Mini + TB6612FNG + KY-003
//
// Serial commands:
//   R      = one rotation
//   F      = motor forward continuously
//   S      = coast / stop
//   B      = active brake
//   V255   = set motor speed (0-255)
//   ?      = print status

constexpr int PIN_HALL = 3;

constexpr int PIN_AIN1 = 4;
constexpr int PIN_AIN2 = 5;
constexpr int PIN_PWMA = 6;
constexpr int PIN_STBY = 7;

// PWM
constexpr int PWM_FREQ = 20000;      // 20 kHz
constexpr int PWM_RESOLUTION = 8;    // 8 bit = 0-255

int motorSpeed = 128;
// motorSpeed = 255;   // 100%
// motorSpeed = 200;   // ~78%
// motorSpeed = 128;   // ~50%
// motorSpeed = 64;    // ~25%


// --------------------------------------------------
// Motor control
// --------------------------------------------------

void motorForward()
{
    digitalWrite(PIN_STBY, HIGH);

    digitalWrite(PIN_AIN1, HIGH);
    digitalWrite(PIN_AIN2, LOW);

    ledcWrite(PIN_PWMA, motorSpeed);
}


void motorCoast()
{
    // Stop PWM first
    ledcWrite(PIN_PWMA, 0);

    digitalWrite(PIN_AIN1, LOW);
    digitalWrite(PIN_AIN2, LOW);
}


void motorBrake()
{
    digitalWrite(PIN_STBY, HIGH);

    digitalWrite(PIN_AIN1, HIGH);
    digitalWrite(PIN_AIN2, HIGH);

    // Full strength active braking
    ledcWrite(PIN_PWMA, 255);
}


// --------------------------------------------------
// Hall sensor
// --------------------------------------------------

bool magnetDetected()
{
    return digitalRead(PIN_HALL) == LOW;
}


// --------------------------------------------------
// One revolution
// --------------------------------------------------

void doRotation()
{
    Serial.print("Starting rotation at speed ");
    Serial.println(motorSpeed);

    motorForward();

    unsigned long startTime = millis();

    // If we're currently sitting at HOME,
    // first wait until the magnet leaves the sensor.
    if (magnetDetected()) {

        Serial.println("Leaving HOME...");

        while (magnetDetected()) {

            if (millis() - startTime > 3000) {
                motorBrake();
                delay(100);
                motorCoast();

                Serial.println("ERROR: Could not leave HOME");
                return;
            }

            delay(1);
        }
    }

    Serial.println("Waiting for HOME...");

    // Reset timeout now that we've left HOME.
    startTime = millis();

    while (!magnetDetected()) {

        if (millis() - startTime > 3000) {
            motorBrake();
            delay(100);
            motorCoast();

            Serial.println("ERROR: Hall sensor timeout");
            return;
        }

        delay(1);
    }

    // Magnet returned.
    motorBrake();

    Serial.println("HOME detected -> BRAKE");

    // Hold active braking briefly.
    delay(100);

    motorCoast();

    Serial.println("Rotation complete.");
}


// --------------------------------------------------
// Serial command handling
// --------------------------------------------------

void handleCommand(String command)
{
    command.trim();

    if (command.length() == 0)
        return;

    // ----- Set speed -----

    if (command.startsWith("V") || command.startsWith("v")) {

        int requestedSpeed = command.substring(1).toInt();

        requestedSpeed = constrain(requestedSpeed, 0, 255);

        motorSpeed = requestedSpeed;

        Serial.print("Motor speed set to ");
        Serial.print(motorSpeed);

        Serial.print(" / 255 (");
        Serial.print((motorSpeed / 255.0) * 100.0, 1);
        Serial.println("%)");

        return;
    }


    // ----- Other commands -----

    if (command.equalsIgnoreCase("R")) {

        doRotation();

    }
    else if (command.equalsIgnoreCase("F")) {

        motorForward();

        Serial.print("Motor forward, speed = ");
        Serial.println(motorSpeed);

    }
    else if (command.equalsIgnoreCase("S")) {

        motorCoast();
        Serial.println("Motor coast.");

    }
    else if (command.equalsIgnoreCase("B")) {

        motorBrake();
        Serial.println("Motor brake.");

    }
    else if (command == "?") {

        Serial.println();
        Serial.println("--- STATUS ---");

        Serial.print("Motor speed: ");
        Serial.print(motorSpeed);
        Serial.print(" / 255 (");
        Serial.print((motorSpeed / 255.0) * 100.0, 1);
        Serial.println("%)");

        Serial.print("Hall sensor: ");

        if (magnetDetected())
            Serial.println("MAGNET DETECTED");
        else
            Serial.println("no magnet");

        Serial.println("--------------");

    }
    else {

        Serial.print("Unknown command: ");
        Serial.println(command);
    }
}


// --------------------------------------------------
// Setup
// --------------------------------------------------

void setup()
{
    Serial.begin(115200);

    pinMode(PIN_AIN1, OUTPUT);
    pinMode(PIN_AIN2, OUTPUT);
    pinMode(PIN_STBY, OUTPUT);

    pinMode(PIN_HALL, INPUT_PULLUP);

    // Configure ESP32 PWM on PWMA.
    ledcAttach(PIN_PWMA, PWM_FREQ, PWM_RESOLUTION);

    motorCoast();

    Serial.println();
    Serial.println("Ready.");
    Serial.println();
    Serial.println("Commands:");
    Serial.println("  R     = one rotation");
    Serial.println("  F     = forward");
    Serial.println("  S     = coast");
    Serial.println("  B     = brake");
    Serial.println("  Vxxx  = speed 0-255");
    Serial.println("  ?     = status");
    Serial.println();
}


// --------------------------------------------------
// Main loop
// --------------------------------------------------

void loop()
{
    if (Serial.available()) {

        String command = Serial.readStringUntil('\n');

        handleCommand(command);
    }
}