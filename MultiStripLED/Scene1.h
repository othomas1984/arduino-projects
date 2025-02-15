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
};

#endif
