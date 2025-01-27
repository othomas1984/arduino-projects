// LEDSection.cpp
#include "LEDSection.h"

LEDSection::LEDSection(Adafruit_NeoPixel* strip, int pixelStart, int pixelEnd) :
  strip(strip), 
  numPixels(pixelEnd - pixelStart + 1)
{
  ledArray = new LED*[numPixels];
  for (int i = 0; i < numPixels; i++) {
      ledArray[i] = new LED(strip, i + pixelStart);  // Properly initialize each LED
  }
  // Serial.println(numPixels);
}

LEDSection::LEDSection(Adafruit_NeoPixel* strip, int numPixels, int* pixels) :
  strip(strip), 
  numPixels(numPixels)
{
  ledArray = new LED*[numPixels];
  for (int i = 0; i < numPixels; i++) {
      ledArray[i] = new LED(strip, pixels[i]);  // Properly initialize each LED
  }
}

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

