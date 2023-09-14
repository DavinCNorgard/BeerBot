
#include <PS4BT.h>
#include <usbhub.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>
#include <Servo.h>

// Servos
Servo LEFT_THROTTLE;
Servo LEFT_SHIFT1;
Servo LEFT_SHIFT2;
Servo RIGHT_THROTTLE;
Servo RIGHT_SHIFT1;
Servo RIGHT_SHIFT2;
int LEFT_THROTTLE_PIN = 2;
int LEFT_SHIFT_PIN1 = 3;
int LEFT_SHIFT_PIN2 = 4;
int RELAY_LEFT = 5;
int RIGHT_THROTTLE_PIN = 8;
int RIGHT_SHIFT_PIN1 = 9;
int RIGHT_SHIFT_PIN2 = 11;
int RELAY_RIGHT = 12;
const int SERVO_ZERO = 90;
//dont use pin 10

// Control modes
const int TANK_MODE_MANUAL = 1;
const int TANK_MODE_AUTO = 2;
const int GAME_MODE = 3;
int CURRENT_MODE = 3;

// control constants
int THRESHOLD = 8;
bool LEFT_GEAR = true;
bool RIGHT_GEAR = true;

//Bluetooth PS4 setup
USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so

/* You can create the instance of the PS4BT class in two ways */
// This will start an inquiry and then pair with the PS4 controller - you only have to do this once
// You will need to hold down the PS and Share button at the same time, the PS4 controller will then start to blink rapidly indicating that it is in pairing mode
PS4BT PS4(&Btd, PAIR);

// After that you can simply create the instance like so and then press the PS button on the device
//PS4BT PS4(&Btd);

bool printAngle, printTouch;
uint8_t oldL2Value, oldR2Value;

void setup() {
  LEFT_THROTTLE.attach(LEFT_THROTTLE_PIN); 
  LEFT_SHIFT1.attach(LEFT_SHIFT_PIN1); 
  LEFT_SHIFT2.attach(LEFT_SHIFT_PIN2); 
  RIGHT_THROTTLE.attach(RIGHT_THROTTLE_PIN); 
  RIGHT_SHIFT1.attach(RIGHT_SHIFT_PIN1); 
  RIGHT_SHIFT2.attach(RIGHT_SHIFT_PIN2); 

  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); // Halt
  }
  Serial.print(F("\r\nPS4 Bluetooth Library Started"));
  // initialize digital pin as an output.
  // pinMode(RELAY_RIGHT, OUTPUT);
  // pinMode(RELAY_LEFT, OUTPUT);
  
}

// helper functions

double normalizeJoystick(int input) {
  return ((127.5 - input)/127.5)*100;
}

double exponential(double input) {
  if(input < 0){
    return -input*input/100;
  }else{
    return input*input/100;
  }
  
}

int mapToServoRight(double throttleValue){
  return SERVO_ZERO-throttleValue;
  
}

void loop() {
  Usb.Task();

  if (PS4.connected()) {
    // digitalWrite(RELAY_RIGHT, HIGH);
    // digitalWrite(RELAY_LEFT, HIGH);
    // if (PS4.getAnalogHat(LeftHatX) > 137 || PS4.getAnalogHat(LeftHatX) < 117 || PS4.getAnalogHat(LeftHatY) > 137 || PS4.getAnalogHat(LeftHatY) < 117 || PS4.getAnalogHat(RightHatX) > 137 || PS4.getAnalogHat(RightHatX) < 117 || PS4.getAnalogHat(RightHatY) > 137 || PS4.getAnalogHat(RightHatY) < 117) {
    //   Serial.print(F("\r\nLeftHatX: "));
    //   Serial.print(exponential(normalizeJoystick(PS4.getAnalogHat(LeftHatX))));
    //   // Serial.print(F("\tLeftHatY: "));
    //   // Serial.print(PS4.getAnalogHat(LeftHatY));
    //   // Serial.print(F("\tRightHatX: "));
    //   // Serial.print(PS4.getAnalogHat(RightHatX));
    //   // Serial.print(F("\tRightHatY: "));
    //   // Serial.print(PS4.getAnalogHat(RightHatY));
    // }

    if(PS4.getButtonClick(LEFT)) {
      PS4.setLed(Red);
      CURRENT_MODE = TANK_MODE_MANUAL;      
    }
    if(PS4.getButtonClick(UP)) {
      PS4.setLed(Yellow);
      CURRENT_MODE = TANK_MODE_AUTO;      
    }
    if(PS4.getButtonClick(RIGHT)) {
      PS4.setLed(Green);
      CURRENT_MODE = GAME_MODE;      
    }
    if(PS4.getButtonClick(DOWN)) {
      Serial.print("\n Down");
      digitalWrite(RELAY_LEFT, HIGH);    
      delay(300);
    }

    // Serial.print(F("\n"));
    // Serial.print(CURRENT_MODE);

    switch(CURRENT_MODE) {
      case TANK_MODE_MANUAL:

        // if(PS4.getButtonClick(R1)){
        //   Serial.print(F("\n shift right\n"));
        //   RIGHT_GEAR = !RIGHT_GEAR;
        //   if(RIGHT_GEAR){
        //     RIGHT_SHIFT1.write(120);
        //     RIGHT_SHIFT2.write(120);
        //   }else{
        //     RIGHT_SHIFT1.write(90);
        //     RIGHT_SHIFT2.write(90);

        //   }
          
        // }
        // if(PS4.getButtonClick(L1)){
        //   LEFT_GEAR = !LEFT_GEAR;
        //   if(LEFT_GEAR){
        //     //forward
        //     LEFT_SHIFT1.write(123);
        //     LEFT_SHIFT2.write(123);
        //     Serial.print(F("\n shift left 120\n"));
        //   }else{
        //     //reverse
        //     LEFT_SHIFT1.write(50);
        //     LEFT_SHIFT2.write(50);
        //     Serial.print(F("\n shift left 90\n"));
        //   }
        // }

        if(normalizeJoystick(PS4.getAnalogHat(LeftHatY)) > THRESHOLD || normalizeJoystick(PS4.getAnalogHat(LeftHatY)) < -THRESHOLD){
          Serial.print(F("\n\tLeftHatY: "));
          Serial.print(normalizeJoystick(PS4.getAnalogHat(LeftHatY)));
          double leftThrottleValue = exponential(normalizeJoystick(PS4.getAnalogHat(LeftHatY)));
          LEFT_THROTTLE.write(mapToServoRight(leftThrottleValue));
        }else {
          LEFT_THROTTLE.write(SERVO_ZERO);
        }
        if(normalizeJoystick(PS4.getAnalogHat(RightHatY)) > THRESHOLD || normalizeJoystick(PS4.getAnalogHat(RightHatY)) < -THRESHOLD){
          double rightThrottleValue = exponential(normalizeJoystick(PS4.getAnalogHat(RightHatY)));
          RIGHT_THROTTLE.write(mapToServoRight(rightThrottleValue));
          Serial.print(F("\n\tRightHatY: "));
          Serial.print(exponential(normalizeJoystick(PS4.getAnalogHat(RightHatY))));
        }else {
          RIGHT_THROTTLE.write(SERVO_ZERO);
        }

        if (PS4.getButtonClick(CROSS)) {
          Serial.print(F("\r\nCross"));
          PS4.setLedFlash(10, 10); // Set it to blink rapidly
          digitalWrite(RELAY_RIGHT, LOW);
          digitalWrite(RELAY_LEFT, LOW);
        }
        
        break;
      // case TANK_MODE_AUTO:
      // case GAME_MODE:
    }

    
    
  }else{
    PS4.setLedFlash(10, 10); // Set it to blink rapidly
    // digitalWrite(RELAY_RIGHT, LOW);
    // digitalWrite(RELAY_LEFT, LOW);
  }
  //   if (PS4.getAnalogButton(L2) || PS4.getAnalogButton(R2)) { // These are the only analog buttons on the PS4 controller
  //     Serial.print(F("\r\nL2: "));
  //     Serial.print(PS4.getAnalogButton(L2));
  //     Serial.print(F("\tR2: "));
  //     Serial.print(PS4.getAnalogButton(R2));
  //   }
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
