#include <Adafruit_NeoPixel.h>
#include "LEDSection.h"
#include "Scene1.h"

// Define constants
const int PIXEL_COUNT = 300;    // Total number of LEDs
const int PIXEL_PIN = 6;       // Pin connected to the NeoPixel strip

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
Scene1* scene1;

void setup() {
  strip.begin();   // Initialize section 1
  strip.setBrightness(30);
  scene1 = new Scene1(&strip);
  scene1->setup();
  strip.setPixelColor(15, 50, 50, 50);
}

void loop() {
  scene1->loop();
}

void eraseAll(int wait) {
  for(int i = 0; i < PIXEL_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    if(wait > 0) {
      strip.show();
      delay(wait);
    }
  }
}
