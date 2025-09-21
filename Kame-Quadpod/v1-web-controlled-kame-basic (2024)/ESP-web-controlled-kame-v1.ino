#include <PCA9685.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Servo.h>


/*
Verze 1
- z roku 2024, hodne stare
- Ovladani: Webove UI
- Struktury pro LegServosXY nachystany, ale nepouzivany!



*/


// Network Credentials
const char* ssid = "***REMOVED***";
const char* password = "***REMOVED***";

// HTML web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
  <head>
    <title>Kame quadruped cute robot</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { font-family: Arial; text-align: center; margin:0px auto; padding-top: 30px;}
      table { margin-left: auto; margin-right: auto; }
      td { padding: 8 px; }
      .button {
        background-color: #2f4468;
        border: none;
        color: white;
        padding: 10px 20px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 18px;
        margin: 6px 3px;
        cursor: pointer;
        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        -webkit-tap-highlight-color: rgba(0,0,0,0);
      }
      img {  width: auto ;
        max-width: 100% ;
        height: auto ;
      }
    </style>
  </head>
  <body>
    <h1>ESP32-CAM Robot</h1>
    <img src="" id="photo" >
    <table>
      <tr>
        <td colspan="3" align="center">
          <button class="button" onmousedown="toggleCheckbox('forward');" ontouchstart="toggleCheckbox('forward');" onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">Forward</button>
        </td>
      </tr>
      <tr>
        <td align="center">
          <button class="button" onmousedown="toggleCheckbox('left');" ontouchstart="toggleCheckbox('left');" onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">Left</button>
        </td>
        <td align="center">
          <button class="button" onmousedown="toggleCheckbox('stop');" ontouchstart="toggleCheckbox('stop');">Stop</button>
        </td>
        <td align="center">
          <button class="button" onmousedown="toggleCheckbox('right');" ontouchstart="toggleCheckbox('right');" onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">Right</button>
        </td>
      </tr>
      <tr>
        <td colspan="3" align="center">
          <button class="button" onmousedown="toggleCheckbox('backward');" ontouchstart="toggleCheckbox('backward');" onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">Backward</button>
        </td>
      </tr>
      <tr>
        <td align="center">
          <button class="button" onmousedown="toggleCheckbox('lay_flat');" ontouchstart="toggleCheckbox('lay_flat');" onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">Lay Flat</button>
        </td>
        <td align="center">
          <button class="button" onmousedown="toggleCheckbox('bow');" ontouchstart="toggleCheckbox('bow'); onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">Bow</button>
        </td>
        <td align="center">
          <button class="button" onmousedown="toggleCheckbox('wave');" ontouchstart="toggleCheckbox('wave');" onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">Wave</button>
        </td>
      </tr>
      <tr>
        <td align="center">
          <button class="button" onmousedown="toggleCheckbox('look_left');" ontouchstart="toggleCheckbox('look_left');" onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">Look left</button>
        </td>
        <td align="center">
          <button class="button" onmousedown="toggleCheckbox('look_forward');" ontouchstart="toggleCheckbox('look_forward');" onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">Look forward</button>
        </td>
        <td align="center">
          <button class="button" onmousedown="toggleCheckbox('look_right');" ontouchstart="toggleCheckbox('look_right'); onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">Look right</button>
        </td>
      </tr>
    </table>
   <script>
   function toggleCheckbox(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/" + x, true);
     xhr.send();
   }
   //window.onload = document.getElementById("photo").src = window.location.href.slice(0, -1) + ":81/stream";
  </script>
  </body>
</html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

AsyncWebServer server(80);

// GPIO4 (SDA) and GPIO5 (SCL) are used as I2C pins to make it easier
// for people using existing Arduino code, libraries, and sketches.
Adafruit_PWMServoDriver pwmServoDriver = Adafruit_PWMServoDriver(0x40);


// Depending on your servo make, the pulse width min and max may vary, you
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
// Watch video V1 to understand the two lines below: http://youtu.be/y8X9X10Tn1k

// TODO toto predelat specificky pro moje serva
#define SERVOMIN  125 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  575 // this is the 'maximum' pulse length count (out of 4096)

// TODO: transition state machine

// our servo # counter
uint8_t servonum = 0;

struct cameraServo {
  unsigned int pcaPosition;
  const char* leg;
  int MIN;
  int mid;
  int MAX;
  int current_value;
};

struct legServoXY {
  unsigned int pcaPosition;
  const char* leg;
  int front;
  int mid;
  int side;
};

struct legServoZ {
  unsigned int pcaPosition;
  const char* leg;
  int MIN;
  int floor_height;
  int mid;
  int MAX;
};

struct cameraServo topXY = {8, "Top Camera XY", 0, 100, 150, 100};

struct legServoXY frontRightXY = {5, "Front Right Leg XY", 100, 170, 190};
struct legServoXY frontLeftXY = {7, "Front Left Leg XY", 150, 80, 50};
struct legServoXY backRightXY = {4, "Back Right Leg XY", 110, 45, 18};
struct legServoXY backLeftXY = {6, "Back Left Leg XY", 90, 170, 190};

struct legServoZ frontRightZ = {1, "Front Right Leg Z", 60, 110, ((200+60)/2)+20, 200};
struct legServoZ frontLeftZ = {3, "Front Left Leg Z", 150, 100, (150/2) - 20, 0};
struct legServoZ backRightZ = {0, "Back Right Leg Z", 210, 180, ((210+70)/2) - 20, 70};
struct legServoZ backLeftZ = {2, "Back Left Leg Z", 90, 140, ((210+90)/2) + 25, 210};

class MyServo {


};

// Forward Declaration
int angleToPulse(int ang);
int continuous_movement();
void walk_front(int delay_value);
void look_around();
void walk_back();
void turn_left();
void lay_flat();
void stand();
void rotate_left();
void bow();
void wave();
void look_left();
void look_right();
void look_forward();


void setup()
{
  Serial.begin(9600);
  pwmServoDriver.begin();
  pwmServoDriver.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  // Wifi setup
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }

  Serial.println();
  Serial.print("ESP IP Address: http://");
  Serial.println(WiFi.localIP());

  // Send web page to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Receive an HTTP GET request
  server.on("/forward", HTTP_GET, [] (AsyncWebServerRequest *request) {
    walk_front(300);
    request->send(200, "text/plain", "ok");
  });

  // Receive an HTTP GET request
  server.on("/stop", HTTP_GET, [] (AsyncWebServerRequest *request) {
    stand();
    request->send(200, "text/plain", "ok");
  });

  server.on("/wave", HTTP_GET, [] (AsyncWebServerRequest *request) {
    stand();
    wave();
    request->send(200, "text/plain", "ok");
  });

  server.on("/lay_flat", HTTP_GET, [] (AsyncWebServerRequest *request) {
    lay_flat();
    request->send(200, "text/plain", "ok");
  });

  server.on("/bow", HTTP_GET, [] (AsyncWebServerRequest *request) {
    bow();
    request->send(200, "text/plain", "ok");
  });

  //server.on("/look_around", HTTP_GET, [] (AsyncWebServerRequest *request) {
  //  look_around();
  //  request->send(200, "text/plain", "ok");
  //});

  server.on("/look_left", HTTP_GET, [] (AsyncWebServerRequest *request) {
    look_left();
    request->send(200, "text/plain", "ok");
  });

  server.on("/look_right", HTTP_GET, [] (AsyncWebServerRequest *request) {
    look_right();
    request->send(200, "text/plain", "ok");
  });

  server.on("/look_forward", HTTP_GET, [] (AsyncWebServerRequest *request) {
    look_forward();
    request->send(200, "text/plain", "ok");
  });

  server.onNotFound(notFound);
  server.begin();
}

// the code inside loop() has been updated by Robojax
void loop() {

  //watch video for details: https://youtu.be/bal2STaoQ1M
  //for(int i=0; i<16; i++)
  //{
  //  for(int angle = 0; angle < 181; angle += 10){
  //    delay(50);
  //      pwmServoDriver.setPWM(i, 0, angleToPulse(angle));
  //      // see YouTube video for details (robojax)
  //  }
  //}

  //pwmServoDriver.setPWM(frontLeftXY, 0, angleToPulse(frontLeftXY.mid));
  //pwmServoDriver.setPWM(frontRightXY, 0, angleToPulse(frontRightXY.mid));
  //pwmServoDriver.setPWM(backLeftXY, 0, angleToPulse(backLeftXY.mid));
  //pwmServoDriver.setPWM(backRightXY, 0, angleToPulse(backRightXY.mid));

  //stand();
  //delay(1000);
  //look_around();
  //delay(1000);
  //lay_flat();
  //delay(2000);
  //stand();
  //delay(1000);

  //turn_left();
  //delay(2000);

  //for(int cycles = 0; cycles < 10; cycles++)
  //  walk_front(300);

  //wave();
  //delay(5000);
  //bow();
  //delay(5000);
  //walk();
  //delay(2000);


  // robojax PCA9865 16 channel Servo control
  //delay(1000);// wait for 1 second
}


/***********************************************************************/
/*********************** Function declaration **************************/
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
   int pulse = map(ang,0, 180, SERVOMIN, SERVOMAX);// map angle of 0 to 180 to Servo min and Servo max
   Serial.print("Angle: ");Serial.print(ang);
   Serial.print(" pulse: ");Serial.println(pulse);
   return pulse;
}

// funkce ktera vytvori plynuly pohyb postupnym zrychlenim a zpomalenim na zacatku a konci daneho pohybu.
int continuous_movement()
{
  return 0;
}

/*
 * @param: int delay_value - Urcuje rychlost pohybu
*/
void walk_front(int delay_value)
{
  // 1. polovina cyklu
  pwmServoDriver.setPWM(frontRightZ.pcaPosition, 0, angleToPulse(frontRightZ.floor_height));
  pwmServoDriver.setPWM(backLeftZ.pcaPosition, 0, angleToPulse(backLeftZ.floor_height));

  pwmServoDriver.setPWM(frontRightXY.pcaPosition, 0, angleToPulse(frontRightXY.side));
  pwmServoDriver.setPWM(backLeftXY.pcaPosition, 0, angleToPulse(backLeftXY.mid - 20));

  pwmServoDriver.setPWM(frontLeftXY.pcaPosition, 0, angleToPulse(frontLeftXY.side));
  pwmServoDriver.setPWM(backRightXY.pcaPosition, 0, angleToPulse(backRightXY.mid + 20));

  pwmServoDriver.setPWM(frontRightXY.pcaPosition, 0, angleToPulse(frontRightXY.mid));
  pwmServoDriver.setPWM(backLeftXY.pcaPosition, 0, angleToPulse(backLeftXY.side));

  pwmServoDriver.setPWM(frontRightZ.pcaPosition, 0, angleToPulse(frontRightZ.mid));
  pwmServoDriver.setPWM(backLeftZ.pcaPosition, 0, angleToPulse(backLeftZ.mid));
  delay(delay_value);

  // 2. polovina cyklu
  pwmServoDriver.setPWM(frontLeftZ.pcaPosition, 0, angleToPulse(frontLeftZ.floor_height));
  pwmServoDriver.setPWM(backRightZ.pcaPosition, 0, angleToPulse(backRightZ.floor_height));

  pwmServoDriver.setPWM(frontRightXY.pcaPosition, 0, angleToPulse(frontRightXY.side));
  pwmServoDriver.setPWM(backLeftXY.pcaPosition, 0, angleToPulse(backLeftXY.mid - 20));

  pwmServoDriver.setPWM(frontLeftXY.pcaPosition, 0, angleToPulse(frontLeftXY.mid));
  pwmServoDriver.setPWM(backRightXY.pcaPosition, 0, angleToPulse(backRightXY.side));
  pwmServoDriver.setPWM(frontLeftZ.pcaPosition, 0, angleToPulse(frontLeftZ.mid));
  pwmServoDriver.setPWM(backRightZ.pcaPosition, 0, angleToPulse(backRightZ.mid));
  delay(delay_value);
}

void look_left()
{
  if (topXY.current_value + 5 < topXY.MAX)
  {
    topXY.current_value += 5;
    pwmServoDriver.setPWM(topXY.pcaPosition, 0, angleToPulse(topXY.current_value));
  }
  else
  {
    topXY.current_value = topXY.MAX;
  }
}

void look_right()
{
  if (topXY.current_value - 5 > topXY.MIN)
  {
    topXY.current_value -= 5;
    pwmServoDriver.setPWM(topXY.pcaPosition, 0, angleToPulse(topXY.current_value));
  }
  else
  {
    topXY.current_value = topXY.MIN;
  }
}

void look_forward()
{
  topXY.current_value = topXY.mid;
  pwmServoDriver.setPWM(topXY.pcaPosition, 0, angleToPulse(topXY.mid));
}

void look_around()
{
  pwmServoDriver.setPWM(topXY.pcaPosition, 0, angleToPulse(topXY.mid));

  for(int i = topXY.mid; i >= topXY.MIN+30; i--)
  {
    pwmServoDriver.setPWM(topXY.pcaPosition, 0, angleToPulse(i));
    delay(10);
  }

  for(int i = topXY.MIN+30; i < topXY.mid; i++)
  {
    pwmServoDriver.setPWM(topXY.pcaPosition, 0, angleToPulse(i));
    delay(10);
  }

  for(int i = topXY.mid; i < topXY.MAX; i++)
  {
    pwmServoDriver.setPWM(topXY.pcaPosition, 0, angleToPulse(i));
    delay(10);
  }

  for(int i = topXY.MAX; i >= topXY.mid; i--)
  {
    pwmServoDriver.setPWM(topXY.pcaPosition, 0, angleToPulse(i));
    delay(10);
  }

  delay(500);

  pwmServoDriver.setPWM(topXY.pcaPosition, 0, angleToPulse(topXY.MAX));
  delay(500);

  pwmServoDriver.setPWM(topXY.pcaPosition, 0, angleToPulse(topXY.mid));
}

//TODO
void push_up()
{
  // zadni nohy dozadu

  // predni nohy na bok, a klikovat
  
}

void walk_back()
{

}

void turn_left()
{
  // One half-cycle
  pwmServoDriver.setPWM(frontRightZ.pcaPosition, 0, angleToPulse(frontRightZ.MIN));
  pwmServoDriver.setPWM(backLeftZ.pcaPosition, 0, angleToPulse(backLeftZ.MIN));
  delay(300);

  pwmServoDriver.setPWM(frontRightXY.pcaPosition, 0, angleToPulse(frontRightXY.mid-20));
  pwmServoDriver.setPWM(backLeftXY.pcaPosition, 0, angleToPulse(backLeftXY.side));
  delay(300);

  pwmServoDriver.setPWM(frontRightZ.pcaPosition, 0, angleToPulse(frontRightZ.mid));
  pwmServoDriver.setPWM(backLeftZ.pcaPosition, 0, angleToPulse(backLeftZ.mid));
  delay(300);


  // first pair of legs do the step
  pwmServoDriver.setPWM(frontRightXY.pcaPosition, 0, angleToPulse(frontRightXY.mid+20));
  pwmServoDriver.setPWM(backLeftXY.pcaPosition, 0, angleToPulse(backLeftXY.side-20));
  // Move second pair of legs up
  pwmServoDriver.setPWM(frontLeftZ.pcaPosition, 0, angleToPulse(frontLeftZ.MIN));
  pwmServoDriver.setPWM(backRightZ.pcaPosition, 0, angleToPulse(backRightZ.MIN));
  delay(300);

  // Second half-cycle
  pwmServoDriver.setPWM(frontLeftXY.pcaPosition, 0, angleToPulse(frontLeftXY.mid-20));
  pwmServoDriver.setPWM(backRightXY.pcaPosition, 0, angleToPulse(backRightXY.side));
  delay(300);

  pwmServoDriver.setPWM(frontLeftZ.pcaPosition, 0, angleToPulse(frontLeftZ.mid));
  pwmServoDriver.setPWM(backRightZ.pcaPosition, 0, angleToPulse(backRightZ.mid));
  delay(300);

  pwmServoDriver.setPWM(frontLeftXY.pcaPosition, 0, angleToPulse(frontLeftXY.mid+20));
  pwmServoDriver.setPWM(backRightXY.pcaPosition, 0, angleToPulse(backRightXY.side+20));
  delay(300);
}

/*
rozplacnout se na zem
*/
void lay_flat()
{
  pwmServoDriver.setPWM(frontLeftZ.pcaPosition, 0, angleToPulse(frontLeftZ.floor_height));
  pwmServoDriver.setPWM(frontRightZ.pcaPosition, 0, angleToPulse(frontRightZ.floor_height));
  pwmServoDriver.setPWM(backRightZ.pcaPosition, 0, angleToPulse(backRightZ.floor_height));
  pwmServoDriver.setPWM(backLeftZ.pcaPosition, 0, angleToPulse(backLeftZ.floor_height));
}

void stand()
{
  pwmServoDriver.setPWM(backRightZ.pcaPosition, 0, angleToPulse(backRightZ.mid));
  pwmServoDriver.setPWM(backLeftZ.pcaPosition, 0, angleToPulse(backLeftZ.mid));
  pwmServoDriver.setPWM(frontLeftZ.pcaPosition, 0, angleToPulse(frontLeftZ.mid));
  pwmServoDriver.setPWM(frontRightZ.pcaPosition, 0, angleToPulse(frontRightZ.mid));

  pwmServoDriver.setPWM(frontRightXY.pcaPosition, 0, angleToPulse(frontRightXY.mid));
  pwmServoDriver.setPWM(frontLeftXY.pcaPosition, 0, angleToPulse(frontLeftXY.mid));
  pwmServoDriver.setPWM(backRightXY.pcaPosition, 0, angleToPulse(backRightXY.mid));
  pwmServoDriver.setPWM(backLeftXY.pcaPosition, 0, angleToPulse(backLeftXY.mid));
}

void rotate_left()
{
  pwmServoDriver.setPWM(frontRightXY.pcaPosition, 0, angleToPulse(frontRightXY.side));
  pwmServoDriver.setPWM(frontRightXY.pcaPosition, 0, angleToPulse(frontRightXY.side));
}

void bow()
{
  //for(int i = )
  pwmServoDriver.setPWM(frontLeftZ.pcaPosition, 0, angleToPulse(frontLeftZ.floor_height));
  pwmServoDriver.setPWM(frontRightZ.pcaPosition, 0, angleToPulse(frontRightZ.floor_height));
}

void wave()
{
  // sit (back legs down)
  pwmServoDriver.setPWM(backRightZ.pcaPosition, 0, angleToPulse(backRightZ.floor_height));
  pwmServoDriver.setPWM(backLeftZ.pcaPosition, 0, angleToPulse(backLeftZ.floor_height));
  pwmServoDriver.setPWM(frontLeftZ.pcaPosition, 0, angleToPulse(frontLeftZ.MAX));
  //pwmServoDriver.setPWM(frontLeftXY.pcaPosition, 0, angleToPulse(frontLeftXY.front));

  // 
  pwmServoDriver.setPWM(frontRightXY.pcaPosition, 0, angleToPulse(frontRightXY.mid));
  pwmServoDriver.setPWM(frontRightZ.pcaPosition, 0, angleToPulse(frontRightZ.MIN));
  delay(1000);

  // wave
  pwmServoDriver.setPWM(frontRightXY.pcaPosition, 0, angleToPulse(frontRightXY.front+30));
  delay(200);
  pwmServoDriver.setPWM(frontRightXY.pcaPosition, 0, angleToPulse(frontRightXY.side-30));
  delay(200);
  pwmServoDriver.setPWM(frontRightXY.pcaPosition, 0, angleToPulse(frontRightXY.front+30));
  delay(200);
  pwmServoDriver.setPWM(frontRightXY.pcaPosition, 0, angleToPulse(frontRightXY.side-30));
  delay(200);

  pwmServoDriver.setPWM(frontRightZ.pcaPosition, 0, angleToPulse(frontRightZ.mid));
}
