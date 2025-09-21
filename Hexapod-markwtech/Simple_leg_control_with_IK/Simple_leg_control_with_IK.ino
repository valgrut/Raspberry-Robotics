/*
 * Control one hexapod 3DOF leg with Node MCU ESP8266 using servo driver PCA9685.
 *
 * Set given position using Inverse Kinematics.
 */

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Network Credentials
const char* ssid = "INFOS_Peska";
const char* password = "jirela2002len";

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
        <td align="center">
          <button class="button" onmousedown="toggleCheckbox('xpp');" ontouchstart="toggleCheckbox('xpp');" onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">xpp</button>
        </td>
        <td align="center">
          <button class="button" onmousedown="toggleCheckbox('xmm');" ontouchstart="toggleCheckbox('xmm');" onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">xmm</button>
        </td>
      </tr>
      <tr>
        <td align="center">
          <button class="button" onmousedown="toggleCheckbox('ypp');" ontouchstart="toggleCheckbox('ypp');" onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">ypp</button>
        </td>
        <td align="center">
          <button class="button" onmousedown="toggleCheckbox('ymm');" ontouchstart="toggleCheckbox('ymm');" onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">ymm</button>
        </td>
      </tr>
      <tr>
        <td align="center">
          <button class="button" onmousedown="toggleCheckbox('zpp');" ontouchstart="toggleCheckbox('zpp');" onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">zpp</button>
        </td>
        <td align="center">
          <button class="button" onmousedown="toggleCheckbox('zmm');" ontouchstart="toggleCheckbox('zmm');" onmouseup="toggleCheckbox('stop');" ontouchend="toggleCheckbox('stop');">zmm</button>
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

#define SHOULDER_SERVO 0
#define ELBOW_SERVO 1
#define WRIST_SERVO 2

#define SERVOMIN  125 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  575 

//Servo shoulder_servo;  // create servo object to control a servo
// twelve servo objects can be created on most boards
//Servo elbow_servo;
//Servo wrist_servo;

int angleToPulse(int ang){
   int pulse = map(ang,0, 180, SERVOMIN, SERVOMAX);// map angle of 0 to 180 to Servo min and Servo max
   //Serial.print("Angle: ");Serial.print(ang);
   //Serial.print(" pulse: ");Serial.println(pulse);
   return pulse;
}

struct angles {
  double base;
  double elbow;
  double shoulder;
};

struct angles calc_ik(int d1, int d2_forearm, int tx, int ty, int tz)
{
    //if (d1 + d2) >= tx or (d1 + d2) >= ty or (d1 + d2) >= tz:
        //print("Invalid values")

    double r = sqrt(pow(tx,2) + pow(ty,2) + pow(tz,2));
    double base = atan2(ty,tx);
    double acos_value = (pow(r,2) - pow(d1,2) - pow(d2_forearm,2)) / (2 * d1 * d2_forearm);
    //print(acos_value)
    double elbow = -acos(acos_value);
    double shoulder = asin(tz/r) + atan2((d2_forearm * sin(elbow)), (d1 + (d2_forearm * cos(elbow))));
    
    struct angles angs;
    angs.base = base * (180.0 / M_PI);
    angs.elbow = elbow * (180.0 / M_PI);
    angs.shoulder = shoulder * (180.0 / M_PI);

    return angs;
}

int d1 = 6;
int d2_forearm = 12;

// NodeMCU ESP8266: GPIO4 (SDA) and GPIO5 (SCL)
Adafruit_PWMServoDriver pwmServoDriver = Adafruit_PWMServoDriver(0x40);

int x = 0;
int y = 0;
int z = 0;

void setup() {
  Serial.begin(9600);
  pwmServoDriver.begin();
  pwmServoDriver.setPWMFreq(60);

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
  server.on("/xpp", HTTP_GET, [] (AsyncWebServerRequest *request) {
    x++;

    struct angles curr_angs = calc_ik(d1, d2_forearm, x, y, z);
    Serial.print("elbow: ");Serial.print(curr_angs.elbow);
    Serial.print("Shoulder: ");Serial.print(curr_angs.shoulder);
    Serial.print("base: ");Serial.println(curr_angs.base);
    pwmServoDriver.setPWM(SHOULDER_SERVO, 0, angleToPulse(curr_angs.elbow));
    pwmServoDriver.setPWM(ELBOW_SERVO, 0, angleToPulse(curr_angs.shoulder));
    pwmServoDriver.setPWM(WRIST_SERVO, 0, angleToPulse(curr_angs.base));
    request->send(200, "text/plain", "ok");
  });

  // Receive an HTTP GET request
  server.on("/xmm", HTTP_GET, [] (AsyncWebServerRequest *request) {
    x--;

    struct angles curr_angs = calc_ik(d1, d2_forearm, x, y, z);
    Serial.print("elbow: ");Serial.print(curr_angs.elbow);
    Serial.print("Shoulder: ");Serial.print(curr_angs.shoulder);
    Serial.print("base: ");Serial.println(curr_angs.base);
    pwmServoDriver.setPWM(SHOULDER_SERVO, 0, angleToPulse(curr_angs.elbow));
    pwmServoDriver.setPWM(ELBOW_SERVO, 0, angleToPulse(curr_angs.shoulder));
    pwmServoDriver.setPWM(WRIST_SERVO, 0, angleToPulse(curr_angs.base));
    request->send(200, "text/plain", "ok");
  });

  // Receive an HTTP GET request
  server.on("/ypp", HTTP_GET, [] (AsyncWebServerRequest *request) {
    y++;

    struct angles curr_angs = calc_ik(d1, d2_forearm, x, y, z);
    Serial.print("elbow: ");Serial.print(curr_angs.elbow);
    Serial.print("Shoulder: ");Serial.print(curr_angs.shoulder);
    Serial.print("base: ");Serial.println(curr_angs.base);
    pwmServoDriver.setPWM(SHOULDER_SERVO, 0, angleToPulse(curr_angs.elbow));
    pwmServoDriver.setPWM(ELBOW_SERVO, 0, angleToPulse(curr_angs.shoulder));
    pwmServoDriver.setPWM(WRIST_SERVO, 0, angleToPulse(curr_angs.base));

    request->send(200, "text/plain", "ok");
  });

  // Receive an HTTP GET request
  server.on("/ymm", HTTP_GET, [] (AsyncWebServerRequest *request) {
    y--;

    struct angles curr_angs = calc_ik(d1, d2_forearm, x, y, z);
    Serial.print("elbow: ");Serial.print(curr_angs.elbow);
    Serial.print("Shoulder: ");Serial.print(curr_angs.shoulder);
    Serial.print("base: ");Serial.println(curr_angs.base);
    pwmServoDriver.setPWM(SHOULDER_SERVO, 0, angleToPulse(curr_angs.elbow));
    pwmServoDriver.setPWM(ELBOW_SERVO, 0, angleToPulse(curr_angs.shoulder));
    pwmServoDriver.setPWM(WRIST_SERVO, 0, angleToPulse(curr_angs.base));

    request->send(200, "text/plain", "ok");
  });

  // Receive an HTTP GET request
  server.on("/zpp", HTTP_GET, [] (AsyncWebServerRequest *request) {
    z++;

    struct angles curr_angs = calc_ik(d1, d2_forearm, x, y, z);
    Serial.print("elbow: ");Serial.print(curr_angs.elbow);
    Serial.print("Shoulder: ");Serial.print(curr_angs.shoulder);
    Serial.print("base: ");Serial.println(curr_angs.base);
    pwmServoDriver.setPWM(SHOULDER_SERVO, 0, angleToPulse(curr_angs.elbow));
    pwmServoDriver.setPWM(ELBOW_SERVO, 0, angleToPulse(curr_angs.shoulder));
    pwmServoDriver.setPWM(WRIST_SERVO, 0, angleToPulse(curr_angs.base));

    request->send(200, "text/plain", "ok");
  });
  
  // Receive an HTTP GET request
  server.on("/zmm", HTTP_GET, [] (AsyncWebServerRequest *request) {
    z--;

    struct angles curr_angs = calc_ik(d1, d2_forearm, x, y, z);
    Serial.print("elbow: ");Serial.print(curr_angs.elbow);
    Serial.print("Shoulder: ");Serial.print(curr_angs.shoulder);
    Serial.print("base: ");Serial.println(curr_angs.base);
    pwmServoDriver.setPWM(SHOULDER_SERVO, 0, angleToPulse(curr_angs.elbow));
    pwmServoDriver.setPWM(ELBOW_SERVO, 0, angleToPulse(curr_angs.shoulder));
    pwmServoDriver.setPWM(WRIST_SERVO, 0, angleToPulse(curr_angs.base));

    request->send(200, "text/plain", "ok");
  });

  server.on("/stop", HTTP_GET, [] (AsyncWebServerRequest *request) {
    Serial.print("x: ");Serial.print(x);
    Serial.print("y: ");Serial.print(y);
    Serial.print("z: ");Serial.println(z);
    request->send(200, "text/plain", "ok");
  });

  server.onNotFound(notFound);
  server.begin();
}
               
void loop() 
{
  /* Samostatny test serv */
  /*
  pwmServoDriver.setPWM(SHOULDER_SERVO, 0, angleToPulse(180));
  pwmServoDriver.setPWM(ELBOW_SERVO, 0, angleToPulse(180));
  pwmServoDriver.setPWM(WRIST_SERVO, 0, angleToPulse(90));
  */

  /* Test IK */
  /*
  struct angles curr_angs = calc_ik(d1, d2_forearm, x, y, z);
  Serial.print("Base: ");Serial.print(curr_angs.base);
  Serial.print("Shoulder: ");Serial.print(curr_angs.shoulder);
  Serial.print("Elbow: ");Serial.println(curr_angs.elbow);
  pwmServoDriver.setPWM(SHOULDER_SERVO, 0, angleToPulse(curr_angs.base));
  pwmServoDriver.setPWM(ELBOW_SERVO, 0, angleToPulse(curr_angs.shoulder));
  pwmServoDriver.setPWM(WRIST_SERVO, 0, angleToPulse(curr_angs.elbow));

  delay(2000);*/

  /* Pohyb po care / ctverci / kresleni ... */
  /*
  for (int x = 0; x < 10; x++)
  {
    struct angles curr_angs = calc_ik(d1, d2_forearm, x, 2, 0);
    pwmServoDriver.setPWM(SHOULDER_SERVO, 0, angleToPulse(curr_angs.base));
    pwmServoDriver.setPWM(ELBOW_SERVO, 0, angleToPulse(curr_angs.elbow));
    pwmServoDriver.setPWM(WRIST_SERVO, 0, angleToPulse(curr_angs.shoulder));
    delay(100);
  }
  */
}
