#include <SPI.h>
#include <Wire.h>
#include "virtuabotixRTC.h" 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <string.h>
#include <stdio.h>

/*
* Board: LOLIN C3 Mini
* Controller: ESP32-C3 mini
*
* Description:
*   Visual reminder to drink water.
*   When it is working hours and working day then the counter is shown.
*   If there is no change in counter value for defined time, the LED starts blinking.
*/

// TODO: Update RTC over wifi from RTC server.
// TODO: Create my own RTC class, which will be fasade for the RTC library. The rest of the code will then not be dependend on library's implementation, because my class will parse it.


// Definition of Workdays, worktime and such
unsigned int waterDrinkedCounter = 0;
unsigned int startOfDay = 9;
unsigned int endOfDay = 18;
unsigned long notificationIntervalInMinutes = 50UL; // How long to wait until notification starts.

unsigned int minutesSinceLastPress = 0;
const unsigned long kIdleMs = notificationIntervalInMinutes * 60UL * 1000UL; // 50 minutes
unsigned long lastPressMs = 0;

unsigned CURRENT_SECONDS, CURRENT_MINUTES, CURRENT_HOURS;
unsigned CURRENT_DAY_OF_WEEK, CURRENT_DAY_OF_MONTH, CURRENT_MONTH, CURRENT_YEAR;

// #define CURRENT_SECONDS 50
// #define CURRENT_MINUTES 59
// #define CURRENT_HOURS 8
// #define CURRENT_DAY_OF_WEEK 4
// #define CURRENT_DAY_OF_MONTH 18
// #define CURRENT_MONTH 10
// #define CURRENT_YEAR 2025


// ===== RTC config =====
#define DS1302_CLK_PIN 3
#define DS1302_DAT_PIN 2
#define DS1302_RST_PIN 21

virtuabotixRTC myRTC(DS1302_CLK_PIN, DS1302_DAT_PIN, DS1302_RST_PIN); 


// ===== Display config =====
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define CENTER_WIDTH (SCREEN_WIDTH / 2 - 6)
#define DISPLAY_SDA 8
#define DISPLAY_SCL 9

#define COUNTER_TEXT_SIZE 4


// ===== Button =====
#define BUTTON_PIN 10
bool isReleased = true;


// ===== LED =====
#define LED_PIN 1


bool isWorkingDay(int dayOfWeek)
{
  if(dayOfWeek < 6)
    return true;
  return false;
}

bool isWorkingHours(int hours)
{
  if(hours >= startOfDay && hours < endOfDay)
    return true;
  return false;
}

enum DayState {
  DAYSTARTED,
  DAYINPROGRESS,
  DAYENDED
};

DayState dayProgress = DayState::DAYENDED;


/*
* Parse __DATE__ and __TIME__ and fill variables in.
* By: ChatGPT5 (Thinking)
*/
static void initCompileTimeVars()
{
  char mstr[4]; int y,d,h,mi,s;
  sscanf(__DATE__, "%3s %d %d", mstr, &d, &y);
  const char *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
  int mo = (int)((strstr(months, mstr) - months)/3) + 1;         // 1..12
  sscanf(__TIME__, "%d:%d:%d", &h, &mi, &s);

  // Sakamoto: 0=Sun..6=Sat  -> +1 => 1..7
  int Y=y, M=mo, D=d; static const int t[]={0,3,2,5,0,3,5,1,4,6,2,4};
  Y -= (M < 3);
  unsigned dow0 = (unsigned)((Y + Y/4 - Y/100 + Y/400 + t[M-1] + D) % 7);
  unsigned dow  = ((dow0 + 6) % 7) + 1;  // -> Mon=1..Sun=7 (Sun->7, Mon->1, ..., Sat->6)

  CURRENT_YEAR  = y;
  CURRENT_MONTH = mo;
  CURRENT_DAY_OF_MONTH = d;
  CURRENT_HOURS = h;
  CURRENT_MINUTES = mi;
  CURRENT_SECONDS = s;
  CURRENT_DAY_OF_WEEK = dow;  // 1=Sun ... 7=Sat
}

/*
* Draw date and time at the bottom of the display, while preserving display settings.
*/
void drawDateAndTime()
{
  // preserve current settings
  int currentCursorX = display.getCursorX();
  int currentCursorY = display.getCursorY();

  // Draw date and time]
  display.setCursor(0, 25);
  display.setTextSize(1);
  display.print(myRTC.year);
  display.print("-");
  display.print(myRTC.month);
  display.print("-");
  display.print(myRTC.dayofmonth);
  display.print(" ");

  display.print(myRTC.hours);
  display.print(":");
  display.print(myRTC.minutes);
  display.print(":");
  display.print(myRTC.seconds);
  display.display();
  delay(5);

  // Revert settings to the original ones
  display.setCursor(currentCursorX, currentCursorY);
  display.setTextSize(COUNTER_TEXT_SIZE);
}

/*
* Split one large delay() into multiple smaller ones to prevent blocking by native delay().
*/
static inline bool nonBlockingDelay(uint16_t ms)
{
  int delayGranularity = 5;
  for (uint16_t t = 0; t < ms; t += delayGranularity)
  {
    if (digitalRead(BUTTON_PIN) == LOW)
    {
      delay(10);
      return true;
    }

    delay(delayGranularity);
  }
  return false;
}

/*
* Celebrate another drank glass of water
* By: ChatGPT5 (Thinking)
*/
void celebrateFirework(Adafruit_SSD1306 &display) {
  // ---- Params you can tweak ----
  const uint8_t launchStep    = 2;   // how many pixels per frame during launch
  const uint8_t launchDelayMs = 20;  // frame delay during launch
  const uint8_t ringStep      = 2;   // how fast the ring grows
  const uint8_t ringDelayMs   = 30;  // frame delay during ring expansion
  const uint8_t fadeSteps     = 6;   // how many sparkle fade frames
  const uint8_t fadeDelayMs   = 35;  // frame delay during fade
  const uint8_t minApex       = 6;   // minimum height from top (avoid clipping)
  const uint8_t maxApex       = 16;  // max apex (screen is only 32px high)

  // Pick a random launch X and apex height
  int16_t x = random(12, display.width() - 12);
  int16_t groundY = display.height() - 1;
  int16_t apexY   = random(minApex, maxApex);

  // ---- Launch phase (simple dotted trail + nose) ----
  for (int16_t y = groundY; y >= apexY; y -= launchStep) {
    display.clearDisplay();

    // Dotted trail
    for (int8_t t = 0; t < 7; ++t) {
      int16_t ty = y + 2 * t;
      if (ty >= 0 && ty < display.height()) {
        display.drawPixel(x, ty, SSD1306_WHITE);
      }
    }

    // Nose (tiny triangle)
    int16_t yNose = y - 1;
    if (yNose >= 0) {
      display.fillTriangle(x - 2, y + 4, x + 2, y + 4, x, yNose, SSD1306_WHITE);
    }

    display.display();
    delay(launchDelayMs);
  }

  // ---- Explosion phase (ring + spokes + sparkles) ----
  // 8 spoke directions without floats
  const int8_t dir[8][2] = {
    { 1,  0}, { 1,  1}, { 0,  1}, {-1,  1},
    {-1,  0}, {-1, -1}, { 0, -1}, { 1, -1}
  };

  uint8_t maxR = random(8, 14); // max explosion radius (keep within 32px height)

  for (uint8_t r = 1; r <= maxR; r += ringStep) {
    display.clearDisplay();

    // Core
    display.fillCircle(x, apexY, 1, SSD1306_WHITE);

    // Ring
    display.drawCircle(x, apexY, r, SSD1306_WHITE);

    // Spokes
    for (uint8_t k = 0; k < 8; ++k) {
      int16_t x2 = x + dir[k][0] * r;
      int16_t y2 = apexY + dir[k][1] * r;
      display.drawLine(x, apexY, x2, y2, SSD1306_WHITE);
    }

    // Random sparkles around the ring
    uint8_t sparkles = 10;
    for (uint8_t s = 0; s < sparkles; ++s) {
      int16_t sx = x + random(-(int)r, (int)r + 1);
      int16_t sy = apexY + random(-(int)r, (int)r + 1);
      if (sx >= 0 && sx < display.width() && sy >= 0 && sy < display.height()) {
        display.drawPixel(sx, sy, SSD1306_WHITE);
      }
    }

    display.display();
    delay(ringDelayMs);
  }

  // ---- Sparkle fade (shrinking radius, fewer dots) ----
  for (uint8_t f = 0; f < fadeSteps; ++f) {
    display.clearDisplay();
    uint8_t r = (maxR > f * 2) ? (maxR - f * 2) : 0;
    uint8_t sparkles = (r > 2) ? (12 - f * 2) : 4;

    for (uint8_t s = 0; s < sparkles; ++s) {
      int16_t sx = x + random(-(int)r, (int)r + 1);
      int16_t sy = apexY + random(-(int)r, (int)r + 1);
      if (sx >= 0 && sx < display.width() && sy >= 0 && sy < display.height()) {
        display.drawPixel(sx, sy, SSD1306_WHITE);
      }
    }

    display.display();
    delay(fadeDelayMs);
  }

  // Optional: brief “CHEERS!” text flash (fits 128×32)
  // display.clearDisplay();
  // display.setTextSize(1);
  // display.setTextColor(SSD1306_WHITE);
  // int16_t x0 = (display.width() - 6 * 7) / 2; // approx center for 7 chars at 6px each
  // int16_t y0 = (display.height() - 8) / 2;
  // display.setCursor(x0, y0);
  // display.print(F("CHEERS!"));
  // display.display();
  // delay(250);

  // Clear back to normal UI
  display.clearDisplay();
  display.display();
}


void setup()
{
  Serial.begin(115200);
  while (!Serial); // wait for serial port to connect. Needed for native USB
  Wire.begin(DISPLAY_SDA, DISPLAY_SCL);

  lastPressMs = millis();

  // RTC initialization
  initCompileTimeVars();
  myRTC.setDS1302Time(
    CURRENT_SECONDS, 
    CURRENT_MINUTES, 
    CURRENT_HOURS, 
    CURRENT_DAY_OF_WEEK, 
    CURRENT_DAY_OF_MONTH, 
    CURRENT_MONTH, 
    CURRENT_YEAR 
	); 


  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  Serial.println("Initialize display");
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Error: Don't proceed, loop forever.
  }

  display.clearDisplay();
  display.setTextSize(COUNTER_TEXT_SIZE);
  display.setTextColor(WHITE);
  display.stopscroll();
  display.print("Welcome!");
  display.display();

  // button setup
  pinMode(BUTTON_PIN, INPUT_PULLUP); // config GPIO21 as input pin and enable the internal pull-up resistor

  // LED setup
  pinMode(LED_PIN, OUTPUT);
}


void loop()
{
  display.clearDisplay();
  display.setCursor(CENTER_WIDTH, 0);

  myRTC.updateTime();

  bool isWorkingTime = isWorkingDay(myRTC.dayofweek) && isWorkingHours(myRTC.hours);

  // Day just started
  if(isWorkingTime && dayProgress == DayState::DAYENDED)
  {
    Serial.println("Day started.");
    dayProgress = DayState::DAYINPROGRESS;

    // Reset notification timer and counters
    minutesSinceLastPress = 0;
    waterDrinkedCounter = 0;
    lastPressMs = millis();

    // Play welcoming animation
    display.clearDisplay();
    for(int16_t i = 0; i < display.height() / 2; i += 2)
    {
      display.drawRect(i, i, display.width()-2*i, display.height()-2*i, SSD1306_WHITE);
      display.display(); // Update screen with each newly-drawn rectangle
      delay(150);
    }
  }

  // Day just ended
  if(!isWorkingTime)
  {
    // When working day ends, play animation and set sleep mode until next working day.
    if(dayProgress == DayState::DAYINPROGRESS)
    { 
      Serial.println("Day ended.");
      dayProgress = DayState::DAYENDED;

      // Play ending animation
      display.clearDisplay();
      for(int16_t i = 0; i < display.height() / 2; i += 2)
      {
        display.drawRect(i, i, display.width()-2*i, display.height()-2*i, SSD1306_WHITE);
        display.display(); // Update screen with each newly-drawn rectangle
        delay(150);
      }
      display.clearDisplay();
    }

    Serial.printf("Outside of the working day (%d) or working hours (%d).\n", myRTC.dayofweek, myRTC.hours);
    display.setCursor(0, 0);
    //display.println("Off!");
    drawDateAndTime();
    display.display();
    delay(10000);
  }

  // Day in progress  
  if(isWorkingTime && dayProgress == DayState::DAYINPROGRESS)
  {
    int buttonState = digitalRead(BUTTON_PIN);
    if(!buttonState && isReleased == true)
    {
      isReleased = false;
      waterDrinkedCounter++;
      if (waterDrinkedCounter > 7)
      {
        waterDrinkedCounter = 0;
      }

      // Reset notification timer
      minutesSinceLastPress = 0;
      lastPressMs = millis();

      // A little graphical celebration of the new glass of water drinked.
      celebrateFirework(display);
      celebrateFirework(display);
      celebrateFirework(display);
    }
    
    // Prevent counter incrementing when the button is held down.
    if(buttonState && isReleased == false)
    {
      isReleased = true;
    }

    // Start notification until the counter is incremented by button or workday ends
    //if(minutesSinceLastPress > notificationIntervalInMinutes)
    if(millis() - lastPressMs >= kIdleMs)
    {
      Serial.printf("Get new water! %d\n", lastPressMs);
      while(buttonState && isWorkingTime)
      {
        digitalWrite(LED_PIN, HIGH);
        if(nonBlockingDelay(500))
        {
          buttonState = !buttonState;
          digitalWrite(LED_PIN, LOW);
          break;
        }

        digitalWrite(LED_PIN, LOW);
        if(nonBlockingDelay(500))
        {
          buttonState = !buttonState;
          break;
        }
      }
    }

    // Show number of drank glasses of water so far.
    display.println(waterDrinkedCounter);
    //drawDateAndTime();
    display.display();
  }
  
  delay(50);
}
