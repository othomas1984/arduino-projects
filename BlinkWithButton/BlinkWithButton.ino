// BlinkWithButton
/*  Turns an LED on for one second, off for one second, 
 *  and then repeats forever.
 */

const int LED_PIN = 5;
const int ONBOARD_LED_PIN = 13;
const int BUTTON_PIN = 10;

const int SHORT_DELAY = 500;
const int LONG_DELAY = 1000;

void setup() {
  // put your setup code here, to run once:

  // set the LED pin to be an output
  pinMode(LED_PIN, OUTPUT);
  pinMode(ONBOARD_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(LED_PIN, HIGH); // thru LED on
  digitalWrite(ONBOARD_LED_PIN, LOW); // thru LED on

  if(digitalRead(BUTTON_PIN) == HIGH) {
    delay(SHORT_DELAY);
  } else {
    delay(LONG_DELAY);
  }

  digitalWrite(LED_PIN, LOW); // turn LED off
  digitalWrite(ONBOARD_LED_PIN, HIGH); // turn LED off

  if(digitalRead(BUTTON_PIN) == HIGH) {
    delay(SHORT_DELAY);
  } else {
    delay(LONG_DELAY);
  }
}
