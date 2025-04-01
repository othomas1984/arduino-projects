#include <FastLED.h>

#define STRIP1_LEDS 67
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

  if (abs(drift) < 30) {
    // Ignore small drift
    Serial.print("Drift ignored (too small): ");
    Serial.print(drift);
    Serial.println(" ms");
    return;
  }
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

Scene scene;
Segment *pianoSegment, *drumSegment, *stringSegment;
Show loseYourselfShow;

void initSegments() {
  const uint16_t pianoLEDs[] = {0,1,2,3,4,5,6,7,8,9,
                                10,11,12,13,14,15,16,17,18,19,
                                20,21,22,23,24,25,26,27,28,29};
  StripLEDConfig pianoCfg[] = {{1, pianoLEDs, 30}};
  pianoSegment = scene.createSegment(pianoCfg, 1);

  const uint16_t drumLEDs1[] = {30,31,32,33,34,35,36,37,38,39,
                                40,41,42,43,44,45,46,47,48,49};
  const uint16_t drumLEDs2[] = {0,1,2,3,4,5,6,7,8,9};
  StripLEDConfig drumCfg[] = {
    {1, drumLEDs1, 20},
    {2, drumLEDs2, 10}
  };
  drumSegment = scene.createSegment(drumCfg, 2);

  const uint16_t stringLEDs1[] = {50,51,52,53,54,55,56,57,58,59,
                                  60,61,62,63,64,65,66};
  const uint16_t stringLEDs2[] = {15,16,17,18,19,20,21,22,23,24,25,26,27,28};
  StripLEDConfig stringCfg[] = {
    {1, stringLEDs1, 17},
    {2, stringLEDs2, 14}
  };
  stringSegment = scene.createSegment(stringCfg, 2);
}

void initShow() {
  uint16_t bpmIntro = 6600;
  uint16_t bpmMain  = 7900;

  Cue* cueIntro = new Cue(&scene, 32000, bpmIntro);
  auto pianoIntro = new PatternBeatAnimation("| Q q Q q | Q q Q q |", bpmIntro, CRGB::Blue);
  pianoIntro->addSegment(pianoSegment, 100);
  cueIntro->addAnimation(pianoIntro);
  loseYourselfShow.addCue(cueIntro);

  Cue* cueVerse1 = new Cue(&scene, 40000, bpmMain);
  auto drums1 = new PatternBeatAnimation("| E e E e | E E |", bpmMain, CRGB::Red);
  drums1->addSegment(drumSegment, 100);
  cueVerse1->addAnimation(drums1);
  loseYourselfShow.addCue(cueVerse1);

  Cue* cueChorus1 = new Cue(&scene, 20000, bpmMain);
  auto strings1 = new PatternBeatAnimation("| Q Q q q | Q Q Q Q |", bpmMain, CRGB::Yellow);
  strings1->addSegment(stringSegment, 100);
  cueChorus1->addAnimation(strings1);
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
  outro->addSegment(pianoSegment, 50);
  cueOutro->addAnimation(outro);
  loseYourselfShow.addCue(cueOutro);
}

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812B, 16, GRB>(strip1, STRIP1_LEDS);
  FastLED.addLeds<WS2812B, 17, GRB>(strip2, STRIP2_LEDS);
  FastLED.setBrightness(51); // ~20%
  initSegments();
  initShow();
  Serial.println("Ready. Send 's' to start the show or 'T:<ms>' to sync.");
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();

    if (!showStarted && (c == 's' || c == 'S')) {
      showStartMillis = millis();
      lastLogMillis = 0;
      showStarted = true;
      Serial.println("Show started!");
    } else if (c == '\n' || c == '\r') {
      serialBuffer[serialIndex] = '\0';
      if (strncmp(serialBuffer, "T:", 2) == 0 && serialIndex >= 5) {
        Serial.print("[Serial] Received: ");
        Serial.println(serialBuffer);

        // Validate that what's after T: is numeric
        char* p = serialBuffer + 2;
        bool valid = true;
        while (*p) {
          if (!isdigit(*p)) {
            valid = false;
            break;
          }
          p++;
        }

        if (valid) {
          uint32_t reportedMillis = strtoul(serialBuffer + 2, NULL, 10);
          if (showStarted) syncToSerialTime(reportedMillis);
        } else {
          Serial.println("[Serial] Ignored: Invalid T:<ms> format");
        }
      }
      serialIndex = 0;
    } else if (serialIndex >= sizeof(serialBuffer) - 1) {
  Serial.println("[Serial] Warning: input overflow, discarding");
  serialIndex = 0;
} else {
      serialBuffer[serialIndex++] = c;
    }
  }

  if (showStarted) {
    unsigned long elapsedMillis = millis() - showStartMillis;
    loseYourselfShow.update(elapsedMillis);
    FastLED.show();

    if (millis() - lastLogMillis >= 1000) {
      lastLogMillis += 1000;
      Serial.print("Show time: ");
      Serial.print(elapsedMillis / 1000);
      Serial.println("s");
    }
  }
}
