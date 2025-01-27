#include <Adafruit_NeoPixel.h>
#include "LEDSection.h"

// Define constants
const int PIXEL_COUNT = 300;    // Total number of LEDs
const int PIXEL_PIN = 6;       // Pin connected to the NeoPixel strip

// Define the lengths of each section

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);


int pixels1[] = {60, 63, 67, 66, 62, 64, 65, 69, 68, 61};  // Example: controlling pixels 2, 4, 6, 8, and 10
LEDSection section6(&strip, 10, pixels1);
int pixels2[] = {80, 81, 82, 83, 84, 85, 86, 87, 89, 88};  // Example: controlling pixels 2, 4, 6, 8, and 10
LEDSection section8(&strip, 10, pixels2);
LEDSection section10(&strip, 0, 299);
LEDSection sections[6] = {
    LEDSection(&strip, 0, 49),
    LEDSection(&strip, 50, 99),
    LEDSection(&strip, 100, 149),
    LEDSection(&strip, 150, 199),
    LEDSection(&strip, 200, 249),
    LEDSection(&strip, 250, 299)

    // section6,
    // section8,
    // LEDSection(&strip, 70, 79)
};
// Create multiple instances for each section

void setup() {

    Serial.begin(9600);
    strip.begin();   // Initialize section 1
    // strip.clear();   // Initialize section 3

    strip.setBrightness(3);
    strip.show();    // Turn off all LEDs in section 3
}

void eraseAll(int wait) {
  for(int i = 0; i < PIXEL_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
    if(wait > 0) {
      strip.show();
      delay(wait);
    }
  }
}

void loop() {
  Serial.println("Start Loop");
  // eraseAll(0);
// return;
  // for (int i = 0; i < NUM_STRIPS - 4; i++) {
  //   rainbowWipe(strip, 0 + PIXELS_PER_STRIP * i, PIXELS_PER_STRIP + (PIXELS_PER_STRIP * i), 10, true);
  // }
  for (int i = 0; i < sizeof(sections) / sizeof(sections[0]); i++) {
    rainbowWipe(sections[i], 1, true);
  }
  // rainbowWipe(section1, 0, sizeof(pixels2) / sizeof(pixels2[0]), 100, true);
  // rainbowWipe(section2, 0, sizeof(pixels2) / sizeof(pixels2[0]), 100, true);
  // strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.show();
  // strips[1].show();
  // Show the changes
    // strip.show();

  delay(200); // Wait for 1 second
  // for (int i = 0; i < sizeof(sections) / sizeof(sections[0]); i++) {
  //   // rainbowWipe(sections[i], 1, true);
  //   rainbow(sections[i], 10, 3, false);
  // }
  rainbow(section10, 5, 3, false);
  eraseAll(1);
  Serial.println("End Loop");

  // for (int i = NUM_STRIPS - 1; i >= 0; i--) {
  //   rainbowWipe(strip, 0 + PIXELS_PER_STRIP * i, PIXELS_PER_STRIP + (PIXELS_PER_STRIP * i), 10, false);
  //   // setSectionColor(strip, 0 + PIXELS_PER_STRIP * i, 20 + PIXELS_PER_STRIP * i, strip.Color(255, 0, 0));
  // }
  // // strips[1].show();
  // // Show the changes
  //   // strip.show();

  // delay(1000); // Wait for 1 second
// return;
  // for (int i = 0; i < NUM_STRIPS; i++) {
  //   setSectionColor(strips[i], 0, 29, strips[i].Color(255, 0, 0));
  //   // for (int j = 0; j <= 30; j++) {
  //   //   strips[i].setPixelColor(j, 255, 0, 0);
  //   // }
  //   strips[i].show();
  //   Serial.println(i);
  // }
  // Serial.println(6);

  // setSectionColor(strips[0], 0, SECTION_1_COUNT - 1, strips[0].Color(255, 255, 0));

  // Example: Set section 2 to green
  // setSectionColor(strip2, 0, SECTION_2_COUNT - 1, strip2.Color(0, 255, 0));

  // // Example: Set section 3 to blue
  // setSectionColor(strip3, 0, SECTION_3_COUNT - 1, strip3.Color(0, 0, 255));

  // // Example: Set section 4 to Yellow
  // setSectionColor(strip4, 0, SECTION_3_COUNT - 1, strip3.Color(255, 255, 0));

  // Show the updates
  // strips[0].show();
  // strip2.show();
  // strip3.show();
  // strip4.show();

  // delay(1000); // Pause for 1 second
}

// void setupSections() {
//   section1.begin();
//   section2.begin();
//   section3.begin();
// }

// void loopSections() {
//   section1.setColor(section1.Color(255, 0, 0)); // Red
//   section2.setColor(section2.Color(0, 255, 0)); // Green
//   section3.setColor(section3.Color(0, 0, 255)); // Blue

//   section1.show();
//   section2.show();
//   section3.show();

//   delay(1000);
// }

// #include <Adafruit_NeoPixel.h>


// void setupStrips() {
//  // Turn off all LEDs in section 3
// }

// void setSectionColor(Adafruit_NeoPixel &strip, int start, int end, uint32_t color) {
//   for (int i = start; i <= end; i++) {
//     strip.setPixelColor(i, color);
//   }
// }

// void loopStrips() {
//   // setSectionColor(strip1, 0, SECTION_1_COUNT - 1, strip1.Color(255, 0, 0));
//   // strip1.show();

//   // strip1.setPixelColor(0, strip1.Color(255, 255, 0));
//   // strip2.setPixelColor(0, strip1.Color(255, 255, 0));
//   // strip3.setPixelColor(0, strip1.Color(255, 255, 0));
//   // // }
//   // // strips[i]->setBrightness(20);
//   // // strips[i]->show(); // Turn off all LEDs initially
//   // // strip1.setBrightness(20);
//   // strip1.show(); // Turn off all LEDs initially
//   // strip2.show(); // Turn off all LEDs initially
//   // strip3.show(); // Turn off all LEDs initially
//   // delay(1000); // Pause for 1 second

//   // return;
//   // // for (int i = 0; i < NUM_STRIPS; i++) {
//   // //   rainbowWipe(strips[i], 1000, true);
//   // // }
//   // Serial.println("StartLoop");
//   // // strips[0]->setPixelColor(0, 255, 0, 0);
//   // delay(1000);
//   // Serial.println("EndLoop");
//       // rainbowWipe(strips[0], 5, true);

//   // // Example: Set section 0 to black
//   // setSectionColor(strip0, 0, SECTION_1_COUNT - 1, strip1.Color(0, 0, 0));

//   // // Example: Set section 1 to red
//   // setSectionColor(strip1, 0, SECTION_1_COUNT - 1, strip1.Color(255, 0, 0));

//   // // Example: Set section 2 to green
//   // setSectionColor(strip2, 0, SECTION_2_COUNT - 1, strip2.Color(0, 255, 0));

//   // // Example: Set section 3 to blue
//   // setSectionColor(strip3, 0, SECTION_3_COUNT - 1, strip3.Color(0, 0, 255));

//   // // Show the updates
//   // strip0.show();
//   // strip1.show();
//   // strip2.show();
//   // strip3.show();

//   // delay(1000); // Pause for 1 second
// }

void rainbowWipe(Adafruit_NeoPixel &strip, int start, int end, int wait, bool forwards) {
  // Hue of first pixel runs 3 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time
  // means we'll make 3*65536/256 = 768 passes through this outer loop:
    if(forwards) {
      for(int i=start; i<end; i++) { // For each pixel in strip...
        int pixelHue = 0 + ((end - i - start) * 65536L / (end - start));
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
        strip.show(); // Update strip with new contents
        delay(wait);
      }
    } else {
        for(int i=end; i>start; i--) { // For each pixel in strip...
        int pixelHue = 0 + ((end - i - start) * 65536L / (end - start));
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
        strip.show(); // Update strip with new contents
        delay(wait);
      }
    }
    // strip.show(); // Update strip with new contents
    // delay(wait);  // Pause for a moment
}
void rainbowWipe(LEDSection section, int wait, bool forwards) {
  rainbowWipe(section, 0, section.numPixels, wait, forwards);
}

void rainbowWipe(LEDSection section, int start, int end, int wait, bool forwards) {
  // Hue of first pixel runs 3 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time
  // means we'll make 3*65536/256 = 768 passes through this outer loop:
    if(forwards) {
      for(int i=start; i<end; i++) { // For each pixel in strip...
        int pixelHue = 0 + ((end - i - start) * 65536L / (end - start));
        section[i]->setPixelColor(strip.gamma32(strip.ColorHSV(pixelHue)));
        strip.show(); // Update strip with new contents
        delay(wait);
      }
    } else {
        for(int i=end; i>start; i--) { // For each pixel in strip...
        int pixelHue = 0 + ((end - i - start) * 65536L / (end - start));
        section[i]->setPixelColor(strip.gamma32(strip.ColorHSV(pixelHue)));
        strip.show(); // Update strip with new contents
        delay(wait);
      }
    }
    // strip.show(); // Update strip with new contents
    // delay(wait);  // Pause for a moment
}

void rainbow(LEDSection section, int wait, int cycles, bool forwards) {
  // Hue of first pixel runs 3 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time
  // means we'll make 3*65536/256 = 768 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < cycles*65536; firstPixelHue += 256) {
    Serial.println(firstPixelHue);
    if(forwards) {
      for(int i=0; i<section.numPixels; i++) { // For each pixel in strip...
        // Offset pixel hue by an amount to make one full revolution of the
        // color wheel (range of 65536) along the length of the strip
        // (strip.numPixels() steps):
        int pixelHue = firstPixelHue + (i * 65536L / section.numPixels);
        // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
        // optionally add saturation and value (brightness) (each 0 to 255).
        // Here we're using just the single-argument hue variant. The result
        // is passed through strip.gamma32() to provide 'truer' colors
        // before assigning to each pixel:
        section[i]->setPixelColor(strip.gamma32(strip.ColorHSV(pixelHue)));
      }
    } else {
      for(int i=section.numPixels - 1; i>=0; i--) {
        int pixelHue = firstPixelHue + ((section.numPixels - i) * 65536L / section.numPixels);
        section[i]->setPixelColor(strip.gamma32(strip.ColorHSV(pixelHue)));
      }
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}
// // Function to set a specific section's color
// void setSectionColor(Adafruit_NeoPixel &strip, int start, int end, uint32_t color) {
//   for (int i = start; i <= end; i++) {
//     strip.setPixelColor(i, color);
//   }
// }
