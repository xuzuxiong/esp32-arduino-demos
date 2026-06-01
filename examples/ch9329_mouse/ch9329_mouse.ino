/**
 * Minimal example: CH9329 UART HID mouse click
 *
 * Hardware: ESP32 GPIO16(RX) <- CH9329 TX, GPIO17(TX) -> CH9329 RX, GND common
 * Baud: 115200 8N1
 *
 * Every 3 seconds sends left click to USB host via CH9329.
 */
#include <Arduino.h>

static HardwareSerial ch9329Serial(2);

static const int RX_PIN = 16;
static const int TX_PIN = 17;
static const uint32_t BAUD = 115200;

static void sendFrame(const uint8_t* frame, size_t length) {
  ch9329Serial.write(frame, length);
}

static void sendAbsMouse(uint16_t x, uint16_t y) {
  const uint16_t screenW = 1920;
  const uint16_t screenH = 1080;
  const uint16_t chipX = (x * 4096) / screenW;
  const uint16_t chipY = (y * 4096) / screenH;

  uint8_t frame[] = {
      0x57, 0xAB, 0x00, 0x04, 0x07, 0x02, 0x00,
      lowByte(chipX), highByte(chipX),
      lowByte(chipY), highByte(chipY),
      0x00, 0x00, 0x00};

  uint8_t sum = 0;
  for (size_t i = 0; i < 12; ++i) sum += frame[i];
  frame[12] = sum;
  sendFrame(frame, sizeof(frame));
}

static void clickLeft() {
  uint8_t press[] = {0x57, 0xAB, 0x00, 0x05, 0x05, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t release[] = {0x57, 0xAB, 0x00, 0x05, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  uint8_t sum = 0;
  for (int i = 0; i < 11; ++i) sum += press[i];
  press[11] = sum;
  sum = 0;
  for (int i = 0; i < 11; ++i) sum += release[i];
  release[11] = sum;
  sendFrame(press, sizeof(press));
  delay(20);
  sendFrame(release, sizeof(release));
}

void setup() {
  Serial.begin(115200);
  delay(500);
  ch9329Serial.begin(BAUD, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.println("CH9329 mouse example ready");
}

void loop() {
  Serial.println("Move + click at (960,540)");
  sendAbsMouse(960, 540);
  delay(100);
  clickLeft();
  delay(3000);
}
