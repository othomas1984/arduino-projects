#ifndef SEGMENTRAINBOWCHASEBEATANIMATION_H
#define SEGMENTRAINBOWCHASEBEATANIMATION_H

#include <Animation.h>

class SegmentRainbowChaseAnimation : public Animation {
public:
  const char* pattern;
  uint16_t bpm100;
  unsigned long totalDuration;
  uint8_t spacing = 3;
  bool isReversed;
  CRGBPalette16 palette;
  uint8_t brightnessPercent = 100;

  SegmentRainbowChaseAnimation(
    const char* pattern,
    uint16_t bpm100,
    const CRGBPalette16& palette,
    bool isReversed = false
  )
    : pattern(pattern),
      bpm100(bpm100),
      palette(palette),
      isReversed(isReversed),
      totalDuration(0) {

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
          Segment* seg = segCfg.segment;

          uint8_t segIndex = isReversed ? (segmentCount - 1 - j) : j;

          CRGB output = CRGB::Black;
          if (!isRest && ((segIndex + beatIndex) % spacing == 0)) {
            // Color scrolls forward with beatIndex
            uint8_t colorIndex = ((segIndex + beatIndex) * 32) % 255;
            output = ColorFromPalette(palette, colorIndex);
            output.nscale8_video((brightnessPercent * segCfg.brightnessPercent) / 100);
          }

          seg->setColor(output);
        }
        break;
      }

      elapsed += duration;
      if (!isRest) beatIndex++;
    }
  }

  void applyOverlay(unsigned long time, BlendMode mode) override {
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
        if (!isRest) {
          for (uint8_t j = 0; j < segmentCount; j++) {
            const SegmentConfig& segCfg = segments[j];
            Segment* seg = segCfg.segment;

            uint8_t segIndex = isReversed ? (segmentCount - 1 - j) : j;

            CRGB overlay = CRGB::Black;
            if ((segIndex + beatIndex) % spacing == 0) {
              uint8_t colorIndex = ((segIndex + beatIndex) * 32) % 255;
              overlay = ColorFromPalette(palette, colorIndex);
              overlay.nscale8_video((brightnessPercent * segCfg.brightnessPercent) / 100);
            }

            for (uint8_t k = 0; k < seg->ledCount; k++) {
              CRGB& dst = seg->leds[k].strip[seg->leds[k].index];
              dst = blendColor(dst, overlay, mode);
            }
          }
        }
        break;
      }

      elapsed += duration;
      if (!isRest) beatIndex++;
    }
  }
};

#endif