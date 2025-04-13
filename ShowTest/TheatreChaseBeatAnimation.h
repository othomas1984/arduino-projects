#ifndef THEATRECHASEBEATANIMATION_H
#define THEATRECHASEBEATANIMATION_H

#include <Animation.h>

class TheatreChaseBeatAnimation : public Animation {
public:
  const char* pattern;
  CRGB color;
  uint16_t bpm100;
  unsigned long totalDuration;
  uint8_t brightnessPercent = 100;
  bool isReversed;
  uint8_t spacing;

  TheatreChaseBeatAnimation(const char* pattern, uint16_t bpm100, CRGB color, uint8_t spacing, bool isReversed = false)
    : pattern(pattern), spacing(spacing), bpm100(bpm100), color(color), totalDuration(0), isReversed(isReversed) {
    for (uint16_t i = 0; pattern[i]; i++) {
      char c = pattern[i];
      if (c == '|' || c == ' ') continue;
      totalDuration += getNoteDuration(c, bpm100);
    }
  }

  void apply(unsigned long time) override {
    if (totalDuration == 0) return;
    unsigned long t = time % totalDuration;
    unsigned long elapsed = 0;
    uint16_t beatIndex = 0;

    for (uint16_t i = 0; pattern[i]; i++) {
      char c = pattern[i];
      if (c == '|' || c == ' ') continue;
      bool isRest = (c >= 'a' && c <= 'z');

      unsigned long duration = getNoteDuration(c, bpm100);
      if (t < elapsed + duration) {

        for (uint8_t j = 0; j < segmentCount; j++) {
          const SegmentConfig& segCfg = segments[j];
          Segment* seg = segCfg.segment;

          for (uint8_t k = 0; k < seg->ledCount; k++) {
            CRGB output = CRGB::Black;

            if (!isRest) {
              uint8_t ledIndex = isReversed ? k : (seg->ledCount - 1 - k);

              if ((ledIndex + beatIndex) % spacing == 0) {
                output = color;
                output.nscale8_video((brightnessPercent * segCfg.brightnessPercent) / 100);
              }
            }

            seg->leds[k].strip[seg->leds[k].index] = output;
          }
        }

        break;
      }

      elapsed += duration;
      if (!isRest) {
        beatIndex++;  // Only advance the chase for actual beats
      }
    }
  }
};

#endif