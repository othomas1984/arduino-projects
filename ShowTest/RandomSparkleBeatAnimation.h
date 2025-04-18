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
  uint8_t fadeInPercent;
  uint8_t fadeOutPercent;
  bool fadeOutAfterBeat;

  unsigned long totalDuration = 0;
  uint16_t lastBeatIndex = 65535;
  unsigned long lastTimeInCycle = 0;

  static const uint16_t MAX_LEDS = 256;
  bool sparkleMask[MAX_SEGMENTS_PER_ANIMATION][MAX_LEDS];
  CRGB sparkleColor[MAX_SEGMENTS_PER_ANIMATION][MAX_LEDS];

  bool lastSparkleMask[MAX_SEGMENTS_PER_ANIMATION][MAX_LEDS];
  CRGB lastSparkleColor[MAX_SEGMENTS_PER_ANIMATION][MAX_LEDS];

  RandomSparkleBeatAnimation(
    const char* pattern,
    uint16_t bpm100,
    const CRGBPalette16& palette,
    uint8_t densityPercent = 10,
    uint8_t sparkleSize = 2,
    uint8_t brightnessPercent = 100,
    uint8_t fadeInPercent = 0,
    uint8_t fadeOutPercent = 0,
    bool fadeOutAfterBeat = false
  )
    : pattern(pattern),
      bpm100(bpm100),
      palette(palette),
      densityPercent(densityPercent),
      sparkleSize(sparkleSize),
      brightnessPercent(brightnessPercent),
      fadeInPercent(fadeInPercent),
      fadeOutPercent(fadeOutPercent),
      fadeOutAfterBeat(fadeOutAfterBeat) {

    for (uint16_t i = 0; pattern[i]; i++) {
      char c = pattern[i];
      if (c == '|' || c == ' ') continue;
      totalDuration += getNoteDuration(c, bpm100);
    }

    memset(sparkleMask, 0, sizeof(sparkleMask));
    memset(sparkleColor, 0, sizeof(sparkleColor));
    memset(lastSparkleMask, 0, sizeof(lastSparkleMask));
    memset(lastSparkleColor, 0, sizeof(lastSparkleColor));
  }

  void apply(unsigned long time) override {
    if (totalDuration == 0) return;

    unsigned long t = time % totalDuration;
    if (t < lastTimeInCycle) {
      lastBeatIndex = 65535; // force refresh on loop
    }
    lastTimeInCycle = t;

    unsigned long elapsed = 0;
    uint16_t beatIndex = 0;

    for (uint16_t i = 0; pattern[i]; i++) {
      char c = pattern[i];
      if (c == '|' || c == ' ') continue;

      bool isRest = (c >= 'a' && c <= 'z');
      unsigned long duration = getNoteDuration(c, bpm100);
      unsigned long fadeOutTail = (duration * fadeOutPercent) / 100;
      unsigned long fadeOutEnd = fadeOutAfterBeat ? (elapsed + duration + fadeOutTail)
                                                  : (elapsed + duration);

      if (t < fadeOutEnd) {
        float beatProgress = float(t - elapsed) / float(duration);
        beatProgress = constrain(beatProgress, 0.0f, 1.0f);

        float fadeMultiplier = 1.0f;

        if (fadeOutAfterBeat && t >= elapsed + duration) {
          float tailProgress = float(t - (elapsed + duration)) / float(fadeOutTail);
          fadeMultiplier = 1.0f - constrain(tailProgress, 0.0f, 1.0f);
        } else {
          float fadeIn = fadeInPercent / 100.0f;
          float fadeOut = fadeOutPercent / 100.0f;
          if (fadeIn > 0 && beatProgress < fadeIn) {
            fadeMultiplier = beatProgress / fadeIn;
          } else if (!fadeOutAfterBeat && fadeOut > 0 && beatProgress > (1.0f - fadeOut)) {
            fadeMultiplier = (1.0f - beatProgress) / fadeOut;
          }
        }

        if (!isRest && beatIndex != lastBeatIndex) {
          lastBeatIndex = beatIndex;

          // Copy current → last
          memcpy(lastSparkleMask, sparkleMask, sizeof(sparkleMask));
          memcpy(lastSparkleColor, sparkleColor, sizeof(sparkleColor));
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

        // Render
        for (uint8_t j = 0; j < segmentCount; j++) {
          const SegmentConfig& segCfg = segments[j];
          Segment* seg = segCfg.segment;

          for (uint16_t k = 0; k < seg->ledCount; k++) {
            CRGB output = CRGB::Black;
            bool inCurrent = sparkleMask[j][k];
            bool inLast = lastSparkleMask[j][k];

            if (inCurrent && inLast) {
              output = blend(lastSparkleColor[j][k], sparkleColor[j][k], uint8_t(beatProgress * 255));
            } else if (inCurrent) {
              output = sparkleColor[j][k];
            } else if (inLast) {
              output = lastSparkleColor[j][k];
            }

            if (output != CRGB::Black) {
              output.nscale8(uint8_t(fadeMultiplier * 255));
            }

            seg->leds[k].strip[seg->leds[k].index] = output;
          }
        }

        return;
      }

      elapsed += duration;
      if (!isRest) beatIndex++;
    }

    // Past last beat — blackout
    for (uint8_t j = 0; j < segmentCount; j++) {
      const SegmentConfig& segCfg = segments[j];
      Segment* seg = segCfg.segment;

      for (uint16_t k = 0; k < seg->ledCount; k++) {
        seg->leds[k].strip[seg->leds[k].index] = CRGB::Black;
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
      unsigned long fadeOutTail = (duration * fadeOutPercent) / 100;
      unsigned long fadeOutEnd = fadeOutAfterBeat ? (elapsed + duration + fadeOutTail)
                                                  : (elapsed + duration);

      if (t < fadeOutEnd) {
        float beatProgress = float(t - elapsed) / float(duration);
        beatProgress = constrain(beatProgress, 0.0f, 1.0f);

        float fadeMultiplier = 1.0f;

        if (fadeOutAfterBeat && t >= elapsed + duration) {
          float tailProgress = float(t - (elapsed + duration)) / float(fadeOutTail);
          fadeMultiplier = 1.0f - constrain(tailProgress, 0.0f, 1.0f);
        } else {
          float fadeIn = fadeInPercent / 100.0f;
          float fadeOut = fadeOutPercent / 100.0f;
          if (fadeIn > 0 && beatProgress < fadeIn) {
            fadeMultiplier = beatProgress / fadeIn;
          } else if (!fadeOutAfterBeat && fadeOut > 0 && beatProgress > (1.0f - fadeOut)) {
            fadeMultiplier = (1.0f - beatProgress) / fadeOut;
          }
        }

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
            color.nscale8(uint8_t(fadeMultiplier * 255));

            if (!isRest) {
              for (uint8_t o = 0; o < sparkleSize && (start + o) < count; o++) {
                occupied[start + o] = true;
                CRGB& dst = seg->leds[start + o].strip[seg->leds[start + o].index];
                dst = blendColor(dst, color, mode);
              }
            }
          }
        }
        return;
      }

      if (!isRest) beatIndex++;
      elapsed += duration;
    }
  }
};

#endif
