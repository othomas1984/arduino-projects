// LEDSection.h
#ifndef LEDSECTION_H
#define LEDSECTION_H

#include <Adafruit_NeoPixel.h>
#include "LED.h"

class LEDSection {
public:
  LEDSection(Adafruit_NeoPixel* strip, int pixelStart, int pixelEnd);
  LEDSection(Adafruit_NeoPixel* strip, int numPixels, int* pixels);
  LED* operator[](int index) const; // Overloaded subscript operator for const objects
  int numPixels;

  void setSectionColor(uint32_t color);
  void rainbowWipe(int wait, bool forwards);
  void rainbowWipe(int start, int end, int wait, bool forwards);
  void rainbow(int wait, int cycles, bool forwards);
  void eraseAll(int wait);

  void kelvinToRGB(int temperature, int &red, int &green, int &blue);
private:
  Adafruit_NeoPixel* strip;
  LED** ledArray;
};


#endif
