#include <FastLED.h>

#define LED_PIN 16
#define NUM_LEDS 192
#define BASE_COLOR CRGB(255, 105, 180)  // Base pink
#define MAX_BLOBS 10

CRGB leds[NUM_LEDS];

struct Blob {
    int position;
    int size;
    int speed;  // Using integer speed for reliability
    bool stuck;
};

Blob blobs[MAX_BLOBS];
int blobCount = 0;

void setup() {
    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    Serial.begin(115200);  // Debugging
    FastLED.clear();
}

void loop() {
    Serial.println("Loop start");

    // Randomly add new blobs
    if (blobCount < MAX_BLOBS && random(100) < 5) {  // 5% chance per loop
        blobs[blobCount] = {0, random(3, 8), random(1, 3), false};  // Speed is now int
        Serial.print("New blob added at position 0 with size ");
        Serial.println(blobs[blobCount].size);
        blobCount++;
    }

    // Move blobs
    for (int i = 0; i < blobCount; i++) {
        if (!blobs[i].stuck) {
            blobs[i].position += blobs[i].speed;
            if (blobs[i].position >= NUM_LEDS - blobs[i].size) {
                // Instead of getting stuck, remove the blob
                Serial.print("Blob drained out at position ");
                Serial.println(blobs[i].position);
                
                // Remove blob safely by shifting array elements
                for (int j = i; j < blobCount - 1; j++) {
                    blobs[j] = blobs[j + 1];
                }
                blobCount--;
                i--;  // Adjust loop index after removal
            }
        }
    }

    // Check for merging (iterate backward to avoid skipping blobs)
    for (int i = blobCount - 1; i > 0; i--) {
        if (blobs[i - 1].stuck && blobs[i].position >= blobs[i - 1].position - blobs[i].size) {
            blobs[i - 1].size = blobs[i - 1].size * 0.75 + blobs[i].size * 0.75;
            blobs[i - 1].stuck = false; // Unstick merged blob
            Serial.print("Blobs merged at position ");
            Serial.println(blobs[i - 1].position);
            
            // Remove merged blob safely
            for (int j = i; j < blobCount - 1; j++) {
                blobs[j] = blobs[j + 1];
            }
            blobCount--;
        }
    }

    // Render blobs
    FastLED.clear();
    for (int i = 0; i < blobCount; i++) {
        // Adjust color based on size (bigger blobs are slightly darker)
        int brightness = map(blobs[i].size, 3, 15, 255, 180);  // Scale brightness from 255 (small) to 180 (large)
        CRGB blobColor = CRGB(BASE_COLOR.r * brightness / 255, BASE_COLOR.g * brightness / 255, BASE_COLOR.b * brightness / 255);
        
        for (int j = 0; j < blobs[i].size; j++) {
            int pos = blobs[i].position + j;
            if (pos < NUM_LEDS) leds[pos] = blobColor;
        }
    }
    FastLED.show();
    
    Serial.print("Active blobs: ");
    Serial.println(blobCount);

    delay(100);
}