/*
 * MotorKnob
 *
 * A stepper motor follows the turns of a potentiometer
 * (or other sensor) on analog input 0.
 *
 * http://www.arduino.cc/en/Reference/Stepper
 * This example code is in the public domain.
 */

int Gas_analog = 32;    // used for ESP32
int Gas_digital = 33;   // used for ESP32

void setup() {
  Serial.begin(115200);
  pinMode(Gas_digital, INPUT);
}

void loop() {
  int gassensorAnalog = analogRead(Gas_analog);
  int gassensorDigital = digitalRead(Gas_digital);

  if(gassensorAnalog > 0)
  {
    Serial.print("Gas Sensor: ");
    Serial.print(gassensorAnalog);
    Serial.print("\t");
    Serial.print("Gas Class: ");
    Serial.print(gassensorDigital);
    Serial.print("\t");
    Serial.print("\n");
  }
  
  delay(100);
}