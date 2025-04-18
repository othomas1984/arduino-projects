#ifndef SMOOTHPALETTEFADEBEATANIMATION_H
#define SMOOTHPALETTEFADEBEATANIMATION_H

#include <Animation.h>

class SmoothPaletteFadeBeatAnimation : public Animation {
public:
  const char* pattern;
  CRGBPalette16 palette;
  uint16_t bpm100;
  bool loopPalette;
  unsigned long totalDuration;

  SmoothPaletteFadeBeatAnimation(
    const char* pattern,
    uint16_t bpm100,
    const CRGBPalette16& palette,
    bool loopPalette = true
  )
    : pattern(pattern),
      palette(palette),
      bpm100(bpm100),
      loopPalette(loopPalette),
      totalDuration(0) {

    for (uint16_t i = 0; pattern[i]; i++) {
      if (pattern[i] != '|' && pattern[i] != ' ') {
        totalDuration += getNoteDuration(pattern[i], bpm100);
      }
    }
  }

  void apply(unsigned long time) override {
    if (segmentCount == 0 || totalDuration == 0) return;

    unsigned long t = time % totalDuration;
    unsigned long elapsed = 0;
    uint8_t groupIndex = 0;
    bool inNoteGroup = false;
    unsigned long groupStart = 0;
    unsigned long groupDuration = 0;

    for (uint16_t i = 0; pattern[i]; i++) {
      char c = pattern[i];
      if (c == '|' || c == ' ') continue;

      bool isRest = (c >= 'a' && c <= 'z');
      unsigned long noteDuration = getNoteDuration(c, bpm100);

      if (t < elapsed + noteDuration) {
        if (!isRest) {
          if (!inNoteGroup) {
            groupStart = elapsed;
            groupDuration = noteDuration;
            inNoteGroup = true;
          } else {
            groupDuration += noteDuration;
          }

          float fraction = float(t - groupStart) / float(groupDuration);
          if (!loopPalette && (groupIndex % 2 == 1)) {
            fraction = 1.0f - fraction; // bounce back
          }

          uint8_t colorIndex = uint8_t(fraction * 255);
          CRGB color = ColorFromPalette(palette, colorIndex);

          for (uint8_t j = 0; j < segmentCount; j++) {
            const SegmentConfig& segCfg = segments[j];
            CRGB output = color;
            output.nscale8_video((brightnessPercent * segCfg.brightnessPercent) / 100);
            segCfg.segment->setColor(output);
          }
        }
        break;
      }

      // end of a group
      if (inNoteGroup && isRest) {
        groupIndex++;
        inNoteGroup = false;
      }

      elapsed += noteDuration;
    }
  }

  void applyOverlay(unsigned long time, BlendMode mode) override {
    if (segmentCount == 0 || totalDuration == 0) return;

    unsigned long t = time % totalDuration;
    unsigned long elapsed = 0;
    uint8_t groupIndex = 0;
    bool inNoteGroup = false;
    unsigned long groupStart = 0;
    unsigned long groupDuration = 0;

    for (uint16_t i = 0; pattern[i]; i++) {
      char c = pattern[i];
      if (c == '|' || c == ' ') continue;

      bool isRest = (c >= 'a' && c <= 'z');
      unsigned long noteDuration = getNoteDuration(c, bpm100);

      if (t < elapsed + noteDuration) {
        if (!isRest) {
          if (!inNoteGroup) {
            groupStart = elapsed;
            groupDuration = noteDuration;
            inNoteGroup = true;
          } else {
            groupDuration += noteDuration;
          }

          float fraction = float(t - groupStart) / float(groupDuration);
          if (!loopPalette && (groupIndex % 2 == 1)) {
            fraction = 1.0f - fraction; // bounce back
          }

          uint8_t colorIndex = uint8_t(fraction * 255);
          CRGB overlay = ColorFromPalette(palette, colorIndex);

          for (uint8_t j = 0; j < segmentCount; j++) {
            const SegmentConfig& segCfg = segments[j];
            Segment* seg = segCfg.segment;
            overlay.nscale8_video((brightnessPercent * segCfg.brightnessPercent) / 100);

            for (uint8_t k = 0; k < seg->ledCount; k++) {
              CRGB& dst = seg->leds[k].strip[seg->leds[k].index];
              dst = blendColor(dst, overlay, mode);
            }
          }
        }
        break;
      }

      if (inNoteGroup && isRest) {
        groupIndex++;
        inNoteGroup = false;
      }

      elapsed += noteDuration;
    }
  }

  uint8_t brightnessPercent = 100;
};


#endif