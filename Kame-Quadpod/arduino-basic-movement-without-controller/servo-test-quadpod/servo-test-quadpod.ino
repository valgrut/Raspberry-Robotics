#include <PCA9685.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>


#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>

#include <ESPAsyncWebServer.h>

// Library Manager -> napsat Servo -> install
#include <Servo.h>

// Network Credentials


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
        <td align="center">
          <button class="button" onmousedown="toggleCheckbox('look_around');" ontouchstart="toggleCheckbox('look_around');" onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">Look Around</button>
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
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwmServoDriver = Adafruit_PWMServoDriver(0x41);

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
// Watch video V1 to understand the two lines below: http://youtu.be/y8X9X10Tn1k
//TODO toto predelat specificky pro moje serva
#define SERVOMIN  125 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  575 // this is the 'maximum' pulse length count (out of 4096)

// TODO: transition state machine

// our servo # counter
uint8_t servonum = 0;

struct legServo {
  unsigned int pcaPosition;
  
}

enum SERVOS {
  BACK_RIGHT_Z = 0, //OK
  FRONT_RIGHT_Z = 1, //OK
  FRONT_RIGHT_XY = 5, //OK
  FRONT_LEFT_Z = 3, //OK
  BACK_LEFT_XY = 6, //OK
  BACK_LEFT_Z = 2, //OK
  BACK_RIGHT_XY = 4, //ok
  FRONT_LEFT_XY = 7, //OK
  TOP = 8 //OK
};

int height_modificator = 40; //40 je ok pro postavenou pozici robota

enum LEG_RANGES {
  // X-Y range
  FRONT_RIGHT_XY_front = 100,
  FRONT_RIGHT_XY_mid = 170,
  FRONT_RIGHT_XY_side = 190,

  FRONT_LEFT_XY_front = 150,
  FRONT_LEFT_XY_mid = 80,
  FRONT_LEFT_XY_side = 50,

  BACK_RIGHT_XY_front = 110,
  BACK_RIGHT_XY_mid = 45,
  BACK_RIGHT_XY_side = 18,

  BACK_LEFT_XY_front = 90,
  BACK_LEFT_XY_mid = 170,
  BACK_LEFT_XY_side = 190,

  // Z-height range
  FRONT_RIGHT_Z_min = 60,  // upmost limit (Skrcena noha)
  FRONT_RIGHT_Z_floor = 110,  // Body and legs both touching the floor
  FRONT_RIGHT_Z_mid = ((200+60)/2) + 20,
  FRONT_RIGHT_Z_max = 200,  // leg stretched limit (Natazena noha)

  FRONT_LEFT_Z_min = 150,
  FRONT_LEFT_Z_floor = 100, //TODO
  FRONT_LEFT_Z_mid = (150/2) - 20,
  FRONT_LEFT_Z_max = 0,

  BACK_RIGHT_Z_min = 210,
  BACK_RIGHT_Z_floor = 180, //TODO
  BACK_RIGHT_Z_mid = ((210+70)/2) - 20,
  BACK_RIGHT_Z_max = 70,

  BACK_LEFT_Z_min = 90, //TODO
  BACK_LEFT_Z_floor = 140, //TODO
  BACK_LEFT_Z_mid = ((210+90)/2) + 25,
  BACK_LEFT_Z_max = 210, //TODO

  // camera range
  TOP_min = 0,
  TOP_mid = 100,
  TOP_max = 150
};


// Forward Declaration
int angleToPulse(int ang);
int continuous_movement();
int walk_front(int delay_value);
int look_around();
int walk_back();
int turn_left();
int lay_flat();
int stand();
int rotate_left();
int bow();
int wave();


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

  server.on("/look_around", HTTP_GET, [] (AsyncWebServerRequest *request) {
    look_around();
    delay(2000);
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

  //pwmServoDriver.setPWM(FRONT_LEFT_XY, 0, angleToPulse(FRONT_LEFT_XY_mid));
  //pwmServoDriver.setPWM(FRONT_RIGHT_XY, 0, angleToPulse(FRONT_RIGHT_XY_mid));
  //pwmServoDriver.setPWM(BACK_LEFT_XY, 0, angleToPulse(BACK_LEFT_XY_mid));
  //pwmServoDriver.setPWM(BACK_RIGHT_XY, 0, angleToPulse(BACK_RIGHT_XY_mid));

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
   int pulse = map(ang,0, 180, SERVOMIN,SERVOMAX);// map angle of 0 to 180 to Servo min and Servo max 
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
int walk_front(int delay_value)
{
  // 1. polovina cyklu
  pwmServoDriver.setPWM(FRONT_RIGHT_Z, 0, angleToPulse(FRONT_RIGHT_Z_floor));
  pwmServoDriver.setPWM(BACK_LEFT_Z, 0, angleToPulse(BACK_LEFT_Z_floor));
  
  pwmServoDriver.setPWM(FRONT_RIGHT_XY, 0, angleToPulse(FRONT_RIGHT_XY_side));
  pwmServoDriver.setPWM(BACK_LEFT_XY, 0, angleToPulse(BACK_LEFT_XY_mid - 20));

  pwmServoDriver.setPWM(FRONT_LEFT_XY, 0, angleToPulse(FRONT_LEFT_XY_side));
  pwmServoDriver.setPWM(BACK_RIGHT_XY, 0, angleToPulse(BACK_RIGHT_XY_mid + 20));
  
  pwmServoDriver.setPWM(FRONT_RIGHT_XY, 0, angleToPulse(FRONT_RIGHT_XY_mid));
  pwmServoDriver.setPWM(BACK_LEFT_XY, 0, angleToPulse(BACK_LEFT_XY_side));

  pwmServoDriver.setPWM(FRONT_RIGHT_Z, 0, angleToPulse(FRONT_RIGHT_Z_mid));
  pwmServoDriver.setPWM(BACK_LEFT_Z, 0, angleToPulse(BACK_LEFT_Z_mid));
  delay(delay_value);

  // 2. polovina cyklu
  pwmServoDriver.setPWM(FRONT_LEFT_Z, 0, angleToPulse(FRONT_LEFT_Z_floor));
  pwmServoDriver.setPWM(BACK_RIGHT_Z, 0, angleToPulse(BACK_RIGHT_Z_floor));

  pwmServoDriver.setPWM(FRONT_RIGHT_XY, 0, angleToPulse(FRONT_RIGHT_XY_side));
  pwmServoDriver.setPWM(BACK_LEFT_XY, 0, angleToPulse(BACK_LEFT_XY_mid - 20));

  pwmServoDriver.setPWM(FRONT_LEFT_XY, 0, angleToPulse(FRONT_LEFT_XY_mid));
  pwmServoDriver.setPWM(BACK_RIGHT_XY, 0, angleToPulse(BACK_RIGHT_XY_side));
  pwmServoDriver.setPWM(FRONT_LEFT_Z, 0, angleToPulse(FRONT_LEFT_Z_mid));
  pwmServoDriver.setPWM(BACK_RIGHT_Z, 0, angleToPulse(BACK_RIGHT_Z_mid));
  delay(delay_value);

  return 0;
}


int look_around()
{
  pwmServoDriver.setPWM(TOP, 0, angleToPulse(TOP_mid));

  for(int i = TOP_mid; i >= TOP_min+30; i--)
  {
    pwmServoDriver.setPWM(TOP, 0, angleToPulse(i));
    delay(10);
  }

  for(int i = TOP_min+30; i < TOP_mid; i++)
  {
    pwmServoDriver.setPWM(TOP, 0, angleToPulse(i));
    delay(10);
  }

  for(int i = TOP_mid; i < TOP_max; i++)
  {
    pwmServoDriver.setPWM(TOP, 0, angleToPulse(i));
    delay(10);
  }

  for(int i = TOP_max; i >= TOP_mid; i--)
  {
    pwmServoDriver.setPWM(TOP, 0, angleToPulse(i));
    delay(10);
  }

  delay(500);

  pwmServoDriver.setPWM(TOP, 0, angleToPulse(TOP_max));
  delay(500);

  pwmServoDriver.setPWM(TOP, 0, angleToPulse(TOP_mid));

  return 0;
}

int walk_back()
{
  return 0;
}

int turn_left()
{
  // One half-cycle
  pwmServoDriver.setPWM(FRONT_RIGHT_Z, 0, angleToPulse(FRONT_RIGHT_Z_min));
  pwmServoDriver.setPWM(BACK_LEFT_Z, 0, angleToPulse(BACK_LEFT_Z_min));
  delay(300);

  pwmServoDriver.setPWM(FRONT_RIGHT_XY, 0, angleToPulse(FRONT_RIGHT_XY_mid-20));
  pwmServoDriver.setPWM(BACK_LEFT_XY, 0, angleToPulse(BACK_LEFT_XY_side));
  delay(300);

  pwmServoDriver.setPWM(FRONT_RIGHT_Z, 0, angleToPulse(FRONT_RIGHT_Z_mid));
  pwmServoDriver.setPWM(BACK_LEFT_Z, 0, angleToPulse(BACK_LEFT_Z_mid));
  delay(300);


  // first pair of legs do the step
  pwmServoDriver.setPWM(FRONT_RIGHT_XY, 0, angleToPulse(FRONT_RIGHT_XY_mid+20));
  pwmServoDriver.setPWM(BACK_LEFT_XY, 0, angleToPulse(BACK_LEFT_XY_side-20));
  // Move second pair of legs up
  pwmServoDriver.setPWM(FRONT_LEFT_Z, 0, angleToPulse(FRONT_LEFT_Z_min));
  pwmServoDriver.setPWM(BACK_RIGHT_Z, 0, angleToPulse(BACK_RIGHT_Z_min));
  delay(300);

  // Second half-cycle
  pwmServoDriver.setPWM(FRONT_LEFT_XY, 0, angleToPulse(FRONT_LEFT_XY_mid-20));
  pwmServoDriver.setPWM(BACK_RIGHT_XY, 0, angleToPulse(BACK_RIGHT_XY_side));
  delay(300);

  pwmServoDriver.setPWM(FRONT_LEFT_Z, 0, angleToPulse(FRONT_LEFT_Z_mid));
  pwmServoDriver.setPWM(BACK_RIGHT_Z, 0, angleToPulse(BACK_RIGHT_Z_mid));
  delay(300);

  pwmServoDriver.setPWM(FRONT_LEFT_XY, 0, angleToPulse(FRONT_LEFT_XY_mid+20));
  pwmServoDriver.setPWM(BACK_RIGHT_XY, 0, angleToPulse(BACK_RIGHT_XY_side+20));
  delay(300);

  return 0;
}

/* 
rozplacnout se na zem 
*/
int lay_flat()
{
  pwmServoDriver.setPWM(FRONT_LEFT_Z, 0, angleToPulse(FRONT_LEFT_Z_floor));
  pwmServoDriver.setPWM(FRONT_RIGHT_Z, 0, angleToPulse(FRONT_RIGHT_Z_floor));
  pwmServoDriver.setPWM(BACK_RIGHT_Z, 0, angleToPulse(BACK_RIGHT_Z_floor));
  pwmServoDriver.setPWM(BACK_LEFT_Z, 0, angleToPulse(BACK_LEFT_Z_floor));

  return 0;
}

int stand()
{
  pwmServoDriver.setPWM(BACK_RIGHT_Z, 0, angleToPulse(BACK_RIGHT_Z_mid));
  pwmServoDriver.setPWM(BACK_LEFT_Z, 0, angleToPulse(BACK_LEFT_Z_mid));
  pwmServoDriver.setPWM(FRONT_LEFT_Z, 0, angleToPulse(FRONT_LEFT_Z_mid));
  pwmServoDriver.setPWM(FRONT_RIGHT_Z, 0, angleToPulse(FRONT_RIGHT_Z_mid));

  pwmServoDriver.setPWM(FRONT_RIGHT_XY, 0, angleToPulse(FRONT_RIGHT_XY_mid));
  pwmServoDriver.setPWM(FRONT_LEFT_XY, 0, angleToPulse(FRONT_LEFT_XY_mid));
  pwmServoDriver.setPWM(BACK_RIGHT_XY, 0, angleToPulse(BACK_RIGHT_XY_mid));
  pwmServoDriver.setPWM(BACK_LEFT_XY, 0, angleToPulse(BACK_LEFT_XY_mid));

  return 0;
}

int rotate_left()
{
  pwmServoDriver.setPWM(FRONT_RIGHT_XY, 0, angleToPulse(FRONT_RIGHT_XY_side));
  pwmServoDriver.setPWM(FRONT_RIGHT_XY, 0, angleToPulse(FRONT_RIGHT_XY_side));

  return 0;
}

int bow()
{
  //for(int i = )
  pwmServoDriver.setPWM(FRONT_LEFT_Z, 0, angleToPulse(FRONT_LEFT_Z_floor));
  pwmServoDriver.setPWM(FRONT_RIGHT_Z, 0, angleToPulse(FRONT_RIGHT_Z_floor));

  return 0;
}

int wave() 
{
  pwmServoDriver.setPWM(FRONT_RIGHT_XY, 0, angleToPulse(FRONT_RIGHT_XY_mid));
  pwmServoDriver.setPWM(FRONT_RIGHT_Z, 0, angleToPulse(FRONT_RIGHT_Z_min));
  delay(1000);
  
  pwmServoDriver.setPWM(FRONT_RIGHT_XY, 0, angleToPulse(FRONT_RIGHT_XY_front+30));
  delay(200);
  pwmServoDriver.setPWM(FRONT_RIGHT_XY, 0, angleToPulse(FRONT_RIGHT_XY_side-30));
  delay(200);
  pwmServoDriver.setPWM(FRONT_RIGHT_XY, 0, angleToPulse(FRONT_RIGHT_XY_front+30));
  delay(200);
  pwmServoDriver.setPWM(FRONT_RIGHT_XY, 0, angleToPulse(FRONT_RIGHT_XY_side-30));
  delay(200);
  
  pwmServoDriver.setPWM(FRONT_RIGHT_Z, 0, angleToPulse(FRONT_RIGHT_Z_mid));

  return 0;
}
