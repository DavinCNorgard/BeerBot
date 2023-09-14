#include <Servo.h>
int servoPin = 7;
Servo Servo1;

void setup() {
  // put your setup code here, to run once:
  Servo1.attach(servoPin); 
  Serial.begin(9600);
  Serial.println("serial test 0021");

}

void loop() {
  // put your main code here, to run repeatedly:
  // Make servo go to 0 degrees 
  Servo1.write(20); 
  delay(4000); 
  // Make servo go to 90 degrees 
  Servo1.write(50); 
  delay(2000);

}
