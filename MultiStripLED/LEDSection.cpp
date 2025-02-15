// LEDSection.cpp
#include "LEDSection.h"

LED* LEDSection::operator[](int index) const {
    // Serial.println(index);
    return ledArray[index];  // Return a const reference for read-only access
}

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

LEDSection::LEDSection(Adafruit_NeoPixel* strip, int numPixels, const int* pixels) :
  strip(strip), 
  numPixels(numPixels)
{
  ledArray = new LED*[numPixels];
  for (int i = 0; i < numPixels; i++) {
      ledArray[i] = new LED(strip, pixels[i]);  // Properly initialize each LED
  }
}

void LEDSection::eraseAll(int wait, uint32_t color) {
  for(int i = 0; i < numPixels; i++) {
    ledArray[i]->setPixelColor(color);
    if(wait > 0) {
      strip->show();
      delay(wait);
    }
  }
}

void LEDSection::eraseAll(int wait) {
  eraseAll(wait, strip->Color(0, 0, 0));
}

void LEDSection::setSectionColor(uint32_t color) {
  for (int i = 0; i < numPixels; i++) {
    ledArray[i]->setPixelColor(color);
  }
}

void LEDSection::rainbowWipe(int wait, bool forwards) {
  rainbowWipe(0, numPixels, wait, forwards);
}

void LEDSection::rainbowWipe(int start, int end, int wait, bool forwards) {
  if(forwards) {
    for(int i=start; i<end; i++) { // For each pixel in strip...
      int pixelHue = 0 + ((end - i - start) * 65536L / (end - start));
      ledArray[i]->setPixelColor(strip->gamma32(strip->ColorHSV(pixelHue)));
      strip->show(); // Update strip with new contents
      delay(10);
    }
  } else {
    for(int i=end; i>start; i--) { // For each pixel in strip...
      int pixelHue = 0 + ((end - i - start) * 65536L / (end - start));
      ledArray[i]->setPixelColor(strip->gamma32(strip->ColorHSV(pixelHue)));
      strip->show(); // Update strip with new contents
      delay(wait);
    }
  }
}

void LEDSection::rainbow(int wait, int cycles, bool forwards) {
  for(long offset = 0; offset < cycles * numPixels; offset += 1) {
    if(forwards) {
      for(int i=0; i<numPixels; i++) { // For each pixel in strip...
        int pixelHue = ((offset + i) * 65536 / numPixels) % 65536;
        ledArray[i]->setPixelColor(strip->gamma32(strip->ColorHSV(pixelHue)));
      }
    } else {
      for(int i=numPixels - 1; i>=0; i--) { // For each pixel in strip...
        int pixelHue = (((cycles * numPixels - offset) + i - 9) * 65536 / numPixels) % 65536;
        // int pixelHue = offset + ((numPixels - i) * 65536L / numPixels);
        ledArray[i]->setPixelColor(strip->gamma32(strip->ColorHSV(pixelHue)));
      }
    }
    strip->show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

void LEDSection::kelvinToRGB(int temperature, int &red, int &green, int &blue) {
    float temp = temperature / 100.0;

    // Red
    if (temp <= 66) {
        red = 255;
    } else {
        red = 329.69 * pow(temp - 60, -0.1332);
        red = constrain(red, 0, 255);
    }

    // Green
    if (temp <= 66) {
        green = 99.47 * log(temp) - 161.12;
    } else {
        green = 288.12 * pow(temp - 60, -0.0755);
    }
    green = constrain(green, 0, 255);

    // Blue
    if (temp >= 66) {
        blue = 255;
    } else if (temp <= 19) {
        blue = 0;
    } else {
        blue = 138.52 * log(temp - 10) - 305.04;
        blue = constrain(blue, 0, 255);
    }
}

void LEDSection::rotate(bool forwards) {
  if(forwards) {
    for(int i=0; i<numPixels; i++) { // For each pixel in strip...
      int indexToCopy = i + 1;
      if(indexToCopy == numPixels) {
        indexToCopy = 0;
      }
      uint32_t newColor = ledArray[indexToCopy]->color;
      ledArray[i]->setPixelColor(newColor);
    }
  } else {
    for(int i=numPixels-1; i>=0; i--) { // For each pixel in strip...
      int indexToCopy = i - 1;
      if(indexToCopy == -1) {
        indexToCopy = numPixels-1;
      }
      uint32_t newColor = ledArray[indexToCopy]->color;
      ledArray[i]->setPixelColor(newColor);
    }
  }
}