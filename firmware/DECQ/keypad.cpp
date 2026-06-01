#include "keypad.h"
#include "menu.h"
#include "oled_display.h"
#include <Preferences.h>
#include "TimerManager.h"
#include "scan_controller.h"

extern Preferences nvs;
bool state = false;
bool statea = false;

Keypad::Keypad(const int* pins, int numKeys) {
  _buttonPins = pins;
  _numKeys = numKeys;
  _keyState = new bool[_numKeys]();
  _lastKeyState = new bool[_numKeys]();
  _lastDebounceTime = new unsigned long[_numKeys]();
  _lastPressTime = new unsigned long[_numKeys]();
  _lastReleaseTime = new unsigned long[_numKeys]();
  _clickCount = new int[_numKeys]();
  _longPressReported = new bool[_numKeys]();
  _lastHoldIncrementTime = new unsigned long[_numKeys]();
  for (int i = 0; i < _numKeys; i++) {
    pinMode(_buttonPins[i], INPUT_PULLUP);
    _longPressReported[i] = false;
    _lastHoldIncrementTime[i] = 0;
  }
}

Keypad::~Keypad() {
  delete[] _keyState;
  delete[] _lastKeyState;
  delete[] _lastDebounceTime;
  delete[] _lastPressTime;
  delete[] _lastReleaseTime;
  delete[] _clickCount;
  delete[] _longPressReported;
  delete[] _lastHoldIncrementTime;
}

void Keypad::update() {
  for (int i = 0; i < _numKeys; i++) {
    debounce(i);
    if (_keyState[i]) {
      checkLongPress(i);
      checkHoldIncrement(i);
    }
    checkClickTimeout(i);
  }
}

void Keypad::debounce(int keyIndex) {
  bool rawState = (digitalRead(_buttonPins[keyIndex]) == LOW);
  if (rawState != _lastKeyState[keyIndex]) {
    _lastDebounceTime[keyIndex] = millis();
  }
  if ((millis() - _lastDebounceTime[keyIndex]) > DEBOUNCE_TIME) {
    if (rawState != _keyState[keyIndex]) {
      _keyState[keyIndex] = rawState;
      if (_keyState[keyIndex]) {
        handlePress(keyIndex);
      } else {
        handleRelease(keyIndex);
      }
    }
  }
  _lastKeyState[keyIndex] = rawState;
}

void Keypad::handlePress(int keyIndex) {
  _lastPressTime[keyIndex] = millis();
  _lastHoldIncrementTime[keyIndex] = 0;
}

void Keypad::handleRelease(int keyIndex) {
  unsigned long pressDuration = millis() - _lastPressTime[keyIndex];
  _lastReleaseTime[keyIndex] = millis();
  if (pressDuration < LONG_PRESS_TIME) {
    _clickCount[keyIndex]++;
  }
  _longPressReported[keyIndex] = false;
}

void Keypad::checkLongPress(int keyIndex) {
  unsigned long pressDuration = millis() - _lastPressTime[keyIndex];
  if (pressDuration > LONG_PRESS_TIME && !_longPressReported[keyIndex]) {
    Serial.print("长按检测: 按键");
    Serial.println(keyIndex);
    _longPressReported[keyIndex] = true;
  }
}

void Keypad::checkClickTimeout(int keyIndex) {
  if (_clickCount[keyIndex] > 0 && (millis() - _lastReleaseTime[keyIndex]) > DOUBLE_CLICK_TIME) {
    if (_clickCount[keyIndex] == 1) {
      if (keyIndex == 2) {
        MUT_YL=!MUT_YL;
        Serial.println("静音控制");
        if (MUT_YL == true) { drawMultiTextInvert("开启声音", 26, y_Pos); }
        if (MUT_YL== false) { drawMultiTextInvert("静音", 42, y_Pos); }
        nvs.putBool("MUT_YL", MUT_YL);  // 存储 bool 类型数据
        delay(500);  // 适当延时，避免CPU占用过高（非阻塞）
      }

      Serial.print("单击检测: 按键");
      Serial.println(keyIndex);
    } else if (_clickCount[keyIndex] >= 2) {
if(keyIndex == 4) {
    // 先切换状态
    enabled = !enabled;
    
    // 根据新状态执行相应操作
    if(enabled) {
        drawMultiTextInvert("定时开启", 10, y_Pos);
         Serial.println(enabled);
    } else {
        drawMultiTextInvert("定时关闭", 10, y_Pos);
         Serial.println(enabled);
    }
    
    // 延时和保存状态（注意延时可能阻塞UI）
    DYHC=true;
    delay(500);
    nvs.putBool("enabled", enabled);
}

if(keyIndex == 0){  
    state = !state;
      if (state){ 
         Menu_QH=4;
      Menu_GYXX=0;
      }else {
    Menu_QH=0;
    }

      
      }
if(keyIndex == 1){

      if (!variableValue_WLZT){
        drawMultiTextInvert("网络状态：启用", 0, y_Pos);
 
              variableValue_WLZT = true;
           nvs.putBool("WLZT", variableValue_WLZT); 
           delay(1000);
           ESP.restart();  // 重启设备
            
      }
      if (variableValue_WLZT){
        drawMultiTextInvert("网络状态：停用", 0, y_Pos);
             variableValue_WLZT = false; 
           nvs.putBool("WLZT", variableValue_WLZT); 
           delay(1000);
           ESP.restart();  // 重启设备
    }

}

if(keyIndex == 5){
      statea = !statea;
if(statea){Menu=4;YDYD=true;}
if(!statea){Menu=0;}


}


      Serial.print("双击检测: 按键");
      Serial.println(keyIndex);
    }

    _clickCount[keyIndex] = 0;
  }
}
// 长按不放检测

void Keypad::checkHoldIncrement(int keyIndex) {
  unsigned long pressDuration = millis() - _lastPressTime[keyIndex];
  if (pressDuration > HOLD_TIME_THRESHOLD && (millis() - _lastHoldIncrementTime[keyIndex]) > HOLD_INCREMENT_INTERVAL) {
    if (keyIndex == 4) {
      digitalWrite(MUT_UP, LOW);  // 抛竿按键初始为高电平
    //  drawMultiTextInvert("音量++", 26, y_Pos);
      delay(100);                 // 适当延时，避免CPU占用过高（非阻塞）
    }

    if (keyIndex == 5) {
      digitalWrite(MUT_DN, LOW);  // 抛竿按键初始为高电平
     // drawMultiTextInvert("音量--", 26, y_Pos);
      delay(100);                 // 适当延时，避免CPU占用过高（非阻塞）
    }

    if (keyIndex == 0) {
     
         
      if (MenuKEY_A) {
        if (Menu_KZ == 0) {
          Menu++;
          if (Menu == 5) {
            Menu = 0;
            DDS_A1=false;
             DYS_A1=true;
            
          }
        }
        if (Menu_KZ == 1) {
          Menu_DY++;
          if (Menu_DY == 7) {
            Menu_DY = 0;
          }
        }
        if (Menu_KZ == 2) {
          Menu_DG++;
          if (Menu_DG == 18) {
            Menu_DG = 0;
          }
        }
        if (Menu_KZ == 3) {
          Menu_SZ++;
          if (Menu_SZ == 6) {
            Menu_SZ = 0;
          }
        }
        if (Menu_KZ == 4) {
          Menu_GY++;
          if (Menu_GY == 4) {
            Menu_GY = 0;
          }
        }
      } else {

        if (Menu_DYXX == 0) {
          variableValue_AJ1++;
          if (variableValue_AJ1 == 100000) { variableValue_AJ1 = 1; }
           nvs.putInt("AJ1", variableValue_AJ1 );  //按下时间储存
        }
        if (Menu_DYXX == 1) {
          variableValue_AJ2++;
          if (variableValue_AJ2 == 100000) { variableValue_AJ2 = 1; }
           nvs.putInt("AJ2", variableValue_AJ2 );  //按下时间储存
        }
        if (Menu_DYXX == 2) {
          variableValue_AJ3++;
          if (variableValue_AJ3 == 100000) { variableValue_AJ3 = 1; }
           nvs.putInt("AJ3", variableValue_AJ3 );  //按下时间储存
        }
        if (Menu_DYXX == 3) {
          variableValue_AJ4++;
          if (variableValue_AJ4 == 100000) { variableValue_AJ4 = 1; }
           nvs.putInt("AJ4", variableValue_AJ4 );  //按下时间储存
        }
        if (Menu_DYXX == 4) {
          variableValue_AJ5++;
          if (variableValue_AJ5 == 100000) { variableValue_AJ5 = 1; }
           nvs.putInt("AJ5", variableValue_AJ5 );  //按下时间储存
        }
        if (Menu_DYXX == 5) {
          variableValue_AJ6++;
          if (variableValue_AJ6 == 100000) { variableValue_AJ6 = 1; }
           nvs.putInt("AJ6", variableValue_AJ6 );  //按下时间储存
        }

        if (Menu_DGXX == 0) {
          variableValue_JN1++;
          if (variableValue_JN1 == 100000) { variableValue_JN1 = 1; }
           nvs.putInt("JN1", variableValue_JN1 );  //按下时间储存
        }
        if (Menu_DGXX == 1) {
          variableValue_JN2++;
          if (variableValue_JN2 == 100000) { variableValue_JN2 = 1; }
           nvs.putInt("JN2", variableValue_JN2 );  //按下时间储存
        }
        if (Menu_DGXX == 2) {
          variableValue_JN3++;
          if (variableValue_JN3 == 100000) { variableValue_JN3 = 1; }
           nvs.putInt("JN3", variableValue_JN3 );  //按下时间储存
        }
        if (Menu_DGXX == 3) {
          variableValue_JN4++;
          if (variableValue_JN4 == 100000) { variableValue_JN4 = 1; }
          nvs.putInt("JN4", variableValue_JN4 );  //按下时间储存
        }
        if (Menu_DGXX == 4) {
          variableValue_JN5++;
          if (variableValue_JN5 == 100000) { variableValue_JN5 = 1; }
            nvs.putInt("JN5", variableValue_JN5 );  //按下时间储存
        }
        if (Menu_DGXX == 5) {
          variableValue_BF1++;
          if (variableValue_BF1 == 100000) { variableValue_BF1 = 1; }
            nvs.putInt("BF1", variableValue_BF1 );  //按下时间储存
        }
        if (Menu_DGXX == 6) {
          variableValue_BF2++;
          if (variableValue_BF2 == 100000) { variableValue_BF2 = 1; }
            nvs.putInt("BF2", variableValue_BF2 );  //按下时间储存
        }
        if (Menu_DGXX == 7) {
          variableValue_BF3++;
          if (variableValue_BF3 == 100000) { variableValue_BF3 = 1; }
            nvs.putInt("BF3", variableValue_BF3 );  //按下时间储存
        }
        if (Menu_DGXX == 8) {
          variableValue_BF4++;
          if (variableValue_BF4 == 100000) { variableValue_BF4 = 1; }
           nvs.putInt("BF4", variableValue_BF4 );  //按下时间储存
        }
        if (Menu_DGXX == 9) {
          variableValue_BF5++;
          if (variableValue_BF5 == 100000) { variableValue_BF5 = 1; }
           nvs.putInt("BF5", variableValue_BF5 );  //按下时间储存
        }
        if (Menu_DGXX == 10) {
          variableValue_GH1++;
          if (variableValue_GH1 == 100000) { variableValue_GH1 = 1; }
           nvs.putInt("GH1", variableValue_GH1 );  //按下时间储存
        }
        if (Menu_DGXX == 11) {
          variableValue_GH2++;
          if (variableValue_GH2 == 100000) { variableValue_GH2 = 1; }
           nvs.putInt("GH2", variableValue_GH2 );  //按下时间储存
        }
        if (Menu_DGXX == 12) {
          variableValue_GH3++;
          if (variableValue_GH3 == 100000) { variableValue_GH3 = 1; }
          nvs.putInt("GH3", variableValue_GH3 );  //按下时间储存
        }
        if (Menu_DGXX == 13) {
          variableValue_S++;
          if (variableValue_S == 100000) { variableValue_S = 1; }
          nvs.putInt("S", variableValue_S );  //按下时间储存
        }
        if (Menu_DGXX == 14) {
          variableValue_X++;
          if (variableValue_X == 100000) { variableValue_X = 1; }
          nvs.putInt("X", variableValue_X );  //按下时间储存
        }
        if (Menu_DGXX == 15) {
          variableValue_Z++;
          if (variableValue_Z == 100000) { variableValue_Z = 1; }
            nvs.putInt("Z", variableValue_Z );  //按下时间储存
        }
        if (Menu_DGXX == 16) {
          variableValue_Y++;
          if (variableValue_Y == 100000) { variableValue_Y = 1; }
            nvs.putInt("Y", variableValue_Y );  //按下时间储存
        }

        if (Menu_SZXX == 0) {
          variableValue_AX++;
          if (variableValue_AX == 100000) { variableValue_AX = 1; }
          nvs.putInt("AX", variableValue_AX);  //按下时间储存
        }
        if (Menu_SZXX == 1) {
          variableValue_MS = !variableValue_MS;
            
        nvs.putBool("MS", variableValue_MS);  // 存储 bool 类型数据
        }
        if (Menu_SZXX == 2) {
          variableValue_DGFS = !variableValue_DGFS;
           nvs.putBool("DGFS", variableValue_DGFS);  // 存储 bool 类型数据

        }
                if (Menu_SZXX == 3) {
             variableValue_HC++;
          if (variableValue_HC == 100000) { variableValue_HC = 1; }
          nvs.putInt("variableValue_HC", variableValue_HC);  //按下时间储存
        }
        if (Menu_SZXX == 4) {
          variableValue_WLZT = !variableValue_WLZT;
          nvs.putBool("WLZT", variableValue_WLZT);
          ESP.restart();  // 重启设备
        }
        if (Menu_SZXX == 5) {
          variableValue_CCSZ = !variableValue_CCSZ;
        }
      }  //
    }

    if (keyIndex == 1) {
      if (MenuKEY_B) {
        if (Menu_KZ == 0) {
          Menu--;
          if (Menu > 4) {
            Menu = 4;
          }
        }
        if (Menu_KZ == 1) {
          Menu_DY--;
          if (Menu_DY > 6) {
            Menu_DY = 6;
          }
        }
        if (Menu_KZ == 2) {
          Menu_DG--;
          if (Menu_DG > 17) {
            Menu_DG = 17;
          }
        }
        if (Menu_KZ == 3) {
          Menu_SZ--;
          if (Menu_SZ > 5) {
            Menu_SZ = 5;
          }
        }
        if (Menu_KZ == 4) {
          Menu_GY--;
          if (Menu_GY > 3) {
            Menu_GY = 3;
          }
        }
      } else {

        if (Menu_DYXX == 0) {
          variableValue_AJ1--;
          if (variableValue_AJ1 == 0) { variableValue_AJ1 = 99999; }
            nvs.putInt("AJ1", variableValue_AJ1 );  //按下时间储存
        }
        if (Menu_DYXX == 1) {
          variableValue_AJ2--;
          if (variableValue_AJ2 == 0) { variableValue_AJ2 = 99999; }
            nvs.putInt("AJ2", variableValue_AJ2 );  //按下时间储存
        }
        if (Menu_DYXX == 2) {
          variableValue_AJ3--;
          if (variableValue_AJ3 == 0) { variableValue_AJ3 = 99999; }
            nvs.putInt("AJ3", variableValue_AJ3 );  //按下时间储存
        }
        if (Menu_DYXX == 3) {
          variableValue_AJ4--;
          if (variableValue_AJ4 == 0) { variableValue_AJ4 = 99999; }
            nvs.putInt("AJ4", variableValue_AJ4 );  //按下时间储存
        }
        if (Menu_DYXX == 4) {
          variableValue_AJ5--;
          if (variableValue_AJ5 == 0) { variableValue_AJ5 = 99999; }
          nvs.putInt("AJ5", variableValue_AJ5 );  //按下时间储存
        }
        if (Menu_DYXX == 5) {
          variableValue_AJ6--;
          if (variableValue_AJ6 == 0) { variableValue_AJ6 = 99999; }
          nvs.putInt("AJ6", variableValue_AJ6 );  //按下时间储存
        }

        if (Menu_DGXX == 0) {
          variableValue_JN1--;
          if (variableValue_JN1 == 0) { variableValue_JN1 = 99999; }
          nvs.putInt("JN1", variableValue_JN1 );  //按下时间储存
        }
        if (Menu_DGXX == 1) {
          variableValue_JN2--;
          if (variableValue_JN2 == 0) { variableValue_JN2 = 99999; }
          nvs.putInt("JN2", variableValue_JN2 );  //按下时间储存
        }
        if (Menu_DGXX == 2) {
          variableValue_JN3--;
          if (variableValue_JN3 == 0) { variableValue_JN3 = 99999; }
          nvs.putInt("JN3", variableValue_JN3 );  //按下时间储存
        }
        if (Menu_DGXX == 3) {
          variableValue_JN4--;
          if (variableValue_JN4 == 0) { variableValue_JN4 = 99999; }
          nvs.putInt("JN4", variableValue_JN4 );  //按下时间储存
        }
        if (Menu_DGXX == 4) {
          variableValue_JN5--;
          if (variableValue_JN5 == 0) { variableValue_JN5 = 99999; }
           nvs.putInt("JN5", variableValue_JN5 );  //按下时间储存
        }
        if (Menu_DGXX == 5) {
          variableValue_BF1--;
          if (variableValue_BF1 == 0) { variableValue_BF1 = 99999; }
           nvs.putInt("BF1", variableValue_BF1 );  //按下时间储存
        }
        if (Menu_DGXX == 6) {
          variableValue_BF2--;
          if (variableValue_BF2 == 0) { variableValue_BF2 = 99999; }
           nvs.putInt("BF2", variableValue_BF2 );  //按下时间储存
        }
        if (Menu_DGXX == 7) {
          variableValue_BF3--;
          if (variableValue_BF3 == 0) { variableValue_BF3 = 99999; }
           nvs.putInt("BF3", variableValue_BF3 );  //按下时间储存
        }
        if (Menu_DGXX == 8) {
          variableValue_BF4--;
          if (variableValue_BF4 == 0) { variableValue_BF4 = 99999; }
           nvs.putInt("BF4", variableValue_BF4 );  //按下时间储存
        }
        if (Menu_DGXX == 9) {
          variableValue_BF5--;
          if (variableValue_BF5 == 0) { variableValue_BF5 = 99999; }
          nvs.putInt("BF5", variableValue_BF5 );  //按下时间储存
        }
        if (Menu_DGXX == 10) {
          variableValue_GH1--;
          if (variableValue_GH1 == 0) { variableValue_GH1 = 99999; }
          nvs.putInt("GH1", variableValue_GH1 );  //按下时间储存
        }
        if (Menu_DGXX == 11) {
          variableValue_GH2--;
          if (variableValue_GH2 == 0) { variableValue_GH2 = 99999; }
           nvs.putInt("GH2", variableValue_GH2 );  //按下时间储存
        }
        if (Menu_DGXX == 12) {
          variableValue_GH3--;
          if (variableValue_GH3 == 0) { variableValue_GH3 = 99999; }
           nvs.putInt("GH3", variableValue_GH3 );  //按下时间储存
        }
        if (Menu_DGXX == 13) {
          variableValue_S--;
          if (variableValue_S == 0) { variableValue_S = 99999; }
           nvs.putInt("S", variableValue_S );  //按下时间储存
        }
        if (Menu_DGXX == 14) {
          variableValue_X--;
          if (variableValue_X == 0) { variableValue_X = 99999; }
           nvs.putInt("X", variableValue_X );  //按下时间储存
        }
        if (Menu_DGXX == 15) {
          variableValue_Z--;
          if (variableValue_Z == 0) { variableValue_Z = 99999; }
           nvs.putInt("Z", variableValue_Z );  //按下时间储存
        }
        if (Menu_DGXX == 16) {
          variableValue_Y--;
          if (variableValue_Y == 0) { variableValue_Y = 99999; }
           nvs.putInt("Y", variableValue_Y );  //按下时间储存
        }

        if (Menu_SZXX == 0) {
          variableValue_AX--;
          if (variableValue_AX == 0) { variableValue_AX = 99999; }
          nvs.putInt("AX", variableValue_AX);  //按下时间储存
        }
        if (Menu_SZXX == 1) {
          variableValue_MS = !variableValue_MS;
        }
        if (Menu_SZXX == 2) {
          variableValue_DGFS = !variableValue_DGFS;
        }
             if (Menu_SZXX == 3) {
             variableValue_HC--;
          if (variableValue_HC == 0) { variableValue_HC = 99999; }
          nvs.putInt("variableValue_HC", variableValue_HC);  //按下时间储存
        }
        if (Menu_SZXX == 4) {
          variableValue_WLZT = !variableValue_WLZT;
           nvs.putBool("WLZT", variableValue_WLZT);  //钓鱼的模式存储
        }
        if (Menu_SZXX == 5) {
          variableValue_CCSZ = !variableValue_CCSZ;
        }
      }
    }
    digitalWrite(MUT_DN, HIGH);  // 音量
    digitalWrite(MUT_UP, HIGH);  //音量
    _lastHoldIncrementTime[keyIndex] = millis();
  }
}