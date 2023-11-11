Microcontrollers Documentation and Notes
========================================

- TODO: Rejstrik

<!------------------------------------------------------------------------------->
## 1. Microcontrollers

Poznamky o microcontrollerech, jejich vyhody a nevyhody jednoho oproti druhemu, odkazy a zdroje.

### 1.1. Vyvojove prostredi, relevantni aplikace
* Arduino IDE 2.0
* Thonny

### 1.1. NodeMCU
- Levnejsi a vykonnejsi nez arduino
#### 1.1.1. ESP8266
- Nema Bluetooth
- Lack of security

#### 1.1.2. ESP32
- [Intro to the ESP32 - Setup, Docs](https://makeabilitylab.github.io/physcomp/esp32/esp32.html#important-notes)
- Bluetooth, Wifi
- Lepsi verze nez ESP8266
- Vykonnejsi nez ESP8266
- Better security

### 1.2. Arduino
- Podobne funkce jako NodeMCU a Raspberry Pico
- Nema Bluetooth a Wifi

#### 1.2.1. Nano


### 1.3. Raspberry Pi
- flask, python, ssh
- Wifi, Bluetooth

#### 1.3.1. Pico W
- Programovano v (mikro-)Pythonu

#### 1.3.2. Pi 4B
- Wifi, Bluetooth
- Linux - SSH, ...


<!------------------------------------------------------------------------------->
## 2. How to:
### 2.1. Installing Libraries
#### 2.1.1. ESP libraries
1. Download zip of the repos:
    - ESP32:
        - https://github.com/me-no-dev/ESPAsyncWebServer
        - https://github.com/me-no-dev/AsyncTCP
    - ESP8266
        - https://github.com/me-no-dev/ESPAsyncWebServer
        - https://github.com/me-no-dev/ESPAsyncTCP

2. Include downloaded libraries
    ```
    Sketch > Include Library > Add .zip Library and select the libraries
    ```

#### 2.1.2. Install and select ESP8266 into Arduino IDE
1.  Then we need to add the ESP8266 to the Boards list.

    ```
    Open the Arduino IDE Preferences panel.
    On Windows: "File->Preferences"
    On Mac: "Arduino->Preferences..."
    ```

2. Add the following URL to the "Additional Boards Manager URLs":
    - http://arduino.esp8266.com/stable/package_esp8266com_index.json

3. Library Manager -> Type ESP8266 and click install


<!------------------------------------------------------------------------------->
## 3. Elektronicke soucastky, zapojeni a schemata

### 3.1. DC Motor Drivers

TODO co to je a proc to je

#### 3.1.1. TB6612FNG Dual H-Bridge

![Img: TB6612FNG](attachments/TB6612FNG.jpg)

### 3.2. Servo Drivers
#### 3.2.1. PCA9685 (16-channel PWM)

![Img: PCA9685](attachments/pca9685-module.png)

**Popis:**

> The PCA9685 module is a 16-channel controller that can control 16 PWM outputs via I2C communication. Among other things, it allows you to free up inputs and outputs of your microcontroller and drive up to 16 LEDs or servomotors (or any other module taking a PWM signal as input) using two pins (SCL and SDA) while keeping the pins of your microcontroller for other modules such as sensors.

> The module is based on the PCA9685 controller, which allows PWM outputs to be controlled using I2C communication and an integrated clock. The module has 6 bridges to select the address of the board and thus allow up to 62 controllers to be placed on the same bus for a total of 992 servomotors (addresses available **0x40 to 0x7F**).
It can drive PWM outputs with adjustable frequency and 12-bit resolution. The module is compatible with 5V and 3.3V microcontrollers.

**Napajeni:**

> The Servo Driver board uses I2C protocol to communicate with the Pi4. This uses the set of pins down either end of the board to connect to the I2C pins on the Pi4. It operates at 3.3V.

> A separate power supply to the board must be used for the servos operating at 5V. This needs to be powerful enough to source sufficient current for the number of servos connected. Each board can support up to 16 servos. This project used 3 servos, each with a maximum current of 140mA   (140mA x 3 = 420mA) so a 1A supply was used.

Do not connect the V+ pin on the ends of the servo board to the 3.3 pin on the Pi4 or you will damage your Pi4!

**How does PWM controlling works**:

The servos in this project use a PWM signal of varying pulse duration to control the position of the servo motor.

The code example uses the Python SMBus module which is included with the default Raspbian install. This is used to send a series of bytes using I2C protocol to the servo driver’s PCA9685 chip registers. By setting the various registers the board modulates the PWM signal to each server and sets its position.

The servo datasheet shows that the servos operate at a PWM frequency of 50Hz (20ms pulse frequency). A pulse duration of 1.5ms is required for the Standard & Continuous servos to set the centre position.

Initially the board PWM frequency pre-scale register is set to 50Hz using the equation from the PCA9685 datasheet for an Oscillator Clock of 25MHz. (25MHz / (4096 x 50Hz)) – 1 = 122 (0x7A).

Note: An oscilloscope was used to tweak this value to exactly 50Hz so a value of 0x08 is used.

**Calculation of the speed**:

Servo go as fast as they can to the set point you give them, so you don't control how fast they get there. but you can control timing by taking smaller steps as explained above.

if you want to move from angle A to angle B (>A) you have B-A degrees to turn. if you want to take those steps in N seconds, then (assuming moving 1° is almost instantaneous) you can make 1° steps and pause for 1000UL* N / (B-A-1) (ms) in between steps

eg for going from 10° to 61° you have 51° to move and say you want to get there in ~10 seconds.
You take 50 steps of 1° and pause for 1000*10/50 = 200ms in between steps

Servo pulse timing varies so you need to find the minimum and maximum for your specific servo (at 0° and 180° — carefully adjust as hitting the physical limits of travel can damage your servo / the gears)

Once you have that, then to go to a specific angle in degrees between 0 and 180, you can use

```
pulselength = map(angle, 0, 180, SERVOMIN, SERVOMAX);
```


* Adafruit - Servokit
```
sudo pip3 install adafruit-circuitpython-servokit
```

* C library
```
#include <Adafruit_PWMServoDriver.h>
```

* Addressing multiple chained servos
```
Adafruit_PWMServoDriver myServo1 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver myServo2 = Adafruit_PWMServoDriver(0x41);
Adafruit_PWMServoDriver myServo3 = Adafruit_PWMServoDriver(0x4A);
```

* Continuous rotation servos using 'throttle'
```
kit.continuous_servo[1].throttle = 1
```

* Useful servo functions (PCA9685 breakout, Python and ServoKit!)
```
from adafruit_servokit import ServoKit
kit.servo[0].set_pulse_width_range(1000, 2000)
servokit.servo[0].actuation_range = 160
```

* Resources
    - [PCA9685 Datasheet](https://cdn-shop.adafruit.com/datasheets/PCA9685.pdf)
    - https://learn.adafruit.com/16-channel-pwm-servo-driver?view=all
    - https://mytectutor.com/how-to-use-pca9685-16-channel-12-bit-pwm-servo-driver-with-arduino/

### 3.3. Sensors
#### 3.3.1. Gyroskopy
#### 3.3.2. Distance
##### 3.3.2.1. HC-SR04
![Img: HC-SR04](attachments/hc-sr04.jpg)
#### 3.3.3. Obstacles
- Infracerveny detekcni modul prekazek

### 3.4. LiPo Baterie, zapojeni a nabijeni

### 3.5. Motory
#### 3.5.1. Servo motory
#### 3.5.2. Analog Feedback Servo

## X. Schemata

<!------------------------------------------------------------------------------->
## 4. Troubleshooting
### 4.1. Random characters in Serial Output Monitor

Change boud rate in the code to '9600'.

![Random characters in output](attachments/random_characters.png)

### 4.2. Cannot open /dev/ttyUSB0: Permission denied
1. Toto pomohlo

    ```
    sudo su
    //type your password
    cd /
    cd dev
    chown username ttyUSB0
    ```

2. Nevim, jestli neni potreba i toto:

    ```
    sudo usermod -a -G dialout $USER
    sudo usermod -a -G uucp $USER
    sudo usermod -a -G tty $USER
    ```

### 4.3. ESP8266 cant be found in the MC selection
- See 2.1.2. Install and select ESP8266 into Arduino IDE




