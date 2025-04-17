#include <FastLED.h>

#define STRIP1_LEDS 248
#define STRIP2_LEDS 29

CRGB strip1[STRIP1_LEDS];
CRGB strip2[STRIP2_LEDS];
CRGB* strips[] = {strip1, strip2};

#include <Show.h>
#include <Pallets.h>

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
  // Serial.print("currentMillis: ");
  // Serial.println(currentMillis);
  // Serial.print("reportedMillis: ");
  // Serial.println(reportedMillis);
  // Serial.print("showStartMillis: ");
  // Serial.println(showStartMillis);
  // Serial.print("localElapsedMillis: ");
  // Serial.println(localElapsedMillis);
  // Serial.print("drift: ");
  // Serial.println(drift);

  // Limit correction to ±100ms
  // if (drift > 100L) drift = 100L;
  // if (drift < -100L) drift = -100L;
  // Serial.print("clamped drift: ");
  // Serial.println(drift);

  showStartMillis -= drift;
  // Serial.print("new showStartMillis: ");
  // Serial.println(showStartMillis);

  Serial.print("Drift correction applied: ");
  Serial.println(drift);
  // Serial.println(" ms");
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

#include <PatternBeatAnimation.h>
#include <SegmentTheatreChaseBeatAnimation.h>
#include <SegmentRainbowChaseAnimation.h>
#include <TheatreChaseBeatAnimation.h>
#include <TheatreChasePaletteBeatAnimation.h>
#include <RandomSparkleBeatAnimation.h>

// ----- Show Definition: "Lose Yourself" -----

Scene scene, scene1, scene2, scene3;
Segment *piano1Segment, *piano2Segment, *drumSegment, *baseSegment, *stringSegment;
Segment *scene1Segment1, *scene1Segment2, *scene1Segment3, *scene1Segment4, *scene1Segment5, *scene1Segment6, *scene1Segment7;
Segment *scene2Segment1;
Segment *MDSegment, *MCSegment, *LASegment, *SBSegment, *SASegment, *TESegment, *TDSegment, *TCSegment, *TBSegment, *TASegment, *LBSegment, *MASegment, *SCSegment, *SDSegment, *MBSegment, *TFSegment;
Show loseYourselfShow, testShow, ambianceShow, weAreYourFriendsShow;

void initStripSegments() {
  const uint16_t ledsMD[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28};
  StripLEDConfig ledsMDCfg[] = {{1, ledsMD, sizeof(ledsMD) / sizeof(ledsMD[0])}};
  MDSegment = scene3.createSegment(ledsMDCfg, 1);
  const uint16_t ledsMC[] = {29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57};
  StripLEDConfig ledsMCCfg[] = {{1, ledsMC, sizeof(ledsMC) / sizeof(ledsMC[0])}};
  MCSegment = scene3.createSegment(ledsMCCfg, 1);
  const uint16_t ledsLA[] = {58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93};
  StripLEDConfig ledsLACfg[] = {{1, ledsLA, sizeof(ledsLA) / sizeof(ledsLA[0])}};
  LASegment = scene3.createSegment(ledsLACfg, 1);
  const uint16_t ledsSB[] = {107,106,105,104,103,102,101,100,99,98,97,96,95,94};
  StripLEDConfig ledsSBCfg[] = {{1, ledsSB, sizeof(ledsSB) / sizeof(ledsSB[0])}};
  SBSegment = scene3.createSegment(ledsSBCfg, 1);
  const uint16_t ledsSA[] = {108,109,110,111,112,113,114,115,116,117,118,119,120,121};
  StripLEDConfig ledsSACfg[] = {{1, ledsSA, sizeof(ledsSA) / sizeof(ledsSA[0])}};
  SASegment = scene3.createSegment(ledsSACfg, 1);
  // const uint16_t ledsTE[] = {};
  // StripLEDConfig ledsTECfg[] = {{1, ledsTE, sizeof(ledsTE) / sizeof(ledsTE[0])}};
  // TESegment = scene3.createSegment(ledsTECfg, 1);
  const uint16_t ledsTD[] = {122,123};
  StripLEDConfig ledsTDCfg[] = {{1, ledsTD, sizeof(ledsTD) / sizeof(ledsTD[0])}};
  TDSegment = scene3.createSegment(ledsTDCfg, 1);
  // const uint16_t ledsTC[] = {};
  // StripLEDConfig ledsTCCfg[] = {{1, ledsTC, sizeof(ledsTC) / sizeof(ledsTC[0])}};
  // TCSegment = scene3.createSegment(ledsTCCfg, 1);
  // const uint16_t ledsTB[] = {};
  // StripLEDConfig ledsTBCfg[] = {{1, ledsTB, sizeof(ledsTB) / sizeof(ledsTB[0])}};
  // TBSegment = scene3.createSegment(ledsTBCfg, 1);
  // const uint16_t ledsTA[] = {};
  // StripLEDConfig ledsTACfg[] = {{1, ledsTA, sizeof(ledsTA) / sizeof(ledsTA[0])}};
  // TASegment = scene3.createSegment(ledsTACfg, 1);
  const uint16_t ledsLB[] = {124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159};
  StripLEDConfig ledsLBCfg[] = {{1, ledsLB, sizeof(ledsLB) / sizeof(ledsLB[0])}};
  LBSegment = scene3.createSegment(ledsLBCfg, 1);
  const uint16_t ledsMA[] = {160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188};
  StripLEDConfig ledsMACfg[] = {{1, ledsMA, sizeof(ledsMA) / sizeof(ledsMA[0])}};
  MASegment = scene3.createSegment(ledsMACfg, 1);
  const uint16_t ledsSC[] = {189,190,191,192,193,194,195,196,197,198,199,200,201,202};
  StripLEDConfig ledsSCCfg[] = {{1, ledsSC, sizeof(ledsSC) / sizeof(ledsSC[0])}};
  SCSegment = scene3.createSegment(ledsSCCfg, 1);
  const uint16_t ledsSD[] = {203,204,205,206,207,208,209,210,211,212,213,214,215,216};
  StripLEDConfig ledsSDCfg[] = {{1, ledsSD, sizeof(ledsSD) / sizeof(ledsSD[0])}};
  SDSegment = scene3.createSegment(ledsSDCfg, 1);
  const uint16_t ledsMB[] = {217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245};
  StripLEDConfig ledsMBCfg[] = {{1, ledsMB, sizeof(ledsMB) / sizeof(ledsMB[0])}};
  MBSegment = scene3.createSegment(ledsMBCfg, 1);
  const uint16_t ledsTF[] = {246,247};
  StripLEDConfig ledsTFCfg[] = {{1, ledsTF, sizeof(ledsTF) / sizeof(ledsTF[0])}};
  TFSegment = scene3.createSegment(ledsTFCfg, 1);
}

void initScene2Segments() {
  const uint16_t leds1[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247};
  StripLEDConfig leds1Cfg[] = {{1, leds1, sizeof(leds1) / sizeof(leds1[0])}};
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
  uint16_t bpm = 100;

  Cue* introCue = new Cue(&scene2, 600.0, bpm);
  // auto intro = new PatternBeatAnimation("| W W |", bpm, CRGB(0, 0, 255)); // Pure Blue
  // auto intro = new PatternBeatAnimation("| W W |", bpm, CRGB(0, 148, 255)); // Teal Blue
  auto intro = new PatternBeatAnimation("| W W |", bpm, CRGB(200, 0, 100)); // Pink
  // auto intro = new TheatreChasePaletteBeatAnimation("| SSS |", bpm, RainbowColors_p, false);
  intro->addSegment(scene2Segment1, 100);
  introCue->addAnimation(intro);
  ambianceShow.addCue(introCue);
  Serial.println("Init: Cue1");

}

void initWeAreYourFriendsShow() {
  uint16_t bpm = 12293;

  Cue* introCue = new Cue(&scene, 43800UL, bpm);
  auto introAnimation = new RandomSparkleBeatAnimation("| QQQQ |", bpm, WhitePalette(), 10, 3, 100, 3, 3, true);
  introAnimation->addSegment(TFSegment, 50);
  introCue->addAnimation(introAnimation);
  weAreYourFriendsShow.addCue(introCue);

  Cue* wholeStarsCue = new Cue(&scene2, 16.0, bpm);
  auto wholeStarsAnimation = new RandomSparkleBeatAnimation("| W | W |", bpm, WhitePalette(), 30, 2, 100, 3, 3, true);
  wholeStarsAnimation->addSegment(LASegment, 50);
  wholeStarsCue->addAnimation(wholeStarsAnimation);
  weAreYourFriendsShow.addCue(wholeStarsCue);

  Cue* halfStarsCue = new Cue(&scene2, 8.0, bpm);
  auto halfStarsAnimation = new RandomSparkleBeatAnimation("| H H |", bpm, make2StopGradient(CRGB(0, 0, 68), CRGB(48, 0, 72)), 30, 2, 100, 3, 3, true); // Dark Blue to Deep Purple
  halfStarsAnimation->addSegment(LBSegment, 20);
  halfStarsCue->addAnimation(halfStarsAnimation);
  halfStarsCue->addAnimation(wholeStarsAnimation);
  weAreYourFriendsShow.addCue(halfStarsCue);

  Cue* quarterStarsCue = new Cue(&scene2, 16.0, bpm);
  auto quarterStarsAnimation = new RandomSparkleBeatAnimation("| Q Q Q Q |", bpm, make2StopGradient(CRGB(48, 0, 72), CRGB(200, 0, 100)), 30, 3, 100, 3, 3, true); // Deep Purple to Cosmic Pink
  quarterStarsAnimation->addSegment(SCSegment, 20);
  quarterStarsAnimation->addSegment(SDSegment, 20);
  quarterStarsAnimation->addSegment(MASegment, 20);
  quarterStarsAnimation->addSegment(MBSegment, 20);
  quarterStarsAnimation->addSegment(MCSegment, 20);
  quarterStarsAnimation->addSegment(MDSegment, 20);
  quarterStarsAnimation->addSegment(TFSegment, 20);
  quarterStarsCue->addAnimation(quarterStarsAnimation);
  quarterStarsCue->addAnimation(halfStarsAnimation);
  quarterStarsCue->addAnimation(wholeStarsAnimation);
  weAreYourFriendsShow.addCue(quarterStarsCue);

  Cue* eighthStarsCue = new Cue(&scene2, 5.0, bpm);
  auto eighthStarsAnimation = new RandomSparkleBeatAnimation("| EE EE EE EE | EE |", bpm, make2StopGradient(CRGB(200, 0, 100), CRGB(0, 200, 150)), 35, 1, 100, 3, 3, true); // Cosmic Pink to Electric Teal
  eighthStarsAnimation->addSegment(SASegment, 20);
  eighthStarsAnimation->addSegment(SBSegment, 20);
  eighthStarsAnimation->addSegment(TDSegment, 20);
  eighthStarsCue->addAnimation(eighthStarsAnimation);
  eighthStarsCue->addAnimation(quarterStarsAnimation);
  eighthStarsCue->addAnimation(halfStarsAnimation);
  eighthStarsCue->addAnimation(wholeStarsAnimation);
  weAreYourFriendsShow.addCue(eighthStarsCue);

  Cue* chorus1Cue1 = new Cue(&scene2, 7.0, bpm);
  Cue* chorus1Cue2 = new Cue(&scene2, 28.0, bpm);
  auto chorus1BecauseWeAnimation = new TheatreChasePaletteBeatAnimation("q sS sS | ss q h |", bpm, make4StopGradient(CRGB(0, 0, 68), CRGB(48, 0, 72), CRGB(200, 0, 100), CRGB(0, 200, 150)), true, 1);
  auto chorus1CymbalCrashAnimation = new TheatreChasePaletteBeatAnimation("q h | Q q h |", bpm, make4StopGradient(CRGB(0, 0, 68), CRGB(48, 0, 72), CRGB(200, 0, 100), CRGB(0, 200, 150)), true, 1);
  auto chorus1SynthLeftAnimation = new TheatreChasePaletteBeatAnimation("| EeEeEeEe | qqqq | EeEeEeEe | qqqq | EeEeEeEe | qqqq | EeEeEeEe |", bpm, make4StopGradient(CRGB(0, 0, 68), CRGB(48, 0, 72), CRGB(200, 0, 100), CRGB(0, 200, 150)), true, 1);
  auto chorus1SynthRightAnimation = new TheatreChasePaletteBeatAnimation("| qqqq | EeEeEeEe | qqqq | EeEeEeEe | qqqq | EeEeEeEe | qqqq | ", bpm, make4StopGradient(CRGB(0, 0, 68), CRGB(48, 0, 72), CRGB(200, 0, 100), CRGB(0, 200, 150)), true, 1);
  auto chorus1Base12Animation = new TheatreChasePaletteBeatAnimation("| EeEeEEEE | EeEeEEEE | EeEeEEEE | EeEeEEEE | EeEeEEEE | EeEeEEEE | EeEeEEEE | ", bpm, CRGBPalette16(CRGB(0, 0, 68)), true, 2);
  auto chorus1Base3Animation = new TheatreChasePaletteBeatAnimation("| qqEEEE | qqEEEE | qqEEEE | qqEEEE | qqEEEE | qqEEEE | qqEEEE | ", bpm, make2StopGradient(CRGB(0, 0, 68), CRGB(48, 0, 72)), true, 3);
  auto chorus1Base3AndAnimation = new TheatreChasePaletteBeatAnimation("| qqeEEE | qqeEEE | qqeEEE | qqeEEE | qqeEEE | qqeEEE | qqeEEE | ", bpm, make2StopGradient(CRGB(0, 0, 68), CRGB(48, 0, 72)), false, 3);
  auto chorus1Base4Animation = new TheatreChasePaletteBeatAnimation("| qqeeEE | qqeeEE | qqeeEE | qqeeEE | qqeeEE | qqeeEE | qqeeEE | ", bpm, make2StopGradient(CRGB(48, 0, 72), CRGB(200, 0, 100)), false, 3);
  auto chorus1Base4AndAnimation = new TheatreChasePaletteBeatAnimation("| qqeeeE | qqeeeE | qqeeeE | qqeeeE | qqeeeE | qqeeeE | qqeeeE | ", bpm, make2StopGradient(CRGB(200, 0, 100), CRGB(0, 200, 150)), false, 3);
  chorus1BecauseWeAnimation->addSegment(LASegment, 70);
  chorus1BecauseWeAnimation->addSegment(LBSegment, 70);
  chorus1BecauseWeAnimation->addSegment(SASegment, 70);
  chorus1BecauseWeAnimation->addSegment(SBSegment, 70);
  chorus1CymbalCrashAnimation->addSegment(MBSegment, 70);
  chorus1CymbalCrashAnimation->addSegment(SDSegment, 70);
  chorus1CymbalCrashAnimation->addSegment(TFSegment, 70);
  chorus1SynthLeftAnimation->addSegment(MASegment, 20);
  chorus1SynthLeftAnimation->addSegment(SCSegment, 20);
  chorus1SynthRightAnimation->addSegment(MCSegment, 20);
  chorus1SynthRightAnimation->addSegment(MDSegment, 20);
  // chorus1Base12Animation->addSegment(TASegment, 20);
  // chorus1Base12Animation->addSegment(TBSegment, 20);
  // chorus1Base12Animation->addSegment(TCSegment, 20);
  chorus1Base12Animation->addSegment(TDSegment, 20);
  // chorus1Base12Animation->addSegment(TESegment, 20);
  chorus1Base3Animation->addSegment(SASegment, 20);
  chorus1Base3AndAnimation->addSegment(SBSegment, 20);
  chorus1Base4Animation->addSegment(LBSegment, 20);
  chorus1Base4AndAnimation->addSegment(LASegment, 20);
  chorus1Cue1->addAnimation(chorus1BecauseWeAnimation);
  chorus1Cue1->addAnimation(chorus1CymbalCrashAnimation);
  chorus1Cue2->addAnimation(chorus1SynthLeftAnimation);
  chorus1Cue2->addAnimation(chorus1SynthRightAnimation);
  chorus1Cue2->addAnimation(chorus1Base12Animation);
  chorus1Cue2->addAnimation(chorus1Base3Animation);
  chorus1Cue2->addAnimation(chorus1Base3AndAnimation);
  chorus1Cue2->addAnimation(chorus1Base4Animation);
  chorus1Cue2->addAnimation(chorus1Base4AndAnimation);
  weAreYourFriendsShow.addCue(chorus1Cue1);
  weAreYourFriendsShow.addCue(chorus1Cue2);

}

void initLoseYourselfShow() {
  uint16_t bpmIntro = 6593;
  uint16_t bpmMain  = 8572;

  Cue* silence = new Cue(&scene3, 2800UL, bpmIntro);
  loseYourselfShow.addCue(silence);

  Cue* cueIntro = new Cue(&scene3, 8.0*4, bpmIntro);
  auto piano1Intro = new TheatreChaseBeatAnimation("| EE EE EE EE | EE EE ee SSSS | EE EE EE EE | EE Ee q q | EE EE EE ESS | Ee ee ee EE | EE EE EE Q | Q ee q q |", bpmIntro, CRGB::Blue, 2);
  piano1Intro->addSegment(MDSegment, 100);
  piano1Intro->addSegment(MCSegment, 100);
  piano1Intro->addSegment(MASegment, 100);
  piano1Intro->addSegment(SCSegment, 100);
  cueIntro->addAnimation(piano1Intro);
  loseYourselfShow.addCue(cueIntro);

  Cue* cueVerse1 = new Cue(&scene3, 16.0*4, bpmMain);
  auto cueVerse1drums1 = new PatternBeatAnimation("| E e E e E e E e | e e e e e e e t t T t |", bpmMain, CRGB::Red);
  auto cueVerse1drums2 = new PatternBeatAnimation("| e e e e e e e e | E e E e E e E t t T t |", bpmMain, CRGB::OrangeRed);
  auto base1 = new PatternBeatAnimation("| wwwwwww | h q esTt | SsSs q h | SsSs q q esTt | Sse eSs Ssss q | Ee q SsSs esTt | SsSs q SsSs eSs | Sse q SsSs eTtTt | SsSs q SsSs | eE eE SsSs SsSs |  ", bpmMain, CRGB::Purple);
  cueVerse1drums1->addSegment(LASegment, 75);
  cueVerse1drums2->addSegment(LBSegment, 50);
  base1->addSegment(SBSegment, 50);
  base1->addSegment(SASegment, 50);
  base1->addSegment(TDSegment, 50);
  cueVerse1->addAnimation(cueVerse1drums1);
  cueVerse1->addAnimation(cueVerse1drums2);
  cueVerse1->addAnimation(base1);
  loseYourselfShow.addCue(cueVerse1);

  Cue* cueChorus1 = new Cue(&scene3, 8.0*4, bpmMain);
  auto cueChorus1drums1 = new PatternBeatAnimation("| E e E e E e E e | E e E e E e E t t T t |", bpmMain, CRGB::Red);
  auto strings1 = new PatternBeatAnimation("| Q Q q q | Q Q q q |", bpmMain, CRGB::Yellow);
  auto base2 = new PatternBeatAnimation("| TtsTts TtsTts TtsTts TtsTts|  ", bpmMain, CRGB::Purple);
  auto cueChorus1lyrics1 = new TheatreChasePaletteBeatAnimation("| EEE |", bpmMain, RainbowColors_p, false);
  strings1->addSegment(MBSegment, 50);
  strings1->addSegment(SDSegment, 50);
  base2->addSegment(SBSegment, 100);
  base2->addSegment(SASegment, 100);
  base2->addSegment(TDSegment, 100);
  cueChorus1lyrics1->addSegment(LASegment, 100);
  cueChorus1lyrics1->addSegment(LBSegment, 100);
  cueChorus1->addAnimation(cueChorus1drums1);
  cueChorus1->addAnimation(piano1Intro);
  cueChorus1->addAnimation(strings1);
  cueChorus1->addAnimation(base2);
  cueChorus1->addAnimation(cueChorus1lyrics1);
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
  auto halfNotesChase = new TheatreChaseBeatAnimation("| SSSS |", bpm, CRGB::DarkOrange, 3, false);
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
  initStripSegments();
  Serial.println("Init: Scene1Segments");
  initLoseYourselfShow();
  initTestShow();
  initAmbianceShow();


  initWeAreYourFriendsShow();
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
        if (showStarted) syncToSerialTime(reportedMillis);
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
        case 4:
          activeShow = weAreYourFriendsShow;
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