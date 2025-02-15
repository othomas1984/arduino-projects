// Grid.h
#ifndef Grid_H
#define Grid_H

#include <Adafruit_NeoPixel.h>
#include "LEDSection.h"

class Grid {
public:
  Grid(Adafruit_NeoPixel* strip);
  void setup();
  void loop();

private:
  Adafruit_NeoPixel* strip;
  LEDSection sections[17];
  LEDSection sections2[17];
  LEDSection fullStrip;
  LEDSection fullStrip2;

  void smoothTemperatureTransition(int startTemp, int endTemp, int steps, int delayTime);
};

#endif
