#ifndef MENU_H
#define MENU_H

#include <Arduino.h>

extern byte Menu;
extern byte y_Pos;
extern byte X_Pos;
extern bool isSelected_xs;
extern byte Menu_KZ;
extern byte Menu_DY;
extern byte Menu_DG;
extern byte Menu_SZ;
extern byte Menu_GY;
extern bool isPaused;
extern byte Menu_QH;
extern bool MenuQH;
extern byte Menu_DYXX;
extern byte Menu_DGXX;
extern byte Menu_SZXX;
extern byte Menu_GYXX;
extern bool MenuKEY_A;
extern bool MenuKEY_B;
extern bool SJKJ;

extern unsigned int variableValue_AJ1;
extern unsigned int variableValue_AJ2;
extern unsigned int variableValue_AJ3;
extern unsigned int variableValue_AJ4;
extern unsigned int variableValue_AJ5;
extern unsigned int variableValue_AJ6;

extern unsigned int variableValue_JN1;
extern unsigned int variableValue_JN2;
extern unsigned int variableValue_JN3;
extern unsigned int variableValue_JN4;
extern unsigned int variableValue_JN5;
extern unsigned int variableValue_BF1;
extern unsigned int variableValue_BF2;
extern unsigned int variableValue_BF3;
extern unsigned int variableValue_BF4;
extern unsigned int variableValue_BF5;
extern unsigned int variableValue_GH1;
extern unsigned int variableValue_GH2;
extern unsigned int variableValue_GH3;
extern unsigned int variableValue_S;
extern unsigned int variableValue_X;
extern unsigned int variableValue_Z;
extern unsigned int variableValue_Y;
extern unsigned int variableValue_HC;
extern unsigned int variableValue_AX;

extern bool variableValue_MS;
extern bool variableValue_DGFS;
extern bool variableValue_WLZT;
extern bool variableValue_CCSZ;
extern bool MUT_YL;

extern char combinedText[50];
extern char combinedText_AJ1[50];
extern char combinedText_AJ2[50];
extern char combinedText_AJ3[50];
extern char combinedText_AJ4[50];
extern char combinedText_AJ5[50];
extern char combinedText_AJ6[50];
extern char combinedText_DYHC[50];
extern char combinedText_JN1[50];
extern char combinedText_JN2[50];
extern char combinedText_JN3[50];
extern char combinedText_JN4[50];
extern char combinedText_JN5[50];
extern char combinedText_BF1[50];
extern char combinedText_BF2[50];
extern char combinedText_BF3[50];
extern char combinedText_BF4[50];
extern char combinedText_BF5[50];
extern char combinedText_GH1[50];
extern char combinedText_GH2[50];
extern char combinedText_GH3[50];
extern char combinedText_S[50];
extern char combinedText_X[50];
extern char combinedText_Z[50];
extern char combinedText_Y[50];
extern char combinedText_DGHC[50];
extern char combinedText_AX[50];
extern char combinedText_ID[50];
extern char chipIdHexStr[32];

void Menu_begin();
void legend_pause_clear_hid(void);

#endif
