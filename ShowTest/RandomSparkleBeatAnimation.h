#ifndef RANDOMSPARKLEBEATANIMATION_H
#define RANDOMSPARKLEBEATANIMATION_H

#include <Animation.h>

class RandomSparkleBeatAnimation : public Animation {
public:
  const char* pattern;
  uint16_t bpm100;
  CRGBPalette16 palette;
  uint8_t densityPercent = 10;
  uint8_t sparkleSize = 2;
  uint8_t brightnessPercent = 100;

  unsigned long totalDuration;

  RandomSparkleBeatAnimation(
    const char* pattern,
    uint16_t bpm100,
    const CRGBPalette16& palette,
    uint8_t densityPercent = 10,
    uint8_t sparkleSize = 2
  )
    : pattern(pattern),
      bpm100(bpm100),
      palette(palette),
      densityPercent(densityPercent),
      sparkleSize(sparkleSize),
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
        // On this beat
        for (uint8_t j = 0; j < segmentCount; j++) {
          const SegmentConfig& segCfg = segments[j];
          Segment* seg = segCfg.segment;

          // Clear segment first
          for (uint16_t k = 0; k < seg->ledCount; k++) {
            seg->leds[k].strip[seg->leds[k].index] = CRGB::Black;
          }

          if (!isRest) {
            // Determine how many LEDs to light
            uint16_t sparkleLEDs = (seg->ledCount * densityPercent) / 100;
            uint16_t sparkleCount = max(1, sparkleLEDs / sparkleSize);

            // Track used ranges to avoid clustering
            bool occupied[seg->ledCount] = { false };

            for (uint16_t s = 0; s < sparkleCount; s++) {
              // Find a gap
              uint16_t attempts = 0;
              uint16_t start = random(seg->ledCount);

              while (attempts < 10) {
                bool ok = true;
                for (uint8_t o = 0; o < sparkleSize && start + o < seg->ledCount; o++) {
                  if (occupied[start + o]) {
                    ok = false;
                    break;
                  }
                }
                if (ok) break;
                start = random(seg->ledCount);
                attempts++;
              }

              // Pick a varied color from the palette
              uint8_t colorIndex = (random8() + beatIndex * 47 + s * 73) % 255;
              CRGB color = ColorFromPalette(palette, colorIndex);
              color.nscale8_video((brightnessPercent * segCfg.brightnessPercent) / 100);

              // Paint sparkle
              for (uint8_t o = 0; o < sparkleSize && start + o < seg->ledCount; o++) {
                occupied[start + o] = true;
                seg->leds[start + o].strip[seg->leds[start + o].index] = color;
              }
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
