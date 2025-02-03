// Scene1.h
#ifndef Scene1_H
#define Scene1_H

#include <Adafruit_NeoPixel.h>
#include "LEDSection.h"

class Scene1 {
public:
  Scene1(Adafruit_NeoPixel* strip);
  void setup();
  void loop();

private:
  Adafruit_NeoPixel* strip;
  LEDSection sections[6];
  LEDSection fullStrip;
  void setSectionColor(LEDSection& section, uint32_t color);
  void rainbowWipe(LEDSection& section, int wait, bool forwards);
  void rainbowWipe(LEDSection& section, int start, int end, int wait, bool forwards);
  void rainbow(LEDSection& section, int wait, int cycles, bool forwards);
  void eraseAll(int wait);
};

#endif
