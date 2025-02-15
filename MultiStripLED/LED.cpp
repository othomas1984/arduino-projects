// LEDSection.cpp
#include "LED.h"
#include "LEDSection.h"

LED::LED(Adafruit_NeoPixel* strip, int pixelNum) :
  strip(strip), 
  pixelNum(pixelNum),
  color(strip->Color(0, 0, 0))
{
    // Serial.println(pixelNum);
}

void LED::setPixelColor(uint32_t newColor) {
  // Serial.println(pixelNum);
  color = newColor;
  strip->setPixelColor(pixelNum, newColor);
}

void LED::setPixelColor(int red, int green, int blue) {
  setPixelColor(strip->Color(red, green, blue));
}

