#ifndef ANIMATION_H
#define ANIMATION_H

#include <Segment.h>

struct SegmentConfig {
  Segment* segment;
  uint8_t brightnessPercent;
};


#define MAX_SEGMENTS_PER_ANIMATION 8

class Animation {
protected:
  SegmentConfig segments[MAX_SEGMENTS_PER_ANIMATION];
  uint8_t segmentCount = 0;

public:
  virtual ~Animation() {}

  void addSegment(Segment* s, uint8_t brightness = 100) {
    if (segmentCount < MAX_SEGMENTS_PER_ANIMATION) {
      segments[segmentCount++] = {s, brightness};
    }
  }

  virtual void apply(unsigned long time) = 0;
};

#endif