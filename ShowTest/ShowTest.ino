#include <FastLED.h>

#define STRIP1_LEDS 161
#define STRIP2_LEDS 29
#define MAX_LEDS_PER_SEGMENT 64
#define MAX_SEGMENTS_PER_SCENE 8
#define MAX_SEGMENTS_PER_ANIMATION 8
#define MAX_ANIMATIONS_PER_SCENE 8
#define MAX_CUES_PER_SHOW 32

CRGB strip1[STRIP1_LEDS];
CRGB strip2[STRIP2_LEDS];
CRGB* strips[] = {strip1, strip2};

// ----- Serial Sync Flag -----
bool showStarted = false;
unsigned long showStartMillis = 0;
unsigned long lastLogMillis = 0;

char serialBuffer[32];
uint8_t serialIndex = 0;

void syncToSerialTime(uint32_t reportedMillis) {
  unsigned long currentMillis = millis();
  unsigned long localElapsedMillis = currentMillis - showStartMillis;

  long drift = (long)reportedMillis - (long)localElapsedMillis;

  // if (abs(drift) < 30) {
  //   // Ignore small drift
  //   Serial.print("Drift ignored (too small): ");
  //   Serial.print(drift);
  //   Serial.println(" ms");
  //   return;
  // }
  Serial.print("currentMillis: ");
  Serial.println(currentMillis);
  Serial.print("reportedMillis: ");
  Serial.println(reportedMillis);
  Serial.print("showStartMillis: ");
  Serial.println(showStartMillis);
  Serial.print("localElapsedMillis: ");
  Serial.println(localElapsedMillis);
  Serial.print("drift: ");
  Serial.println(drift);

  // Limit correction to ±100ms
  // if (drift > 100L) drift = 100L;
  // if (drift < -100L) drift = -100L;
  Serial.print("clamped drift: ");
  Serial.println(drift);

  showStartMillis -= drift;
  Serial.print("new showStartMillis: ");
  Serial.println(showStartMillis);

  Serial.print("Drift correction applied: ");
  Serial.print(drift);
  Serial.println(" ms");
}

// ----- Core Structures -----

struct LEDPointer {
  CRGB* strip;
  uint16_t index;
};

struct StripLEDConfig {
  uint8_t stripNumber;
  const uint16_t* indexes;
  uint8_t count;
};

class Segment {
public:
  LEDPointer leds[MAX_LEDS_PER_SEGMENT];
  uint8_t ledCount;

  Segment() : ledCount(0) {}

  void addLED(CRGB* strip, uint16_t index) {
    if (ledCount < MAX_LEDS_PER_SEGMENT) {
      leds[ledCount++] = {strip, index};
    }
  }

  void setColor(CRGB color) {
    for (uint8_t i = 0; i < ledCount; i++) {
      leds[i].strip[leds[i].index] = color;
    }
  }
};

class Scene {
public:
  Segment* segments[MAX_SEGMENTS_PER_SCENE];
  uint8_t segmentCount;

  Scene() : segmentCount(0) {}

  Segment* createSegment(const StripLEDConfig* configs, uint8_t configCount) {
    if (segmentCount >= MAX_SEGMENTS_PER_SCENE) return nullptr;
    Segment* segment = new Segment();
    for (uint8_t i = 0; i < configCount; i++) {
      const StripLEDConfig& cfg = configs[i];
      CRGB* strip = strips[cfg.stripNumber - 1];
      for (uint8_t j = 0; j < cfg.count; j++) {
        segment->addLED(strip, cfg.indexes[j]);
      }
    }
    segments[segmentCount++] = segment;
    return segment;
  }
};

// ----- Animation Base Class -----

struct SegmentConfig {
  Segment* segment;
  uint8_t brightnessPercent;
};

class Animation {
protected:
  SegmentConfig segments[MAX_SEGMENTS_PER_ANIMATION];
  uint8_t segmentCount = 0;

public:
  virtual ~Animation() {}

  void addSegment(Segment* s, uint8_t brightness = 100) {
    if (segmentCount < MAX_SEGMENTS_PER_ANIMATION) {
      segments[segmentCount++] = {s, brightness};
    }
  }

  virtual void apply(unsigned long time) = 0;
};

// ----- Note Duration with bpm100 -----

uint16_t getNoteDuration(char symbol, uint16_t bpm100) {
  unsigned long beat = 6000000UL / bpm100; // 60,000 * 100 / bpm
  switch (symbol) {
    case 'W': case 'w': return beat * 4;
    case 'H': case 'h': return beat * 2;
    case 'Q': case 'q': return beat;
    case 'E': case 'e': return beat / 2;
    case 'S': case 's': return beat / 4;
    case 'T': case 't': return beat / 8;
    default: return 0;
  }
}

// ----- PatternBeatAnimation with bpm100 -----

class PatternBeatAnimation : public Animation {
public:
  const char* pattern;
  CRGB color;
  uint16_t bpm100;
  unsigned long totalDuration;
  uint8_t brightnessPercent = 100;

  PatternBeatAnimation(const char* pattern, uint16_t bpm100, CRGB color)
    : pattern(pattern), bpm100(bpm100), color(color), totalDuration(0) {
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
          CRGB output = isRest ? CRGB::Black : color;
          output.nscale8_video((brightnessPercent * segCfg.brightnessPercent) / 100);
          segCfg.segment->setColor(output);
        }
        break;
      }
      elapsed += duration;
    }
  }
};

// ----- SegmentTheatreChaseBeatAnimation with bpm100 ----- 

class SegmentTheatreChaseBeatAnimation : public Animation {
public:
  const char* pattern;
  CRGB color;
  uint16_t bpm100;
  unsigned long totalDuration;
  bool isReversed;
  uint8_t brightnessPercent = 100;
  uint8_t spacing = 3;

  SegmentTheatreChaseBeatAnimation(const char* pattern, uint16_t bpm100, CRGB color, bool isReversed = false)
    : pattern(pattern), bpm100(bpm100), color(color), isReversed(isReversed), totalDuration(0) {
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

          // Forward or reverse index for the chase effect
          uint8_t segIndex = isReversed ? (segmentCount - 1 - j) : j;

          CRGB output = CRGB::Black;
          if (!isRest && ((segIndex + beatIndex) % spacing == 0)) {
            output = color;
            output.nscale8_video((brightnessPercent * segCfg.brightnessPercent) / 100);
          }

          segCfg.segment->setColor(output);
        }
        break;
      }

      elapsed += duration;
      if (!isRest) beatIndex++;
    }
  }
};

// ----- TheatreChaseBeatAnimation with bpm100 -----

class TheatreChaseBeatAnimation : public Animation {
public:
  const char* pattern;
  CRGB color;
  uint16_t bpm100;
  unsigned long totalDuration;
  uint8_t brightnessPercent = 100;
  bool isReversed;
  uint8_t spacing = 3;

  TheatreChaseBeatAnimation(const char* pattern, uint16_t bpm100, CRGB color, bool isReversed = false)
    : pattern(pattern), bpm100(bpm100), color(color), totalDuration(0), isReversed(isReversed) {
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


// ----- Cue and Show -----

class Cue {
public:
  Scene* scene;
  unsigned long duration;
  uint16_t bpm100;
  Animation* animations[MAX_ANIMATIONS_PER_SCENE];
  uint8_t animationCount;

  Cue(Scene* scene, unsigned long duration, uint16_t bpm100)
    : scene(scene), duration(duration), bpm100(bpm100), animationCount(0) {}

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

class Show {
public:
  Cue* cues[MAX_CUES_PER_SHOW];
  uint8_t cueCount;

  Show() : cueCount(0) {}

  void addCue(Cue* cue) {
    if (cueCount < MAX_CUES_PER_SHOW) {
      cues[cueCount++] = cue;
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
};

// ----- Show Definition: "Lose Yourself" -----

Scene scene, scene1;
Segment *piano1Segment, *piano2Segment, *drumSegment, *baseSegment, *stringSegment;
Segment *scene1Segment1, *scene1Segment2, *scene1Segment3, *scene1Segment4, *scene1Segment5, *scene1Segment6, *scene1Segment7;
Show loseYourselfShow, testShow;

void initScene1Segments() {
  const uint16_t leds1[] = {0,1};
  StripLEDConfig leds1Cfg[] = {{1, leds1, 2}};
  scene1Segment1 = scene1.createSegment(leds1Cfg, 1);
  const uint16_t leds2[] = {2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37};
  StripLEDConfig leds2Cfg[] = {{1, leds2, 36}};
  scene1Segment2 = scene1.createSegment(leds2Cfg, 1);
  const uint16_t leds3[] = {38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66};
  StripLEDConfig leds3Cfg[] = {{1, leds3, 29}};
  scene1Segment3 = scene1.createSegment(leds3Cfg, 1);
  const uint16_t leds4[] = {67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102};
  StripLEDConfig leds4Cfg[] = {{1, leds4, 36}};
  scene1Segment4 = scene1.createSegment(leds4Cfg, 1);
  const uint16_t leds5[] = {103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131};
  StripLEDConfig leds5Cfg[] = {{1, leds5, 29}};
  scene1Segment5 = scene1.createSegment(leds5Cfg, 1);
  const uint16_t leds6[] = {132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160};
  StripLEDConfig leds6Cfg[] = {{1, leds6, 29}};
  scene1Segment6 = scene1.createSegment(leds6Cfg, 1);
  const uint16_t leds7[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28};
  StripLEDConfig leds7Cfg[] = {{2, leds7, 29}};
  scene1Segment7 = scene1.createSegment(leds7Cfg, 1);
}

void initSegments() {
  const uint16_t piano1LEDs[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29};
  StripLEDConfig piano1Cfg[] = {{1, piano1LEDs, 15}};
  piano1Segment = scene.createSegment(piano1Cfg, 1);

  const uint16_t piano2LEDs[] = {0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28};
  StripLEDConfig piano2Cfg[] = {{1, piano2LEDs, 15}};
  piano2Segment = scene.createSegment(piano2Cfg, 1);

  const uint16_t drumLEDs1[] = {30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49};
  const uint16_t drumLEDs2[] = {0,1,2,3,4,5,6,7,8,9};
  StripLEDConfig drumCfg[] = {{1, drumLEDs1, 20}, {2, drumLEDs2, 10}};
  drumSegment = scene.createSegment(drumCfg, 2);

  const uint16_t stringLEDs1[] = {56,57,58,59,60,61,62,63,64,65,66};
  const uint16_t stringLEDs2[] = {15,16,17,18,19,20,21,22,23,24,25,26,27,28};
  StripLEDConfig stringCfg[] = {{1, stringLEDs1, 11}, {2, stringLEDs2, 14}};
  stringSegment = scene.createSegment(stringCfg, 2);

  const uint16_t baseLEDs1[] = {50,51,52,53,54,55};
  StripLEDConfig baseCfg[] = {{1, baseLEDs1, 6}};
  baseSegment = scene.createSegment(baseCfg, 1);
}

void initLoseYourselfShow() {
  uint16_t bpmIntro = 6593;
  uint16_t bpmMain  = 8572;

  Cue* silence = new Cue(&scene, 2000 + 1000, bpmIntro);
  loseYourselfShow.addCue(silence);

  Cue* cueIntro = new Cue(&scene, 29100, bpmIntro);
  auto piano1Intro = new PatternBeatAnimation("| Ee Ee Ee Ee | Ee Ee ee SsSs | Ee Ee Ee Ee | Ee Ee q q |", bpmIntro, CRGB::Blue);
  auto piano2Intro = new PatternBeatAnimation("| eE eE eE eE | eE eE ee sSsS | eE eE eE eE | eE ee q q |", bpmIntro, CRGB::Blue);
  piano1Intro->addSegment(piano1Segment, 100);
  piano2Intro->addSegment(piano2Segment, 100);
  cueIntro->addAnimation(piano1Intro);
  cueIntro->addAnimation(piano2Intro);
  loseYourselfShow.addCue(cueIntro);

  Cue* cueVerse1 = new Cue(&scene, 44850, bpmMain);
  auto drums1 = new PatternBeatAnimation("| E e E e E e E e | E e E e E e E t t T t |", bpmMain, CRGB::Red);
  auto base1 = new PatternBeatAnimation("| wwwwwww | h q esTt | SsSs q h | SsSs q q esTt | Sse eSs Ssss q | Ee q SsSs esTt | SsSs q SsSs eSs | Sse q SsSs eTtTt | SsSs q SsSs | eE eE SsSs SsSs |  ", bpmMain, CRGB::Purple);
  drums1->addSegment(drumSegment, 100);
  base1->addSegment(baseSegment, 100);
  cueVerse1->addAnimation(drums1);
  cueVerse1->addAnimation(base1);
  loseYourselfShow.addCue(cueVerse1);

  Cue* cueChorus1 = new Cue(&scene, 20000, bpmMain);
  auto strings1 = new PatternBeatAnimation("| Q Q q q | Q Q Q Q |", bpmMain, CRGB::Yellow);
  auto base2 = new PatternBeatAnimation("| TtsTts TtsTts TtsTts TtsTts|  ", bpmMain, CRGB::Purple);
  strings1->addSegment(stringSegment, 100);
  base2->addSegment(baseSegment, 100);
  cueChorus1->addAnimation(drums1);
  cueChorus1->addAnimation(piano1Intro);
  cueChorus1->addAnimation(strings1);
  cueChorus1->addAnimation(base2);
  loseYourselfShow.addCue(cueChorus1);

  Cue* cueVerse2 = new Cue(&scene, 40000, bpmMain);
  auto drums2 = new PatternBeatAnimation("| E e E e | E E |", bpmMain, CRGB::Red);
  drums2->addSegment(drumSegment, 100);
  cueVerse2->addAnimation(drums2);
  loseYourselfShow.addCue(cueVerse2);

  Cue* cueChorus2 = new Cue(&scene, 20000, bpmMain);
  auto strings2 = new PatternBeatAnimation("| Q Q q q | Q Q Q Q |", bpmMain, CRGB::Yellow);
  strings2->addSegment(stringSegment, 100);
  cueChorus2->addAnimation(strings2);
  loseYourselfShow.addCue(cueChorus2);

  Cue* cueOutro = new Cue(&scene, 10000, bpmIntro);
  auto outro = new PatternBeatAnimation("| q q q q | q q q q |", bpmIntro, CRGB::Blue);
  outro->addSegment(piano1Segment, 50);
  cueOutro->addAnimation(outro);
  loseYourselfShow.addCue(cueOutro);
}

void initTestShow() {
  uint16_t bpm = 12000;

  Cue* cue1 = new Cue(&scene1, 2000, bpm);
  auto wholeNotes = new PatternBeatAnimation("| W | w |", bpm, CRGB::Red);
  wholeNotes->addSegment(scene1Segment1, 50);
  cue1->addAnimation(wholeNotes);
  testShow.addCue(cue1);
  Serial.println("Init: Cue1");

  Cue* cueIntro = new Cue(&scene, 2000, bpm);
  auto piano1Intro = new PatternBeatAnimation("| Ee Ee Ee Ee | Ee Ee ee SsSs | Ee Ee Ee Ee | Ee Ee q q |", bpm, CRGB::Blue);
  auto piano2Intro = new PatternBeatAnimation("| eE eE eE eE | eE eE ee sSsS | eE eE eE eE | eE ee q q |", bpm, CRGB::Blue);
  piano1Intro->addSegment(piano1Segment, 100);
  piano2Intro->addSegment(piano2Segment, 100);
  cueIntro->addAnimation(piano1Intro);
  cueIntro->addAnimation(piano2Intro);
  testShow.addCue(cueIntro);

  Cue* cue1point5 = new Cue(&scene1, 4000, bpm);
  auto halfNotesChase = new TheatreChaseBeatAnimation("| SSS |", bpm, CRGB::DarkOrange, false);
  halfNotesChase->addSegment(scene1Segment2, 50);
  halfNotesChase->addSegment(scene1Segment3, 50);
  halfNotesChase->addSegment(scene1Segment4, 50);
  halfNotesChase->addSegment(scene1Segment5, 50);
  halfNotesChase->addSegment(scene1Segment6, 50);
  cue1point5->addAnimation(wholeNotes);
  cue1point5->addAnimation(halfNotesChase);
  testShow.addCue(cue1point5);
  Serial.println("Init: Cue2");

  Cue* cue1point7 = new Cue(&scene1, 4000, bpm);
  auto halfNotesReverseChase = new TheatreChaseBeatAnimation("| SSS |", bpm, CRGB::Purple, true);
  halfNotesReverseChase->addSegment(scene1Segment2, 50);
  cue1point7->addAnimation(halfNotesReverseChase);
  testShow.addCue(cue1point7);

  Cue* cue1point8 = new Cue(&scene1, 4000, bpm);
  auto segmentChase = new SegmentTheatreChaseBeatAnimation("| SSS |", bpm, CRGB::Blue);
  segmentChase->addSegment(scene1Segment3, 50);
  segmentChase->addSegment(scene1Segment2, 50);
  segmentChase->addSegment(scene1Segment6, 50);
  segmentChase->addSegment(scene1Segment7, 50);
  segmentChase->addSegment(scene1Segment5, 50);
  segmentChase->addSegment(scene1Segment4, 50);
  cue1point8->addAnimation(segmentChase);
  testShow.addCue(cue1point8);

  Cue* cue1point9 = new Cue(&scene1, 4000, bpm);
  auto segmentChase2 = new SegmentTheatreChaseBeatAnimation("| SSS |", bpm, CRGB::Green, true);
  segmentChase2->addSegment(scene1Segment3, 50);
  segmentChase2->addSegment(scene1Segment2, 50);
  segmentChase2->addSegment(scene1Segment6, 50);
  segmentChase2->addSegment(scene1Segment7, 50);
  segmentChase2->addSegment(scene1Segment5, 50);
  segmentChase2->addSegment(scene1Segment4, 50);
  cue1point9->addAnimation(segmentChase2);
  testShow.addCue(cue1point9);

  Cue* cue2 = new Cue(&scene1, 10000, bpm);
  auto halfNotes = new PatternBeatAnimation("| H h |", bpm, CRGB::DarkOrange);
  halfNotes->addSegment(scene1Segment2, 50);
  cue2->addAnimation(wholeNotes);
  cue2->addAnimation(halfNotes);
  testShow.addCue(cue2);
  Serial.println("Init: Cue2");


  Cue* cue3 = new Cue(&scene1, 10000, bpm);
  auto quarterNotes = new PatternBeatAnimation("| Q q Q q |", bpm, CRGB::Yellow);
  quarterNotes->addSegment(scene1Segment3, 50);
  cue3->addAnimation(wholeNotes);
  cue3->addAnimation(halfNotes);
  cue3->addAnimation(quarterNotes);
  testShow.addCue(cue3);
  Serial.println("Init: Cue2");

  Cue* cue4 = new Cue(&scene1, 10000, bpm);
  auto eigthNotes = new PatternBeatAnimation("| Ee Ee Ee Ee |", bpm, CRGB::Green);
  eigthNotes->addSegment(scene1Segment4, 50);
  cue4->addAnimation(wholeNotes);
  cue4->addAnimation(halfNotes);
  cue4->addAnimation(quarterNotes);
  cue4->addAnimation(eigthNotes);
  testShow.addCue(cue4);
  Serial.println("Init: Cue2");

  Cue* cue5 = new Cue(&scene1, 10000, bpm);
  auto sixteenthNotes = new PatternBeatAnimation("| SsSs SsSs SsSs SsSs |", bpm, CRGB::Blue);
  sixteenthNotes->addSegment(scene1Segment5, 50);
  cue5->addAnimation(wholeNotes);
  cue5->addAnimation(halfNotes);
  cue5->addAnimation(quarterNotes);
  cue5->addAnimation(eigthNotes);
  cue5->addAnimation(sixteenthNotes);
  testShow.addCue(cue5);
  Serial.println("Init: Cue2");

  Cue* cue6 = new Cue(&scene1, 10000, bpm);
  auto thirtysecondNotes = new PatternBeatAnimation("| TtTtTtTt TtTtTtTt TtTtTtTt TtTtTtTt |", bpm, CRGB::Purple);
  thirtysecondNotes->addSegment(scene1Segment6, 50);
  cue6->addAnimation(wholeNotes);
  cue6->addAnimation(halfNotes);
  cue6->addAnimation(quarterNotes);
  cue6->addAnimation(eigthNotes);
  cue6->addAnimation(sixteenthNotes);
  cue6->addAnimation(thirtysecondNotes);
  testShow.addCue(cue6);
  Serial.println("Init: Cue2");

  for (int i = 0; i < 9; i++) {
    testShow.addCue(cue1);
    testShow.addCue(cue2);
    testShow.addCue(cue3);
    testShow.addCue(cue4);
    testShow.addCue(cue5);
    testShow.addCue(cue6);
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Setup");
  FastLED.addLeds<WS2812B, 21, RGB>(strip1, STRIP1_LEDS);
  FastLED.addLeds<WS2812B, 22, RGB>(strip2, STRIP2_LEDS);
  FastLED.setBrightness(51); // ~20%
  initSegments();
  Serial.println("Init: Segments");
  initScene1Segments();
  Serial.println("Init: Scene1Segments");
  initLoseYourselfShow();
  initTestShow();
  Serial.println("Init: TestShow");
  Serial.println("Ready. Send 's' to start the show or 'T:<ms>' to sync.");
}

bool parsePrefixedInt(const char* input, const char* prefix, uint32_t& outValue) {
  size_t prefixLen = strlen(prefix);
  if (strncmp(input, prefix, prefixLen) != 0) return false;

  const char* digits = input + prefixLen;
  if (*digits == '\0') return false; // No number after prefix

  // Ensure all remaining characters are digits
  for (const char* p = digits; *p; ++p) {
    if (!isdigit(*p)) return false;
  }

  outValue = strtoul(digits, NULL, 10);
  return true;
}

void loop() {
  if (Serial.available()) {
    size_t len = Serial.readBytesUntil('\n', serialBuffer, sizeof(serialBuffer) - 1);
    serialBuffer[len] = '\0';

    Serial.print("[Serial] Received: ");
    Serial.println(serialBuffer);

    if (len > 2 && strncmp(serialBuffer, "T:", 2) == 0) {
      uint32_t reportedMillis;
      if (parsePrefixedInt(serialBuffer, "T:", reportedMillis)) {
        Serial.print("Parsed T: ");
        Serial.println(reportedMillis);
      } else {
        Serial.println("[Serial] Ignored: Invalid T:<ms> format");
      }
    } else if (len > 2 && strncmp(serialBuffer, "D:", 2) == 0) {
      uint32_t dimmerSetting;
      if (parsePrefixedInt(serialBuffer, "D:", dimmerSetting) && dimmerSetting >= 0 && dimmerSetting <256 ) {
        Serial.print("Parsed D: ");
        Serial.println(dimmerSetting);
        FastLED.setBrightness(dimmerSetting);
      } else {
        Serial.println("[Serial] Ignored: Invalid D:[0-255] format");
      }
    } else if (len == 1 && (serialBuffer[0] == 's' || serialBuffer[0] == 'S')) {
      showStartMillis = millis();
      lastLogMillis = millis();
      showStarted = true;
      for (uint8_t i = 0; i < STRIP1_LEDS; i++) {
        strip1[i] = CRGB::Black;
      }
      Serial.println("Show started!");
    } else if (len == 1 && (serialBuffer[0] == 'x' || serialBuffer[0] == 'X')) {
      showStartMillis = 0;
      lastLogMillis = millis();
      showStarted = false;
      for (uint8_t i = 0; i < STRIP1_LEDS; i++) {
        strip1[i] = CRGB::Black;
      }
      Serial.println("Show stopped!");
    }
  }

  if (showStarted) {
    unsigned long elapsedMillis = millis() - showStartMillis;
    // loseYourselfShow.update(elapsedMillis);
    testShow.update(elapsedMillis);

    if (millis() - lastLogMillis >= 1000) {
      lastLogMillis += 1000;
      Serial.print("Show time: ");
      Serial.print(elapsedMillis / 1000);
      Serial.println("s");
    }
  }

  FastLED.show();
} // end of loop