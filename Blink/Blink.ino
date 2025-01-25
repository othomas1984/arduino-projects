// Blink
/*  Turns an LED on for one second, off for one second, 
 *  and then repeats forever.
 */

const int LED_PIN = 13;

void setup() {
  // put your setup code here, to run once:

  // set the LED pin to be an output
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(LED_PIN, HIGH); // thru LED on
  delay(1000);
  digitalWrite(LED_PIN, LOW); // turn LED off
  delay(1000);
}
