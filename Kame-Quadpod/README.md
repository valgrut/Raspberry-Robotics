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
- **Dir**: git:Robotics/Kame


### 1.2. Original project

- Thingiverse: [Kame: 8DOF small quadruped robot](https://www.thingiverse.com/thing:1265766)
- [Project page: Kame quarduped robot](https://hackaday.io/project/9334-kame-esp8266-based-quadruped)


### 1.3. Kame resources and remakes

- https://javierih.com/robots/kame32/
- https://github.com/JavierIH/kame32/blob/main/code/src/main.cpp


### 1.4. Motivace

- Walking in the room (remotely) over the phone
- Exploring with the camera on
- State machine transitions, autonomous movements when IDLE
- Communication with the other robot(s) via IR?
- R2D2 sounds
- Close all related tabs in browser.


---------------------------------------------------------------------------------------------

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


### 2.2. Dily - Mechanicke

- [ ] 9x Small servo 9g SG90
- [ ] 12x M3\*30mm + maticky
- [ ] 8x Lozisko F693ZZ Flange Bearing 3x8x4 mm
- [ ] Wires
- [ ] Rubber stickers for the leg tips (helps against leg slipping)


### 2.3. Dily - Elektro

- [ ] ESP-wroom32 (WiFi + Bluetooth)
- [ ] Baterie: 2x 18650 3.7V 2600mAh 5C
- [ ] PCA9685 Servo Controller (Could be done without it)
- [ ] 470uF - 1000uF Capacitor
- [ ] Step-down 7.4V -> 5V


### 2.3. Rozsireni (Extensions)

- [ ] Battery indicator (2S)
- [ ] HC-SR04 - Ultrasonic distance sensor
- [ ] Adafruit PowerBoost 500C - Nabijeni baterie a napajeni MCU
- [ ] ESP camera modul
- [ ] LED to the front and back ??
- [ ] LED into the top mount head as a flashlight
- [ ] Laser into the top mount head
- [ ] Display with eyes at the front


### 2.4. Dropped

- [ ] TB4056 Li-Po battery charger
- [ ] 3v3 voltage regulator (3.7V -> 3.3V)
- [ ] Step-up booster (3.7V -> 5V)
- [ ] Baterie Lipol 1200MAh 3.7V
    - Does not provide enough current (C=1)


---------------------------------------------------------------------------------------------

## 3. Zapojeni a sestaveni

- Nutne ESP32 (ESP-wroom32, ...) kvuli podpore BL (a ne pouze BLE)
- Nelze pouzit knihovnu PS4Controll s ESP-8266, protoze to podporuje pouze Low-Energy BT
- Ovladani pres Controller nakonec skrze knihovnu Bluepad32
- Nutno zvolit 'board' z "rodiny" bluepad32.

![Image: kame](kame-scheme.png)


### 3.1. Poznamky k zapojeni

- Hlidat si required Voltage u senzoru, esp a pod (3v3 vs 3.7V z baterie, a pod!!!)
- Nutno ESP32. Pro ESP8266 neni PS4Controll knihovna kompatibilni!
- Tluste draty pro propojeni baterie az k V+ u PCA9685 - bo vyssi current.
- Capacitor mezi V+ a GND u PCA9685 (current balancing because of spikes from active servos)
  - cca 100uF na 1 servo -->
  - --> 9 serv -> ~900uF


### 3.2. Setup a Programovani

#### 3.2.1. Required libraries (Arduino-IDE)


- **How to add library**:
  - `Sketch -> Manage Libraries -> search`

- **Libraries**:
  - Adafruit_PWMServoDriver.h
  - Bluepad32.h


#### 3.2.2. Board selection

- **Board**:
  - ESP32-WROOM-DA Module (esp32-bluepad32:esp32:esp32da)
    - pri pouziti velkeho ESP32
  - AI Thinker ESP32-CAM (esp32-bluepad32:esp32:esp32cam)
    - pokud pouziju CAM esp
- IDE -> Tools -> Board -> esp32_bluepad32 -> ^^^


#### 3.2.3. Controller pairing

- **XBox controller + Bluepad32**
  - 1. Just turn on the xbox controller with the large middle button.
  - 2. Press pairing (in the front side of controller to turn on pairing mode)

- (**PS4 controller + PS4Controller.h**)
  - Nekompatibilni s XBox controllerem
  - Neslo rozchodit s tim Frog Controllerem z Aliexpressu.


#### 3.2.4. Writing to ESP

- ! Hold right button of ESP (usb at the bottom) during "Connecting......." phase.


### 3.3. Updates

#### 3.3.1. Removed TP-4056 battery charger

- Proc?
- Max current output: 3A


### 3.4. Movement pattern observing

- Rozdelit video na framy a vypozorovat sekvenci pohybu.

```
ffmpeg -i file.mp4 -vf fps=10 out%d.png   # 10 frames / 1 second
ffmpeg -i file.mp4 -vf fps=1/600 out%d.png   # 1 frames / 60 second
```


### 3.5. Build diary timeline

Od nejnovejsiho.

- 2025-10-20
    - Novinka: Brownout, kdyz zapojeno ESP i ESP-cam.
    - Pripominka, ze PCA's V+ terminal ma max 6V !!! Nutny dalsi Buck, nelze tam rvat 8V z baterie!!!
    - Cekani na dily (XT30 connector, step-down 5A, MG90s serva)

- 2025-10-19
    - Vymodeloval jsem temp dilek mezi telo a hlavu
    - Zprovoznil jsem kameru (lepsi framerate, latence)
    - Rozdelil jsem (konecne) zdrojak na vice souboru.
    - Kame-src-v2: BT controller again, Gaits, Walking, Laser.

- 2025-10-18
    - Kompletne jsem vykuchal a prepajel vnitrek robota, aby to nebyl takovy mess a vse tam slo nacpat.
    - Odstranil jsem tu hruzu se stepdown + kondenzatory na pajivem poli, ktera mela slouzit proti brownoutu.
    - Kame sam chodi na zemi!!!
    - Baterie: RC 70C nebo kolik, 7.4V

- 2025-10-12
    - Gaits: Konecne spravne a opraveno. Also, TurnLeft a TurnRigt done!

- 2025-10-11
    - Fusion360: Vymodeloval jsem podstavec pro robota, abych mohl testovat pohyb nad zemi.

- 2025-??-??
    - Kompletni osekani zdrojaku na absolutni minimum, a snaha o pokracovani z minima.
    - Pochopeni delta time a Gait processingu
    - Working.
    - Zjisteni C parametru u baterii a zacal jsem pouzivat Lab zdroj se 7.6V + 4A.
    - Prulom a konecne vyrazny progress.

- 2025-06-21
    - Snaha o odkoukani a zprovozneni TurnLeft a TurnRight gaitu.
    - Dlouho jsem se snazil udelat vse naraz (gaits, BT controll, Web controll, IDLE states...)

- 2025-06-09
    - Snaha o vyreseni brownoutu:
        - Experimenty s kondenzatorama, ruznymi LDO, step-down.
        - Experimenty se seriovym zapojenim baterii (jsou ale 1C, takze to nepomohlo!)

- 2025-05-31
    - Snaha o eliminaci Brownoutu u ESP32 - pripajeni LDO regulatoru, a pod.
    - Porad pouzivam puvodni placatou 1C 3.7V baterii a Voltage boost-up na 5V.

- 2025-05-19
    - Zkraceni kabelu od vsech servo motorkuu, aby se daly nacpat do tela.

- 2024-01-23
    - Fusion360: Design a prototypovani noveho tela pro Kame (misto pro PCA a pro ESP-8266)

- 2023-10-21
    - Funkcni Walking Gait (Video, jak spadl malem ze stolu)
    - PCA9685 je jeste venku z Kame.
    - Puvodni telo (OG projekt)
    - Jeste skrz Arduino Uno
    - PCA napajeno jeste z USB stripnutym dratem.
    - Otacel vrskem
    - Umel lehnout a stoupnout.

- 2023-03-31
    - OG Kame je sestaveny (puvodni telo, vymodelovany adapter pro uchyceni hlavy)


---------------------------------------------------------------------------------------------

## 4. Troubleshooting

### 4.1. A fatal error occurred: Failed to connect to ESP32: Wrong boot mode detected (0x13)! The chip needs to be in download mode

- Right after compilation, before flashing, hold BOOT/FLASH button.
  - Kdyz mame ESP tak, ze je USB port dole, tak je to to napravo.

- 1. Zacit drzet behem "Connecting ......"
- 2. Pustit, kdyz zacnou "Writing at 0x00s0f0f0dd0 (x %)"

https://support.aimagin.com/boards/9/topics/689
