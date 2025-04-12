#ifndef SCENE_H
#define SCENE_H

#include <Segment.h>

#define MAX_SEGMENTS_PER_SCENE 8

struct StripLEDConfig {
  uint8_t stripNumber;
  const uint16_t* indexes;
  uint8_t count;
};

class Scene {
public:
  Segment* segments[MAX_SEGMENTS_PER_SCENE];
  uint8_t segmentCount;

  Scene() : segmentCount(0) {}

  Segment* createSegment(const StripLEDConfig* configs, uint8_t configCount) {
    if (segmentCount >= MAX_SEGMENTS_PER_SCENE) return nullptr;
    Segment* segment = new Segment();
    for (uint8_t i = 0; i < configCount; i++) {
      const StripLEDConfig& cfg = configs[i];
      CRGB* strip = strips[cfg.stripNumber - 1];
      for (uint8_t j = 0; j < cfg.count; j++) {
        segment->addLED(strip, cfg.indexes[j]);
      }
    }
    segments[segmentCount++] = segment;
    return segment;
  }
};

#endif