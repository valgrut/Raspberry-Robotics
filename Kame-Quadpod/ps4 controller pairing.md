#### notes [SORT]

- SixaxisPairTool
  - Pro nahrati ESP mac adresy do Controlleru.

- Library:
  - PS4Controller by Juan Pablo Martinez
  - (DabbleESP32 by STEMpedia...)


## PS4 Controller pairing (Actually trying to pair Data Frog controller)


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
- Library:
  - PS4Controller by Juan Pablo Martinez
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


- Another try - Bluepad32
  - via bluepad32
  - link1: https://racheldebarros.com/esp32-projects/connect-your-game-controller-to-an-esp32/
  - link2: https://www.youtube.com/watch?v=EEViXFoSzww
  - 1. Preferences -> Add URLs:
    - ESP32 Boards Manager:
      - https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
    - Bluepad32 Boards Manager:
      - https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json
  - 2. Then install boards: esp32_bluepad32 by Ricardo Quesada
  - 3. **Select board**: Tools -> Board -> esp32_bluepad32 -> ESP32 Dev Module
  - 4. Verify with example: File -> Examples -> Bluepad32_ESP32 -> Controller
  - 5. Compile and Write code into ESP
  - 6. Open Serial console
  - 7. Click EN button (kdyz je usb dole, tak ten vlevo.)
  - 8. Turn On Pairing mode on Controller
  - 9. Error:
      ```
      <------- sdp_query_timeout()
      uni_hid_device_disconnect: Unknown GAP connection type: 0
      Failed to perform SDP VID/PID query
      uni_hid_device_disconnect: Unknown GAP connection type: 0
      uni_hid_device_disconnect: Unknown GAP connection type: 0
      Arduino: unexpected value for on_device_ready; got: 0, want: -1
      Platform declined controller, deleting it
      uni_hid_device_disconnect: Unknown GAP connection type: 0
      ```

- File -> Preferences -> Add this URL:
```
https://dl.espressif.com/dl/package_esp32_index.json
```

- esp-idf tools
  - https://github.com/espressif/vscode-esp-idf-extension
  - https://dl.espressif.com/dl/esp-idf/?idf=4.4
  - sdk, menuconfig
- https://github.com/aed3/PS4-esp32
- https://bluepad32.readthedocs.io/en/latest/

- Data Frog controller je v parovani, ne kdyz blika pravidelne, ale kdyz 2x blikne v intervalech.
  - JAK? Zmacknout a drzet HOME  + 2x? zmacknout Share + pustit home


#### fatal error: esp_bt_defs.h: No such file or directory

- !!!!! Toto byl problem, kdyz jsem zkousel parovat s ESP8266, ktere ale nepodporuje klasicke BT, ale jen BLE.

- Proc?
  - Kdyz neni povoleno Bluetooth na Boardu

- Jak povolit bluetooth?


  - ESP32 Dev Module
    - Nutny kvuli Tools > Partition Scheme -> Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS) 
      - (to enable bluetooth)
