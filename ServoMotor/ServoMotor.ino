
#include <Servo.h>

Servo servo;

void setup() {
  // put your setup code here, to run once:
  servo.attach(9);
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int pos = 0; pos <= 150; pos ++) {
    servo.write(pos);
    delay(10);
  }

  for(int pos = 60; pos > 0; pos--) {
    servo.write(pos);
    delay(10);
  }
}
