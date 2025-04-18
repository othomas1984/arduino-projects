#include <FastLED.h>

#define STRIP1_LEDS 256
#define STRIP2_LEDS 29
#define BUTTON_PIN 22

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
    case 'D': case 'd': return beat * 4 * 16;
    case 'C': case 'c': return beat * 4 * 8;
    case 'B': case 'b': return beat * 4 * 4;
    case 'A': case 'a': return beat * 4 * 2;
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
#include <SmoothPaletteFadeBeatAnimation.h>

// ----- Show Definition: "Lose Yourself" -----

Scene scene, scene1, scene2, scene3, scene4;
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
  const uint16_t ledsTE[] = {122,123};
  StripLEDConfig ledsTECfg[] = {{1, ledsTE, sizeof(ledsTE) / sizeof(ledsTE[0])}};
  TESegment = scene3.createSegment(ledsTECfg, 1);
  const uint16_t ledsTD[] = {124,125};
  StripLEDConfig ledsTDCfg[] = {{1, ledsTD, sizeof(ledsTD) / sizeof(ledsTD[0])}};
  TDSegment = scene3.createSegment(ledsTDCfg, 1);
  const uint16_t ledsTC[] = {126,127};
  StripLEDConfig ledsTCCfg[] = {{1, ledsTC, sizeof(ledsTC) / sizeof(ledsTC[0])}};
  TCSegment = scene3.createSegment(ledsTCCfg, 1);
  const uint16_t ledsTB[] = {128,129};
  StripLEDConfig ledsTBCfg[] = {{1, ledsTB, sizeof(ledsTB) / sizeof(ledsTB[0])}};
  TBSegment = scene3.createSegment(ledsTBCfg, 1);
  const uint16_t ledsTA[] = {130,131};
  StripLEDConfig ledsTACfg[] = {{1, ledsTA, sizeof(ledsTA) / sizeof(ledsTA[0])}};
  TASegment = scene3.createSegment(ledsTACfg, 1);
  const uint16_t ledsLB[] = {132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167};
  StripLEDConfig ledsLBCfg[] = {{1, ledsLB, sizeof(ledsLB) / sizeof(ledsLB[0])}};
  LBSegment = scene3.createSegment(ledsLBCfg, 1);
  const uint16_t ledsMA[] = {168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196};
  StripLEDConfig ledsMACfg[] = {{1, ledsMA, sizeof(ledsMA) / sizeof(ledsMA[0])}};
  MASegment = scene3.createSegment(ledsMACfg, 1);
  const uint16_t ledsSC[] = {197,198,199,200,201,202,203,204,205,206,207,208,209,210};
  StripLEDConfig ledsSCCfg[] = {{1, ledsSC, sizeof(ledsSC) / sizeof(ledsSC[0])}};
  SCSegment = scene3.createSegment(ledsSCCfg, 1);
  const uint16_t ledsSD[] = {211,212,213,214,215,216,217,218,219,220,221,222,223,224};
  StripLEDConfig ledsSDCfg[] = {{1, ledsSD, sizeof(ledsSD) / sizeof(ledsSD[0])}};
  SDSegment = scene3.createSegment(ledsSDCfg, 1);
  const uint16_t ledsMB[] = {225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253};
  StripLEDConfig ledsMBCfg[] = {{1, ledsMB, sizeof(ledsMB) / sizeof(ledsMB[0])}};
  MBSegment = scene3.createSegment(ledsMBCfg, 1);
  const uint16_t ledsTF[] = {254,255};
  StripLEDConfig ledsTFCfg[] = {{1, ledsTF, sizeof(ledsTF) / sizeof(ledsTF[0])}};
  TFSegment = scene3.createSegment(ledsTFCfg, 1);
}

Segment *CenterVerticalRow1Segment, *CenterVerticalRow2Segment, *CenterVerticalRow3Segment, *CenterVerticalRow4Segment, *CenterVerticalRow5Segment, *CenterVerticalRow6Segment,
  *Foo;

void initRowSegments() {
  const uint16_t ledsCenterVerticalRow1[] = {
    58,59,60,61,62,63,64,65, // LA
    91,92,93, // LA
    143,144,145,146,147,148,149, // LB
  };
  StripLEDConfig ledsCenterVerticalRow1Cfg[] = {{1, ledsCenterVerticalRow1, sizeof(ledsCenterVerticalRow1) / sizeof(ledsCenterVerticalRow1[0])}};
  CenterVerticalRow1Segment = scene4.createSegment(ledsCenterVerticalRow1Cfg, 1);

  const uint16_t ledsCenterVerticalRow2[] = {
    66,67, // LA
    89,90, // LA
    141,142, // LB
    150,151, // LB

    100,99,98,97,96,95,94, // SB
    116,117,118,119,120,121, // SA    
  };
  StripLEDConfig ledsCenterVerticalRow2Cfg[] = {{1, ledsCenterVerticalRow2, sizeof(ledsCenterVerticalRow2) / sizeof(ledsCenterVerticalRow2[0])}};
  CenterVerticalRow2Segment = scene4.createSegment(ledsCenterVerticalRow2Cfg, 1);
  
  const uint16_t ledsCenterVerticalRow3[] = {
    68,69, // LA
    87,88, // LA
    139,140, // LB
    152,153, // LB

    104,103,102,101, // SB
    113,114,115, // SA    

    122,123, // TE
    124,125, // TD
  };
  StripLEDConfig ledsCenterVerticalRow3Cfg[] = {{1, ledsCenterVerticalRow3, sizeof(ledsCenterVerticalRow3) / sizeof(ledsCenterVerticalRow3[0])}};
  CenterVerticalRow3Segment = scene4.createSegment(ledsCenterVerticalRow3Cfg, 1);

  const uint16_t ledsCenterVerticalRow4[] = {
    70,71, // LA
    85,86, // LA
    137,138, // LB
    154,155, // LB
    165,166,167, // LB

    107,106,105, // SB
    110,111,112, // SA    

    126,127, // TC
    128,129, // TB
    130,131, // TA
  };
  StripLEDConfig ledsCenterVerticalRow4Cfg[] = {{1, ledsCenterVerticalRow4, sizeof(ledsCenterVerticalRow4) / sizeof(ledsCenterVerticalRow4[0])}};
  CenterVerticalRow4Segment = scene4.createSegment(ledsCenterVerticalRow4Cfg, 1);

  const uint16_t ledsCenterVerticalRow5[] = {
    72, // LA
    83,84, // LA
    108,109, // SA    
    132,133,134,135,136, // LB
  };
  StripLEDConfig ledsCenterVerticalRow5Cfg[] = {{1, ledsCenterVerticalRow5, sizeof(ledsCenterVerticalRow5) / sizeof(ledsCenterVerticalRow5[0])}};
  CenterVerticalRow5Segment = scene4.createSegment(ledsCenterVerticalRow5Cfg, 1);

  const uint16_t ledsCenterVerticalRow6[] = {
    156,157,158,159,160,161,162,163,164, // LB
    73,74,75,76,77,78,79,80,81,82, // LA
  };
  StripLEDConfig ledsCenterVerticalRow6Cfg[] = {{1, ledsCenterVerticalRow6, sizeof(ledsCenterVerticalRow6) / sizeof(ledsCenterVerticalRow6[0])}};
  CenterVerticalRow6Segment = scene4.createSegment(ledsCenterVerticalRow6Cfg, 1);
}

void initScene2Segments() {
  const uint16_t leds1[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,252,253,254};
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
  uint16_t bpm = 6000;

  ambianceShow.repeats = true;

  Cue* fadeCue = new Cue(&scene2, 29.0, bpm);
  auto fadeAnimation = new SmoothPaletteFadeBeatAnimation("| C |", bpm, make2StopGradient(CRGB::Black, CRGB(200, 0, 100)), false);
  fadeAnimation->addSegment(scene2Segment1, 100);
  fadeCue->addAnimation(fadeAnimation);
  ambianceShow.addCue(fadeCue);

  Cue* introCue = new Cue(&scene2, 86400.0, bpm);
  // auto intro = new PatternBeatAnimation("| W W |", bpm, CRGB(0, 0, 255)); // Pure Blue
  // auto intro = new PatternBeatAnimation("| W W |", bpm, CRGB(0, 148, 255)); // Teal Blue
  auto intro = new PatternBeatAnimation("| W W |", bpm,  CRGB(200, 0, 100)); // Pink
  // auto intro = new TheatreChasePaletteBeatAnimation("| SSS |", bpm, RainbowColors_p, false);
  intro->addSegment(scene2Segment1, 100);
  introCue->addAnimation(intro);
  ambianceShow.addCue(introCue);
  Serial.println("Init: Cue1");

}

void initWeAreYourFriendsShow() {
  uint16_t bpmIntro1 = 8160;
  uint16_t bpm = 12293;
  uint16_t averageFastbpm = 25100;
  
  Cue* dingCue = new Cue(&scene4, 4500UL, bpmIntro1);
  auto cueDing1Animation = new PatternBeatAnimation("| q EeEeEe| wwww |", bpmIntro1, CRGB::White);
  cueDing1Animation->addSegment(TFSegment, 50);
  dingCue->addAnimation(cueDing1Animation);
  weAreYourFriendsShow.addCue(dingCue);

  Cue* loadingSlowCue = new Cue(&scene4, 24.0, bpmIntro1);
  auto cueloadingSlowRow1Animation = new PatternBeatAnimation("| EeEeEeEe| W | W | W | W | W |", bpmIntro1, CRGB::White);
  auto cueloadingSlowRow2Animation = new PatternBeatAnimation("| w | EeEeEeEe | W | W | W | W |", bpmIntro1, CRGB::White);
  auto cueloadingSlowRow3Animation = new PatternBeatAnimation("| w | w | EeEeEeEe | W | W | W |", bpmIntro1, CRGB::White);
  auto cueloadingSlowRow4Animation = new PatternBeatAnimation("| w | w | w | EeEeEeEe | W | W |", bpmIntro1, CRGB::White);
  auto cueloadingSlowRow5Animation = new PatternBeatAnimation("| w | w | w | w | EeEeEeEe | W |", bpmIntro1, CRGB::White);
  auto cueloadingSlowRow6Animation = new PatternBeatAnimation("| w | w | w | w | w | EeEeEeEe |", bpmIntro1, CRGB::White);
  cueloadingSlowRow1Animation->addSegment(CenterVerticalRow1Segment, 50);
  cueloadingSlowRow2Animation->addSegment(CenterVerticalRow2Segment, 50);
  cueloadingSlowRow3Animation->addSegment(CenterVerticalRow3Segment, 50);
  cueloadingSlowRow4Animation->addSegment(CenterVerticalRow4Segment, 50);
  cueloadingSlowRow5Animation->addSegment(CenterVerticalRow5Segment, 50);
  cueloadingSlowRow6Animation->addSegment(CenterVerticalRow6Segment, 50);
  loadingSlowCue->addAnimation(cueloadingSlowRow1Animation);
  loadingSlowCue->addAnimation(cueloadingSlowRow2Animation);
  loadingSlowCue->addAnimation(cueloadingSlowRow3Animation);
  loadingSlowCue->addAnimation(cueloadingSlowRow4Animation);
  loadingSlowCue->addAnimation(cueloadingSlowRow5Animation);
  loadingSlowCue->addAnimation(cueloadingSlowRow6Animation);
  weAreYourFriendsShow.addCue(loadingSlowCue);

  Cue* loadingSlow2Cue = new Cue(&scene4, 9.0, bpmIntro1);
  auto cueloadingSlow2Row1Animation = new PatternBeatAnimation("| EeEeEeEe| W | W | W | W | W |", bpmIntro1, CRGB::White);
  auto cueloadingSlow2Row2Animation = new PatternBeatAnimation("| w | EeEeEeEe | W | W | W | W |", bpmIntro1, CRGB::White);
  auto cueloadingSlow2Row3Animation = new PatternBeatAnimation("| w | w | EeEeEeEe | W | W | W |", bpmIntro1, CRGB::White);
  cueloadingSlow2Row1Animation->addSegment(CenterVerticalRow1Segment, 50);
  cueloadingSlow2Row2Animation->addSegment(CenterVerticalRow2Segment, 50);
  cueloadingSlow2Row3Animation->addSegment(CenterVerticalRow3Segment, 50);
  loadingSlow2Cue->addAnimation(cueloadingSlow2Row1Animation);
  loadingSlow2Cue->addAnimation(cueloadingSlow2Row2Animation);
  loadingSlow2Cue->addAnimation(cueloadingSlow2Row3Animation);
  weAreYourFriendsShow.addCue(loadingSlow2Cue);

  // 0:30
  Cue* loadingFastCue = new Cue(&scene4, 63.0, averageFastbpm);
  auto introAnimation = new RandomSparkleBeatAnimation("| QQQQ |", averageFastbpm, WhitePalette(), 10, 3, 100, 3, 3, true);
  auto loadingFastCueRow1Animation = new PatternBeatAnimation("| EeEeEeEe| W | W | W | W | W |", averageFastbpm, CRGB::White);
  auto loadingFastCueRow2Animation = new PatternBeatAnimation("| w | EeEeEeEe | W | W | W | W |", averageFastbpm, CRGB::White);
  auto loadingFastCueRow3Animation = new PatternBeatAnimation("| w | w | EeEeEeEe | W | W | W |", averageFastbpm, CRGB::White);
  auto loadingFastCueRow4Animation = new PatternBeatAnimation("| w | w | w | EeEeEeEe | W | W |", averageFastbpm, CRGB::White);
  auto loadingFastCueRow5Animation = new PatternBeatAnimation("| w | w | w | w | EeEeEeEe | W |", averageFastbpm, CRGB::White);
  auto loadingFastCueRow6Animation = new PatternBeatAnimation("| w | w | w | w | w | EeEeEeEe |", averageFastbpm, CRGB::White);
  loadingFastCueRow1Animation->addSegment(CenterVerticalRow1Segment, 50);
  loadingFastCueRow2Animation->addSegment(CenterVerticalRow2Segment, 50);
  loadingFastCueRow3Animation->addSegment(CenterVerticalRow3Segment, 50);
  loadingFastCueRow4Animation->addSegment(CenterVerticalRow4Segment, 50);
  loadingFastCueRow5Animation->addSegment(CenterVerticalRow5Segment, 50);
  loadingFastCueRow6Animation->addSegment(CenterVerticalRow6Segment, 50);
  loadingFastCue->addAnimation(loadingFastCueRow1Animation);
  loadingFastCue->addAnimation(loadingFastCueRow2Animation);
  loadingFastCue->addAnimation(loadingFastCueRow3Animation);
  loadingFastCue->addAnimation(loadingFastCueRow4Animation);
  loadingFastCue->addAnimation(loadingFastCueRow5Animation);
  loadingFastCue->addAnimation(loadingFastCueRow6Animation);
  weAreYourFriendsShow.addCue(loadingFastCue);

  CRGB darkBlue(0, 0, 68);
  CRGB deepPurple(48, 0, 72);
  CRGB cosmicPink(200, 0, 100);
  CRGB electricTeal(0, 200, 150);

  // 0:44
  Cue* wholeStarsCue = new Cue(&scene2, 16.0, bpm);
  auto wholeStarsAnimation = new RandomSparkleBeatAnimation("| W | W |", bpm, WhitePalette(), 30, 2, 100, 3, 3, true);
  wholeStarsAnimation->addSegment(LASegment, 50);
  wholeStarsCue->addAnimation(wholeStarsAnimation);
  weAreYourFriendsShow.addCue(wholeStarsCue);

  Cue* halfStarsCue = new Cue(&scene2, 8.0, bpm);
  auto halfStarsAnimation = new RandomSparkleBeatAnimation("| H H |", bpm, make2StopGradient(darkBlue, deepPurple), 30, 2, 100, 3, 3, true); // Dark Blue to Deep Purple
  halfStarsAnimation->addSegment(LBSegment, 20);
  halfStarsCue->addAnimation(halfStarsAnimation);
  halfStarsCue->addAnimation(wholeStarsAnimation);
  weAreYourFriendsShow.addCue(halfStarsCue);

  // 0:56
  Cue* quarterStarsCue = new Cue(&scene2, 16.0, bpm);
  auto quarterStarsAnimation = new RandomSparkleBeatAnimation("| Q Q Q Q |", bpm, make2StopGradient(deepPurple, cosmicPink), 30, 3, 100, 3, 3, true); // Deep Purple to Cosmic Pink
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

  // 63s
  Cue* eighthStarsCue = new Cue(&scene2, 6.0, bpm);
  auto eighthStarsAnimation = new RandomSparkleBeatAnimation("| EE EE EE EE | EE EE |", bpm, make2StopGradient(cosmicPink, electricTeal), 35, 1, 100, 3, 3, true); // Cosmic Pink to Electric Teal
  eighthStarsAnimation->addSegment(SASegment, 20);
  eighthStarsAnimation->addSegment(SBSegment, 20);
  eighthStarsAnimation->addSegment(TASegment, 20);
  eighthStarsAnimation->addSegment(TBSegment, 20);
  eighthStarsAnimation->addSegment(TCSegment, 20);
  eighthStarsAnimation->addSegment(TDSegment, 20);
  eighthStarsAnimation->addSegment(TESegment, 20);
  eighthStarsCue->addAnimation(eighthStarsAnimation);
  eighthStarsCue->addAnimation(quarterStarsAnimation);
  eighthStarsCue->addAnimation(halfStarsAnimation);
  eighthStarsCue->addAnimation(wholeStarsAnimation);
  weAreYourFriendsShow.addCue(eighthStarsCue);

  // 66s
  Cue* chorus1Cue1 = new Cue(&scene2, 6.0, bpm);
  auto chorus1BecauseWeAnimation = new TheatreChasePaletteBeatAnimation("sS sS | q q h | q", bpm, make4StopGradient(darkBlue, deepPurple, cosmicPink, electricTeal), true, 1);
  auto chorus1CymbalCrashAnimation = new TheatreChasePaletteBeatAnimation("h | Q q h | q", bpm, make4StopGradient(darkBlue, deepPurple, cosmicPink, electricTeal), true, 1);
  auto chorus1Quarters = new RandomSparkleBeatAnimation("| Q Q | Q Q Q Q | q", bpm, make2StopGradient(deepPurple, cosmicPink), 30, 3, 100, 3, 3, true);
  chorus1Quarters->addSegment(SCSegment, 20);
  chorus1Quarters->addSegment(MASegment, 20);
  chorus1Quarters->addSegment(MCSegment, 20);
  chorus1Quarters->addSegment(MDSegment, 20);

  chorus1BecauseWeAnimation->addSegment(LASegment, 70);
  chorus1BecauseWeAnimation->addSegment(LBSegment, 70);
  chorus1BecauseWeAnimation->addSegment(SASegment, 70);
  chorus1BecauseWeAnimation->addSegment(SBSegment, 70);
  chorus1CymbalCrashAnimation->addSegment(MBSegment, 70);
  chorus1CymbalCrashAnimation->addSegment(SDSegment, 70);
  chorus1CymbalCrashAnimation->addSegment(TFSegment, 70);
  chorus1Cue1->addAnimation(chorus1Quarters);
  chorus1Cue1->addAnimation(chorus1BecauseWeAnimation);
  chorus1Cue1->addAnimation(chorus1CymbalCrashAnimation);
  weAreYourFriendsShow.addCue(chorus1Cue1);

  Cue* chorus1Cue2 = new Cue(&scene2, 28.0, bpm);
  auto chorus1SynthLeftAnimation = new TheatreChasePaletteBeatAnimation("| EeEeEeEe | qqqq | EeEeEeEe | qqqq | EeEeEeEe | qqqq | EeEeEeEe | qqqq | EeEeEeEe |", bpm, make4StopGradient(darkBlue, deepPurple, cosmicPink, electricTeal), true, 1);
  auto chorus1SynthRightAnimation = new TheatreChasePaletteBeatAnimation("| qqqq | EeEeEeEe | qqqq | EeEeEeEe | qqqq | EeEeEeEe | qqqq | EeEeEeEe | qqqq | ", bpm, make4StopGradient(darkBlue, deepPurple, cosmicPink, electricTeal), true, 1);
  auto chorus1Base12Animation = new TheatreChasePaletteBeatAnimation("| EeEeEEEE | EeEeEEEE | EeEeEEEE | EeEeEEEE | EeEeEEEE | EeEeEEEE | EeEeEEEE | EeEeEEEE | EeEeEEEE | ", bpm, CRGBPalette16(darkBlue), true, 2);
  auto chorus1Base3Animation = new TheatreChasePaletteBeatAnimation("| qqEEEE | qqEEEE | qqEEEE | qqEEEE | qqEEEE | qqEEEE | qqEEEE | qqEEEE | qqEEEE | ", bpm, make2StopGradient(darkBlue, deepPurple), true, 3);
  auto chorus1Base3AndAnimation = new TheatreChasePaletteBeatAnimation("| qqeEEE | qqeEEE | qqeEEE | qqeEEE | qqeEEE | qqeEEE | qqeEEE | qqeEEE | qqeEEE | ", bpm, make2StopGradient(darkBlue, deepPurple), false, 3);
  auto chorus1Base4Animation = new TheatreChasePaletteBeatAnimation("| qqeeEE | qqeeEE | qqeeEE | qqeeEE | qqeeEE | qqeeEE | qqeeEE | qqeeEE | qqeeEE | ", bpm, make2StopGradient(deepPurple, cosmicPink), false, 3);
  auto chorus1Base4AndAnimation = new TheatreChasePaletteBeatAnimation("| qqeeeE | qqeeeE | qqeeeE | qqeeeE | qqeeeE | qqeeeE | qqeeeE | qqeeeE | qqeeeE | ", bpm, make2StopGradient(cosmicPink, electricTeal), false, 3);
  chorus1SynthLeftAnimation->addSegment(MASegment, 20);
  chorus1SynthLeftAnimation->addSegment(SCSegment, 20);
  chorus1SynthRightAnimation->addSegment(MCSegment, 20);
  chorus1SynthRightAnimation->addSegment(MDSegment, 20);
  chorus1Base12Animation->addSegment(TASegment, 20);
  chorus1Base12Animation->addSegment(TBSegment, 20);
  chorus1Base12Animation->addSegment(TCSegment, 20);
  chorus1Base12Animation->addSegment(TDSegment, 20);
  chorus1Base12Animation->addSegment(TESegment, 20);
  chorus1Base3Animation->addSegment(SASegment, 20);
  chorus1Base3AndAnimation->addSegment(SBSegment, 20);
  chorus1Base4Animation->addSegment(LBSegment, 20);
  chorus1Base4AndAnimation->addSegment(LASegment, 20);
  chorus1Cue2->addAnimation(chorus1SynthLeftAnimation);
  chorus1Cue2->addAnimation(chorus1SynthRightAnimation);
  chorus1Cue2->addAnimation(chorus1Base12Animation);
  chorus1Cue2->addAnimation(chorus1Base3Animation);
  chorus1Cue2->addAnimation(chorus1Base3AndAnimation);
  chorus1Cue2->addAnimation(chorus1Base4Animation);
  chorus1Cue2->addAnimation(chorus1Base4AndAnimation);
  weAreYourFriendsShow.addCue(chorus1Cue2);

  // 82s
  Cue* sixBeatSynth = new Cue(&scene2, 6.0, bpm);
  auto sixBeatSynthLeftAnimation = new TheatreChasePaletteBeatAnimation("ESs q | q q q q |", bpm, make4StopGradient(darkBlue, deepPurple, cosmicPink, electricTeal), true, 1);
  auto sixBeatSynthCenterAnimation = new TheatreChasePaletteBeatAnimation("q ESs | q q ESs q |", bpm, make4StopGradient(darkBlue, deepPurple, cosmicPink, electricTeal), true, 1);
  auto sixBeatSynthRightAnimation = new TheatreChasePaletteBeatAnimation("q q | ESs q q q |", bpm, make4StopGradient(darkBlue, deepPurple, cosmicPink, electricTeal), true, 1);
  auto sixBeatSynthCeilingAnimation = new TheatreChasePaletteBeatAnimation("q q | q ESs q ESs |", bpm, make4StopGradient(darkBlue, deepPurple, cosmicPink, electricTeal), true, 1);
  sixBeatSynthLeftAnimation->addSegment(SCSegment, 20);
  sixBeatSynthLeftAnimation->addSegment(MASegment, 20);
  sixBeatSynthCenterAnimation->addSegment(LASegment, 20);
  sixBeatSynthCenterAnimation->addSegment(LBSegment, 20);
  sixBeatSynthCenterAnimation->addSegment(SASegment, 20);
  sixBeatSynthCenterAnimation->addSegment(SBSegment, 20);
  sixBeatSynthCenterAnimation->addSegment(TASegment, 20);
  sixBeatSynthCenterAnimation->addSegment(TBSegment, 20);
  sixBeatSynthCenterAnimation->addSegment(TCSegment, 20);
  sixBeatSynthCenterAnimation->addSegment(TDSegment, 20);
  sixBeatSynthCenterAnimation->addSegment(TESegment, 20);
  sixBeatSynthRightAnimation->addSegment(MCSegment, 20);
  sixBeatSynthRightAnimation->addSegment(MDSegment, 20);
  sixBeatSynthCeilingAnimation->addSegment(MBSegment, 20);
  sixBeatSynthCeilingAnimation->addSegment(TFSegment, 20);
  sixBeatSynth->addAnimation(sixBeatSynthLeftAnimation);
  sixBeatSynth->addAnimation(sixBeatSynthCenterAnimation);
  sixBeatSynth->addAnimation(sixBeatSynthRightAnimation);
  sixBeatSynth->addAnimation(sixBeatSynthCeilingAnimation);
  weAreYourFriendsShow.addCue(sixBeatSynth);

  // 85s
  Cue* funkyBaseCue = new Cue(&scene2, 2.0, bpm);
  auto funkyBase1Animation = new TheatreChasePaletteBeatAnimation("E E TTTT Tttt |", bpm, make4StopGradient(darkBlue, deepPurple, cosmicPink, electricTeal), true, 1);
  auto funkyBase2Animation = new TheatreChasePaletteBeatAnimation("e E TTTT Tttt |", bpm, make4StopGradient(darkBlue, deepPurple, cosmicPink, electricTeal), true, 1);
  auto funkyBase3Animation = new TheatreChasePaletteBeatAnimation("e e tTTT Tttt |", bpm, make4StopGradient(darkBlue, deepPurple, cosmicPink, electricTeal), true, 1);
  auto funkyBase4Animation = new TheatreChasePaletteBeatAnimation("e e ttTT Tttt |", bpm, make4StopGradient(darkBlue, deepPurple, cosmicPink, electricTeal), true, 1);
  auto funkyBase5Animation = new TheatreChasePaletteBeatAnimation("e e tttT Tttt |", bpm, make4StopGradient(darkBlue, deepPurple, cosmicPink, electricTeal), true, 1);
  auto funkyBase6Animation = new TheatreChasePaletteBeatAnimation("e e tttt Tttt |", bpm, make4StopGradient(darkBlue, deepPurple, cosmicPink, electricTeal), true, 1);
  funkyBase1Animation->addSegment(TASegment, 20);
  funkyBase1Animation->addSegment(TBSegment, 20);
  funkyBase1Animation->addSegment(TCSegment, 20);
  funkyBase1Animation->addSegment(TDSegment, 20);
  funkyBase2Animation->addSegment(SASegment, 20);
  funkyBase3Animation->addSegment(SBSegment, 20);
  funkyBase4Animation->addSegment(LBSegment, 20);
  funkyBase5Animation->addSegment(LASegment, 20);
  funkyBaseCue->addAnimation(funkyBase1Animation);
  funkyBaseCue->addAnimation(funkyBase2Animation);
  funkyBaseCue->addAnimation(funkyBase3Animation);
  funkyBaseCue->addAnimation(funkyBase4Animation);
  funkyBaseCue->addAnimation(funkyBase5Animation);
  weAreYourFriendsShow.addCue(funkyBaseCue);

  // 86
  // TODO: Second 32 should "Have a chase sequence happen in the Center wall. (cosmic colors)" instead of bottom-to-top build
  Cue* instrumentalBaseCue = new Cue(&scene4, 64.0, bpm);
  auto instrumentalBase1Animation = new PatternBeatAnimation("| EttTt EeSsTtTt q| eE SsSs Ee SsTtTt | W | H Q E e | W | W | W | W |", bpm, deepPurple);
  auto instrumentalBase2Animation = new PatternBeatAnimation("| w | w | EttTt EeSsTtTt q| eE SsSs Ee Sse | W | W | W | W |", bpm, deepPurple);
  auto instrumentalBase3Animation = new PatternBeatAnimation("| w | w | w | w | Ee EeSsTtTt q| eE SsSs Ee SsTtTt | W | W |", bpm, deepPurple);
  auto instrumentalBase4Animation = new PatternBeatAnimation("| w | w | w | w | Ee q h | w | EttTt EeSsTtTt q| W |", bpm, deepPurple);
  auto instrumentalBase5Animation = new PatternBeatAnimation("| w | w | w | w | Ee q h | w | w | eE SsSs H |", bpm, deepPurple);
  auto instrumentalBase6Animation = new PatternBeatAnimation("| w | w | w | w | Ee q h | w | w | h Ee SsTtTt |", bpm, deepPurple);
  auto instrumentalSnareAnimation = new PatternBeatAnimation("| q Q q FfFfs TtTt | q Q q FfFfs TtTt | q Q q FfFfs TtTt | q Q q FfFfs TtTt |", bpm, cosmicPink);
  auto instrumentalKickAnimation = new PatternBeatAnimation("| SsSs q Ee eSs |Ee q Ee q | SsSs q Ee eSs |Ee q Ee q | ", bpm, darkBlue);
  auto instrumentalHighHatAnimation = new PatternBeatAnimation("| eSs eSs eSs eSs |eSs eSs eSs eSs | eSs eSs eSs eSs |eSs eSs eSs eSs |", bpm, electricTeal);
  auto instrumentalKeyboardAnimation = new TheatreChasePaletteBeatAnimation("| wwwwwwww | QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ |", bpm, make4StopGradient(darkBlue, deepPurple, cosmicPink, electricTeal), true, 2);

  instrumentalBase1Animation->addSegment(CenterVerticalRow1Segment, 80);
  instrumentalBase2Animation->addSegment(CenterVerticalRow2Segment, 80);
  instrumentalBase3Animation->addSegment(CenterVerticalRow3Segment, 80);
  instrumentalBase4Animation->addSegment(CenterVerticalRow4Segment, 80);
  instrumentalBase5Animation->addSegment(CenterVerticalRow5Segment, 80);
  instrumentalBase6Animation->addSegment(CenterVerticalRow6Segment, 80);
  instrumentalSnareAnimation->addSegment(MCSegment, 20);
  instrumentalSnareAnimation->addSegment(MDSegment, 20);
  instrumentalKickAnimation->addSegment(MASegment, 50);
  instrumentalHighHatAnimation->addSegment(SCSegment, 10);
  instrumentalKeyboardAnimation->addSegment(MBSegment, 50);
  instrumentalKeyboardAnimation->addSegment(SDSegment, 50);
  instrumentalKeyboardAnimation->addSegment(TFSegment, 50);
  instrumentalBaseCue->addAnimation(instrumentalBase1Animation);
  instrumentalBaseCue->addAnimation(instrumentalBase2Animation);
  instrumentalBaseCue->addAnimation(instrumentalBase3Animation);
  instrumentalBaseCue->addAnimation(instrumentalBase4Animation);
  instrumentalBaseCue->addAnimation(instrumentalBase5Animation);
  instrumentalBaseCue->addAnimation(instrumentalBase6Animation);
  instrumentalBaseCue->addAnimation(instrumentalSnareAnimation);
  instrumentalBaseCue->addAnimation(instrumentalKickAnimation);
  instrumentalBaseCue->addAnimation(instrumentalHighHatAnimation);
  instrumentalBaseCue->addAnimation(instrumentalKeyboardAnimation);
  weAreYourFriendsShow.addCue(instrumentalBaseCue);

  // 117
  // TODO: Occasionally flash a big pink X with black background
  Cue* chorus2Cue1 = new Cue(&scene2, 32.0, bpm/32.0*32.5);
  chorus2Cue1->addAnimation(chorus1SynthLeftAnimation);
  chorus2Cue1->addAnimation(chorus1SynthRightAnimation);
  chorus2Cue1->addAnimation(chorus1Base12Animation);
  chorus2Cue1->addAnimation(chorus1Base3Animation);
  chorus2Cue1->addAnimation(chorus1Base3AndAnimation);
  chorus2Cue1->addAnimation(chorus1Base4Animation);
  chorus2Cue1->addAnimation(chorus1Base4AndAnimation);
  weAreYourFriendsShow.addCue(chorus2Cue1);

  // 133 Chorus 
  // TODO: PAC MAN Chase scene!
  Cue* chorus3Cue1 = new Cue(&scene2, 32.0, bpm);
  chorus3Cue1->addAnimation(chorus1SynthLeftAnimation);
  chorus3Cue1->addAnimation(chorus1SynthRightAnimation);
  chorus3Cue1->addAnimation(chorus1Base12Animation);
  chorus3Cue1->addAnimation(chorus1Base3Animation);
  chorus3Cue1->addAnimation(chorus1Base3AndAnimation);
  chorus3Cue1->addAnimation(chorus1Base4Animation);
  chorus3Cue1->addAnimation(chorus1Base4AndAnimation);
  weAreYourFriendsShow.addCue(chorus3Cue1);

  // 150 Chorus 
  // TODO: Alternate left and right every quarter note
  Cue* chorus4Cue1 = new Cue(&scene2, 32.0, bpm);
  auto chorus4SynthLeftAnimation = new TheatreChasePaletteBeatAnimation("| Ee q Ee q |", bpm, make4StopGradient(darkBlue, deepPurple, cosmicPink, electricTeal), true, 1);
  auto chorus4SynthRightAnimation = new TheatreChasePaletteBeatAnimation("| q Ee q Ee | ", bpm, make4StopGradient(darkBlue, deepPurple, cosmicPink, electricTeal), true, 1);
  chorus4SynthLeftAnimation->addSegment(MASegment, 20);
  chorus4SynthLeftAnimation->addSegment(SCSegment, 20);
  chorus4SynthRightAnimation->addSegment(MCSegment, 20);
  chorus4SynthRightAnimation->addSegment(MDSegment, 20);
  chorus4Cue1->addAnimation(chorus4SynthLeftAnimation);
  chorus4Cue1->addAnimation(chorus4SynthRightAnimation);
  chorus4Cue1->addAnimation(chorus1Base12Animation);
  chorus4Cue1->addAnimation(chorus1Base3Animation);
  chorus4Cue1->addAnimation(chorus1Base3AndAnimation);
  chorus4Cue1->addAnimation(chorus4SynthLeftAnimation);
  chorus4Cue1->addAnimation(chorus4SynthRightAnimation);

  // Reuse funky base
  // TODO: OOps, was supposed to use the instrumental instead
  chorus4Cue1->addAnimation(funkyBase1Animation);
  chorus4Cue1->addAnimation(funkyBase2Animation);
  chorus4Cue1->addAnimation(funkyBase3Animation);
  chorus4Cue1->addAnimation(funkyBase4Animation);
  chorus4Cue1->addAnimation(funkyBase5Animation);

  weAreYourFriendsShow.addCue(chorus4Cue1);

  // 164
  Cue* chorus5Cue1 = new Cue(&scene2, 32.0, bpm);
  chorus5Cue1->addAnimation(chorus1SynthLeftAnimation);
  chorus5Cue1->addAnimation(chorus1SynthRightAnimation);
  chorus5Cue1->addAnimation(chorus1Base12Animation);
  chorus5Cue1->addAnimation(chorus1Base3Animation);
  chorus5Cue1->addAnimation(chorus1Base3AndAnimation);
  chorus5Cue1->addAnimation(chorus1Base4Animation);
  chorus5Cue1->addAnimation(chorus1Base4AndAnimation);

  // Reuse instrumental drum and Keyboard
  // TODO: Follow Taylor's Instructions
  // For this section:
  // Have the LA perimeter of lights pulse on the half note 4 times in white. (“We are your friends”) 
  // Then have the LB ring of lights continue to pulse on the half note 4 times in yellow while the white lights stay on. (“You'll never be alone again). 
  // Then have the SB and SA lines pulse on the half note 4 times in orange while the white and yellow lights stay on. (“Well Come On x2”)
  // Have the TA TB TC TD TE lights pulse on the half note 4 times in red while the orange, yellow, and white lights stay on. (“Well Come On x2)
  // For the remaining quarter notes (On the cymbal beat) of the chorus, pulse the same colors in reverse from the middle outwards so they are flashing out at the viewer (instead of the wall flashing in unison on the beat, have TA TB TC TD TE pulse on the cymbal quarter note, and the other layers pulse a split second after each other so it feels like a pulse coming out of the center)
  // TA TB TC TD TE: White
  // SA SB: Yellow
  // LB: Orange
  // LA: Red
  chorus5Cue1->addAnimation(instrumentalBase1Animation);
  chorus5Cue1->addAnimation(instrumentalBase2Animation);
  chorus5Cue1->addAnimation(instrumentalBase3Animation);
  chorus5Cue1->addAnimation(instrumentalBase4Animation);
  chorus5Cue1->addAnimation(instrumentalBase5Animation);
  chorus5Cue1->addAnimation(instrumentalBase6Animation);
  chorus5Cue1->addAnimation(instrumentalKeyboardAnimation);
  weAreYourFriendsShow.addCue(chorus5Cue1);

  // 178
  // TODO: Combine with the section above and follow Taylor's instructions
  Cue* chorus6Cue1 = new Cue(&scene2, 16.0, bpm);
  chorus6Cue1->addAnimation(chorus1SynthLeftAnimation);
  chorus6Cue1->addAnimation(chorus1SynthRightAnimation);
  chorus6Cue1->addAnimation(chorus1Base12Animation);
  chorus6Cue1->addAnimation(chorus1Base3Animation);
  chorus6Cue1->addAnimation(chorus1Base3AndAnimation);
  chorus6Cue1->addAnimation(chorus1Base4Animation);
  chorus6Cue1->addAnimation(chorus1Base4AndAnimation);
  weAreYourFriendsShow.addCue(chorus6Cue1);

  // 186
  // TODO: Follow Taylor's instructions
  // For this section, have single “stars” similar to the beginning pulse on the quarter note for the whole chorus (32x), 
  // but each star pulses on a different wall for each beat in a slightly different spot on that wall.
  // Location Pattern:
  // * Center
  // * Ceiling 
  // * Left
  // * Right
  // Color Pattern: Killscreen (we're going to skip pink and use it at the end, so that the colors and the locations don't 
  // stay exactly the same the whole time)
  // * Green
  // * Purple 
  // * Blue
  Cue* chorus7Cue1 = new Cue(&scene2, 32.0, bpm);
  chorus7Cue1->addAnimation(chorus1SynthLeftAnimation);
  chorus7Cue1->addAnimation(chorus1SynthRightAnimation);
  chorus7Cue1->addAnimation(chorus1Base12Animation);
  chorus7Cue1->addAnimation(chorus1Base3Animation);
  chorus7Cue1->addAnimation(chorus1Base3AndAnimation);
  chorus7Cue1->addAnimation(chorus1Base4Animation);
  chorus7Cue1->addAnimation(chorus1Base4AndAnimation);
  weAreYourFriendsShow.addCue(chorus7Cue1);

  // 205
  // For the final note of the song, have all the lights pulse pink from quickly dim to bright. 
  //As the music fades out (3:26-3:27), the pink lights all flicker in unison for a moment before going out together, 
  //leaving the viewer in darkness (but not really).
  Cue* fadeOut1Cue = new Cue(&scene2, 4.0, bpm);
  auto fadeOutFullPink = new PatternBeatAnimation("| sSsS sSsS tTtTtTtT fFfFfFfFfFfFfFfF |", bpm, cosmicPink, CRGB(100, 0, 50));
  fadeOutFullPink->addSegment(scene2Segment1, 40);
  fadeOut1Cue->addAnimation(fadeOutFullPink);
  weAreYourFriendsShow.addCue(fadeOut1Cue);

  Cue* fadeOut2Cue = new Cue(&scene2, 2.0, bpm);
  auto fadeOutLightPink = new PatternBeatAnimation("StFtFfF tTttTtS", bpm, CRGB(100, 0, 50));
  fadeOutLightPink->addSegment(scene2Segment1, 25);
  fadeOut2Cue->addAnimation(fadeOutLightPink);
  weAreYourFriendsShow.addCue(fadeOut2Cue);

  Cue* fadeOut3Cue = new Cue(&scene2, 2.0, bpm);
  auto fadeOutBrightPink = new PatternBeatAnimation("q TFfF", bpm, CRGB(255, 0, 120));
  fadeOutBrightPink->addSegment(scene2Segment1, 100);
  fadeOut3Cue->addAnimation(fadeOutBrightPink);
  weAreYourFriendsShow.addCue(fadeOut3Cue);

  Cue* blackCue = new Cue(&scene2, 10000UL, bpm);
  auto blackAnimation = new PatternBeatAnimation("| WWW |", bpm, CRGB::Black);
  blackAnimation->addSegment(scene2Segment1, 100);
  blackCue->addAnimation(blackAnimation);
  weAreYourFriendsShow.addCue(blackCue);
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
  base1->addSegment(TASegment, 50);
  base1->addSegment(TBSegment, 50);
  base1->addSegment(TCSegment, 50);
  base1->addSegment(TDSegment, 50);
  base1->addSegment(TESegment, 50);
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
  base2->addSegment(TASegment, 100);
  base2->addSegment(TBSegment, 100);
  base2->addSegment(TCSegment, 100);
  base2->addSegment(TDSegment, 100);
  base2->addSegment(TESegment, 100);
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
  // FastLED.addLeds<WS2812B, 22, RGB>(strip2, STRIP2_LEDS);
  FastLED.setBrightness(51); // ~20%

  // Setup Button
  pinMode(BUTTON_PIN, INPUT);

  initSegments();
  Serial.println("Init: Segments");
  initScene1Segments();
  Serial.println("Init: Scene1Segments");
  initScene2Segments();
  initStripSegments();
  initRowSegments();
  Serial.println("Init: Scene1Segments");
  initLoseYourselfShow();
  initTestShow();
  initAmbianceShow();


  initWeAreYourFriendsShow();
  Serial.println("Init: TestShow");
  Serial.println("Ready. Send 's' to start the show or 'T:<ms>' to sync.");

  activeShow = ambianceShow;

  showStartMillis = millis();
  lastLogMillis = millis();
  showStarted = true;
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

int button_presses = 0;
uint32_t button_debounce_last_pressed_millis = 0;
uint32_t long_press_start_millis = 0;

void loop() {

  uint32_t time_since_last_detected_button_press = millis() - button_debounce_last_pressed_millis;
  if(digitalRead(BUTTON_PIN) == HIGH) {
    if(time_since_last_detected_button_press < 10) {
      // No-op: Debounce electornics
    } else if(time_since_last_detected_button_press < 500) {
      // Multi-Tap
      button_presses += 1;
      Serial.print("Multi Press:");
      Serial.println(button_presses);
    } else if(time_since_last_detected_button_press > 1000) {
      Serial.println("First Press");
      button_presses = 1;
      long_press_start_millis = millis();
    }
    button_debounce_last_pressed_millis = millis();
  } else if(button_presses == 1 and millis() - long_press_start_millis > 1000 and millis() - long_press_start_millis < 5000) {
    Serial.println("Long Press");
    Serial.print("BTN:0");
    button_presses = 0;
    long_press_start_millis = 0;
  } else if(button_presses > 0 and time_since_last_detected_button_press > 500) {
    Serial.print("BTN:");
    Serial.println(button_presses);
    button_presses = 0;
    long_press_start_millis = 0;
  }

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
      for (uint16_t i = 0; i < STRIP1_LEDS; i++) {
        strip1[i] = CRGB::Black;
      }
      Serial.println("Show started!");
    } else if (len == 1 && (serialBuffer[0] == 'x' || serialBuffer[0] == 'X')) {
      showStartMillis = 0;
      lastLogMillis = millis();
      showStarted = false;
      for (uint16_t i = 0; i < STRIP1_LEDS; i++) {
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
      for (uint16_t i = 0; i < STRIP1_LEDS; i++) {
        strip1[i] = CRGB::Black;
      }
      Serial.println("Show started!");
    }
  }

  if (showStarted) {
    unsigned long elapsedMillis = millis() - showStartMillis;
    activeShow.update(elapsedMillis);

    // Start new show automatically
    if (!activeShow.repeats && activeShow.isFinished(elapsedMillis)) {
      Serial.println("Show complete. Switching to next show...");

      // Example: cycle to testShow after weAreYourFriendsShow
      if (&activeShow == &weAreYourFriendsShow) {
        Serial.print("BTN:");
        Serial.println(100);
        // activeShow = ambianceShow;
      } else if (&activeShow == &loseYourselfShow) {
        Serial.print("BTN:");
        Serial.println(1);
        // activeShow = weAreYourFriendsShow;
      } else {
        Serial.print("BTN:");
        Serial.println(100);
        // activeShow = ambianceShow;
      }

      showStartMillis = millis();
      Serial.println("Next show started.");
    }

    if (millis() - lastLogMillis >= 1000) {
      lastLogMillis += 1000;
      Serial.print("Show time: ");
      Serial.print(elapsedMillis / 1000);
      Serial.println("s");
    }
  }

  FastLED.show();
} // end of loop