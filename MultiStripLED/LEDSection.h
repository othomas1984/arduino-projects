// LEDSection.h
#ifndef LEDSECTION_H
#define LEDSECTION_H

#include <Adafruit_NeoPixel.h>
#include "LED.h"

class LEDSection {
public:
  LEDSection(Adafruit_NeoPixel* strip, int pixelStart, int pixelEnd);
  LEDSection(Adafruit_NeoPixel* strip, int numPixels, int* pixels);
  const LED* operator[](int index) const; // Overloaded subscript operator for const objects
  int numPixels;

private:
  Adafruit_NeoPixel* strip;
  LED** ledArray;
};


#endif
