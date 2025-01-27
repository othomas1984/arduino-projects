// LEDSection.h
#ifndef LEDSECTION_H
#define LEDSECTION_H

#include <Adafruit_NeoPixel.h>

class LED {
public:
  LED(Adafruit_NeoPixel* strip, int pixelNum);
  int pixelNum;
  void setPixelColor(uint32_t color);
  void setPixelColor(int red, int green, int blue);

private:
  Adafruit_NeoPixel* strip;
};

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
