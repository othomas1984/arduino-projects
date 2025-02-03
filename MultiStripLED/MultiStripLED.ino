#include <Adafruit_NeoPixel.h>
#include "LEDSection.h"
#include "Scene1.h"

// Define constants
const int PIXEL_COUNT = 300;    // Total number of LEDs
const int PIXEL_PIN = 6;       // Pin connected to the NeoPixel strip

// Define the lengths of each section

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
Scene1* scene1;

void setup() {
  strip.begin();   // Initialize section 1
  strip.setBrightness(30);
  scene1 = new Scene1(&strip);
  scene1->setup();
  strip.setPixelColor(15, 50, 50, 50);
}

void loop() {
  scene1->loop();
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

// void rainbowWipe(Adafruit_NeoPixel &strip, int start, int end, int wait, bool forwards) {
//   // Hue of first pixel runs 3 complete loops through the color wheel.
//   // Color wheel has a range of 65536 but it's OK if we roll over, so
//   // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time
//   // means we'll make 3*65536/256 = 768 passes through this outer loop:
//     if(forwards) {
//       for(int i=start; i<end; i++) { // For each pixel in strip...
//         int pixelHue = 0 + ((end - i - start) * 65536L / (end - start));
//         strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
//         strip.show(); // Update strip with new contents
//         delay(wait);
//       }
//     } else {
//         for(int i=end; i>start; i--) { // For each pixel in strip...
//         int pixelHue = 0 + ((end - i - start) * 65536L / (end - start));
//         strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
//         strip.show(); // Update strip with new contents
//         delay(wait);
//       }
//     }
//     // strip.show(); // Update strip with new contents
//     // delay(wait);  // Pause for a moment
// }
// void rainbowWipe(LEDSection section, int wait, bool forwards) {
//   rainbowWipe(section, 0, section.numPixels, wait, forwards);
// }

// void rainbowWipe(LEDSection section, int start, int end, int wait, bool forwards) {
//   // Hue of first pixel runs 3 complete loops through the color wheel.
//   // Color wheel has a range of 65536 but it's OK if we roll over, so
//   // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time
//   // means we'll make 3*65536/256 = 768 passes through this outer loop:
//     if(forwards) {
//       for(int i=start; i<end; i++) { // For each pixel in strip...
//         int pixelHue = 0 + ((end - i - start) * 65536L / (end - start));
//         section[i]->setPixelColor(strip.gamma32(strip.ColorHSV(pixelHue)));
//         strip.show(); // Update strip with new contents
//         delay(wait);
//       }
//     } else {
//         for(int i=end; i>start; i--) { // For each pixel in strip...
//         int pixelHue = 0 + ((end - i - start) * 65536L / (end - start));
//         section[i]->setPixelColor(strip.gamma32(strip.ColorHSV(pixelHue)));
//         strip.show(); // Update strip with new contents
//         delay(wait);
//       }
//     }
//     // strip.show(); // Update strip with new contents
//     // delay(wait);  // Pause for a moment
// }

// void rainbow(LEDSection section, int wait, int cycles, bool forwards) {
//   // Hue of first pixel runs 3 complete loops through the color wheel.
//   // Color wheel has a range of 65536 but it's OK if we roll over, so
//   // just count from 0 to 3*65536. Adding 256 to firstPixelHue each time
//   // means we'll make 3*65536/256 = 768 passes through this outer loop:
//   for(long firstPixelHue = 0; firstPixelHue < cycles*65536; firstPixelHue += 256) {
//     Serial.println(firstPixelHue);
//     if(forwards) {
//       for(int i=0; i<section.numPixels; i++) { // For each pixel in strip...
//         // Offset pixel hue by an amount to make one full revolution of the
//         // color wheel (range of 65536) along the length of the strip
//         // (strip.numPixels() steps):
//         int pixelHue = firstPixelHue + (i * 65536L / section.numPixels);
//         // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
//         // optionally add saturation and value (brightness) (each 0 to 255).
//         // Here we're using just the single-argument hue variant. The result
//         // is passed through strip.gamma32() to provide 'truer' colors
//         // before assigning to each pixel:
//         section[i]->setPixelColor(strip.gamma32(strip.ColorHSV(pixelHue)));
//       }
//     } else {
//       for(int i=section.numPixels - 1; i>=0; i--) {
//         int pixelHue = firstPixelHue + ((section.numPixels - i) * 65536L / section.numPixels);
//         section[i]->setPixelColor(strip.gamma32(strip.ColorHSV(pixelHue)));
//       }
//     }
//     strip.show(); // Update strip with new contents
//     delay(wait);  // Pause for a moment
//   }
// }
// // // Function to set a specific section's color
// // void setSectionColor(Adafruit_NeoPixel &strip, int start, int end, uint32_t color) {
// //   for (int i = start; i <= end; i++) {
// //     strip.setPixelColor(i, color);
// //   }
// // }

// void eraseAll(int wait) {
//   for(int i = 0; i < PIXEL_COUNT; i++) {
//     strip.setPixelColor(i, strip.Color(0, 0, 0));
//     if(wait > 0) {
//       strip.show();
//       delay(wait);
//     }
//   }
// }
