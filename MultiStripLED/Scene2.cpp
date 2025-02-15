// Scene2.cpp
#include "Scene2.h"
#include "LEDSection.h"

Scene2::Scene2(Adafruit_NeoPixel* strip) :
  strip(strip),
  sections {
    LEDSection(strip, 0, 25),
    LEDSection(strip, 30, 55),
    LEDSection(strip, 60, 85),
    LEDSection(strip, 90, 115),
    LEDSection(strip, 120, 145),
  },
  fullStrip(strip, 0, 149)
{
}

void Scene2::setup() {
  fullStrip.eraseAll(0);
}

void Scene2::loop() {
  smoothTemperatureTransition(2000, 100000, 5, 1000);  // Adjust startTemp, endTemp, steps, delay as needed
  fullStrip.eraseAll(10);
}

// Function to smoothly transition temperature
void Scene2::smoothTemperatureTransition(int startTemp, int endTemp, int steps, int delayTime) {
    for (int i = 0; i < steps; i++) {
        int temp = startTemp + ((endTemp - startTemp) * i) / (steps - 1);
        int r, g, b;
        sections[i].kelvinToRGB(temp, r, g, b);

        sections[i].setSectionColor(strip->Color(r, g, b));
        // Apply color to the LED strip
        // fullStrip[i]->setPixelColor(strip->Color(r, g, b));
        strip->show();

        // Debugging output
        Serial.print("Step "); Serial.print(i + 1);
        Serial.print("/"); Serial.print(steps);
        Serial.print(": Temp "); Serial.print(temp);
        Serial.print("K -> RGB("); Serial.print(r);
        Serial.print(", "); Serial.print(g);
        Serial.print(", "); Serial.print(b);
        Serial.println(")");

        delay(delayTime);
    }
}
