/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 This example is for a 128x32 pixel display using I2C to communicate
 3 pins are required to interface (two I2C and one reset).
 **************************************************************************/

/*
- Verze 2
	- Eye Structures Fixed, used.
	- Ne FreeRTOS

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

*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <cstdlib>
#include <ctime>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define VIBRATION_SENSOR_PIN 33
#define BUZZER_PIN 32

#define TRIG_PIN 25
#define ECHO_PIN 26


int vibration_sensor_reading;
bool shake_detected = false;


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
    u_int width;
    u_int height;
    u_int corner_radius;
	
  public:
	  EyeShapeSquare(u_int w, u_int h, u_int r) : width(w), height(h), corner_radius(r)
    {
      // Check that radius is not larger than half of side lenght.
      if(this->corner_radius > width/2 || this->corner_radius > height/2)
      {
        this->corner_radius = (width-1)/2;
      }
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


void setup()
{
  Serial.begin(9600);

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

  // distance sensor initialisation
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}


void loop() 
{
  // Process vibration sensor
  vibration_sensor_reading = analogRead(VIBRATION_SENSOR_PIN);
  Serial.printf("Vibration value: %d\n", vibration_sensor_reading);
  
  if(vibration_sensor_reading < 4095)
  {
    shake_detected = true;
  }

  // Process Distance sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN,  HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  const unsigned long duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration / 29 / 2;
  if(duration==0)
  {
    Serial.println("Warning: no pulse from sensor");
  }  
  else
  {
    Serial.printf("Distance to nearest object: %d cm.\n", distance);
  }

  // Behave according to readings
  if(!shake_detected)
  {
    look_around_and_blink();
  }
  else
  {
    // If shaking detected, draw scared eyes: (X X)
    display.clearDisplay();
    eyeLeft.draw(&xEye);
    eyeRight.draw(&xEye);
    display.display();
    delay(2000);  // While shaking, keep this eyes
    shake_detected = false;
  }
}


void look_around_and_blink()
{ 
    display.clearDisplay();

    // Generate random position change for the eyes
    int rand_x_change = (rand() % 10) + 1 - 5;
    int rand_y_change = (rand() % 10) + 1 - 5;
    int rand_time = (rand() % 400) + 1 + 400;
    int rand_blink = (rand() % 3);

    // Randomly change slightly position of the eyes and draw them
    eyeLeft.moveEye(rand_x_change, rand_y_change);
    eyeRight.moveEye(rand_x_change, rand_y_change);
    eyeLeft.draw();
    eyeRight.draw();
    display.display();

    // Keep the eyes drawn on this position for random time
    delay(rand_time);

    if(rand_blink == 0)
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
      tone(BUZZER_PIN, blink_tone, 3);  // Make sound when blinked
      // delay(3);
      noTone(BUZZER_PIN);
      delay(300);
    }
    
    // if(rand_blink == 1)
    // {
    //   display.clearDisplay();
    //   display.drawBitmap(
    //     (display.width()  - 128 ) / 2,
    //     (display.height() - 32) / 2,
    //     epd_bitmap, 128, 32, 1);
    //   display.display();
    //   delay(1000);
    // }

    // Move eyes back to the center
    eyeLeft.centerEye();
    eyeRight.centerEye();
}

