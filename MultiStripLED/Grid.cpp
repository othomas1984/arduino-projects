// Grid.cpp
#include "Grid.h"
#include "LEDSection.h"

// static const int column0[] = {0, 33, 34, 67, 68, 101, 102, 135, 136, 169, 170, 203, 204, 237, 238, 271, 272};
// static const int column1[] = {1, 32, 35, 66, 69, 100, 103, 134, 137, 168, 171, 202, 205, 236, 239, 270, 273};
// static const int column2[] = {2, 31, 36, 65, 70, 99, 104, 133, 138, 167, 172, 201, 206, 235, 240, 269, 274};
// static const int column3[] = {3, 30, 37, 64, 71, 98, 105, 132, 139, 166, 173, 200, 207, 234, 241, 268, 275};
// static const int column4[] = {4, 29, 38, 63, 72, 97, 106, 131, 140, 165, 174, 199, 208, 233, 242, 267, 276};
// static const int column5[] = {5, 28, 39, 62, 73, 96, 107, 130, 141, 164, 175, 198, 209, 232, 243, 266, 277};
// static const int column6[] = {6, 27, 40, 61, 74, 95, 108, 129, 142, 163, 176, 197, 210, 231, 244, 265, 278};
// static const int column7[] = {7, 26, 41, 60, 75, 94, 109, 128, 143, 162, 177, 196, 211, 230, 245, 264, 279};
// static const int column8[] = {8, 25, 42, 59, 76, 93, 110, 127, 144, 161, 178, 195, 212, 229, 246, 263, 280};
// static const int column9[] = {9, 24, 43, 58, 77, 92, 111, 126, 145, 160, 179, 194, 213, 228, 247, 262, 281};
// static const int column10[] = {10, 23, 44, 57, 78, 91, 112, 125, 146, 159, 180, 193, 214, 227, 248, 261, 282};
// static const int column11[] = {11, 22, 45, 56, 79, 90, 113, 124, 147, 158, 181, 192, 215, 226, 249, 260, 283};
// static const int column12[] = {12, 21, 46, 55, 80, 89, 114, 123, 148, 157, 182, 191, 216, 225, 250, 259, 284};
// static const int column13[] = {13, 20, 47, 54, 81, 88, 115, 122, 149, 156, 183, 190, 217, 224, 251, 258, 285};
// static const int column14[] = {14, 19, 48, 53, 82, 87, 116, 121, 150, 155, 184, 189, 218, 223, 252, 257, 286};
// static const int column15[] = {15, 18, 49, 52, 83, 86, 117, 120, 151, 154, 185, 188, 219, 222, 253, 256, 287};
// static const int column16[] = {16, 17, 50, 51, 84, 85, 118, 119, 152, 153, 186, 187, 220, 221, 254, 255, 288};

static const int column0[] = {0, 33, 34, 67, 68, 101, 102, 135, 136, 169, 170, 203, 204, 237, 238, 271, 272};
static const int column1[] = {273, 270, 239, 236, 205, 202, 171, 168, 137, 134, 103, 100, 69, 66, 35, 32, 1};
static const int column2[] = {2, 31, 36, 65, 70, 99, 104, 133, 138, 167, 172, 201, 206, 235, 240, 269, 274};
static const int column3[] = {275, 268, 241, 234, 207, 200, 173, 166, 139, 132, 105, 98, 71, 64, 37, 30, 3};
static const int column4[] = {4, 29, 38, 63, 72, 97, 106, 131, 140, 165, 174, 199, 208, 233, 242, 267, 276};
static const int column5[] = {277, 266, 243, 232, 209, 198, 175, 164, 141, 130, 107, 96, 73, 62, 39, 28, 5};
static const int column6[] = {6, 27, 40, 61, 74, 95, 108, 129, 142, 163, 176, 197, 210, 231, 244, 265, 278};
static const int column7[] = {279, 264, 245, 230, 211, 196, 177, 162, 143, 128, 109, 94, 75, 60, 41, 26, 7};
static const int column8[] = {8, 25, 42, 59, 76, 93, 110, 127, 144, 161, 178, 195, 212, 229, 246, 263, 280};
static const int column9[] = {281, 262, 247, 228, 213, 194, 179, 160, 145, 126, 111, 92, 77, 58, 43, 24, 9};
static const int column10[] = {10, 23, 44, 57, 78, 91, 112, 125, 146, 159, 180, 193, 214, 227, 248, 261, 282};
static const int column11[] = {283, 260, 249, 226, 215, 192, 181, 158, 147, 124, 113, 90, 79, 56, 45, 22, 11};
static const int column12[] = {12, 21, 46, 55, 80, 89, 114, 123, 148, 157, 182, 191, 216, 225, 250, 259, 284};
static const int column13[] = {285, 258, 251, 224, 217, 190, 183, 156, 149, 122, 115, 88, 81, 54, 47, 20, 13};
static const int column14[] = {14, 19, 48, 53, 82, 87, 116, 121, 150, 155, 184, 189, 218, 223, 252, 257, 286};
static const int column15[] = {287, 256, 253, 222, 219, 188, 185, 154, 151, 120, 117, 86, 83, 52, 49, 18, 15};
static const int column16[] = {16, 17, 50, 51, 84, 85, 118, 119, 152, 153, 186, 187, 220, 221, 254, 255, 288};

static const int fulStripGrid[] = 
{ 0, 33, 34, 67, 68, 101, 102, 135, 136, 169, 170, 203, 204, 237, 238, 271, 272,
  273, 270, 239, 236, 205, 202, 171, 168, 137, 134, 103, 100, 69, 66, 35, 32, 1,
  2, 31, 36, 65, 70, 99, 104, 133, 138, 167, 172, 201, 206, 235, 240, 269, 274,
  275, 268, 241, 234, 207, 200, 173, 166, 139, 132, 105, 98, 71, 64, 37, 30, 3,
  4, 29, 38, 63, 72, 97, 106, 131, 140, 165, 174, 199, 208, 233, 242, 267, 276,
  277, 266, 243, 232, 209, 198, 175, 164, 141, 130, 107, 96, 73, 62, 39, 28, 5,
  6, 27, 40, 61, 74, 95, 108, 129, 142, 163, 176, 197, 210, 231, 244, 265, 278,
  279, 264, 245, 230, 211, 196, 177, 162, 143, 128, 109, 94, 75, 60, 41, 26, 7,
  8, 25, 42, 59, 76, 93, 110, 127, 144, 161, 178, 195, 212, 229, 246, 263, 280,
  281, 262, 247, 228, 213, 194, 179, 160, 145, 126, 111, 92, 77, 58, 43, 24, 9,
  10, 23, 44, 57, 78, 91, 112, 125, 146, 159, 180, 193, 214, 227, 248, 261, 282,
  283, 260, 249, 226, 215, 192, 181, 158, 147, 124, 113, 90, 79, 56, 45, 22, 11,
  12, 21, 46, 55, 80, 89, 114, 123, 148, 157, 182, 191, 216, 225, 250, 259, 284,
  285, 258, 251, 224, 217, 190, 183, 156, 149, 122, 115, 88, 81, 54, 47, 20, 13,
  14, 19, 48, 53, 82, 87, 116, 121, 150, 155, 184, 189, 218, 223, 252, 257, 286,
  287, 256, 253, 222, 219, 188, 185, 154, 151, 120, 117, 86, 83, 52, 49, 18, 15,
  16, 17, 50, 51, 84, 85, 118, 119, 152, 153, 186, 187, 220, 221, 254, 255, 288};

Grid::Grid(Adafruit_NeoPixel* strip) :
  strip(strip),
  sections {
    LEDSection(strip, 0, 16),
    LEDSection(strip, 17, 33),
    LEDSection(strip, 34, 50),
    LEDSection(strip, 51, 67),
    LEDSection(strip, 68, 84),
    LEDSection(strip, 85, 101),
    LEDSection(strip, 102, 118),
    LEDSection(strip, 119, 135),
    LEDSection(strip, 136, 152),
    LEDSection(strip, 153, 169),
    LEDSection(strip, 170, 186),
    LEDSection(strip, 187, 203),
    LEDSection(strip, 204, 220),
    LEDSection(strip, 221, 237),
    LEDSection(strip, 238, 254),
    LEDSection(strip, 255, 271),
    LEDSection(strip, 272, 288),
  },
  sections2 {
    LEDSection(strip, 17, column0),
    LEDSection(strip, 17, column1),
    LEDSection(strip, 17, column2),
    LEDSection(strip, 17, column3),
    LEDSection(strip, 17, column4),
    LEDSection(strip, 17, column5),
    LEDSection(strip, 17, column6),
    LEDSection(strip, 17, column7),
    LEDSection(strip, 17, column8),
    LEDSection(strip, 17, column9),
    LEDSection(strip, 17, column10),
    LEDSection(strip, 17, column11),
    LEDSection(strip, 17, column12),
    LEDSection(strip, 17, column13),
    LEDSection(strip, 17, column14),
    LEDSection(strip, 17, column15),
    LEDSection(strip, 17, column16),
  },
  fullStrip(strip, 0, 288),
  fullStrip2(strip, 289, fulStripGrid)
{
}

void Grid::setup() {
  fullStrip.eraseAll(0);
}

void Grid::loop() {
  for (int i = 0; i < sizeof(sections) / sizeof(sections[0]); i++) {
    sections[i].rainbowWipe(1, true);
  }
  for (int i = 0; i < 34; i++) {
    for (int sectionIndex = 0; sectionIndex < sizeof(sections) / sizeof(sections[0]); sectionIndex++) {
      sections[sectionIndex].rotate((sectionIndex % 2 == 0));
    }
    strip->show();
    delay(75);
  }
  for (int i = 0; i < sizeof(sections) / sizeof(sections[0]); i++) {
    int pixelHue = 0 + ((i) * 65536L / (sizeof(sections) / sizeof(sections[0])));
    sections[i].setSectionColor(strip->gamma32(strip->ColorHSV(pixelHue)));
    strip->show();
    delay(100);
  }
  // delay(1000); // Wait for 200ms
  fullStrip.rainbow(5, 3, false);
  // fullStrip.eraseAll(1, strip->Color(50, 20, 3));
  fullStrip.eraseAll(1, strip->Color(20, 8, 2));
  // fullStrip.eraseAll(1, strip->Color(10, 4, 1));
  for (int i = 0; i < sizeof(sections2) / sizeof(sections2[0]); i++) {
    sections2[i].rainbowWipe(1, true);
  }
  for (int i = 0; i < 34; i++) {
    for (int sectionIndex = 0; sectionIndex < sizeof(sections2) / sizeof(sections2[0]); sectionIndex++) {
      sections2[sectionIndex].rotate((sectionIndex % 2 == 0));
    }
    strip->show();
    delay(75);
  }
  for (int i = 0; i < sizeof(sections2) / sizeof(sections2[0]); i++) {
    int pixelHue = 0 + ((i) * 65536L / (sizeof(sections2) / sizeof(sections2[0])));
    sections2[i].setSectionColor(strip->gamma32(strip->ColorHSV(pixelHue)));
    strip->show();
    delay(100);
  }
  // delay(1000); // Wait for 200ms
  fullStrip2.rainbow(5, 3, false);
  fullStrip.eraseAll(1, strip->Color(20, 8, 2));
}
