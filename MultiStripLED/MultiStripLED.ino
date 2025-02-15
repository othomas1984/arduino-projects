#include <Adafruit_NeoPixel.h>
#include "Scene1.h"
#include "Scene2.h"

// Define constants
const int PIXEL_COUNT1 = 300;    // Total number of LEDs
const int PIXEL_COUNT2 = 150;    // Total number of LEDs
const int PIXEL_PIN_1 = 4;       // Pin connected to the NeoPixel strip
const int PIXEL_PIN_2 = 2;       // Pin connected to the NeoPixel strip

Adafruit_NeoPixel strip1(PIXEL_COUNT1, PIXEL_PIN_1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2(PIXEL_COUNT2, PIXEL_PIN_2, NEO_GRB + NEO_KHZ800);
Scene1* scene1;
Scene2* scene2;

void setup() {
  Serial.begin(115200);
  strip1.begin();   // Initialize section 1
  strip1.setBrightness(60);
  scene1 = new Scene1(&strip1);
  scene1->setup();
  // strip1.setPixelColor(10, 50, 50, 50);
  strip1.show();

  strip2.begin();   // Initialize section 1
  strip2.setBrightness(20);
  scene2 = new Scene2(&strip2);
  scene2->setup();
  strip2.show();

}

void loop() {
  scene1->loop();
  scene2->loop();
}