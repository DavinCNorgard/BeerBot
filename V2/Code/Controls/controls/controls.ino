
#include <PS4BT.h>
#include <usbhub.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

// Control modes
const int TANK_MODE_MANUAL = 1;
const int TANK_MODE_AUTO = 2;
const int GAME_MODE = 3;
int CURRENT_MODE = 3;

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
int RPWM_2 = 6;  // Digital/PWM pin 3 to the RPWM on the BTS7960
int LPWM_2 = 9;  // Digital/PWM pin 5 to the LPWM on the BTS7960
int L_EN_2 = A0;  // connect Digital/PWM pin 4 to L_EN on the BTS7960
int R_EN_2 = 7;  // connect Digital/PWM pin 2 to R_EN on the BTS7960

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
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(A0, OUTPUT);
  pinMode(7, OUTPUT);

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
  double value = ((127.5 - input)/127.5)*100;
    if(value < 0){
    return -1*value;
  }else{
    return value;
  }
}

double exponential(double input) {
  if(input < 0){
    return -input*input/100;
  }else{
    return input*input/100;
  }
  
}


void loop() {
  Usb.Task();

  if (PS4.connected()) {

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
      killMotors();
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

        if(normalizeJoystick(PS4.getAnalogHat(LeftHatY)) > THRESHOLD ){
          Serial.print(F("\n\tLeftHatY: "));
          Serial.print(normalizeJoystick(PS4.getAnalogHat(LeftHatY)));
          double leftThrottleValue = exponential(normalizeJoystick(PS4.getAnalogHat(LeftHatY)));
          analogWrite(RPWM_1, normalizeJoystick(PS4.getAnalogHat(LeftHatY)));

        } else if (normalizeJoystick(PS4.getAnalogHat(LeftHatY)) < -THRESHOLD) {
          Serial.print(F("\n\tLeftHatY: "));
          Serial.print(normalizeJoystick(PS4.getAnalogHat(LeftHatY)));
          double leftThrottleValue = exponential(normalizeJoystick(PS4.getAnalogHat(LeftHatY)));
          analogWrite(RPWM_1, normalizeJoystick(PS4.getAnalogHat(LeftHatY)));
        
        } else {
          analogWrite(LPWM_1, 0);
          analogWrite(RPWM_1, 0);
        }
        
        // if(normalizeJoystick(PS4.getAnalogHat(RightHatY)) > THRESHOLD || normalizeJoystick(PS4.getAnalogHat(RightHatY)) < -THRESHOLD){
        //   double rightThrottleValue = exponential(normalizeJoystick(PS4.getAnalogHat(RightHatY)));
        //   RIGHT_THROTTLE.write(mapToServoRight(rightThrottleValue));
        //   Serial.print(F("\n\tRightHatY: "));
        //   Serial.print(exponential(normalizeJoystick(PS4.getAnalogHat(RightHatY))));
        // }else {
        //   RIGHT_THROTTLE.write(SERVO_ZERO);
        // }

        // if (PS4.getButtonClick(CROSS)) {
        //   Serial.print(F("\r\nCross"));
        //   PS4.setLedFlash(10, 10); // Set it to blink rapidly
        //   digitalWrite(RELAY_RIGHT, LOW);
        //   digitalWrite(RELAY_LEFT, LOW);
        // }
        
        break;
      // case TANK_MODE_AUTO:
      // case GAME_MODE:
    }

    
    
  }else{
    PS4.setLedFlash(10, 10); // Set it to blink rapidly
    killMotors();
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
