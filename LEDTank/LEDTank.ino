#include <FastLED.h>

#define LED_PIN 16
#define NUM_LEDS 192  // Keep the full strip defined
#define DISPLAY_LEDS 48  // Each tank uses 48 LEDs
#define BASE_COLOR CRGB(255, 0, 255)  // Base pink
#define BLUE_COLOR CRGB(0, 0, 139)  // Deep cobalt blue

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

void updateWave(Wave &wave, float sharedAcceleration) {
    wave.velocity += sharedAcceleration * wave.force;
    wave.velocity *= 0.9;
    wave.position += wave.velocity;

    if (wave.position < 0) {
        wave.position = 0;
        wave.velocity *= -0.7;
    }
    if (wave.position > DISPLAY_LEDS - 1) {
        wave.position = DISPLAY_LEDS - 1;
        wave.velocity *= -0.7;
    }
}

void mergeOrCrossWaves(Wave &wave1, Wave &wave2) {
    if (abs(wave1.position - wave2.position) < 2) {
        if (wave1.force > wave2.force * 1.5) {
            wave1.force += wave2.force * 0.5;
            wave2.position = random(0, DISPLAY_LEDS);
            wave2.velocity = random(-2, 2);
        } else if (wave2.force > wave1.force * 1.5) {
            wave2.force += wave1.force * 0.5;
            wave1.position = random(0, DISPLAY_LEDS);
            wave1.velocity = random(-2, 2);
        } else {
            float tempVel = wave1.velocity;
            wave1.velocity = wave2.velocity * 0.9;
            wave2.velocity = tempVel * 0.9;
        }
    }
}

void loop() {
    FastLED.clear();
    float acceleration = (random(-30, 31) / 20.0);
    
    updateWave(wavePink1, acceleration);
    updateWave(wavePink2, acceleration);
    updateWave(waveBlue1, acceleration);
    updateWave(waveBlue2, acceleration);

    mergeOrCrossWaves(wavePink1, wavePink2);
    mergeOrCrossWaves(waveBlue1, waveBlue2);

    // Render the pink tank (bottom section)
    for (int i = 0; i < DISPLAY_LEDS; i++) {
        float distance1 = abs(i - wavePink1.position);
        float distance2 = abs(i - wavePink2.position);
        float brightnessFactor = max(exp(-pow(distance1 / 8.0, 2)), exp(-pow(distance2 / 8.0, 2)));
        int brightness = map(brightnessFactor * 255 * fillLevel, 0, 255, 50, 220);
        leds[i] = CRGB(BASE_COLOR.r * brightness / 255, BASE_COLOR.g * brightness / 255, BASE_COLOR.b * brightness / 255);
    }

    // Mirror the pink tank for the top section (with fade-out effect)
    for (int i = 0; i < DISPLAY_LEDS; i++) {
        int mirroredIndex = NUM_LEDS - 1 - i;
        float distance1 = abs(i - wavePink1.position);
        float distance2 = abs(i - wavePink2.position);
        float topBrightnessFactor = max(exp(-pow(distance1 / 6.0, 2)), exp(-pow(distance2 / 6.0, 2)));
        int topBrightness = map(topBrightnessFactor * 255 * fillLevel, 0, 255, 0, 255);
        leds[mirroredIndex] = CRGB(BASE_COLOR.r * topBrightness / 255, BASE_COLOR.g * topBrightness / 255, BASE_COLOR.b * topBrightness / 255);
    }

    // Render the blue tank (bottom section)
    for (int i = 0; i < DISPLAY_LEDS; i++) {
        int blueIndex = DISPLAY_LEDS + i;
        float distance1 = abs(i - waveBlue1.position);
        float distance2 = abs(i - waveBlue2.position);
        float brightnessFactor = max(exp(-pow(distance1 / 8.0, 2)), exp(-pow(distance2 / 8.0, 2)));
        int brightness = map(brightnessFactor * 255 * fillLevel, 0, 255, 30, 180);
        leds[blueIndex] = CRGB(BLUE_COLOR.r * brightness / 255, BLUE_COLOR.g * brightness / 255, BLUE_COLOR.b * brightness / 255);
    }

    // Mirror the blue tank for the top section (with fade-out effect)
    for (int i = 0; i < DISPLAY_LEDS; i++) {
        int mirroredIndex = (DISPLAY_LEDS * 2) + (DISPLAY_LEDS - 1 - i);
        float distance1 = abs(i - waveBlue1.position);
        float distance2 = abs(i - waveBlue2.position);
        float topBrightnessFactor = max(exp(-pow(distance1 / 6.0, 2)), exp(-pow(distance2 / 6.0, 2)));
        int topBrightness = map(topBrightnessFactor * 255 * fillLevel, 0, 255, 0, 180);
        leds[mirroredIndex] = CRGB(BLUE_COLOR.r * topBrightness / 255, BLUE_COLOR.g * topBrightness / 255, BLUE_COLOR.b * topBrightness / 255);
    }

    FastLED.show();
    delay(100);
}
