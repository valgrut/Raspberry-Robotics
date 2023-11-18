# Kame quadpod

![kame](kame.jpg)

**Rejstrik**:

- [1. STL, videa, project pages](#1-stl-videa-project-pages)
  - [1.1. Examples](#11-examples)
  - [1.2. Project pages](#12-project-pages)
- [2. Seznam materialu a soucastek k tisku](#2-seznam-materialu-a-soucastek-k-tisku)
  - [2.1. STL list](#21-stl-list)
  - [2.2. Dily](#22-dily)
- [3. Zapojeni a sestaveni](#3-zapojeni-a-sestaveni)
  - [3.1. Zapojeni a sestaveni](#31-zapojeni-a-sestaveni)
    - [3.1.1. Pro Arduino](#311-pro-arduino)
    - [3.1.2. Finalni Zapojeni a sestaveni (ESP32)](#312-finalni-zapojeni-a-sestaveni-esp32)
  - [3.2. Programovani](#32-programovani)
    - [3.2.1. Arduino IDE required libraries](#321-arduino-ide-required-libraries)

## 1. STL, videa, project pages

### 1.1. Examples

- **Dir**: Hobby/Robotika/Kame/
- **Dir**: Hobby/3D Printing models/Robotika/Kame - quadruped robot
- []()

### 1.2. Project pages

- Thingiverse: [Kame: 8DOF small quadruped robot](https://www.thingiverse.com/thing:1265766)
- [Project page: Kame quarduped robot](https://hackaday.io/project/9334-kame-esp8266-based-quadruped)

## 2. Seznam materialu a soucastek k tisku

### 2.1. STL list

- TODO: Pridat modifikovane dily pro horni kameru, prostor pro ESP, a pod
- [ ] 1x bodyamod (modified for SG90 servo)
- [ ] 1x bodybmod (modified for SG90 servo)
- [ ] 2x leftbracket (modified for SG90 servo)
- [ ] 2x rightbracket (modified for SG90 servo)
- [ ] 2x leftgb (modified for SG90 servo)
- [ ] 2x rightgb (modified for SG90 servo)
- [ ] 1x Untitled
- [ ] 1x Montaj2-front-frame-1

### 2.2. Dily

- [ ] 9x Small servo 9g SG90
- [ ] 12x M3\*30mm + maticky
- [ ] 8x Lozisko F693ZZ Flange Bearing 3x8x4 mm
- [ ] NodeMCU-32S ESP32 WiFi + Bluetooth
- [ ] Baterie Lipol 1200MAh
- [ ] PCA9685 Servo Controller pro 10 servo motoru
- [ ] HC-SR04 - Ultrasonic distance sensor
- [ ] Adafruit PowerBoost 500C - Nabijeni baterie a napajeni MCU
- [ ] (ESP camera modul)

## 3. Zapojeni a sestaveni

### 3.1. Zapojeni a sestaveni

#### 3.1.1. Arduino - zapojeni PCA9685 servo controlleru

- ARDUINO - DRAT - PCA9685
- A5 - modra - SCL
- A4 - fialova - SDA
- 5V - cervena - VCC
- GND - cerna - GND
- USB napajeni - cervena+hneda - power-terminal

#### 3.1.2. Finalni Zapojeni a sestaveni (ESP32)

- TODO: schema z programu

### 3.2. Programovani

#### 3.2.1. Arduino IDE required libraries

```
Sketch -> Manage Libraries -> search
```

* Adafruit PWM Servo Driver
* (PCA9685)


