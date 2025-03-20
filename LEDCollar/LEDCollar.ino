#include <FastLED.h>

#define LED_PIN 16
#define NUM_LEDS 96  // Keep the full strip defined
#define DISPLAY_LEDS 48  // Each tank uses 48 LEDs
// #define BASE_COLOR CRGB(200, 0, 250)  // Base pink
// #define BLUE_COLOR CRGB(200, 0, 250)  // Deep cobalt blue
#define BASE_COLOR CRGB(0, 250, 250)  // Base pink
#define BLUE_COLOR CRGB(0, 0, 250)  // Deep cobalt blue

CRGB leds[NUM_LEDS];

struct Wave {
    float position;
    float velocity;
    float force;
};

Wave wavePink1 = {DISPLAY_LEDS / 4, 0, 1.0};
Wave wavePink2 = {DISPLAY_LEDS * 3 / 4, 0, 1.0};
Wave waveBlue1 = {DISPLAY_LEDS / 4, 0, 1.0};
Wave waveBlue2 = {DISPLAY_LEDS * 3 / 4, 0, 1.0};
float fillLevel = 0.85;

void setup() {
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    Serial.begin(115200);
    FastLED.clear();
}

void loop() {
    FastLED.clear();
        int brightness = 80;

    // Render the pink tank (bottom section)
    for (int i = 0; i < 48; i++) {
        leds[i] = CRGB(BASE_COLOR.r * brightness / 255, BASE_COLOR.g * brightness / 255, BASE_COLOR.b * brightness / 255);
    }

    // Mirror the pink tank for the top section (with fade-out effect)
    for (int i = 48; i < 96; i++) {
        leds[i] = CRGB(BLUE_COLOR.r * brightness / 255, BLUE_COLOR.g * brightness / 255, BLUE_COLOR.b * brightness / 255);
    }

    FastLED.show();
    delay(100);
}
