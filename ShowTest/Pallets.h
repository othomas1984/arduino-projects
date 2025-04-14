#pragma once
#include <FastLED.h>

// ===== 2-stop Gradient =====
inline CRGBPalette16 make2StopGradient(CRGB a, CRGB b) {
  CRGBPalette16 p;
  for (int i = 0; i < 16; i++) {
    p[i] = blend(a, b, i * 17);  // 0-255 evenly
  }
  return p;
}

// ===== 3-stop Gradient =====
inline CRGBPalette16 make3StopGradient(CRGB a, CRGB b, CRGB c) {
  CRGBPalette16 p;
  for (int i = 0; i < 8; i++) {
    p[i] = blend(a, b, i * 34);  // 0-255 across 8 steps
  }
  for (int i = 0; i < 8; i++) {
    p[8 + i] = blend(b, c, i * 34);
  }
  return p;
}

// ===== 4-stop Gradient =====
inline CRGBPalette16 make4StopGradient(CRGB a, CRGB b, CRGB c, CRGB d) {
  CRGBPalette16 p;
  for (int i = 0; i < 5; i++) {
    p[i] = blend(a, b, i * 64);  // a→b
  }
  for (int i = 0; i < 6; i++) {
    p[5 + i] = blend(b, c, i * 42);  // b→c
  }
  for (int i = 0; i < 5; i++) {
    p[11 + i] = blend(c, d, i * 64);  // c→d
  }
  return p;
}

// ===== Predefined palettes =====

inline CRGBPalette16 WhitePalette() {
  return CRGBPalette16(CRGB::DarkGray);
}

inline CRGBPalette16 WhiteToDarkBluePalette() {
  return make2StopGradient(CRGB::DarkGray, CRGB(0, 0, 64));
}

inline CRGBPalette16 BlueToPurpleToSeaFoamPalette() {
  return make3StopGradient(
    CRGB(0, 0, 64),        // dark blue
    CRGB(48, 0, 64),       // purple
    CRGB(128, 255, 192)    // sea foam
  );
}
