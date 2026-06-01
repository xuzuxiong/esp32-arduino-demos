#ifndef KEYPAD_H
#define KEYPAD_H

#include <Arduino.h>

#define MUT_OUT 15
#define MUT_UP 19
#define MUT_DN 18
#define Audio 4
#define Wifi_AA 2

#define DEBOUNCE_TIME 50
#define LONG_PRESS_TIME 1000
#define DOUBLE_CLICK_TIME 600
#define HOLD_TIME_THRESHOLD 1500
#define HOLD_INCREMENT_INTERVAL 50

static const int buttonPins[] = {36, 35, 23, 5, 34, 39};

class Keypad {
public:
  Keypad(const int* pins, int numKeys);
  ~Keypad();

  void update();

private:
  void debounce(int keyIndex);
  void handlePress(int keyIndex);
  void handleRelease(int keyIndex);
  void checkLongPress(int keyIndex);
  void checkClickTimeout(int keyIndex);
  void checkHoldIncrement(int keyIndex);

  const int* _buttonPins;
  int _numKeys;
  bool* _keyState;
  bool* _lastKeyState;
  unsigned long* _lastDebounceTime;
  unsigned long* _lastPressTime;
  unsigned long* _lastReleaseTime;
  int* _clickCount;
  bool* _longPressReported;
  unsigned long* _lastHoldIncrementTime;
};

#endif
