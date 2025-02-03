// Scene1.cpp
#include "Scene1.h"
#include "LED.h"
#include "LEDSection.h"

const int PIXEL_COUNT = 300;    // Total number of LEDs

Scene1::Scene1(Adafruit_NeoPixel* strip) :
  strip(strip),
  sections {
    LEDSection(strip, 0, 49),
    LEDSection(strip, 50, 99),
    LEDSection(strip, 100, 149),
    LEDSection(strip, 150, 199),
    LEDSection(strip, 200, 249),
    LEDSection(strip, 250, 299)
  },
  fullStrip(strip, 0, 299)
{
}

void Scene1::setup() {
  eraseAll(0);
}

void Scene1::loop() {
  for (int i = 0; i < sizeof(sections) / sizeof(sections[0]); i++) {
    rainbowWipe(sections[i], 1, true);
  }
  for (int i = 0; i < sizeof(sections) / sizeof(sections[0]); i++) {
    int pixelHue = 0 + ((i) * 65536L / (sizeof(sections) / sizeof(sections[0])));
    setSectionColor(sections[i], strip->gamma32(strip->ColorHSV(pixelHue)));
    strip->show();
    delay(250);
  }
  delay(1000); // Wait for 200ms
  rainbow(fullStrip, 5, 3, false);
  eraseAll(1);
}

void Scene1::setSectionColor(LEDSection& section, uint32_t color) {
  for (int i = 0; i < section.numPixels; i++) {
    section[i]->setPixelColor(color);
  }
}


void Scene1::rainbowWipe(LEDSection& section, int wait, bool forwards) {
  rainbowWipe(section, 0, section.numPixels, wait, forwards);
}

void Scene1::rainbowWipe(LEDSection& section, int start, int end, int wait, bool forwards) {
  if(forwards) {
    for(int i=start; i<end; i++) { // For each pixel in strip...
      int pixelHue = 0 + ((end - i - start) * 65536L / (end - start));
      section[i]->setPixelColor(strip->gamma32(strip->ColorHSV(pixelHue)));
      strip->show(); // Update strip with new contents
      delay(10);
    }
  } else {
    for(int i=end; i>start; i--) { // For each pixel in strip...
      int pixelHue = 0 + ((end - i - start) * 65536L / (end - start));
      section[i]->setPixelColor(strip->gamma32(strip->ColorHSV(pixelHue)));
      strip->show(); // Update strip with new contents
      delay(wait);
    }
  }
}

void Scene1::rainbow(LEDSection& section, int wait, int cycles, bool forwards) {
  for(long firstPixelHue = 0; firstPixelHue < cycles*65536; firstPixelHue += 256) {
    if(forwards) {
      for(int i=0; i<section.numPixels; i++) { // For each pixel in strip...
        int pixelHue = firstPixelHue + (i * 65536L / section.numPixels);
        section[i]->setPixelColor(strip->gamma32(strip->ColorHSV(pixelHue)));
      }
    } else {
      for(int i=section.numPixels - 1; i>=0; i--) {
        int pixelHue = firstPixelHue + ((section.numPixels - i) * 65536L / section.numPixels);
        section[i]->setPixelColor(strip->gamma32(strip->ColorHSV(pixelHue)));
      }
    }
    strip->show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

void Scene1::eraseAll(int wait) {
  for(int i = 0; i < PIXEL_COUNT; i++) {
    strip->setPixelColor(i, strip->Color(0, 0, 0));
    if(wait > 0) {
      strip->show();
      delay(wait);
    }
  }
}
