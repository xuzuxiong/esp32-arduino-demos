#include "menu.h"

#include "TimerManager.h"
#include "oled_display.h"

#include <stdio.h>

byte Menu = 0;
byte y_Pos = 26;
byte X_Pos = 30;
bool isSelected_xs = false;
byte Menu_KZ = 0;
byte Menu_DY = 0;
byte Menu_DG = 0;
byte Menu_SZ = 0;
byte Menu_GY = 0;
byte Menu_QH = 0;
bool MenuQH = true;
byte Menu_DYXX = 0;
byte Menu_DGXX = 0;
byte Menu_SZXX = 0;
byte Menu_GYXX = 0;
bool MenuKEY_A = true;
bool MenuKEY_B = true;

unsigned int variableValue_AJ1 = 1;
unsigned int variableValue_AJ2 = 90;
unsigned int variableValue_AJ3 = 210;
unsigned int variableValue_AJ4 = 270;
unsigned int variableValue_AJ5 = 240;
unsigned int variableValue_AJ6 = 850;

unsigned int variableValue_JN1 = 1;
unsigned int variableValue_JN2 = 2;
unsigned int variableValue_JN3 = 3;
unsigned int variableValue_JN4 = 15;
unsigned int variableValue_JN5 = 10;
unsigned int variableValue_BF1 = 20;
unsigned int variableValue_BF2 = 31;
unsigned int variableValue_BF3 = 1800;
unsigned int variableValue_BF4 = 3600;
unsigned int variableValue_BF5 = 1;
unsigned int variableValue_GH1 = 99999;
unsigned int variableValue_GH2 = 99999;
unsigned int variableValue_GH3 = 99999;
unsigned int variableValue_S = 9999;
unsigned int variableValue_X = 9999;
unsigned int variableValue_Z = 9999;
unsigned int variableValue_Y = 9999;
unsigned int variableValue_HC = 16000;
unsigned int variableValue_AX = 100;

bool variableValue_MS = true;
bool variableValue_DGFS = false;
bool variableValue_WLZT = false;
bool variableValue_CCSZ = false;
bool MUT_YL = true;

char combinedText[50];
char combinedText_AJ1[50] = {0};
char combinedText_AJ2[50] = {0};
char combinedText_AJ3[50] = {0};
char combinedText_AJ4[50] = {0};
char combinedText_AJ5[50] = {0};
char combinedText_AJ6[50] = {0};
char combinedText_DYHC[50] = {0};
char combinedText_JN1[50] = {0};
char combinedText_JN2[50] = {0};
char combinedText_JN3[50] = {0};
char combinedText_JN4[50] = {0};
char combinedText_JN5[50] = {0};
char combinedText_BF1[50] = {0};
char combinedText_BF2[50] = {0};
char combinedText_BF3[50] = {0};
char combinedText_BF4[50] = {0};
char combinedText_BF5[50] = {0};
char combinedText_GH1[50] = {0};
char combinedText_GH2[50] = {0};
char combinedText_GH3[50] = {0};
char combinedText_S[50] = {0};
char combinedText_X[50] = {0};
char combinedText_Z[50] = {0};
char combinedText_Y[50] = {0};
char combinedText_DGHC[50] = {0};
char combinedText_AX[50] = {0};
char combinedText_ID[50] = {0};
char chipIdHexStr[32];

bool isPaused = true;
bool SJKJ = false;

static void formatSecondsValue(char* out, size_t outSize, unsigned int value) {
  if (value == 0 || value >= 99999U) {
    snprintf(out, outSize, "OFF");
    return;
  }

  snprintf(out, outSize, "%us", value);
}

static void buildActionLine(char* out,
                            size_t outSize,
                            const char* nameA,
                            unsigned int valueA,
                            const char* nameB,
                            unsigned int valueB) {
  char left[8];
  char right[8];
  formatSecondsValue(left, sizeof(left), valueA);
  formatSecondsValue(right, sizeof(right), valueB);
  snprintf(out, outSize, "%s:%s %s:%s", nameA, left, nameB, right);
}

static void drawFishingScreen() {
  char line1[24];
  char line2[24];
  char line3[24];
  char line4[24];

  snprintf(line1, sizeof(line1), "FISH %s", isPaused ? "PAUSE" : "RUN");
  snprintf(line2, sizeof(line2), "ST:%s", getFishingStateLabel());

  if (Menu_QH == 4) {
    snprintf(line3, sizeof(line3), "B:= C:- R:scan");
    snprintf(line4, sizeof(line4), "R fallback:0 key");
    OLED_ShowStatusLines(line1, line2, line3, line4);
    return;
  }

  const unsigned long screenPhase = (millis() / 2500UL) % 2UL;
  if (screenPhase == 0) {
    buildActionLine(line3, sizeof(line3), "A1", variableValue_AJ1, "A2",
                    variableValue_AJ2);
    buildActionLine(line4, sizeof(line4), "A3", variableValue_AJ3, "A4",
                    variableValue_AJ4);
  } else {
    buildActionLine(line3, sizeof(line3), "A5", variableValue_AJ5, "A6",
                    variableValue_AJ6);
    snprintf(line4, sizeof(line4), "Bait:%lus",
             (getFishingBaitRemainingMs() + 999UL) / 1000UL);
  }

  OLED_ShowStatusLines(line1, line2, line3, line4);
}

static void drawMonsterScreen() {
  char line1[24];
  char line2[24];
  char line3[24];
  char line4[24];

  snprintf(line1, sizeof(line1), "MON %s", isPaused ? "PAUSE" : "RUN");
  buildActionLine(line2, sizeof(line2), "J1", variableValue_JN1, "J2",
                  variableValue_JN2);
  buildActionLine(line3, sizeof(line3), "J3", variableValue_JN3, "J4",
                  variableValue_JN4);
  snprintf(line4, sizeof(line4), "J5:%us AX:%ums", variableValue_JN5,
           variableValue_AX);
  OLED_ShowStatusLines(line1, line2, line3, line4);
}

static void drawSystemScreen() {
  char line1[24];
  char line2[24];
  char line3[24];
  char line4[24];

  snprintf(line1, sizeof(line1), "SYS MENU:%u", Menu);
  snprintf(line2, sizeof(line2), "NET:%s MUTE:%s",
           variableValue_WLZT ? "ON" : "OFF", MUT_YL ? "ON" : "OFF");
  snprintf(line3, sizeof(line3), "AX:%ums HC:%us", variableValue_AX,
           variableValue_HC);
  snprintf(line4, sizeof(line4), "GUIDE:%s", Menu_QH == 4 ? "ON" : "OFF");
  OLED_ShowStatusLines(line1, line2, line3, line4);
}

void Menu_begin() {
  isSelected_oled = isSelected_xs;

  if (!OLED_CanRefreshStatus()) {
    return;
  }

  static unsigned long lastDrawMs = 0;
  if (millis() - lastDrawMs < 150UL) {
    return;
  }
  lastDrawMs = millis();

  if (Menu == 0) {
    drawFishingScreen();
    return;
  }

  if (Menu == 1) {
    drawMonsterScreen();
    return;
  }

  drawSystemScreen();
}
