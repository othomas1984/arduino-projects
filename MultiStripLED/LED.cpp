// LEDSection.cpp
#include "LED.h"
#include "LEDSection.h"

LED::LED(Adafruit_NeoPixel* strip, int pixelNum) :
  strip(strip), 
  pixelNum(pixelNum)
{
    // Serial.println(pixelNum);
}

const LED* LEDSection::operator[](int index) const {
    // Serial.println(index);
    return ledArray[index];  // Return a const reference for read-only access
}

void LED::setPixelColor(uint32_t color) {
  // Serial.println(pixelNum);
  strip->setPixelColor(pixelNum, color);
}

void LED::setPixelColor(int red, int green, int blue) {
  strip->setPixelColor(pixelNum, red, green, blue);
}

