// Scene1.cpp
#include "Scene1.h"
#include "LED.h"
#include "LEDSection.h"

Scene1::Scene1(Adafruit_NeoPixel* strip) :
  strip(strip),
  sections {
    LEDSection(strip, 0, 49),
    LEDSection(strip, 50, 99),
    LEDSection(strip, 100, 149),
    LEDSection(strip, 150, 199),
    LEDSection(strip, 200, 249),
    LEDSection(strip, 250, 299),
  },
  fullStrip(strip, 0, 299)
{
}

void Scene1::setup() {
  fullStrip.eraseAll(0);
}

void Scene1::loop() {
  for (int i = 0; i < sizeof(sections) / sizeof(sections[0]); i++) {
    sections[i].rainbowWipe(1, true);
  }
  for (int i = 0; i < sizeof(sections) / sizeof(sections[0]); i++) {
    int pixelHue = 0 + ((i) * 65536L / (sizeof(sections) / sizeof(sections[0])));
    sections[i].setSectionColor(strip->gamma32(strip->ColorHSV(pixelHue)));
    strip->show();
    delay(250);
  }
  delay(1000); // Wait for 200ms
  fullStrip.rainbow(5, 3, false);
  fullStrip.eraseAll(1);
}
