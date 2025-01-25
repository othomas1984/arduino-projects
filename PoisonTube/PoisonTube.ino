#include <Adafruit_NeoPixel.h>

#define PIN 6          // Pin for the LED strip
#define NUM_LEDS 300   // Number of LEDs in the strip (increased to 120)
#define LED_TYPE NEO_GRB + NEO_KHZ800
Adafruit_NeoPixel strip(NUM_LEDS, PIN, LED_TYPE);

#define maxBlobs 15     // Max number of blobs allowed at once

// Parameters for the liquid motion
int blobMinSize = 3;        // Minimum blob size (30% smaller)
int blobMaxSize = 10;        // Maximum blob size (30% smaller)
int maxGapSize = 12;        // Maximum gap between blobs
int minGapSize = 6;         // Minimum gap size between blobs

int speed = 1;              // Movement speed of the blobs
int currentPosition = 0;    // Current position of the liquid
int moveInterval = 150;     // Time between moves (slower movement)

unsigned long lastMoveTime = 0;

// Hot/neon pink color (cartoon poison)
uint32_t liquidColor = strip.Color(255, 0, 255); // Neon pink (Red: 255, Green: 0, Blue: 255)

struct Blob {
  int position;
  int size;
  unsigned long lastMoveTime;
};

Blob blobs[maxBlobs];  // Array to store up to 3 blobs

void setup() {
  strip.begin();
  strip.setBrightness(25);  // Set the brightness to 25
  strip.show();  // Initialize all pixels to off
  randomSeed(analogRead(0));  // Seed the random number generator
}

void loop() {
  unsigned long currentMillis = millis();

  // Check if it's time to move the blobs
  if (currentMillis - lastMoveTime >= moveInterval) {
    lastMoveTime = currentMillis;

    // Move the blobs and create new blobs if needed
    moveLiquidBlobs();

    // Update the strip
    strip.show();
  }
}

// Function to move the blobs
void moveLiquidBlobs() {
  // Try to move existing blobs
  for (int i = 0; i < maxBlobs; i++) {
    if (blobs[i].size > 0) {
      moveBlob(i);
    }
  }

  // If there's space for a new blob, create one
  for (int i = 0; i < maxBlobs; i++) {
    if (blobs[i].size == 0) {
      createBlob(i);
      break;
    }
  }
}

// Function to move a single blob
void moveBlob(int blobIndex) {
  Blob &blob = blobs[blobIndex];

  // Clear the old blob from the strip
  fadeOutBlob(blob);

  // Move the blob's position
  blob.position = (blob.position + speed) % NUM_LEDS;

  // Display the blob with fade-in effect at the new position
  fadeInBlob(blob);

  // Leave droplets behind if needed
  leaveDroplets(blob);
}

// Function to create a new blob
void createBlob(int blobIndex) {
  Blob &blob = blobs[blobIndex];

  // Determine the size of the new blob (random length within defined range)
  blob.size = random(blobMinSize, blobMaxSize + 1);

  // Determine the starting position for the new blob (random position)
  blob.position = random(NUM_LEDS);

  // Set the speed for the new blob (randomized)
  speed = random(1, 3);

  // Create the new blob with fade-in effect
  fadeInBlob(blob);
}

// Function to fade in a blob
void fadeInBlob(Blob &blob) {
  int fadeSteps = 10;  // Number of steps to fade in
  for (int i = 0; i < blob.size; i++) {
    int pos = (blob.position + i) % NUM_LEDS;
    uint32_t color = fadeToColor(i, blob.size, liquidColor, fadeSteps);
    strip.setPixelColor(pos, color);
  }
}

// Function to fade out a blob
void fadeOutBlob(Blob &blob) {
  int fadeSteps = 10;  // Number of steps to fade out
  for (int i = 0; i < blob.size; i++) {
    int pos = (blob.position + i) % NUM_LEDS;
    uint32_t color = fadeToColor(i, blob.size, liquidColor, fadeSteps);
    strip.setPixelColor(pos, color);
  }
}

// Function to calculate a fading color for the blob
uint32_t fadeToColor(int step, int totalSteps, uint32_t color, int fadeSteps) {
  float fadeFactor = float(step) / float(totalSteps);
  int r = (fadeFactor * ((color >> 16) & 0xFF));
  int g = (fadeFactor * ((color >>  8) & 0xFF));
  int b = (fadeFactor * (color & 0xFF));
  return strip.Color(r, g, b);
}

// Function to simulate droplets left behind by the blob
void leaveDroplets(Blob &blob) {
  int dropChance = random(0, 5); // Small chance of leaving droplets
  
  // Randomly leave small droplets after the blob
  for (int i = blob.position + blob.size; i < blob.position + blob.size + dropChance && i < NUM_LEDS; i++) {
    if (strip.getPixelColor(i) == 0) {  // If there's no current color (empty space)
      strip.setPixelColor(i, liquidColor); // Leave a tiny droplet behind
    }
  }
}
