#ifndef SEGMENT_H
#define SEGMENT_H

#define MAX_LEDS_PER_SEGMENT 248

struct LEDPointer {
  CRGB* strip;
  uint16_t index;
};

class Segment {
public:
  LEDPointer leds[MAX_LEDS_PER_SEGMENT];
  uint8_t ledCount;

  Segment() : ledCount(0) {}

  void addLED(CRGB* strip, uint16_t index) {
    if (ledCount < MAX_LEDS_PER_SEGMENT) {
      leds[ledCount++] = {strip, index};
    }
  }

  void setColor(CRGB color) {
    for (uint8_t i = 0; i < ledCount; i++) {
      leds[i].strip[leds[i].index] = color;
    }
  }
};

#endif