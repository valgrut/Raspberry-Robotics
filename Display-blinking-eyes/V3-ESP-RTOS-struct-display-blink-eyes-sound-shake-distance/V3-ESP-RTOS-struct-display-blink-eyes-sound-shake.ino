/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 This example is for a 128x32 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).
 **************************************************************************/

/*
- Verze 3
	- Eye Structures Fixed, used.
	- FreeRTOS !!!

- Description: 
	- Oci na displei, ktere mrkaji
	- pri mrknuti delaji zvuk.
	- Kdyz se zatrese, oci se zmeni na XX.
	- Distance sensor.

- Komponenty:
	- Distance sensor
	- Buzzer
	- Display
	- Vibration sensor

- Links:
  - Display tutorial: https://randomnerdtutorials.com/esp32-ssd1306-oled-display-arduino-ide/

*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <cstdlib>
#include <ctime>

// ===== Display config =====
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ===== Pins definitions =====
#define VIBRATION_SENSOR_PIN 33
#define BUZZER_PIN 32

// #define TRIG_PIN 25
// #define ECHO_PIN 26

#define BUTTON_PIN 26

// ===== Sensors variables =====
int vibration_sensor_reading;
int currentButtonState;
bool shake_detected = false;

// ===== FreeRTOS handles =====
TaskHandle_t gDisplayTask = nullptr;
TaskHandle_t gShakeTask   = nullptr;

TimerHandle_t idleTimer   = nullptr;  // one-shot
TimerHandle_t sleepTimer  = nullptr;  // one-shot

// Idle timers (ms)
static const uint32_t IDLE_AFTER_MS  = 10000; // 10 s -> run idle animation
static const uint32_t SLEEP_AFTER_MS = 30000; // 30 s -> sleep eyes

volatile TickType_t g_lastShakeTick = 0; // updated whenever a shake is handled
volatile TickType_t g_animationScheduledInTick = 0;    // 0 => not scheduled


bool state_bored = false;
bool state_sleeping = false;


/*
 * Eye structures
*/
struct EyeShape {
	virtual ~EyeShape() = default;
  virtual void draw(u_int pos_x, u_int pos_y) = 0;
};


struct EyeShapeSquare : EyeShape
{
  private:
    u_int originalWidth;
    u_int originalHeight;
    u_int width;
    u_int height;
    u_int corner_radius;
	
    void _recalculateCornerRadius()
    {
      // Check that radius is not larger than half of side lenght. (because the radius is from both sides in both directions)
      if(this->width >= this->height)
      {
        this->corner_radius = (this->height - 1) / 2;
      }
      else
      {
        this->corner_radius = (this->width - 1) / 2;
      }
    }

  public:
	  EyeShapeSquare(u_int w, u_int h, u_int r) : width(w), height(h), corner_radius(r)
    {
      this->originalWidth = w;
      this->originalHeight = h;

      this->_recalculateCornerRadius();
    }

    void draw(u_int pos_x, u_int pos_y) override
	  {
      // Y position needs adjustment, because square is drawn from its left top corner, and circle is from its middle.
      display.drawRoundRect(
        pos_x - this->width/2, pos_y - this->height/2, 
        this->width, this->height, 
        this->corner_radius, 
        SSD1306_WHITE
      );
      display.fillRoundRect(
        pos_x - this->width/2, pos_y - this->height/2, 
        this->width, this->height, 
        this->corner_radius, 
        SSD1306_INVERSE
      );
    }

    void setEyeHeight(int newHeight)
    {
      //TODO: opet stejny problem jak u cehosi dalsiho - nemuzu nastavovat kazde oko zvlast, protoze sdileji 1 instanci tohoto.
      this->height = newHeight;
      this->_recalculateCornerRadius();
    }

    unsigned int getEyeHeight()
    {
      return this->height;
    }

    void resetOriginalEyeHeight()
    {
      this->height = this->originalHeight;
      this->_recalculateCornerRadius();
    }
};


struct EyeShapeCircle : EyeShape
{
  private:
    u_int radius;

  public:
    EyeShapeCircle(u_int radius) : radius(radius) {}
	
	  void draw(u_int pos_x, u_int pos_y) override
	  {
      display.drawCircle(pos_x, pos_y, this->radius, SSD1306_WHITE);
      display.fillCircle(pos_x, pos_y, this->radius, SSD1306_INVERSE); 
    }
};


struct EyeShapeSleep : EyeShape
{
  private:
    u_int eye_width;

  public:
    EyeShapeSleep(u_int eye_width) : eye_width(eye_width + 2) {}
	
	  void draw(u_int pos_x, u_int pos_y) override
	  {
      // display.drawLine(pos_x - 12, pos_y, pos_x + 12, pos_y, SSD1306_WHITE);
      display.drawLine(pos_x - (this->eye_width / 2 - 4), pos_y, pos_x + (this->eye_width / 2 - 4), pos_y, SSD1306_WHITE);
    }

    void draw(u_int pos_x, u_int pos_y, u_int eye_width)
	  {
      display.drawLine(pos_x - (eye_width / 2 - 4), pos_y, pos_x + (eye_width / 2 - 4), pos_y, SSD1306_WHITE);
    }
};


struct EyeShapeBitmapX : EyeShape
{
  private:
    u_int bitmap_size;  // s*s pixels
    static const uint8_t EYE_X_BITMAP[] PROGMEM;
	
  public:
	  EyeShapeBitmapX()
    {
      this->bitmap_size = 26;
    }

    void draw(u_int pos_x, u_int pos_y) override 
    {
      display.drawBitmap(
        pos_x - (this->bitmap_size / 2),
        pos_y / 2,
        this->EYE_X_BITMAP, 
        this->bitmap_size, this->bitmap_size,
        1
      );
    }
};

// https://javl.github.io/image2cpp/
const uint8_t EyeShapeBitmapX::EYE_X_BITMAP[] PROGMEM = {
  // 'X_symbol, 26x26px
  0x70, 0x00, 0x03, 0x80, 0xf8, 0x00, 0x07, 0xc0, 0xfc, 0x00, 0x0f, 0xc0, 0xfe, 0x00, 0x1f, 0xc0, 
  0x7f, 0x00, 0x3f, 0x80, 0x3f, 0x80, 0x7f, 0x00, 0x1f, 0xc0, 0xfe, 0x00, 0x0f, 0xe1, 0xfc, 0x00, 
  0x07, 0xf3, 0xf8, 0x00, 0x03, 0xff, 0xf0, 0x00, 0x01, 0xff, 0xe0, 0x00, 0x00, 0xff, 0xc0, 0x00, 
  0x00, 0x7f, 0x80, 0x00, 0x00, 0x7f, 0x80, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x01, 0xff, 0xe0, 0x00, 
  0x03, 0xff, 0xf0, 0x00, 0x07, 0xf3, 0xf8, 0x00, 0x0f, 0xe1, 0xfc, 0x00, 0x1f, 0xc0, 0xfe, 0x00, 
  0x3f, 0x80, 0x7f, 0x00, 0x7f, 0x00, 0x3f, 0x80, 0xfe, 0x00, 0x1f, 0xc0, 0xfc, 0x00, 0x0f, 0xc0, 
  0xf8, 0x00, 0x07, 0xc0, 0x70, 0x00, 0x03, 0x80
};

// Main EYE structure:
struct Eye
{
  private:
    u_int initial_pos_x;
    u_int initial_pos_y;
    u_int pos_x;
    u_int pos_y;
    struct EyeShape *eyeShape; // ◯, X, ─, ▢, ▭
  
  public:
    Eye(u_int init_pos_x, u_int init_pos_y, struct EyeShape *eyeShape) : initial_pos_x(init_pos_x), initial_pos_y(init_pos_y), eyeShape(eyeShape)
    {
      this->pos_x = initial_pos_x;
      this->pos_y = initial_pos_y;
    }

    void moveEye(int direction_x, int direction_y)
    {
      // TODO: Add boundaries check for display (display.height(), display.width())
      this->pos_x += direction_x;
      this->pos_y += direction_y;
    }

    void centerEye()
    {
      this->pos_x = this->initial_pos_x;
      this->pos_y = this->initial_pos_y;
    }

    void setEyeShape(struct EyeShape *eyeShape)
    {
      this->eyeShape = eyeShape;
    }

    void draw()
    {
	    this->eyeShape->draw(this->pos_x, this->pos_y);
    }

    void draw(u_int pos_x, u_int pos_y)
    {
	    this->eyeShape->draw(pos_x, pos_y);
    }

    void draw(u_int pos_x, u_int pos_y, struct EyeShape *eyeShape)
    {
	    eyeShape->draw(pos_x, pos_y);
    }

    void draw(struct EyeShape *eyeShape)
    {
      eyeShape->draw(this->pos_x, this->pos_y);
    }

    // If 2 eyes have different width, blink should reflect it and have also different width.
    // TLDR: Blink width should be eye's width.
    // void blink()
    // {
    //   self.draw(&sleepEye);
    // }
};


// struct EyesPair
// {
//   private:
//     u_int pos_x; // default center position x
//     u_int pos_y; // default center position y

//     struct EyeShape *leftEyeShape;
//     struct EyeShape *rightEyeShape;
  
//   public:
//     Eye(u_int pos_x, u_int pos_y) : pos_x(x), pos_y(y) {}

//     void draw()
//     {
// 	    this->leftEyeShape->draw(pos_x, pos_y);
//       this->rightEyeShape->draw(pos_x, pos_y);
//     }

//     void moveEyes(u_int direction_x, u_int direction_y)
//     {}
// };


// ============================================================
// Layout & instances
// ============================================================
// Calculate position for eyes and its attributes
// TODO: Add those into EyesPair class.
u_int eyesSpacing = 3;
u_int eyeWidth = 25;
// u_int eyeRadius = 13;
// u_int eyeWidth = circleEyeRadius * 2;
int eye_left_pos_x = display.width()/2 - eyeWidth/2 - eyesSpacing;
int eye_right_pos_x = display.width()/2 + eyeWidth/2 + eyesSpacing;
int eye_left_pos_y = display.height()/2;
int eye_right_pos_y = display.height()/2;

// Define Eye structures
struct EyeShapeCircle circleEye(eyeWidth/2);  // ●
struct EyeShapeSquare squareEye(eyeWidth, eyeWidth, 3);  // ◼
struct EyeShapeSquare squareEyeRounded(eyeWidth, eyeWidth, 7);  // ▢
struct EyeShapeSquare squareEyeCircled(eyeWidth, eyeWidth, eyeWidth/2);  // ●
struct EyeShapeSquare squareEyeNarrow(eyeWidth, eyeWidth/2, eyeWidth/4);  // ▬
struct EyeShapeBitmapX xEye;  // 🞭
struct EyeShapeSleep sleepEye(eyeWidth);  // or blink eye  // -

// Define Eyes and Set default Eyes shape:
struct Eye eyeLeft(eye_left_pos_x, eye_left_pos_y, &squareEyeRounded);
struct Eye eyeRight(eye_right_pos_x, eye_right_pos_y, &squareEyeRounded);



// ============================================================
// Helpers that are notification-aware (interruptible waits)
// ============================================================

// Returns true if a shake notification arrived during the wait.
bool interruptibleWait(uint32_t total_ms)
{
  /*
  Works basically like this: 
    Instead of one large fixed time period of waiting, we slice it to small parts and 
    after each such slice we check, if there was not some interruption.
  */

  // Slice the wait so the display task stays responsive to shakes
  const uint32_t slice_ms = 20;
  const TickType_t start = xTaskGetTickCount();
  const TickType_t total = pdMS_TO_TICKS(total_ms);

  for (;;)
  {
    // If a shake arrived, tell caller to preempt now
    if (ulTaskNotifyTake(pdTRUE, 0) > 0)
      return true;

    // Finished waiting?
    if ((xTaskGetTickCount() - start) >= total)
      return false;

    vTaskDelay(pdMS_TO_TICKS(slice_ms));
  }
}


// static void idleTimerCallback(xTimerHandle xTimer)
// {
//   (void) xTimer; // no action with parameter
//   Serial.println("Timer callback fired.");
// }


// Show X-eyes for at least 3 seconds
void showScaredEyesBlock3s()
{
  // reset idle timers
  g_lastShakeTick = xTaskGetTickCount();

  // Draw XX eyes
  display.clearDisplay();
  eyeLeft.draw(&xEye);
  eyeRight.draw(&xEye);
  display.display();

  // Keep XX eyes for 2 seconds
  vTaskDelay(pdMS_TO_TICKS(2000));
  //interruptibleWait(2000);
}


// One normal “look around & maybe blink” cycle, but interruptible.
bool look_around_and_blink()
{ 
    display.clearDisplay();

    // Generate random position change for the eyes
    int rand_x_change = (rand() % 10) + 1 - 5;
    int rand_y_change = (rand() % 10) + 1 - 5;
    int rand_time = (rand() % 400) + 1 + 400;
    int rand_blink = (rand() % 4); // lets start with percentage probability
    //int blink_probability = 35;  // 35% probability of blinking

    // Randomly change slightly position of the eyes and draw them
    eyeLeft.moveEye(rand_x_change, rand_y_change);
    eyeRight.moveEye(rand_x_change, rand_y_change);

    eyeLeft.draw();
    eyeRight.draw();
    display.display();
    
    // Hold the gaze, but be ready to preempt.
    if (interruptibleWait(rand_time))
      return true;
  
    if(rand_blink == 0 || rand_blink == 1)
    {
      // Known issues: TODO
      //    - blink eye je definovane s pevnou sirkou, ktera neodpovida oku, ktere mrka.
      // blink() {  # nebo jeste lip eyeLeft.blink()
      display.clearDisplay();
      eyeLeft.draw(&sleepEye);
      eyeRight.draw(&sleepEye);
      display.display();

      // Random range of sounds so it is not annoying.
      int blink_tone = (rand() % 40) + 470;
      tone(BUZZER_PIN, blink_tone, 5);  // Make sound when blinked
      //noTone(BUZZER_PIN);

      if (interruptibleWait(300))
        return true;
    }
    
    // if(rand_blink == 2)
    // {
    //   if (anotherAnimationBoredInterruptible())
    //     return true;
    // }

    // Move eyes back to the center
    eyeLeft.centerEye();
    eyeRight.centerEye();
    return false;
}


bool anotherAnimationBoredInterruptible()
{
  display.clearDisplay();
  eyeLeft.draw(&squareEyeNarrow);
  eyeRight.draw(&squareEyeNarrow);
  display.display();
  if (interruptibleWait(2000))
    return true;

  // return eyes back to normal
  display.clearDisplay();
  eyeLeft.draw();
  eyeRight.draw();
  display.display();

  return false;
}

bool basicAnimationBoredInterruptible()
{
  int steps = 10;
  unsigned int originalEyeHeight = squareEyeNarrow.getEyeHeight();
  for(int i = steps; i > 1; i--)
  {
    squareEyeNarrow.setEyeHeight(originalEyeHeight / steps * i);

    display.clearDisplay();
    eyeLeft.draw(&squareEyeNarrow);
    eyeRight.draw(&squareEyeNarrow);
    eyeLeft.moveEye(0, +1);
    eyeRight.moveEye(0, +1);
    display.display();

    if (interruptibleWait(300))
    {
      squareEyeNarrow.resetOriginalEyeHeight();
      eyeLeft.centerEye();
      eyeRight.centerEye();
      return true;
    }
  }

  eyeLeft.centerEye();
  eyeRight.centerEye();
  squareEyeNarrow.resetOriginalEyeHeight();

  // Step 5: blink 3×, ~1 s cadence
  for(int i = 0; i < 3; ++i)
  {
    // close both (short close)
    display.clearDisplay();
    eyeLeft.draw(&sleepEye);
    eyeRight.draw(&sleepEye);
    tone(BUZZER_PIN, (rand() % 40) + 470, 5);
    display.display();
    if (interruptibleWait(150))
      return true;

    // open both
    display.clearDisplay();
    eyeLeft.draw();
    eyeRight.draw();
    display.display();
    if (interruptibleWait(500))
     return true; // ~1 s between closes
  }

  return false;
}


void sleepUntilShake()
{
  // Draw once
  display.clearDisplay();
  eyeLeft.draw(&sleepEye);
  eyeRight.draw(&sleepEye);
  display.display();

  // Sit here until a shake notification appears
  for(;;)
  {
    if(ulTaskNotifyTake(pdTRUE, 0) > 0)
    {
      // reset idle timers
      g_lastShakeTick = xTaskGetTickCount();
      return;
    }

    vTaskDelay(pdMS_TO_TICKS(20));
  }
}




/************************************************************************
                                   TASKS
*************************************************************************/


// Task: monitors the vibration sensor and notifies the display task.
// TODO: Prejmenovat. Tohle bude takova hlavni smycka. (zisk vstupu od usera, ... poslani notifikace v pripade vstupuu a tak)
void ShakeAndButtonDetectionTask(void *param)
{
  // (void)param;

  for (;;)
  {
    // Process vibration sensor
    vibration_sensor_reading = analogRead(VIBRATION_SENSOR_PIN);
    currentButtonState = digitalRead(BUTTON_PIN);
    //Serial.printf("Vibration value: %d\n", vibration_sensor_reading);
    
    if(vibration_sensor_reading < 4095)
    {
      xTaskNotifyGive(gDisplayTask);
      vTaskDelay(pdMS_TO_TICKS(50)); // debounce
    }
    else
    {
      vTaskDelay(pdMS_TO_TICKS(10));
    }

    // if(currentButtonState == HIGH)
    // {
    //   // Button pressed
    //   Serial.printf("Button pressed!\n");
    //   xTaskNotifyGive(gDisplayTask);
    //   vTaskDelay(pdMS_TO_TICKS(50)); // basic debounce
    // }
    // else
    // {
    //   vTaskDelay(pdMS_TO_TICKS(10));
    // }
  }
}


// Task: owns the OLED and implements behavior; preempts on shake.
void DisplayTask(void *param)
{
  for (;;)
  {
    if(ulTaskNotifyTake(pdTRUE, 0) > 0)
    {
      showScaredEyesBlock3s();
      continue;
    }

    // Compute time since last handled shake
    const TickType_t now = xTaskGetTickCount();
    const TickType_t since_last_shake = (now - g_lastShakeTick) * portTICK_PERIOD_MS; // TODO: TUhle prom. udelat globalni
    const TickType_t sleep_at = g_lastShakeTick + SLEEP_AFTER_MS;

    // 2) Sleep if very idle
    if(since_last_shake >= SLEEP_AFTER_MS)
    {
      sleepUntilShake();        // returns after shake -> ❌ already shown+
      showScaredEyesBlock3s();
      continue;
    }

    // 3) Idle animation if moderately idle
    if(since_last_shake >= IDLE_AFTER_MS)
    {
      //g_animationScheduled = 0;  // consume it; don't repeat until next shake
      // TODO: Tohle je taKY HNUSNY, jakoze checkuju v ifu, esi to vrati tru nebo false. Ja chci poslat notify zase z te funkce.
      bool isAnimationInterrupted = basicAnimationBoredInterruptible();
      if(isAnimationInterrupted)
      {
        showScaredEyesBlock3s();
      }
      // TODO: Cele volani te funkce by mohlo byt jako if random() == 1, pust animaci, at to je nahodne. Jen 1x se pusti urcite.
      // tzn if random == 1 and time_since_last_animation > 100 and time_since_shake < time_to_sleep, then pust animaci.
      continue;
    }

    // Regular animation loop
    bool isLookingAnimationInterrupted = look_around_and_blink();
    if(isLookingAnimationInterrupted)
    {
      showScaredEyesBlock3s();
      continue;
    }

    // Small idle tick to yield (not strictly needed).
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}



void setup()
{
  Serial.begin(9600);

  // Seed RNG a bit
  randomSeed(esp_random());
  srand(esp_random());

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Error: Don't proceed, loop forever.
  }

  // Clear the buffer
  display.clearDisplay();

  // BUZZER_PIN initialisation
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(VIBRATION_SENSOR_PIN, INPUT);

  // Button pin initialisation
  // pinMode(BUTTON_PIN, INPUT_PULLUP);

  // update tick tracker for the sleep and idle since last shake
  g_lastShakeTick = xTaskGetTickCount();

  // Create tasks
  xTaskCreatePinnedToCore(DisplayTask, "DisplayTask", 4096, nullptr, 2, &gDisplayTask, 1);
  xTaskCreatePinnedToCore(ShakeAndButtonDetectionTask, "ShakeTask", 2048, nullptr, 3, &gShakeTask, 0);

  // Create Timers
  xTimerCreate(
          (const char * const) "Random Bored Animation Timer", // used only for debugging purposes
          10000,                // timer period
          pdTRUE,  // Autoreload on or off - should the timer start again after it expired?
          &idleTimer,                 // optional identifier
          idleTimerCallback       // static callback function
  );
  if(idleTimer == NULL)
  {
      assert(pdFAIL);
      return;
  }


  vTaskStartScheduler();  
}


void loop() 
{
  vTaskDelay(pdMS_TO_TICKS(1000));
}


