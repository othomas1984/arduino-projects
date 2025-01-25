
const String A = "This is string a.";
const String B = "This is string b.";
const String C = "This is string c.";

char lastDataReceived;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()) {
    lastDataReceived = Serial.read();
    if(lastDataReceived == 'A') {
      Serial.println(A);
    }
    if(lastDataReceived == 'B') {
      Serial.println(B);
    }
    if(lastDataReceived == 'C') {
      Serial.println(C);
    }
  }
}
