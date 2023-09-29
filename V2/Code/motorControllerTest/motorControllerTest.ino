int RPWM = 3;  // Digital/PWM pin 3 to the RPWM on the BTS7960
int LPWM = 5;  // Digital/PWM pin 5 to the LPWM on the BTS7960

// Enable "Left" and "Right" movement
int L_EN = 4;  // connect Digital/PWM pin 4 to L_EN on the BTS7960
int R_EN = 2;  // connect Digital/PWM pin 2 to R_EN on the BTS7960

void setup() {
  // put your setup code here, to run once:

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);


  digitalWrite(R_EN, HIGH);  
  digitalWrite(L_EN, HIGH);
}

void loop() {
  analogWrite(RPWM, 64); // pulse a signal continually at the rate of 64 
  // the analogWrite line above should start the motor turning in one direction at about 1/4 of power.
  delay(5000); // wait 5 seconds, motor continues to move because the analogWrite is still pulsing
  analogWrite(RPWM, 128); // pulse signal now at 128 (about half power... half of max 255).
  delay(5000);

  // after 5 seconds at half power, stop the motor moving
  analogWrite(RPWM, 0);
  delay(5000);
  // now start moving in opposite direction.
  analogWrite(LPWM, 64);
  delay(5000);
  analogWrite(LPWM, 128);
  delay(5000);
  analogWrite(LPWM, 0); // Stop moving in this direction
  // at this point should be no movement.
  
  delay(5000);

}
