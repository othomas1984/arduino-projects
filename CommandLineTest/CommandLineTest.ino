void setup() {
  Serial.begin(9600);  // Start serial at 9600 baud
  while (!Serial) {
    ; // Wait for serial port to connect (only needed for native USB boards)
  }
  Serial.println("Arduino ready. Send something!");
}

void loop() {
  // Check if data is available
  if (Serial.available() > 0) {
    // Read the incoming byte
    String incoming = Serial.readStringUntil('\n'); // read until newline
    // Echo back to the sender
    Serial.print("Received: ");
    Serial.println(incoming);
  }
}
// osascript -e 'tell application "Spotify" to previous track'; echo "T" > /dev/cu.usbmodem101