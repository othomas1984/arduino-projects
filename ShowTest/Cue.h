#ifndef CUE_H
#define CUE_H

#include <Animation.h>
#include <Scene.h>

#define MAX_ANIMATIONS_PER_SCENE 8

class Cue {
public:
  Scene* scene;
  unsigned long duration;
  uint16_t bpm100;
  Animation* animations[MAX_ANIMATIONS_PER_SCENE];
  uint8_t animationCount;

  Cue(Scene* scene, unsigned long duration, uint16_t bpm100)
    : scene(scene), duration(duration), bpm100(bpm100), animationCount(0) {}

  Cue(Scene* scene, double beats, uint16_t bpm100)
    : scene(scene), duration((unsigned long)((beats * 60000.0 * 100.0) / (double)bpm100)), bpm100(bpm100), animationCount(0) {}

  void addAnimation(Animation* animation) {
    if (animationCount < MAX_ANIMATIONS_PER_SCENE) {
      animations[animationCount++] = animation;
    }
  }

  void update(unsigned long t) {
    for (uint8_t i = 0; i < animationCount; i++) {
      animations[i]->apply(t);
    }
  }
};
#endif