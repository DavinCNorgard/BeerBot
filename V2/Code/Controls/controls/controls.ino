
#include <PS4BT.h>
#include <usbhub.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

// Control modes
const int BEGINNER = 1;
const int TANK_MODE = 2;
const int GAME_MODE = 3;
const int RESET_MODE = 0;
int MAX_POWER = 100;
int CURRENT_MODE = 0;

// control constants
int THRESHOLD = 8;
bool LEFT_GEAR = true;
bool RIGHT_GEAR = true;

// motor controller pins:

//Left Motor (1)
int RPWM_1 = 3;  // Digital/PWM pin 3 to the RPWM on the BTS7960
int LPWM_1 = 5;  // Digital/PWM pin 5 to the LPWM on the BTS7960
int L_EN_1 = 4;  // connect Digital/PWM pin 4 to L_EN on the BTS7960
int R_EN_1 = 2;  // connect Digital/PWM pin 2 to R_EN on the BTS7960

//Right Motor (2)
int RPWM_2 = 6;  // Digital/PWM pin 6 to the RPWM on the BTS7960
int LPWM_2 = 9;  // Digital/PWM pin 9 to the LPWM on the BTS7960
int L_EN_2 = A0;  // connect Digital/PWM pin A0 to L_EN on the BTS7960
int R_EN_2 = 7;  // connect Digital/PWM pin 7 to R_EN on the BTS7960

// motorController functions

void killMotors() {
  analogWrite(LPWM_1, 0); 
  analogWrite(RPWM_1, 0); 
  analogWrite(LPWM_2, 0); 
  analogWrite(RPWM_2, 0); 
}


//Bluetooth PS4 setup
USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so

/* You can create the instance of the PS4BT class in two ways */
// This will start an inquiry and then pair with the PS4 controller - you only have to do this once
// You will need to hold down the PS and Share button at the same time, the PS4 controller will then start to blink rapidly indicating that it is in pairing mode
//PS4BT PS4(&Btd, PAIR);

// After that you can simply create the instance like so and then press the PS button on the device
PS4BT PS4(&Btd);

bool printAngle, printTouch;
uint8_t oldL2Value, oldR2Value;

void setup() {

  Serial.begin(115200);
  #if !defined(__MIPSEL__)
    while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
  #endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); // Halt
  }
  Serial.print(F("\r\nPS4 Bluetooth Library Started"));  

  // set the pin modes to output
  pinMode(R_EN_1, OUTPUT);
  pinMode(RPWM_1, OUTPUT);
  pinMode(L_EN_1, OUTPUT);
  pinMode(LPWM_1, OUTPUT);

  pinMode(RPWM_2, OUTPUT);
  pinMode(LPWM_2, OUTPUT);
  pinMode(L_EN_2, OUTPUT);
  pinMode(R_EN_2, OUTPUT);

  //make sure these pins are set to 0, this prevents motor movement startup
  killMotors();

  //Open h-bridges, enable motor control
  digitalWrite(R_EN_1, HIGH);  
  digitalWrite(L_EN_1, HIGH);
  digitalWrite(R_EN_2, HIGH);  
  digitalWrite(L_EN_2, HIGH);
}

// helper functions
double normalizeJoystick(int input) {
  double value = ((127.5 - input)/127.5)*MAX_POWER;
  return value;
}

double exponential(double input) {
  return input*input/MAX_POWER;
}

//0-1920
void setMaxPower(double touchPadValue){

  int power_level;
  
  //Define power levels:
  if(touchPadValue < 400){
    power_level = 100;
  }else if(touchPadValue >= 400 && touchPadValue < 800){
    power_level = 130;
  }else if(touchPadValue >= 800 && touchPadValue < 1200){
    power_level = 170;
  }else if(touchPadValue >= 1200 && touchPadValue < 1600){
    power_level = 215;
  }else if(touchPadValue >= 1600){
    power_level = 255;
  }else{
    power_level = 100;
  }

  MAX_POWER = power_level;
}

void loop() {
  Usb.Task();

  if (PS4.connected()) {

    //Determine game mode
    if(PS4.getButtonClick(LEFT)) {
      PS4.setLed(Red);
      CURRENT_MODE = TANK_MODE;      
    }
    if(PS4.getButtonClick(UP)) {
      PS4.setLed(Yellow);
      CURRENT_MODE = BEGINNER;      
    }
    if(PS4.getButtonClick(RIGHT)) {
      PS4.setLed(Green);
      CURRENT_MODE = GAME_MODE;      
    }
    //Reset controls, kill motors, disable motor control
    if(PS4.getButtonClick(DOWN)) {
      Serial.print("\n Resetting");
      CURRENT_MODE = RESET_MODE;     
      MAX_POWER = 100;
      PS4.setLed(Blue);
      killMotors();
    }

    if (PS4.isTouching(0) || PS4.isTouching(1)) // Print newline and carriage return if any of the fingers are touching the touchpad
      Serial.print(F("\r\n"));
    for (uint8_t i = 0; i < 1; i++) { // The touchpad track one finger
      if (PS4.isTouching(i)) { // Print the position of the finger if it is touching the touchpad
        Serial.print(F("X")); Serial.print(i + 1); Serial.print(F(": "));
        Serial.print(PS4.getX(i));
        if(PS4.getButtonClick(SQUARE)){
          //set the max power
          setMaxPower(PS4.getX(i));  
        }
      }
    }

    switch(CURRENT_MODE) {
      case TANK_MODE:

        if(normalizeJoystick(PS4.getAnalogHat(LeftHatY)) > THRESHOLD ){
          Serial.print(F("\n\tLeftHatY: "));
          Serial.print(normalizeJoystick(PS4.getAnalogHat(LeftHatY)));
          double leftThrottleValue = exponential(normalizeJoystick(PS4.getAnalogHat(LeftHatY)));
          analogWrite(RPWM_1, leftThrottleValue);

        } else if (normalizeJoystick(PS4.getAnalogHat(LeftHatY)) < -THRESHOLD) {
          Serial.print(F("\n\tLeftHatY: "));
          Serial.print(normalizeJoystick(PS4.getAnalogHat(LeftHatY)));
          double leftThrottleValue = exponential(normalizeJoystick(PS4.getAnalogHat(LeftHatY)));
          analogWrite(LPWM_1, leftThrottleValue);
        
        } else {
          analogWrite(LPWM_1, 0);
          analogWrite(RPWM_1, 0);
        }
        
        if(normalizeJoystick(PS4.getAnalogHat(RightHatY)) > THRESHOLD ){
          Serial.print(F("\n\tRightHatY: "));
          Serial.print(normalizeJoystick(PS4.getAnalogHat(RightHatY)));
          double rightThrottleValue = exponential(normalizeJoystick(PS4.getAnalogHat(RightHatY)));
          analogWrite(LPWM_2, rightThrottleValue);

        } else if (normalizeJoystick(PS4.getAnalogHat(RightHatY)) < -THRESHOLD) {
          Serial.print(F("\n\tRightHatY: "));
          Serial.print(normalizeJoystick(PS4.getAnalogHat(RightHatY)));
          double rightThrottleValue = exponential(normalizeJoystick(PS4.getAnalogHat(RightHatY)));
          analogWrite(RPWM_2, rightThrottleValue);
        
        } else {
          analogWrite(LPWM_2, 0);
          analogWrite(RPWM_2, 0);
        }

        
        break;
    }

    
    
  }else{
    PS4.setLedFlash(10, 10); // Set it to blink rapidly
    killMotors(); //stop the robot from moving
  }

  //   if (PS4.getAnalogButton(L2) != oldL2Value || PS4.getAnalogButton(R2) != oldR2Value) // Only write value if it's different
  //     PS4.setRumbleOn(PS4.getAnalogButton(L2), PS4.getAnalogButton(R2));
  //   oldL2Value = PS4.getAnalogButton(L2);
  //   oldR2Value = PS4.getAnalogButton(R2);

  //   if (PS4.getButtonClick(PS)) {
  //     Serial.print(F("\r\nPS"));
  //     PS4.disconnect();
  //   }
  //   else {
  //     if (PS4.getButtonClick(TRIANGLE)) {
  //       Serial.print(F("\r\nTriangle"));
  //       PS4.setRumbleOn(RumbleLow);
  //     }
  //     if (PS4.getButtonClick(CIRCLE)) {
  //       Serial.print(F("\r\nCircle"));
  //       PS4.setRumbleOn(RumbleHigh);
  //     }
  //     if (PS4.getButtonClick(CROSS)) {
  //       Serial.print(F("\r\nCross"));
  //       PS4.setLedFlash(10, 10); // Set it to blink rapidly
  //     }
  //     if (PS4.getButtonClick(SQUARE)) {
  //       Serial.print(F("\r\nSquare"));
  //       PS4.setLedFlash(0, 0); // Turn off blinking
  //     }

  //     if (PS4.getButtonClick(UP)) {
  //       Serial.print(F("\r\nUp"));
  //       PS4.setLed(Red);
  //     } if (PS4.getButtonClick(RIGHT)) {
  //       Serial.print(F("\r\nRight"));
  //       PS4.setLed(Blue);
  //     } if (PS4.getButtonClick(DOWN)) {
  //       Serial.print(F("\r\nDown"));
  //       PS4.setLed(Yellow);
  //     } if (PS4.getButtonClick(LEFT)) {
  //       Serial.print(F("\r\nLeft"));
  //       PS4.setLed(Green);
  //     }

  //     if (PS4.getButtonClick(L1))
  //       Serial.print(F("\r\nL1"));
  //     if (PS4.getButtonClick(L3))
  //       Serial.print(F("\r\nL3"));
  //     if (PS4.getButtonClick(R1))
  //       Serial.print(F("\r\nR1"));
  //     if (PS4.getButtonClick(R3))
  //       Serial.print(F("\r\nR3"));

  //     if (PS4.getButtonClick(SHARE))
  //       Serial.print(F("\r\nShare"));
  //     if (PS4.getButtonClick(OPTIONS)) {
  //       Serial.print(F("\r\nOptions"));
  //       printAngle = !printAngle;
  //     }
  //     if (PS4.getButtonClick(TOUCHPAD)) {
  //       Serial.print(F("\r\nTouchpad"));
  //       printTouch = !printTouch;
  //     }

  //     if (printAngle) { // Print angle calculated using the accelerometer only
  //       Serial.print(F("\r\nPitch: "));
  //       Serial.print(PS4.getAngle(Pitch));
  //       Serial.print(F("\tRoll: "));
  //       Serial.print(PS4.getAngle(Roll));
  //     }

  //     if (printTouch) { // Print the x, y coordinates of the touchpad
  //       if (PS4.isTouching(0) || PS4.isTouching(1)) // Print newline and carriage return if any of the fingers are touching the touchpad
  //         Serial.print(F("\r\n"));
  //       for (uint8_t i = 0; i < 2; i++) { // The touchpad track two fingers
  //         if (PS4.isTouching(i)) { // Print the position of the finger if it is touching the touchpad
  //           Serial.print(F("X")); Serial.print(i + 1); Serial.print(F(": "));
  //           Serial.print(PS4.getX(i));
  //           Serial.print(F("\tY")); Serial.print(i + 1); Serial.print(F(": "));
  //           Serial.print(PS4.getY(i));
  //           Serial.print(F("\t"));
  //         }
  //       }
  //     }
  //   }
}
