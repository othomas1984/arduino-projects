#include <Adafruit_NeoPixel.h>
#include "LEDSection.h"

// Define constants
const int PIXEL_COUNT = 60;    // Total number of LEDs
const int PIXEL_PIN = 6;       // Pin connected to the NeoPixel strip

// Define the lengths of each section
const int NUM_STRIPS = 6;
const int PIXELS_PER_STRIP = PIXEL_COUNT / NUM_STRIPS;

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Create multiple instances for each section

void setup() {


    strip.begin();   // Initialize section 1
    strip.clear();   // Initialize section 3

    strip.setBrightness(3);
    strip.show();    // Turn off all LEDs in section 3
}

void eraseAll() {
  for(int i = 0; i < PIXEL_COUNT; i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
}

void loop() {
  // eraseAll();
  // return;
  for (int i = 0; i < NUM_STRIPS; i++) {
    rainbowWipe(strip, 0 + PIXELS_PER_STRIP * i, PIXELS_PER_STRIP + (PIXELS_PER_STRIP * i), 10, true);
    // setSectionColor(strip, 0 + PIXELS_PER_STRIP * i, 20 + PIXELS_PER_STRIP * i, strip.Color(255, 0, 0));
  }
  // strips[1].show();
  // Show the changes
    // strip.show();

  delay(1000); // Wait for 1 second

  // for (int i = NUM_STRIPS - 1; i >= 0; i--) {
  //   rainbowWipe(strip, 0 + PIXELS_PER_STRIP * i, PIXELS_PER_STRIP + (PIXELS_PER_STRIP * i), 10, false);
  //   // setSectionColor(strip, 0 + PIXELS_PER_STRIP * i, 20 + PIXELS_PER_STRIP * i, strip.Color(255, 0, 0));
  // }
  // // strips[1].show();
  // // Show the changes
  //   // strip.show();

  // delay(1000); // Wait for 1 second
return;
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

  delay(1000); // Pause for 1 second
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

void setSectionColor(Adafruit_NeoPixel &strip, int start, int end, uint32_t color) {
  for (int i = start; i <= end; i++) {
    strip.setPixelColor(i, color);
  }
}

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

// // Function to set a specific section's color
// void setSectionColor(Adafruit_NeoPixel &strip, int start, int end, uint32_t color) {
//   for (int i = start; i <= end; i++) {
//     strip.setPixelColor(i, color);
//   }
// }
