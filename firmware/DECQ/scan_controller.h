#ifndef SCAN_CONTROLLER_H
#define SCAN_CONTROLLER_H

#include <Arduino.h>
#include <HardwareSerial.h>

constexpr unsigned long BAUD_RATE = 115200;

extern HardwareSerial mySerial;

#ifndef SCREEN_W
#define SCREEN_W 1920
#endif

#ifndef SCREEN_H
#define SCREEN_H 1080
#endif

constexpr int SCAN_START_X = 800;
constexpr int SCAN_START_Y = 180;
constexpr int SCAN_WIDTH = 360;
constexpr int SCAN_HEIGHT = 100;
constexpr int X_STEP = 20;
constexpr int Y_STEP = 20;
constexpr int STEP_DELAY = 1;
constexpr int CLICK_DELAY = 3;

constexpr uint8_t KEY_CODE_9 = 0x39;

constexpr uint8_t KEY_A = 0x04;
constexpr uint8_t KEY_B = 0x05;
constexpr uint8_t KEY_C = 0x06;
constexpr uint8_t KEY_D = 0x07;
constexpr uint8_t KEY_E = 0x08;
constexpr uint8_t KEY_F = 0x09;
constexpr uint8_t KEY_G = 0x0A;
constexpr uint8_t KEY_H = 0x0B;
constexpr uint8_t KEY_I = 0x0C;
constexpr uint8_t KEY_J = 0x0D;
constexpr uint8_t KEY_K = 0x0E;
constexpr uint8_t KEY_L = 0x0F;
constexpr uint8_t KEY_M = 0x10;
constexpr uint8_t KEY_N = 0x11;
constexpr uint8_t KEY_O = 0x12;
constexpr uint8_t KEY_P = 0x13;
constexpr uint8_t KEY_Q = 0x14;
constexpr uint8_t KEY_R = 0x15;
constexpr uint8_t KEY_S = 0x16;
constexpr uint8_t KEY_T = 0x17;
constexpr uint8_t KEY_U = 0x18;
constexpr uint8_t KEY_V = 0x19;
constexpr uint8_t KEY_W = 0x1A;
constexpr uint8_t KEY_X = 0x1B;
constexpr uint8_t KEY_Y = 0x1C;
constexpr uint8_t KEY_Z = 0x1D;

constexpr uint8_t KEY_1 = 0x1E;
constexpr uint8_t KEY_2 = 0x1F;
constexpr uint8_t KEY_3 = 0x20;
constexpr uint8_t KEY_4 = 0x21;
constexpr uint8_t KEY_5 = 0x22;
constexpr uint8_t KEY_6 = 0x23;
constexpr uint8_t KEY_7 = 0x24;
constexpr uint8_t KEY_8 = 0x25;
constexpr uint8_t KEY_9 = 0x26;
constexpr uint8_t KEY_0 = 0x27;

constexpr uint8_t KEY_ENTER = 0x28;
constexpr uint8_t KEY_ESC = 0x29;
constexpr uint8_t KEY_BACKSPACE = 0x2A;
constexpr uint8_t KEY_TAB = 0x2B;
constexpr uint8_t KEY_SPACE = 0x2C;
constexpr uint8_t KEY_MINUS = 0x2D;
constexpr uint8_t KEY_EQUAL = 0x2E;
constexpr uint8_t KEY_LEFT_BRACKET = 0x2F;
constexpr uint8_t KEY_RIGHT_BRACKET = 0x30;
constexpr uint8_t KEY_BACKSLASH = 0x31;
constexpr uint8_t KEY_SEMICOLON = 0x33;
constexpr uint8_t KEY_SINGLE_QUOTE = 0x34;
constexpr uint8_t KEY_TILDE = 0x35;
constexpr uint8_t KEY_COMMA = 0x36;
constexpr uint8_t KEY_DOT = 0x37;
constexpr uint8_t KEY_SLASH = 0x38;
constexpr uint8_t KEY_CAPS_LOCK = 0x39;

constexpr uint8_t KEY_F1 = 0x3A;
constexpr uint8_t KEY_F2 = 0x3B;
constexpr uint8_t KEY_F3 = 0x3C;
constexpr uint8_t KEY_F4 = 0x3D;
constexpr uint8_t KEY_F5 = 0x3E;
constexpr uint8_t KEY_F6 = 0x3F;
constexpr uint8_t KEY_F7 = 0x40;
constexpr uint8_t KEY_F8 = 0x41;
constexpr uint8_t KEY_F9 = 0x42;
constexpr uint8_t KEY_F10 = 0x43;
constexpr uint8_t KEY_F11 = 0x44;
constexpr uint8_t KEY_F12 = 0x45;

constexpr uint8_t KEY_PRINT_SCR = 0x46;
constexpr uint8_t KEY_SCROLL_LOCK = 0x47;
constexpr uint8_t KEY_PAUSE_BREAK = 0x48;
constexpr uint8_t KEY_INSERT = 0x49;
constexpr uint8_t KEY_HOME = 0x4A;
constexpr uint8_t KEY_PAGE_UP = 0x4B;
constexpr uint8_t KEY_DELETE = 0x4C;
constexpr uint8_t KEY_END = 0x4D;
constexpr uint8_t KEY_PAGE_DOWN = 0x4E;

constexpr uint8_t KEY_RIGHT_ARROW = 0x4F;
constexpr uint8_t KEY_LEFT_ARROW = 0x50;
constexpr uint8_t KEY_DOWN_ARROW = 0x51;
constexpr uint8_t KEY_UP_ARROW = 0x52;

constexpr uint8_t KEY_NUM_LOCK = 0x53;
constexpr uint8_t KEY_NUM_DIVIDE = 0x54;
constexpr uint8_t KEY_NUM_MULTIPLY = 0x55;
constexpr uint8_t KEY_NUM_SUBTRACT = 0x56;
constexpr uint8_t KEY_NUM_ADD = 0x57;
constexpr uint8_t KEY_NUM_ENTER = 0x58;
constexpr uint8_t KEY_NUM_7 = 0x59;
constexpr uint8_t KEY_NUM_8 = 0x5A;
constexpr uint8_t KEY_NUM_9 = 0x5B;
constexpr uint8_t KEY_NUM_4 = 0x5C;
constexpr uint8_t KEY_NUM_5 = 0x5D;
constexpr uint8_t KEY_NUM_6 = 0x5E;
constexpr uint8_t KEY_NUM_1 = 0x5F;
constexpr uint8_t KEY_NUM_2 = 0x60;
constexpr uint8_t KEY_NUM_3 = 0x61;
constexpr uint8_t KEY_NUM_0 = 0x62;
constexpr uint8_t KEY_NUM_DECIMAL = 0x63;

constexpr uint8_t KEY_LEFT_CTRL = 0x01;
constexpr uint8_t KEY_RIGHT_CTRL = 0x10;
constexpr uint8_t KEY_LEFT_SHIFT = 0x02;
constexpr uint8_t KEY_RIGHT_SHIFT = 0x20;
constexpr uint8_t KEY_LEFT_ALT = 0x04;
constexpr uint8_t KEY_RIGHT_ALT = 0x40;
constexpr uint8_t KEY_LEFT_WIN = 0x08;
constexpr uint8_t KEY_RIGHT_WIN = 0x80;

uint8_t mapNumberToKeyCode(int number);
void sendKeyByNumber(int number);
void initSerial();
void clickRightPress();
void clickRightRelease();
void clickLeftPress();
void clickLeftRelease();
void scrollWheelDown();
void scrollWheelUp();
void sendAbsMouse(uint16_t x, uint16_t y);
void sendRelMouse(int8_t dx, int8_t dy);
void sendFrame(uint8_t* frame, size_t length);
void performScan();
void releaseKey(uint8_t keyCode);
void releaseAllKeys();
void sendKey(uint8_t keyCode);
void moveMouseTo1278_1008();
void moveMouseTo956_573();
void moveMouseTo1670_93();
void moveMouseTo645_302();
void moveMouseTo645_329();
void moveMouseTo1115_820();
void moveMouseTo944_944();
void moveMouseTo674_334();

#endif
