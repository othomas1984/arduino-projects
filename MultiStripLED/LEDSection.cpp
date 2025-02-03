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
