#ifndef SEGMENTTHEATRECHASEBEATANIMATION_H
#define SEGMENTTHEATRECHASEBEATANIMATION_H

#include <Animation.h>

class SegmentTheatreChaseBeatAnimation : public Animation {
public:
  const char* pattern;
  CRGB color;
  uint16_t bpm100;
  unsigned long totalDuration;
  bool isReversed;
  uint8_t brightnessPercent = 100;
  uint8_t spacing = 3;

  SegmentTheatreChaseBeatAnimation(const char* pattern, uint16_t bpm100, CRGB color, bool isReversed = false)
    : pattern(pattern), bpm100(bpm100), color(color), isReversed(isReversed), totalDuration(0) {
    for (uint16_t i = 0; pattern[i]; i++) {
      char c = pattern[i];
      if (c == '|' || c == ' ') continue;
      totalDuration += getNoteDuration(c, bpm100);
    }
  }

  void apply(unsigned long time) override {
    if (totalDuration == 0 || segmentCount == 0) return;

    unsigned long t = time % totalDuration;
    unsigned long elapsed = 0;
    uint16_t beatIndex = 0;

    for (uint16_t i = 0; pattern[i]; i++) {
      char c = pattern[i];
      if (c == '|' || c == ' ') continue;

      unsigned long duration = getNoteDuration(c, bpm100);
      bool isRest = (c >= 'a' && c <= 'z');

      if (t < elapsed + duration) {
        for (uint8_t j = 0; j < segmentCount; j++) {
          const SegmentConfig& segCfg = segments[j];

          // Forward or reverse index for the chase effect
          uint8_t segIndex = isReversed ? (segmentCount - 1 - j) : j;

          CRGB output = CRGB::Black;
          if (!isRest && ((segIndex + beatIndex) % spacing == 0)) {
            output = color;
            output.nscale8_video((brightnessPercent * segCfg.brightnessPercent) / 100);
          }

          segCfg.segment->setColor(output);
        }
        break;
      }

      elapsed += duration;
      if (!isRest) beatIndex++;
    }
  }
};

#endif