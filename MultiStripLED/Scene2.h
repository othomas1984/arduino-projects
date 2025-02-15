// Scene2.h
#ifndef Scene2_H
#define Scene2_H

#include <Adafruit_NeoPixel.h>
#include "LEDSection.h"

class Scene2 {
public:
  Scene2(Adafruit_NeoPixel* strip);
  void setup();
  void loop();

private:
  Adafruit_NeoPixel* strip;
  LEDSection sections[5];
  LEDSection fullStrip;

  void smoothTemperatureTransition(int startTemp, int endTemp, int steps, int delayTime);
};

#endif
