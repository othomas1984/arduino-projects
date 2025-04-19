#ifndef RANDOMSPARKLEBEATANIMATION_H
#define RANDOMSPARKLEBEATANIMATION_H

#include <Animation.h>

class RandomSparkleBeatAnimation : public Animation {
public:
  const char* pattern;
  uint16_t bpm100;
  CRGBPalette16 palette;
  uint8_t densityPercent;
  uint8_t sparkleSize;
  uint8_t brightnessPercent;
  bool fadeOutAfterBeat;

  unsigned long totalDuration = 0;
  uint16_t lastBeatIndex = 65535;
  unsigned long lastTimeInCycle = 0;

  static const uint16_t MAX_LEDS = 64;
  bool sparkleMask[MAX_SEGMENTS_PER_ANIMATION][MAX_LEDS];
  CRGB sparkleColor[MAX_SEGMENTS_PER_ANIMATION][MAX_LEDS];

  RandomSparkleBeatAnimation(
    const char* pattern,
    uint16_t bpm100,
    const CRGBPalette16& palette,
    uint8_t densityPercent = 10,
    uint8_t sparkleSize = 2,
    uint8_t brightnessPercent = 100,
    bool fadeOutAfterBeat = false
  )
    : pattern(pattern), bpm100(bpm100), palette(palette),
      densityPercent(densityPercent), sparkleSize(sparkleSize),
      brightnessPercent(brightnessPercent), fadeOutAfterBeat(fadeOutAfterBeat) {

    for (uint16_t i = 0; pattern[i]; i++) {
      char c = pattern[i];
      if (c == '|' || c == ' ') continue;
      totalDuration += getNoteDuration(c, bpm100);
    }

    memset(sparkleMask, 0, sizeof(sparkleMask));
    memset(sparkleColor, 0, sizeof(sparkleColor));
  }

  void apply(unsigned long time) override {
    if (totalDuration == 0) return;

    unsigned long t = time % totalDuration;
    if (t < lastTimeInCycle) {
      lastBeatIndex = 65535;
    }
    lastTimeInCycle = t;

    unsigned long elapsed = 0;
    uint16_t beatIndex = 0;

    for (uint16_t i = 0; pattern[i]; i++) {
      char c = pattern[i];
      if (c == '|' || c == ' ') continue;

      bool isRest = (c >= 'a' && c <= 'z');
      unsigned long duration = getNoteDuration(c, bpm100);

      if (t < elapsed + duration) {
        if (!isRest && beatIndex != lastBeatIndex) {
          lastBeatIndex = beatIndex;

          memset(sparkleMask, 0, sizeof(sparkleMask));
          memset(sparkleColor, 0, sizeof(sparkleColor));

          for (uint8_t j = 0; j < segmentCount; j++) {
            const SegmentConfig& segCfg = segments[j];
            Segment* seg = segCfg.segment;
            uint16_t count = seg->ledCount;

            uint16_t sparkleLEDs = (count * densityPercent) / 100;
            uint16_t sparkleCount = max(1, sparkleLEDs / sparkleSize);
            bool occupied[MAX_LEDS] = {false};

            for (uint16_t s = 0; s < sparkleCount; s++) {
              uint8_t attempts = 0;
              uint16_t start = random(count);

              while (attempts++ < 10) {
                bool ok = true;
                for (uint8_t o = 0; o < sparkleSize && (start + o) < count; o++) {
                  if (occupied[start + o]) {
                    ok = false;
                    break;
                  }
                }
                if (ok) break;
                start = random(count);
              }

              uint8_t colorIndex = (random8() + beatIndex * 47 + s * 71) % 255;
              CRGB color = ColorFromPalette(palette, colorIndex);
              color.nscale8((brightnessPercent * segCfg.brightnessPercent) / 100);

              for (uint8_t o = 0; o < sparkleSize && (start + o) < count; o++) {
                occupied[start + o] = true;
                sparkleMask[j][start + o] = true;
                sparkleColor[j][start + o] = color;
              }
            }
          }
        }

        if (!isRest) {
          // Render sparkles for active beat
          for (uint8_t j = 0; j < segmentCount; j++) {
            const SegmentConfig& segCfg = segments[j];
            Segment* seg = segCfg.segment;

            for (uint16_t k = 0; k < seg->ledCount; k++) {
              CRGB output = sparkleMask[j][k] ? sparkleColor[j][k] : CRGB::Black;
              seg->leds[k].strip[seg->leds[k].index] = output;
            }
          }
        } else {
          // On rests, clear sparkles
          for (uint8_t j = 0; j < segmentCount; j++) {
            const SegmentConfig& segCfg = segments[j];
            Segment* seg = segCfg.segment;

            for (uint16_t k = 0; k < seg->ledCount; k++) {
              seg->leds[k].strip[seg->leds[k].index] = CRGB::Black;
            }
          }
        }
        return;
      }

      elapsed += duration;
      if (!isRest) beatIndex++;
    }

    // Outside all beats — blackout
    for (uint8_t j = 0; j < segmentCount; j++) {
      const SegmentConfig& segCfg = segments[j];
      Segment* seg = segCfg.segment;
      for (uint16_t k = 0; k < seg->ledCount; k++) {
        seg->leds[k].strip[seg->leds[k].index] = CRGB::Black;
      }
    }
  }
};

#endif
