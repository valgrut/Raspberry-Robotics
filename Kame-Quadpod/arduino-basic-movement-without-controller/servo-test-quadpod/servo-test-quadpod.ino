#include <PCA9685.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>

#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

// Library Manager -> napsat Servo -> install
#include <Servo.h>

// REPLACE WITH YOUR NETWORK CREDENTIALS


const int output = 0;
Servo s1;

// HTML web page
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
  <head>
    <title>ESP Pushbutton Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
      body { font-family: Arial; text-align: center; margin:0px auto; padding-top: 30px;}
      .button {
        padding: 10px 20px;
        font-size: 24px;
        text-align: center;
        outline: none;
        color: #fff;
        background-color: #2f4468;
        border: none;
        border-radius: 5px;
        box-shadow: 0 6px #999;
        cursor: pointer;
        -webkit-touch-callout: none;
        -webkit-user-select: none;
        -khtml-user-select: none;
        -moz-user-select: none;
        -ms-user-select: none;
        user-select: none;
        -webkit-tap-highlight-color: rgba(0,0,0,0);
      }  
      .button:hover {background-color: #1f2e45}
      .button:active {
        background-color: #1f2e45;
        box-shadow: 0 4px #666;
        transform: translateY(2px);
      }
    </style>
  </head>
  <body>
    <h1>ESP Pushbutton Web Server</h1>
    <button class="button" onmousedown="toggleCheckbox('on');" ontouchstart="toggleCheckbox('on');" onmouseup="toggleCheckbox('off');" ontouchend="toggleCheckbox('off');">LED PUSHBUTTON</button>
   <script>
   function toggleCheckbox(x) {
     var xhr = new XMLHttpRequest();
     xhr.open("GET", "/" + x, true);
     xhr.send();
   }
  </script>
  </body>
</html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

AsyncWebServer server(80);

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwmServoDriver = Adafruit_PWMServoDriver();
// you can also call it with a different address you want
//Adafruit_PWMServoDriver pwmServoDriver = Adafruit_PWMServoDriver(0x41);

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
// Watch video V1 to understand the two lines below: http://youtu.be/y8X9X10Tn1k
#define SERVOMIN  125 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  575 // this is the 'maximum' pulse length count (out of 4096)

// our servo # counter
uint8_t servonum = 0;

void setup() {
  Serial.begin(9600);
  pwmServoDriver.begin();
  pwmServoDriver.setPWMFreq(60);  // Analog servos run at ~60 Hz updates

  s1.attach(output);

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
  
  pinMode(output, OUTPUT);
  digitalWrite(output, LOW);
  
  // Send web page to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Receive an HTTP GET request
  server.on("/on", HTTP_GET, [] (AsyncWebServerRequest *request) {
    //digitalWrite(output, HIGH);
    s1.write(180); 
    request->send(200, "text/plain", "ok");
  });

  // Receive an HTTP GET request
  server.on("/off", HTTP_GET, [] (AsyncWebServerRequest *request) {
    //digitalWrite(output, LOW);
    s1.write(0); 
    request->send(200, "text/plain", "ok");
  });
  
  server.onNotFound(notFound);
  server.begin();
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

// funkce ktera vytvori plynuly pohyb postupnym zrychlenim a zpomalenim na zacatku a konci daneho pohybu.
int continuous_movement() {
  return 0;
}


// v0.2 - Working!!!
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

// v0.1 
int walk_front_v01()
{
  // 1. polovina cyklu
  pwmServoDriver.setPWM(FRONT_RIGHT_XY, 0, angleToPulse(FRONT_RIGHT_XY_side));
  pwmServoDriver.setPWM(BACK_LEFT_XY, 0, angleToPulse(BACK_LEFT_XY_mid - 20));

  // [1] Prvni par: zvedam nohy
  pwmServoDriver.setPWM(FRONT_RIGHT_Z, 0, angleToPulse(FRONT_RIGHT_Z_floor));
  pwmServoDriver.setPWM(BACK_LEFT_Z, 0, angleToPulse(BACK_LEFT_Z_floor));

  pwmServoDriver.setPWM(FRONT_LEFT_XY, 0, angleToPulse(FRONT_LEFT_XY_side));
  pwmServoDriver.setPWM(BACK_RIGHT_XY, 0, angleToPulse(BACK_RIGHT_XY_mid + 20));
  
  pwmServoDriver.setPWM(FRONT_RIGHT_XY, 0, angleToPulse(FRONT_RIGHT_XY_mid));
  pwmServoDriver.setPWM(BACK_LEFT_XY, 0, angleToPulse(BACK_LEFT_XY_side));

  
  pwmServoDriver.setPWM(FRONT_RIGHT_Z, 0, angleToPulse(FRONT_RIGHT_Z_mid));
  pwmServoDriver.setPWM(BACK_LEFT_Z, 0, angleToPulse(BACK_LEFT_Z_mid));
  //delay(100);

  // 2. polovina cyklu
  pwmServoDriver.setPWM(FRONT_LEFT_XY, 0, angleToPulse(FRONT_LEFT_XY_side));
  pwmServoDriver.setPWM(BACK_RIGHT_XY, 0, angleToPulse(BACK_RIGHT_XY_mid - 20));

  pwmServoDriver.setPWM(FRONT_LEFT_Z, 0, angleToPulse(FRONT_LEFT_Z_floor));
  pwmServoDriver.setPWM(BACK_RIGHT_Z, 0, angleToPulse(BACK_RIGHT_Z_floor));

  pwmServoDriver.setPWM(FRONT_RIGHT_XY, 0, angleToPulse(FRONT_RIGHT_XY_side));
  pwmServoDriver.setPWM(BACK_LEFT_XY, 0, angleToPulse(BACK_LEFT_XY_mid + 20));

  pwmServoDriver.setPWM(FRONT_LEFT_XY, 0, angleToPulse(FRONT_LEFT_XY_mid));
  pwmServoDriver.setPWM(BACK_RIGHT_XY, 0, angleToPulse(BACK_RIGHT_XY_side));
  pwmServoDriver.setPWM(FRONT_LEFT_Z, 0, angleToPulse(FRONT_LEFT_Z_mid));
  pwmServoDriver.setPWM(BACK_RIGHT_Z, 0, angleToPulse(BACK_RIGHT_Z_mid));
  //delay(100);

  return 0;
}



// v0.0 BACKUP of walk_front
int walk_front2()
{
  // [1] Prvni par: zvedam nohy
  pwmServoDriver.setPWM(FRONT_RIGHT_Z, 0, angleToPulse(FRONT_RIGHT_Z_min));
  pwmServoDriver.setPWM(BACK_LEFT_Z, 0, angleToPulse(BACK_LEFT_Z_min));
  delay(150);

  // [1] Prvni par: ve vzduchu krok
  pwmServoDriver.setPWM(FRONT_RIGHT_XY, 0, angleToPulse(FRONT_RIGHT_XY_mid-20));
  pwmServoDriver.setPWM(BACK_LEFT_XY, 0, angleToPulse(BACK_LEFT_XY_side));
  delay(150);

  // [2] Druhy par: Udelam krok
  pwmServoDriver.setPWM(FRONT_LEFT_XY, 0, angleToPulse(FRONT_LEFT_XY_mid+20));
  pwmServoDriver.setPWM(BACK_RIGHT_XY, 0, angleToPulse(BACK_RIGHT_XY_side+20));
  delay(150);
  
  // [1] Prvni par: pokladam na zem
  pwmServoDriver.setPWM(FRONT_RIGHT_Z, 0, angleToPulse(FRONT_RIGHT_Z_mid));
  pwmServoDriver.setPWM(BACK_LEFT_Z, 0, angleToPulse(BACK_LEFT_Z_mid));
  delay(300);


  // [2] Druhy par: zvedam nohy
  pwmServoDriver.setPWM(FRONT_LEFT_Z, 0, angleToPulse(FRONT_LEFT_Z_min));
  pwmServoDriver.setPWM(BACK_RIGHT_Z, 0, angleToPulse(BACK_RIGHT_Z_min));
  delay(150);

  // [2] Druhy par: ve vzduchu krok
  pwmServoDriver.setPWM(FRONT_LEFT_XY, 0, angleToPulse(FRONT_LEFT_XY_mid-20));
  pwmServoDriver.setPWM(BACK_RIGHT_XY, 0, angleToPulse(BACK_RIGHT_XY_side));
  delay(150);

  // [1] Prvni par: Udelam krok
  pwmServoDriver.setPWM(FRONT_RIGHT_XY, 0, angleToPulse(FRONT_RIGHT_XY_mid+20));
  pwmServoDriver.setPWM(BACK_LEFT_XY, 0, angleToPulse(BACK_LEFT_XY_side-20));
  delay(150);

  // [2] Druhy par: pokladam na zem
  pwmServoDriver.setPWM(FRONT_LEFT_Z, 0, angleToPulse(FRONT_LEFT_Z_mid));
  pwmServoDriver.setPWM(BACK_RIGHT_Z, 0, angleToPulse(BACK_RIGHT_Z_mid));
  delay(300);

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
 
