#ifndef PATTERNBEATANIMATION_H
#define PATTERNBEATANIMATION_H

#include <Animation.h>

class PatternBeatAnimation : public Animation {
public:
  const char* pattern;
  CRGB color;
  CRGB restColor;
  uint16_t bpm100;
  unsigned long totalDuration;
  uint8_t brightnessPercent = 100;

  PatternBeatAnimation(const char* pattern, uint16_t bpm100, CRGB color, CRGB restColor = CRGB::Black)
    : pattern(pattern), bpm100(bpm100), color(color), restColor(restColor), totalDuration(0) {
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
          CRGB output = isRest ? restColor : color;
          output.nscale8_video((brightnessPercent * segCfg.brightnessPercent) / 100);
          segCfg.segment->setColor(output);
        }
        break;
      }
      elapsed += duration;
    }
  }

  void applyOverlay(unsigned long time, BlendMode mode) override {
    unsigned long t = time % totalDuration;
    unsigned long elapsed = 0;

    for (uint16_t i = 0; pattern[i]; i++) {
      char c = pattern[i];
      if (c == '|' || c == ' ') continue;

      unsigned long dur = getNoteDuration(c, bpm100);
      if (t < elapsed + dur) {
        bool isRest = (c >= 'a' && c <= 'z');
        CRGB overlayColor = isRest ? CRGB::Black : color;

        for (uint8_t j = 0; j < segmentCount; j++) {
          const SegmentConfig& segCfg = segments[j];
          Segment* seg = segCfg.segment;

          // Brightness adjustment before blending
          overlayColor.nscale8_video((brightnessPercent * segCfg.brightnessPercent) / 100);

          if (!isRest) {
            for (uint8_t k = 0; k < seg->ledCount; k++) {
              CRGB& dst = seg->leds[k].strip[seg->leds[k].index];
              dst = blendColor(dst, overlayColor, mode);
            }
          }
        }

        return; // only process one note
      }

      elapsed += dur;
    }
  }

};

#endif
