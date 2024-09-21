#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <s3servo.h>
#include <Encoder.h>

#define SDA0_Pin 6   // select ESP32  I2C pins
#define SCL0_Pin 5


/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUM_SERVOS 4

s3servo servos[NUM_SERVOS];
int servoPins[NUM_SERVOS] = {1,2,3,4};

int pos = 90;

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(8, 9);
//   avoid using pins with LEDs attached
long oldPosition  = -999;
int encBtnPin = 7;
int btnState; 
int btn;

#define RXD1 19
#define TXD1 20

void setup() {
  // put your setup code here, to run once:
  Serial0.begin(9600);
//  Serial.begin(9600, SERIAL_8N1, RXD1, TXD1);
  Serial0.println("Servo Tester v0.1");

  pinMode(encBtnPin, INPUT_PULLUP);
  btnState = 0;

  for(int i=0; i<NUM_SERVOS; i++) {
    servos[i].attach(servoPins[i]);
  }

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.

  delay(250); // wait for the OLED to power up
  Wire.begin(SDA0_Pin, SCL0_Pin);
  display.begin(i2c_Address, true); // Address 0x3C default
 //display.setContrast (0); // dim display
 
  display.display();
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Servo Tester v0.1");
  display.display();

}

void loop() {
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial0.print("Enc: ");
    Serial0.println(newPosition);
    if (newPosition > 0 && pos < 180)
      pos ++;
    else if (newPosition < 0 && pos > 0)
      pos--;
    myEnc.write(0);
    Serial0.print("Pos: ");
    Serial0.println(pos);
    display.clearDisplay();
    display.setCursor(40, 40);
    display.setTextSize(2);
    display.print("   ");
    display.setCursor(40, 40);
    display.print(pos);
    //display.setTextColor(SH110X_BLACK, SH110X_WHITE); // 'inverted' text
    display.display();

  }
  btn = digitalRead(encBtnPin);
  if (btn == LOW && btnState == 0) {
    btnState = 1;
    Serial0.print("Btn: ");
    Serial0.println(btnState);
    pos = 90;
    Serial.print("Pos: ");
    Serial.println(pos);
    display.clearDisplay();
    display.setCursor(40, 40);
    display.setTextSize(2);
    display.print("   ");
    display.setCursor(40, 40);
    display.println(pos);
    //display.setTextColor(SH110X_BLACK, SH110X_WHITE); // 'inverted' text
    display.display();
  }
  else if (btn == HIGH && btnState == 1) {
    btnState = 0;
  }

  for(int i=0; i<NUM_SERVOS; i++) {
    servos[i].write(pos);
  }
  // put your main code here, to run repeatedly:

}
