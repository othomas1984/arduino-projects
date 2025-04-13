#ifndef PATTERNBEATANIMATION_H
#define PATTERNBEATANIMATION_H

#include <Animation.h>

class PatternBeatAnimation : public Animation {
public:
  const char* pattern;
  CRGB color;
  uint16_t bpm100;
  unsigned long totalDuration;
  uint8_t brightnessPercent = 100;

  PatternBeatAnimation(const char* pattern, uint16_t bpm100, CRGB color)
    : pattern(pattern), bpm100(bpm100), color(color), totalDuration(0) {
    for (uint16_t i = 0; pattern[i]; i++) {
      char c = pattern[i];
      if (c == '|' || c == ' ') continue;
      totalDuration += getNoteDuration(c, bpm100);
    }
  }

  void apply(unsigned long time) override {
    unsigned long t = time % totalDuration;
    unsigned long elapsed = 0;

    for (uint16_t i = 0; pattern[i]; i++) {
      char c = pattern[i];
      if (c == '|' || c == ' ') continue;
      unsigned long duration = getNoteDuration(c, bpm100);
      if (t < elapsed + duration) {
        bool isRest = (c >= 'a' && c <= 'z');
        for (uint8_t j = 0; j < segmentCount; j++) {
          const SegmentConfig& segCfg = segments[j];
          CRGB output = isRest ? CRGB::Black : color;
          output.nscale8_video((brightnessPercent * segCfg.brightnessPercent) / 100);
          segCfg.segment->setColor(output);
        }
        break;
      }
      elapsed += duration;
    }
  }
};

#endif