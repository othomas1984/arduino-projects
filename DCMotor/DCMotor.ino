
const int MOTOR_PIN = 11;

void setup() {
  // put your setup code here, to run once:
  pinMode(MOTOR_PIN, OUTPUT);
  Serial.begin(9600);
}

int speed = 100;
int increasing = 1;

void loop() {
  // put your main code here, to run repeatedly:

  changeSpeed();
  delay(75);

}

void changeSpeed() {
  if(increasing == 1) {
    if(speed<254) {
      speed = speed + 1;
    } else { increasing = 0; }
  } else {
    if(speed>75) {
      speed = speed - 1;
    } else { increasing = 1; }
  }
  analogWrite(MOTOR_PIN, speed);
  Serial.println(speed);
}
