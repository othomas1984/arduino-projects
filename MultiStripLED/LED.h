// LEDSection.h
#ifndef LED_H
#define LED_H

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

#endif
