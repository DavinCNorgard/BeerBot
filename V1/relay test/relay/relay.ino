/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-relay
 */

// constants won't change
const int RELAY_PIN1 = 5;  // the Arduino pin, which connects to the IN pin of relay
const int RELAY_PIN2 = 12;  // the Arduino pin, which connects to the IN pin of relay


// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin as an output.
  pinMode(RELAY_PIN1, OUTPUT);
  pinMode(RELAY_PIN2, OUTPUT);

}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(RELAY_PIN1, HIGH);
  delay(500);
  digitalWrite(RELAY_PIN2, HIGH);
  delay(500);
}