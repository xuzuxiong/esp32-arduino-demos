#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H
#include <Wire.h>
#include <U8g2lib.h>
//#include <U8g2lib.h>
#define SDA_PIN 21
#define SCL_PIN 22
// 创建 U8g2 对象，使用 I2C 通信，地址为 0x3C，屏幕尺寸为 128x32
extern U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2;

extern uint16_t getUTF8Width(const char *text);
extern bool isSelected_oled; 
void drawMultiTextInvert(const char* text, uint8_t x, uint8_t y);
void OLED_Init();
void OLED_Clear();
void OLED_DrawText(const char* text, uint8_t x, uint8_t y);
void OLED_ShowFishingAnimation() ;
void OLED_ShowChineseTitle(const char* text, uint8_t y);
void OLED_ShowUpdateProgress(size_t written, size_t totalSize) ;
//void OLED_ShowInvertedText(const char* text, uint8_t x, uint8_t y)//反显示模式

void OLED_ShowStatusLines(const char* line1,
                          const char* line2,
                          const char* line3,
                          const char* line4);
bool OLED_CanRefreshStatus();
#endif // OLED_DISPLAY_H
