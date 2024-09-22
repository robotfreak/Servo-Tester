#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <s3servo.h>  // https://github.com/Rob58329/ESP32S3servo.git
#include <Encoder.h>  // https://github.com/PaulStoffregen/Encoder

#define SDA0_Pin 6  // select ESP32  I2C pins
#define SCL0_Pin 5


/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c  //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1     //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUM_SERVOS 4

s3servo servos[NUM_SERVOS];
int servoPins[NUM_SERVOS] = { 1, 2, 3, 4 };

bool usMode = false;     // microseconds or degree
bool sweepMode = false;  // sweep or normale mode

int pos = 90;
int pos_us = 1500;

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(8, 9);
//   avoid using pins with LEDs attached
long oldPosition = -999;

int encBtnPin = 7;
int encBtnState;
int encBtn;
int sweepBtnPin = 10;
int sweepBtnState;
int sweepBtn;
bool sweepUpDir = true;
int sweepDelay = 10;
int posBtnPin = 11;
int posBtnState;
int posBtn;
#define NUM_POS 12
int posStates[] = { 90, 120, 150, 180, 150, 120, 90, 60, 30, 0, 30, 60 };
int posState = 0;

#define RXD1 19
#define TXD1 20

void initButtons() {
  pinMode(encBtnPin, INPUT_PULLUP);
  encBtnState = 0;
  pinMode(sweepBtnPin, INPUT_PULLUP);
  sweepBtnState = 0;
  pinMode(posBtnPin, INPUT_PULLUP);
  posBtnState = 0;
}

void initDisplay() {
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.

  delay(250);  // wait for the OLED to power up
  Wire.begin(SDA0_Pin, SCL0_Pin);
  display.begin(i2c_Address, true);  // Address 0x3C default
                                     //display.setContrast (0); // dim display
  display.display();
  delay(2000);
  display.display();
}

void initServos() {
  for (int i = 0; i < NUM_SERVOS; i++) {
    servos[i].attach(servoPins[i]);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //  Serial.begin(9600, SERIAL_8N1, RXD1, TXD1);
  Serial.println("Servo Tester v0.1");

  initButtons();
  initDisplay();
  initServos();
}

void readEncoders() {
  long newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.print("Enc: ");
    Serial.println(newPosition);
    if (sweepMode == false) {
      if (newPosition > 0 && pos < 180)
        pos++;
      else if (newPosition < 0 && pos > 0)
        pos--;
      pos_us = map(pos, 0, 180, 500, 2000);
    } else {
      if (newPosition > 0 && sweepDelay < 100)
        sweepDelay++;
      else if (newPosition < 0 && sweepDelay > 0)
        sweepDelay--;
    }
    myEnc.write(0);
  }
}

void readButtons() {
  encBtn = digitalRead(encBtnPin);
  if (encBtn == LOW && encBtnState == 0) {
    encBtnState = 1;
    Serial0.print("Btn: ");
    Serial0.println(encBtnState);
    if (sweepMode == false) {
      pos = 90;
    }
    if (usMode == true) {
      usMode = false;
    } else {
      usMode = true;
    }
  } else if (encBtn == HIGH && encBtnState == 1) {
    encBtnState = 0;
  }
  sweepBtn = digitalRead(sweepBtnPin);
  if (sweepBtn == LOW && sweepBtnState == 0) {
    sweepBtnState = 1;
    if (sweepMode == true)
      sweepMode = false;
    else
      sweepMode = true;
    sweepUpDir = true;
    sweepDelay = 10;
  } else if (sweepBtn == HIGH && sweepBtnState == 1) {
    sweepBtnState = 0;
  }
  posBtn = digitalRead(posBtnPin);
  if (posBtn == LOW && posBtnState == 0) {
    posBtnState = 1;
    if (posState < NUM_POS - 1) {
      posState++;
      pos = posStates[posState];
    } else if (posState == NUM_POS - 1) {
      posState = 0;
      pos = posStates[posState];
    }

  } else if (posBtn == HIGH && posBtnState == 1) {
    posBtnState = 0;
  }
}

void updateDisplay(void) {
  static int oldPos = -1;
  static int oldSweepDelay = -1;
  static bool oldUsMode = true;

  if (pos != oldPos || sweepDelay != oldSweepDelay || usMode != oldUsMode) {

    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0, 0);
    display.println("Servo Tester v0.1");
    display.setCursor(20, 20);
    display.print("pos");
    if (usMode == true)
      display.print(" us");
    display.setCursor(80, 20);
    display.print("dly ms");

    display.setTextSize(2);
    display.setCursor(20, 40);
    if (usMode == false)
      display.print(pos);
    else {
      pos_us = map(pos, 0, 180, 1000, 2000);
      display.print(pos_us);
    }
    //display.setTextColor(SH110X_BLACK, SH110X_WHITE); // 'inverted' text
    oldPos = pos;

    display.setTextSize(2);
    display.setCursor(80, 40);
    display.print(sweepDelay);
    oldSweepDelay = sweepDelay;

    oldUsMode = usMode;

    display.display();
  }
}

void updateServos() {
  if (sweepMode == true) {
    if (sweepDelay == 0) {
      pos = (pos != 0) ? 0 : 180;
      for (int i = 0; i < NUM_SERVOS; i++) {
        servos[i].write(pos);
        delay(200);
      }
    } else {
      if (sweepUpDir == true && pos < 180)
        pos++;
      else if (pos > 0)
        pos--;
      if (pos == 180 && sweepUpDir == true)
        sweepUpDir = false;
      else if (pos == 0 && sweepUpDir == false)
        sweepUpDir = true;
      for (int i = 0; i < NUM_SERVOS; i++) {
        servos[i].write(pos);
        delay(sweepDelay);
      }
    }
  } else {
    for (int i = 0; i < NUM_SERVOS; i++) {
      servos[i].write(pos);
    }
  }
}

void loop() {
  readEncoders();
  readButtons();

  updateServos();
  updateDisplay();
}
