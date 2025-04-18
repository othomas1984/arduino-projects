#ifndef THEATRECHASEPALETTEBEATANIMATION_H
#define THEATRECHASEPALETTEBEATANIMATION_H

#include <Animation.h>

class TheatreChasePaletteBeatAnimation : public Animation {
public:
  const char* pattern;
  uint16_t bpm100;
  unsigned long totalDuration;
  uint8_t brightnessPercent = 100;
  uint8_t spacing;
  bool isReversed;
  CRGBPalette16 palette;

  TheatreChasePaletteBeatAnimation(
    const char* pattern,
    uint16_t bpm100,
    const CRGBPalette16& palette,
    bool isReversed = false,
    uint8_t spacing = 3
  )
    : pattern(pattern),
      bpm100(bpm100),
      palette(palette),
      isReversed(isReversed),
      spacing(spacing),
      totalDuration(0) {

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
                // Use palette instead of flat color
                uint8_t colorIndex = ((ledIndex + beatIndex) * 16) % 255;
                output = ColorFromPalette(palette, colorIndex);
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
        beatIndex++;  // Advance the chase only on notes
      }
    }
  }

  void applyOverlay(unsigned long time, BlendMode mode) override {
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
            CRGB overlay = CRGB::Black;

            if (!isRest) {
              uint8_t ledIndex = isReversed ? k : (seg->ledCount - 1 - k);

              if ((ledIndex + beatIndex) % spacing == 0) {
                uint8_t colorIndex = ((ledIndex + beatIndex) * 16) % 255;
                overlay = ColorFromPalette(palette, colorIndex);
                overlay.nscale8_video((brightnessPercent * segCfg.brightnessPercent) / 100);
              }
            }

            if (!isRest) {
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