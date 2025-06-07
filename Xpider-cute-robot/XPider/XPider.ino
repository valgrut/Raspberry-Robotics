#include <WiFi.h>
#include <WebServer.h>
#include <SparkFun_TB6612.h>
#include <ESP32Servo.h>

const char* ssid = "***REMOVED***";
const char* password = "***REMOVED***";

// TODO: Nekolik prednastavenych SSID + passwd, a v cyklu je dole pri pripojeni pak vyzkouset vsechny.
// Takhle nebudu muset robota preprogramovavat, kdyz budu v PV a budu ho chtit pripojit doma na Wifi.
// Proste prorotuje ty wifi udaje a pripoji se k te, ktera nevyhodi error.

WebServer server(80);

// these constants are used to allow you to make your motor configuration 
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;

// Assign motor driver pins
#define AIN1 1
#define AIN2 2
#define PWMA 3

#define BIN1 4
#define BIN2 5
#define PWMB 7

#define STBY 0

#define EYE_SERVO_PIN 8
#define EYE_LED_PIN 6

// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.
Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY);

// Eye servo definition, initial position.
Servo eyeServo;
int currentEyePosition = 600;  // used by the program to variously control eye.

int LED_state = LOW;

// Web server UI definition
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Robot Control</title>
  <style>
    body {
      font-family: sans-serif;
    }

    .main-layout {
      display: flex;
      flex-direction: row;
      align-items: center;
      justify-content: center;
      gap: 20px;
      margin-top: 40px;
    }

    .controls {
      display: flex;
      flex-direction: column;
      gap: 10px;
      align-items: center;
    }

    .row {
      display: flex;
      gap: 10px;
      justify-content: center;
    }

    button {
      width: 100px;
      height: 60px;
      font-size: 16px;
    }

    #ledToggle {
      width: 120px;
      font-size: 14px;
    }

    .canvas-container {
      display: flex;
      align-items: center;
      justify-content: center;
    }

    canvas {
      border: 1px solid black;
      background-color: #f4f4f4;
    }

    .eye-controls {
      display: flex;
      flex-direction: column;
      gap: 10px;
      align-items: center;
    }

    .eye-controls button {
      width: 100px;
      height: 60px;
    }
  </style>
</head>
<body>
  <div class="main-layout">
    <!-- Left control panel -->
    <div class="controls">
      <div class="row">
        <button id="forward">Forward</button>
      </div>
      <div class="row">
        <button id="left">Left</button>
        <button id="ledToggle">Toggle LED</button>
        <button id="right">Right</button>
      </div>
      <div class="row">
        <button id="backward">Backward</button>
      </div>
    </div>

    <!-- Center canvas -->
    <div class="canvas-container">
      <canvas id="robotCanvas" width="300" height="300"></canvas>
    </div>

    <!-- Right eye movement panel -->
    <div class="eye-controls">
      <button id="eyeUp">Eye Up</button>
      <button id="eyeDown">Eye Down</button>
    </div>

<script>
  const activeKeys = new Set();

  function sendCommand(cmd) {
    fetch(`/cmd?btn=${cmd}`);
  }

  const keyToCommand = {
    ArrowUp: "forward",
    ArrowDown: "backward",
    ArrowLeft: "left",
    ArrowRight: "right",
    ArrowPageUp: "eyeUp",
    ArrowPageDown: "eyeDown"
  };

  document.addEventListener("keydown", (e) => {
    const cmd = keyToCommand[e.key];
    if (cmd && !activeKeys.has(cmd)) {
      activeKeys.add(cmd);
      sendCommand(cmd + "_pressed");
    }
  });

  document.addEventListener("keyup", (e) => {
    const cmd = keyToCommand[e.key];
    if (cmd && activeKeys.has(cmd)) {
      activeKeys.delete(cmd);
      sendCommand(cmd + "_released");
    }
  });

  const buttons = ["forward", "backward", "left", "right", "eyeUp", "eyeDown"];
  buttons.forEach(id => {
    const btn = document.getElementById(id);
    btn.addEventListener("mousedown", () => sendCommand(id + "_pressed"));
    btn.addEventListener("mouseup", () => sendCommand(id + "_released"));
    btn.addEventListener("touchstart", e => { e.preventDefault(); sendCommand(id + "_pressed"); });
    btn.addEventListener("touchend", e => { e.preventDefault(); sendCommand(id + "_released"); });
  });

  const canvas = document.getElementById('robotCanvas');
  const ctx = canvas.getContext('2d');
  const center = { x: canvas.width / 2, y: canvas.height / 2 };
  let heading = 0; // in radians, 0 = facing right
  let target = null;

  function drawRobot() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    // Robot body
    ctx.beginPath();
    ctx.arc(center.x, center.y, 20, 0, Math.PI * 2);
    ctx.fillStyle = 'gray';
    ctx.fill();
    ctx.closePath();

    // Heading arrow
    const dx = Math.cos(heading) * 30;
    const dy = Math.sin(heading) * 30;
    ctx.beginPath();
    ctx.moveTo(center.x, center.y);
    ctx.lineTo(center.x + dx, center.y + dy);
    ctx.strokeStyle = 'red';
    ctx.lineWidth = 3;
    ctx.stroke();
    ctx.closePath();

    // Target point and line
    if (target) {
      // Dotted red line to target
      ctx.setLineDash([5, 5]);
      ctx.beginPath();
      ctx.moveTo(center.x, center.y);
      ctx.lineTo(target.x, target.y);
      ctx.strokeStyle = 'red';
      ctx.lineWidth = 1;
      ctx.stroke();
      ctx.closePath();
      ctx.setLineDash([]);

      // Small + at target
      ctx.beginPath();
      ctx.moveTo(target.x - 5, target.y);
      ctx.lineTo(target.x + 5, target.y);
      ctx.moveTo(target.x, target.y - 5);
      ctx.lineTo(target.x, target.y + 5);
      ctx.strokeStyle = 'black';
      ctx.lineWidth = 1.5;
      ctx.stroke();
      ctx.closePath();
    }
  }

  function getMinimalTurn(currentAngle, targetAngle) {
    let delta = (targetAngle - currentAngle + 360) % 360;
    if (delta === 0) {
      return { angle: 0, direction: 'none' };
    } else if (delta <= 180) {
      return { angle: delta, direction: 'right' };  // clockwise
    } else {
      return { angle: 360 - delta, direction: 'left' };  // counter-clockwise
    }
  }

  canvas.addEventListener('click', (e) => {
    const rect = canvas.getBoundingClientRect();
    const clickX = e.clientX - rect.left;
    const clickY = e.clientY - rect.top;

    const dx = clickX - center.x;
    const dy = clickY - center.y;

    const angleToTarget = Math.atan2(dy, dx);
    let relativeAngle = angleToTarget - heading;

    // Normalize relative angle to -PI..PI
    relativeAngle = Math.atan2(Math.sin(relativeAngle), Math.cos(relativeAngle));

    // Compute minimal turn angle (always positive)
    const relativeAngleDeg = Math.abs(relativeAngle * 180 / Math.PI);

    // Determine direction
    const direction = relativeAngle >= 0 ? 'right' : 'left';

    const distance = Math.sqrt(dx * dx + dy * dy);

    console.log(`Turn ${direction} by ${relativeAngleDeg.toFixed(2)}°, Distance: ${distance.toFixed(2)} px`);

    // Update heading
    heading = angleToTarget;

    // Store target and redraw
    target = { x: clickX, y: clickY };
    drawRobot();

    // Send command to ESP32
    // + => turn right (clockwise)
    // − => turn left (counterclockwise)
    fetch(`/navigate?angle=${relativeAngleDeg.toFixed(2)}&direction=${direction}&distance=${distance.toFixed(2)}`);
  });

  drawRobot();

  // LED toggle logic
  let ledOn = false;
  const ledBtn = document.getElementById('ledToggle');
  ledBtn.addEventListener('click', () => {
    ledOn = !ledOn;
    const state = ledOn ? "on" : "off";
    fetch(`/led?state=${state}`);
    ledBtn.textContent = ledOn ? "Turn LED Off" : "Turn LED On";
  });
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send_P(200, "text/html", index_html);
}

int movement_duratin = 100;

void handleCommand() 
{
  if (server.hasArg("btn")) 
  {
    String btn = server.arg("btn");
    Serial.println("Button event: " + btn);

    if(btn == "left_pressed")
    {
      motor2.drive(-255,movement_duratin);
      Serial.println("Left is pressed");
    }
    else if(btn == "left_released")
    {
      motor2.brake();
    }

    if(btn == "right_pressed")
    {
      motor2.drive(255,movement_duratin);
      Serial.println("R is pressed");
    }
    else if(btn == "right_released")
    {
      motor2.brake();
    }

    if(btn == "backward_pressed")
    {
      motor1.drive(255,movement_duratin);
    }
    else if(btn == "backward_released")
    {
      motor1.brake();
    }
    
    if(btn == "forward_pressed")
    {
      motor1.drive(-255,movement_duratin);
    }
    else if(btn == "forward_released")
    {
      motor1.brake();
    }

    if(btn == "eyeUp_released")
    {
      currentEyePosition += 10;
      int val = map(currentEyePosition, 0, 1023, 0, 180);
      eyeServo.write(val);
      Serial.println(currentEyePosition);
      Serial.println(val);
    }
    if(btn == "eyeDown_released")
    {
      currentEyePosition -= 10;
      int val = map(currentEyePosition, 0, 1023, 0, 180);
      eyeServo.write(val);
      Serial.println(currentEyePosition);
      Serial.println(val);
    }

    server.on("/led", []() 
    {
      if (server.hasArg("state")) 
      {
        String state = server.arg("state");
        if (state == "on") 
        {
          LED_state = HIGH;
        } 
        else if (state == "off") 
        {
          LED_state = LOW;
        }
        digitalWrite(EYE_LED_PIN, LED_state);
        server.send(200, "text/plain", LED_state == HIGH ? "ON" : "OFF");
        Serial.println("LED state set to: " + state);
      }
      else 
      {
        server.send(400, "text/plain", "Missing 'state' parameter");
      }
    });

    server.on("/navigate", []()
    {
      if (server.hasArg("angle") && server.hasArg("distance") && server.hasArg("direction")) 
      {
        String angleStr = server.arg("angle");
        String distanceStr = server.arg("distance");
        String directionStr = server.arg("direction");

        float angle = angleStr.toFloat();
        float distance = distanceStr.toFloat();

        Serial.printf("Navigation command: angle=%.2f°, distance=%.2f px, direction=%s\n", angle, distance, directionStr);

        // Turn robot to the target angle position (facing direction)
        if(directionStr == "left")
        {
          // The 1600 value is experimentally found for given speed of motor and time to get to 'angle'
          motor2.drive(-255, map(angle, 0, 180, 0, 1800));

          // required, otherwise it would turn again and again.
          motor2.brake();
        }

        if(directionStr == "right")
        {
          motor2.drive(255, map(angle, 0, 180, 0, 1800));

          // required, otherwise it would turn again and again.
          motor2.brake();
        }

        // Walk to the given distance. 210 is max distance to get in the 2D field in UI.
        motor1.drive(-255, map(distance, 0, 210, 0, 5000));
        motor1.brake();
        // walkForward(distance);

        directionStr = "";
        server.send(200, "text/plain", "OK");
      } 
      else 
      {
        server.send(400, "text/plain", "Missing parameters");
      }
    });

    server.send(200, "text/plain", "OK");
  } 
  else 
  {
    server.send(400, "text/plain", "Missing param");
  }
}

void randomEyeMovement()
{
  // use currentEyePosition variable.
  // int val = map(currentEyePosition, 0, 1023, 0, 180);
  // eyeServo.write(val);
}

void blinkLastByteOfIP(int num1, int num2, int num3)
{
  // first number
  for(int first = 0; first < num1; first++)
  {
    digitalWrite(EYE_LED_PIN, HIGH);
    delay(200);
    digitalWrite(EYE_LED_PIN, LOW);
    delay(200);
  }
  delay(300);

  // second number
  for(int second = 0; second < num2; second++)
  {
    digitalWrite(EYE_LED_PIN, HIGH);
    delay(200);
    digitalWrite(EYE_LED_PIN, LOW);
    delay(200);
  }
  delay(300);

  // third number
  for(int third = 0; third < num3; third++)
  {
    digitalWrite(EYE_LED_PIN, HIGH);
    delay(200);
    digitalWrite(EYE_LED_PIN, LOW);
    delay(200);
  }
  delay(300);

  digitalWrite(EYE_LED_PIN, HIGH);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/cmd", handleCommand);
  server.begin();

  //eyeServo.setPeriodHertz(50);
  eyeServo.attach(EYE_SERVO_PIN);

  // Initialize the LED pin
  pinMode(EYE_LED_PIN, OUTPUT);
  digitalWrite(EYE_LED_PIN, HIGH);
  LED_state = HIGH;

  // TODO: Write last 8 bits from IP with LED.
  IPAddress ip = WiFi.localIP();
  int lastByte = ip[3];  // Get the last byte of the IP address

  // Split into individual digits
  int ip1 = lastByte / 100;             // Hundreds
  int ip2 = (lastByte / 10) % 10;       // Tens
  int ip3 = lastByte % 10;              // Units

  // blink last byte of ip address through LED
  blinkLastByteOfIP(ip1, ip2, ip3);
  // because ip is written only into Serial print, when powered from battery, I have no output, so
  // I dont know, which ip address is used to access UI.
}

void loop() {
  server.handleClient();

}
