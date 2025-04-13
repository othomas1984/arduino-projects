#include <FastLED.h>

#define STRIP1_LEDS 248
#define STRIP2_LEDS 29

CRGB strip1[STRIP1_LEDS];
CRGB strip2[STRIP2_LEDS];
CRGB* strips[] = {strip1, strip2};

#include <Show.h>

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
    case 'F': case 'f': return beat / 16;
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

// ----- SegmentRainbowChaseAnimation with bpm100 -----

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
  uint8_t spacing;

  TheatreChaseBeatAnimation(const char* pattern, uint16_t bpm100, CRGB color, uint8_t spacing, bool isReversed = false)
    : pattern(pattern), spacing(spacing), bpm100(bpm100), color(color), totalDuration(0), isReversed(isReversed) {
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

// ----- TheatreChasePaletteBeatAnimation with bpm100 -----

class TheatreChasePaletteBeatAnimation : public Animation {
public:
  const char* pattern;
  uint16_t bpm100;
  unsigned long totalDuration;
  uint8_t brightnessPercent = 100;
  uint8_t spacing = 3;
  bool isReversed;
  CRGBPalette16 palette;

  TheatreChasePaletteBeatAnimation(
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
};


// ----- Show Definition: "Lose Yourself" -----

Scene scene, scene1, scene2;
Segment *piano1Segment, *piano2Segment, *drumSegment, *baseSegment, *stringSegment;
Segment *scene1Segment1, *scene1Segment2, *scene1Segment3, *scene1Segment4, *scene1Segment5, *scene1Segment6, *scene1Segment7;
Segment *scene2Segment1;
Show loseYourselfShow, testShow, ambianceShow;

void initScene2Segments() {
  const uint16_t leds1[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247};
  StripLEDConfig leds1Cfg[] = {{1, leds1, 248}};
  scene2Segment1 = scene2.createSegment(leds1Cfg, 1);
}

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

void initAmbianceShow() {
  uint16_t bpm = 6000;

  Cue* introCue = new Cue(&scene2, 600.0, bpm);
  // auto intro = new PatternBeatAnimation("| W |", bpm, CRGB::Wheat);
  auto intro = new TheatreChasePaletteBeatAnimation("| SSS |", bpm, RainbowColors_p, false);
  intro->addSegment(scene2Segment1, 50);
  introCue->addAnimation(intro);
  ambianceShow.addCue(introCue);
  Serial.println("Init: Cue1");

  ambianceShow.repeats = true;
}


void initLoseYourselfShow() {
  uint16_t bpmIntro = 6593;
  uint16_t bpmMain  = 8572;

  Cue* silence = new Cue(&scene, 3000UL, bpmIntro);
  loseYourselfShow.addCue(silence);

  Cue* cueIntro = new Cue(&scene, 8.0*4, bpmIntro);
  auto piano1Intro = new PatternBeatAnimation("| Ee Ee Ee Ee | Ee Ee ee SsSs | Ee Ee Ee Ee | Ee Ee q q |", bpmIntro, CRGB::Blue);
  auto piano2Intro = new PatternBeatAnimation("| eE eE eE eE | eE eE ee sSsS | eE eE eE eE | eE ee q q |", bpmIntro, CRGB::Blue);
  piano1Intro->addSegment(piano1Segment, 100);
  piano2Intro->addSegment(piano2Segment, 100);
  cueIntro->addAnimation(piano1Intro);
  cueIntro->addAnimation(piano2Intro);
  loseYourselfShow.addCue(cueIntro);

  Cue* cueVerse1 = new Cue(&scene, 16.0*4, bpmMain);
  auto drums1 = new PatternBeatAnimation("| E e E e E e E e | E e E e E e E t t T t |", bpmMain, CRGB::Red);
  auto base1 = new PatternBeatAnimation("| wwwwwww | h q esTt | SsSs q h | SsSs q q esTt | Sse eSs Ssss q | Ee q SsSs esTt | SsSs q SsSs eSs | Sse q SsSs eTtTt | SsSs q SsSs | eE eE SsSs SsSs |  ", bpmMain, CRGB::Purple);
  drums1->addSegment(drumSegment, 100);
  base1->addSegment(baseSegment, 100);
  cueVerse1->addAnimation(drums1);
  cueVerse1->addAnimation(base1);
  loseYourselfShow.addCue(cueVerse1);

  Cue* cueChorus1 = new Cue(&scene, 8.0*4, bpmMain);
  auto strings1 = new PatternBeatAnimation("| Q Q q q | Q Q Q Q |", bpmMain, CRGB::Yellow);
  auto base2 = new PatternBeatAnimation("| TtsTts TtsTts TtsTts TtsTts|  ", bpmMain, CRGB::Purple);
  strings1->addSegment(stringSegment, 100);
  base2->addSegment(baseSegment, 100);
  cueChorus1->addAnimation(drums1);
  cueChorus1->addAnimation(piano1Intro);
  cueChorus1->addAnimation(strings1);
  cueChorus1->addAnimation(base2);
  loseYourselfShow.addCue(cueChorus1);

  Cue* cueVerse2 = new Cue(&scene, 40000UL, bpmMain);
  auto drums2 = new PatternBeatAnimation("| E e E e | E E |", bpmMain, CRGB::Red);
  drums2->addSegment(drumSegment, 100);
  cueVerse2->addAnimation(drums2);
  loseYourselfShow.addCue(cueVerse2);

  Cue* cueChorus2 = new Cue(&scene, 20000UL, bpmMain);
  auto strings2 = new PatternBeatAnimation("| Q Q q q | Q Q Q Q |", bpmMain, CRGB::Yellow);
  strings2->addSegment(stringSegment, 100);
  cueChorus2->addAnimation(strings2);
  loseYourselfShow.addCue(cueChorus2);

  Cue* cueOutro = new Cue(&scene, 1000UL, bpmIntro);
  auto outro = new PatternBeatAnimation("| q q q q | q q q q |", bpmIntro, CRGB::Blue);
  outro->addSegment(piano1Segment, 50);
  cueOutro->addAnimation(outro);
  loseYourselfShow.addCue(cueOutro);
}

void initTestShow() {
  uint16_t bpm = 12000;

  Cue* introCue = new Cue(&scene1, 8.0, bpm);
  auto intro = new PatternBeatAnimation("| H | h |", bpm, CRGB::Red);
  intro->addSegment(scene1Segment1, 50);
  intro->addSegment(scene1Segment2, 50);
  intro->addSegment(scene1Segment3, 50);
  intro->addSegment(scene1Segment4, 50);
  intro->addSegment(scene1Segment5, 50);
  intro->addSegment(scene1Segment6, 50);
  intro->addSegment(scene1Segment7, 50);
  introCue->addAnimation(intro);
  testShow.addCue(introCue);
  Serial.println("Init: Cue1");

  Cue* cue1point5 = new Cue(&scene1, 8.0, bpm);
  auto halfNotesChase = new TheatreChaseBeatAnimation("| SSS |", bpm, CRGB::DarkOrange, false);
  halfNotesChase->addSegment(scene1Segment2, 50);
  halfNotesChase->addSegment(scene1Segment3, 50);
  halfNotesChase->addSegment(scene1Segment4, 50);
  halfNotesChase->addSegment(scene1Segment5, 50);
  halfNotesChase->addSegment(scene1Segment6, 50);
  halfNotesChase->addSegment(scene1Segment7, 50);
  cue1point5->addAnimation(halfNotesChase);
  testShow.addCue(cue1point5);
  Serial.println("Init: Cue2");

  Cue* cue1point7 = new Cue(&scene1, 8.0, bpm);
  CRGBPalette16 firePalette = CRGBPalette16(CRGB::Red, CRGB::Red, CRGB::Orange, CRGB::Yellow);
  auto halfNotesReverseFireChase = new TheatreChasePaletteBeatAnimation("| SSS |", bpm, firePalette, true);
  halfNotesReverseFireChase->addSegment(scene1Segment2, 50);
  halfNotesReverseFireChase->addSegment(scene1Segment3, 50);
  halfNotesReverseFireChase->addSegment(scene1Segment4, 50);
  halfNotesReverseFireChase->addSegment(scene1Segment5, 50);
  halfNotesReverseFireChase->addSegment(scene1Segment6, 50);
  halfNotesReverseFireChase->addSegment(scene1Segment7, 50);
  cue1point7->addAnimation(halfNotesReverseFireChase);
  testShow.addCue(cue1point7);
  
  Cue* cue1point75 = new Cue(&scene1, 8.0, bpm);
  auto halfNotesReverseRainbowChase = new TheatreChasePaletteBeatAnimation("| SSS |", bpm, RainbowColors_p, false);
  halfNotesReverseRainbowChase->addSegment(scene1Segment2, 50);
  halfNotesReverseRainbowChase->addSegment(scene1Segment3, 50);
  halfNotesReverseRainbowChase->addSegment(scene1Segment4, 50);
  halfNotesReverseRainbowChase->addSegment(scene1Segment5, 50);
  halfNotesReverseRainbowChase->addSegment(scene1Segment6, 50);
  halfNotesReverseRainbowChase->addSegment(scene1Segment7, 50);
  cue1point75->addAnimation(halfNotesReverseRainbowChase);
  testShow.addCue(cue1point75);

  Cue* cue1point8 = new Cue(&scene1, 8.0, bpm);
  auto segmentChase = new SegmentTheatreChaseBeatAnimation("| SSS |", bpm, CRGB::Blue);
  segmentChase->addSegment(scene1Segment1, 50);
  segmentChase->addSegment(scene1Segment2, 50);
  segmentChase->addSegment(scene1Segment3, 50);
  segmentChase->addSegment(scene1Segment4, 50);
  segmentChase->addSegment(scene1Segment5, 50);
  segmentChase->addSegment(scene1Segment6, 50);
  segmentChase->addSegment(scene1Segment7, 50);
  cue1point8->addAnimation(segmentChase);
  testShow.addCue(cue1point8);

  Cue* cue1point9 = new Cue(&scene1, 8.0, bpm);
  auto segmentChase2 = new SegmentRainbowChaseAnimation("| SSS |", bpm, RainbowColors_p, true);
  segmentChase2->addSegment(scene1Segment1, 50);
  segmentChase2->addSegment(scene1Segment2, 50);
  segmentChase2->addSegment(scene1Segment3, 50);
  segmentChase2->addSegment(scene1Segment4, 50);
  segmentChase2->addSegment(scene1Segment5, 50);
  segmentChase2->addSegment(scene1Segment6, 50);
  segmentChase2->addSegment(scene1Segment7, 50);
  cue1point9->addAnimation(segmentChase2);
  testShow.addCue(cue1point9);

  Cue* cue1 = new Cue(&scene1, 8.0, bpm);
  auto wholeNotes = new PatternBeatAnimation("| W | W |", bpm, CRGB::Red);
  wholeNotes->addSegment(scene1Segment1, 50);
  cue1->addAnimation(wholeNotes);
  testShow.addCue(cue1);
  Serial.println("Init: Cue1");

  Cue* cue2 = new Cue(&scene1, 8.0, bpm);
  auto halfNotes = new PatternBeatAnimation("| H h |", bpm, CRGB::Red);
  halfNotes->addSegment(scene1Segment2, 50);
  cue2->addAnimation(wholeNotes);
  cue2->addAnimation(halfNotes);
  testShow.addCue(cue2);
  Serial.println("Init: Cue2");


  Cue* cue3 = new Cue(&scene1, 8.0, bpm);
  auto quarterNotes = new PatternBeatAnimation("| Q q Q q |", bpm, CRGB::DarkOrange);
  quarterNotes->addSegment(scene1Segment3, 50);
  cue3->addAnimation(wholeNotes);
  cue3->addAnimation(halfNotes);
  cue3->addAnimation(quarterNotes);
  testShow.addCue(cue3);
  Serial.println("Init: Cue2");

  Cue* cue4 = new Cue(&scene1, 8.0, bpm);
  auto eigthNotes = new PatternBeatAnimation("| Ee Ee Ee Ee |", bpm, CRGB::Yellow);
  eigthNotes->addSegment(scene1Segment4, 50);
  cue4->addAnimation(wholeNotes);
  cue4->addAnimation(halfNotes);
  cue4->addAnimation(quarterNotes);
  cue4->addAnimation(eigthNotes);
  testShow.addCue(cue4);
  Serial.println("Init: Cue2");

  Cue* cue5 = new Cue(&scene1, 8.0, bpm);
  auto sixteenthNotes = new PatternBeatAnimation("| SsSs SsSs SsSs SsSs |", bpm, CRGB::Green);
  sixteenthNotes->addSegment(scene1Segment5, 50);
  cue5->addAnimation(wholeNotes);
  cue5->addAnimation(halfNotes);
  cue5->addAnimation(quarterNotes);
  cue5->addAnimation(eigthNotes);
  cue5->addAnimation(sixteenthNotes);
  testShow.addCue(cue5);
  Serial.println("Init: Cue2");

  Cue* cue6 = new Cue(&scene1, 8.0, bpm);
  auto thirtysecondNotes = new PatternBeatAnimation("| TtTtTtTt TtTtTtTt TtTtTtTt TtTtTtTt |", bpm, CRGB::Blue);
  thirtysecondNotes->addSegment(scene1Segment6, 50);
  cue6->addAnimation(wholeNotes);
  cue6->addAnimation(halfNotes);
  cue6->addAnimation(quarterNotes);
  cue6->addAnimation(eigthNotes);
  cue6->addAnimation(sixteenthNotes);
  cue6->addAnimation(thirtysecondNotes);
  testShow.addCue(cue6);
  Serial.println("Init: Cue2");
  
  Cue* cue7 = new Cue(&scene1, 8.0, bpm);
  auto sixtyFourthNotes = new PatternBeatAnimation("| FfFfFfFfFfFfFfFf FfFfFfFfFfFfFfFf FfFfFfFfFfFfFfFf FfFfFfFfFfFfFfFf |", bpm, CRGB::Purple);
  sixtyFourthNotes->addSegment(scene1Segment7, 50);
  cue7->addAnimation(wholeNotes);
  cue7->addAnimation(halfNotes);
  cue7->addAnimation(quarterNotes);
  cue7->addAnimation(eigthNotes);
  cue7->addAnimation(sixteenthNotes);
  cue7->addAnimation(thirtysecondNotes);
  cue7->addAnimation(sixtyFourthNotes);
  testShow.addCue(cue7);
  Serial.println("Init: Cue2");

  for (int i = 0; i < 9; i++) {
    testShow.addCue(cue1);
    testShow.addCue(cue2);
    testShow.addCue(cue3);
    testShow.addCue(cue4);
    testShow.addCue(cue5);
    testShow.addCue(cue6);
    testShow.addCue(cue7);
  }
}

Show activeShow;

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
  initScene2Segments();
  Serial.println("Init: Scene1Segments");
  initLoseYourselfShow();
  initTestShow();
  initAmbianceShow();
  Serial.println("Init: TestShow");
  Serial.println("Ready. Send 's' to start the show or 'T:<ms>' to sync.");

  activeShow = loseYourselfShow;
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
    } else if (len > 5 && strncmp(serialBuffer, "SHOW:", 5) == 0) {
      uint32_t showIndex;
      parsePrefixedInt(serialBuffer, "SHOW:", showIndex);
      Serial.print("Parsed Show: ");
      Serial.println(showIndex);
      switch (showIndex) {
        case 1:
          activeShow = loseYourselfShow;
          break;
        case 2:
          activeShow = testShow;
          break;
        case 3:
          activeShow = ambianceShow;
          break;
        default:
          Serial.println("[Serial] Ignored: Invalid SHOW:[1-3] format");
          break;
      }
      showStartMillis = millis();
      lastLogMillis = millis();
      showStarted = true;
      for (uint8_t i = 0; i < STRIP1_LEDS; i++) {
        strip1[i] = CRGB::Black;
      }
      Serial.println("Show started!");
    }
  }

  if (showStarted) {
    unsigned long elapsedMillis = millis() - showStartMillis;
    activeShow.update(elapsedMillis);

    if (millis() - lastLogMillis >= 1000) {
      lastLogMillis += 1000;
      Serial.print("Show time: ");
      Serial.print(elapsedMillis / 1000);
      Serial.println("s");
    }
  }

  FastLED.show();
} // end of loop