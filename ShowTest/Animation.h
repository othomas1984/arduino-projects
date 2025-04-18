#ifndef ANIMATION_H
#define ANIMATION_H

#include <Segment.h>

struct SegmentConfig {
  Segment* segment;
  uint8_t brightnessPercent;
};

enum BlendMode {
  BLEND_REPLACE,
  BLEND_ADD,
  BLEND_MULTIPLY,
  BLEND_BRIGHTNESS,
  BLEND_MAX,
  BLEND_MIN,
  BLEND_SCREEN
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

  void clearSegments() {
    segmentCount = 0;
  }

  virtual void applyOverlay(unsigned long time, BlendMode mode) {
    apply(time);  // default: fallback to base logic
  }

  virtual void apply(unsigned long time) = 0;
};


inline CRGB blendColor(const CRGB& base, const CRGB& overlay, BlendMode mode) {
  switch (mode) {
    case BLEND_REPLACE:
      return overlay;

    case BLEND_ADD: {
      CRGB result = base;
      result += overlay;
      return result;
    }

    case BLEND_MULTIPLY:
      return CRGB(
        (base.r * overlay.r) / 255,
        (base.g * overlay.g) / 255,
        (base.b * overlay.b) / 255
      );

    case BLEND_BRIGHTNESS: {
      // Use overlay brightness to modulate base color
      uint8_t brightness = overlay.getAverageLight();
      CRGB result = base;
      result.nscale8_video(brightness);
      return result;
    }

    case BLEND_MAX:
      return CRGB(
        max(base.r, overlay.r),
        max(base.g, overlay.g),
        max(base.b, overlay.b)
      );

    case BLEND_MIN:
      return CRGB(
        min(base.r, overlay.r),
        min(base.g, overlay.g),
        min(base.b, overlay.b)
      );

    case BLEND_SCREEN:
      return CRGB(
        255 - ((255 - base.r) * (255 - overlay.r) / 255),
        255 - ((255 - base.g) * (255 - overlay.g) / 255),
        255 - ((255 - base.b) * (255 - overlay.b) / 255)
      );

    default:
      return overlay;  // fallback to replace
  }
}

#endif