#include "oled_display.h"

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
bool isSelected_oled = true;
static unsigned long sStatusRefreshBlockedUntilMs = 0;

static void blockStatusRefresh(unsigned long holdMs) {
  sStatusRefreshBlockedUntilMs = millis() + holdMs;
}

uint16_t getUTF8Width(const char* text) {
  return u8g2.getUTF8Width(text);
}

void OLED_Init() {
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_wqy16_t_gb2312);
  u8g2.setFontMode(0);
}

void OLED_Clear() {
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}

void OLED_DrawText(const char* text, uint8_t x, uint8_t y) {
  blockStatusRefresh(900UL);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawUTF8(x, y, text);
  u8g2.sendBuffer();
}

void OLED_ShowFishingAnimation() {
  for (int progress = 0; progress <= 100; ++progress) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "BOOTING......");
    u8g2.drawFrame(0, 18, 128, 10);
    u8g2.drawBox(0, 18, map(progress, 0, 100, 0, 128), 10);
    u8g2.setCursor(95, 10);
    u8g2.print(progress);
    u8g2.setCursor(110, 10);
    u8g2.print("%");
    u8g2.sendBuffer();
    delay(60);
  }
}

void OLED_ShowChineseTitle(const char* text, uint8_t y) {
  blockStatusRefresh(900UL);
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_wqy16_t_gb2312);
  u8g2.setCursor(0, y);
  u8g2.print(text);
  u8g2.sendBuffer();
}

void drawMultiTextInvert(const char* text, uint8_t x, uint8_t y) {
  blockStatusRefresh(1200UL);
  u8g2.clearBuffer();

  const uint16_t textWidth = getUTF8Width(text);
  const uint8_t fontAscent = u8g2.getAscent();
  const uint8_t fontDescent = u8g2.getDescent();
  const uint8_t totalHeight = fontAscent + fontDescent;
  const uint8_t boxPadding = 2;
  const uint8_t topPadding = 4;
  const uint8_t bottomPadding = 6;
  const uint16_t boxWidth = textWidth + boxPadding * 2;
  const uint16_t boxHeight = totalHeight + topPadding + bottomPadding;

  if (isSelected_oled) {
    u8g2.setDrawColor(1);
    u8g2.drawBox(x - boxPadding, y - fontAscent - topPadding, boxWidth, boxHeight);
    u8g2.setDrawColor(0);
  } else {
    u8g2.setDrawColor(1);
  }

  u8g2.drawUTF8(x, y, text);
  u8g2.sendBuffer();
  u8g2.setDrawColor(1);
}

void OLED_ShowUpdateProgress(size_t written, size_t totalSize) {
  const int percent = totalSize == 0 ? 0 : static_cast<int>((written * 100) / totalSize);

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, "FW Updating....");
  u8g2.setCursor(95, 10);
  u8g2.print(percent);
  u8g2.setCursor(110, 10);
  u8g2.print("%");
  u8g2.drawFrame(0, 18, 128, 10);
  u8g2.drawBox(0, 18, map(percent, 0, 100, 0, 128), 10);
  u8g2.sendBuffer();
}

void OLED_ShowStatusLines(const char* line1,
                          const char* line2,
                          const char* line3,
                          const char* line4) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_5x8_tr);

  if (line1 && line1[0] != '\0') u8g2.drawStr(0, 8, line1);
  if (line2 && line2[0] != '\0') u8g2.drawStr(0, 16, line2);
  if (line3 && line3[0] != '\0') u8g2.drawStr(0, 24, line3);
  if (line4 && line4[0] != '\0') u8g2.drawStr(0, 32, line4);

  u8g2.sendBuffer();
}

bool OLED_CanRefreshStatus() {
  return millis() >= sStatusRefreshBlockedUntilMs;
}
