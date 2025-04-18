#ifndef SHOW_H
#define SHOW_H

#include <Cue.h>

#define MAX_CUES_PER_SHOW 32

class Show {
public:
  Cue* cues[MAX_CUES_PER_SHOW];
  uint8_t cueCount;
  bool repeats = false;
  unsigned long totalDurationMillis = 0;

  Show() : cueCount(0) {}

  void addCue(Cue* cue) {
    if (cueCount < MAX_CUES_PER_SHOW) {
      cues[cueCount++] = cue;
      totalDurationMillis += cue->duration;
    }
  }

  void update(unsigned long time) {
    unsigned long offset = 0;

    // Always clear all LEDs before cue update
    for (int i = 0; i < STRIP1_LEDS; i++) strip1[i] = CRGB::Black;
    for (int i = 0; i < STRIP2_LEDS; i++) strip2[i] = CRGB::Black;

    for (uint8_t i = 0; i < cueCount; i++) {
      if (time < offset + cues[i]->duration) {
        cues[i]->update(time - offset);
        return;
      }
      offset += cues[i]->duration;
    }
  }

  bool isFinished(unsigned long elapsedMillis) {
    return !repeats && elapsedMillis > totalDurationMillis;
  }
};

#endif
