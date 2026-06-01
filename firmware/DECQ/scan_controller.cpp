#include "scan_controller.h"
#include <Arduino.h>
#include "TimerManager.h"
HardwareSerial mySerial(2);


const uint8_t keyCodes[] = {
    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, 
    KEY_K, KEY_L, KEY_M, KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, 
    KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
    KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0,
    KEY_ENTER, KEY_ESC, KEY_BACKSPACE, KEY_TAB, KEY_SPACE, KEY_MINUS, 
    KEY_EQUAL, KEY_LEFT_BRACKET, KEY_RIGHT_BRACKET, KEY_BACKSLASH, 
    KEY_SEMICOLON, KEY_SINGLE_QUOTE, KEY_TILDE, KEY_COMMA, KEY_DOT, 
    KEY_SLASH, KEY_CAPS_LOCK,
    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, 
    KEY_F10, KEY_F11, KEY_F12,
    KEY_PRINT_SCR, KEY_SCROLL_LOCK, KEY_PAUSE_BREAK, KEY_INSERT, KEY_HOME, 
    KEY_PAGE_UP, KEY_DELETE, KEY_END, KEY_PAGE_DOWN,
    KEY_RIGHT_ARROW, KEY_LEFT_ARROW, KEY_DOWN_ARROW, KEY_UP_ARROW,
    KEY_NUM_LOCK, KEY_NUM_DIVIDE, KEY_NUM_MULTIPLY, KEY_NUM_SUBTRACT, 
    KEY_NUM_ADD, KEY_NUM_ENTER, KEY_NUM_7, KEY_NUM_8, KEY_NUM_9, 
    KEY_NUM_4, KEY_NUM_5, KEY_NUM_6, KEY_NUM_1, KEY_NUM_2, KEY_NUM_3, 
    KEY_NUM_0, KEY_NUM_DECIMAL,
    KEY_LEFT_CTRL, KEY_RIGHT_CTRL, KEY_LEFT_SHIFT, KEY_RIGHT_SHIFT, 
    KEY_LEFT_ALT, KEY_RIGHT_ALT, KEY_LEFT_WIN, KEY_RIGHT_WIN
};

uint8_t mapNumberToKeyCode(int number) {
    if (number >= 0 && number < sizeof(keyCodes) / sizeof(keyCodes[0])) {
        return keyCodes[number];
    }
    return 0;  // 处理无效数字的情况
}

// 直接使用数字发送按键的函数
void sendKeyByNumber(int number) {
    uint8_t keyCode = mapNumberToKeyCode(number);
    if (keyCode != 0) {
        sendKey(keyCode);
    } else {
        printf("Invalid number, cannot send key.\n");
    }
}
// 硬件串口初始化
void initSerial() {
 //   Serial.begin(115200);
    mySerial.begin(BAUD_RATE, SERIAL_8N1, 16, 17);
}

// 右键按下（仅发送按下事件）
void clickRightPress() {
  
    uint8_t pressFrame[] = {0x57, 0xAB, 0x00, 0x05, 0x05, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00};
    uint8_t sum = 0;
    for (int i = 0; i < 10; i++) sum += pressFrame[i];
    pressFrame[10] = sum;
    sendFrame(pressFrame, sizeof(pressFrame));
    delayMicroseconds(500);
}

// 右键释放（仅发送释放事件）
void clickRightRelease() {

    uint8_t releaseFrame[] = {0x57, 0xAB, 0x00, 0x05, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t sum = 0;
    for (int i = 0; i < 10; i++) sum += releaseFrame[i];
    releaseFrame[10] = sum;
    sendFrame(releaseFrame, sizeof(releaseFrame));
    delay(2);
}
// 左键按下（仅发送按下事件）
void clickLeftPress() {
    // 定义左键按下的帧数据
    uint8_t pressFrame[] = {0x57, 0xAB, 0x00, 0x05, 0x05, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00};
    uint8_t sum = 0;
    // 计算前 10 个字节的和
    for (int i = 0; i < 10; i++) sum += pressFrame[i];
    // 将和赋值给第 11 个字节作为校验和
    pressFrame[10] = sum;
    // 发送帧数据
    sendFrame(pressFrame, sizeof(pressFrame));
    // 延迟 500 微秒
    delayMicroseconds(2000);
}

// 左键释放（仅发送释放事件）
void clickLeftRelease() {
    // 定义左键释放的帧数据
    uint8_t releaseFrame[] = {0x57, 0xAB, 0x00, 0x05, 0x05, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t sum = 0;
    // 计算前 10 个字节的和
    for (int i = 0; i < 10; i++) sum += releaseFrame[i];
    // 将和赋值给第 11 个字节作为校验和
    releaseFrame[10] = sum;
    // 发送帧数据
    sendFrame(releaseFrame, sizeof(releaseFrame));
    // 延迟 2 毫秒
    delay(2);
}   

// 鼠标滚轮向上滚动（绝对模式，协议2.2.4）
void scrollWheelUp() {

    uint8_t scrollFrame[] = {
        0x57, 0xAB,       // 帧头
        0x00,             // 地址码
        0x04,             // 命令码 CMD_SEND_MS_ABS_DATA
        0x07,             // 后续数据长度=7字节
        0x02,             // 固定标识（协议要求）
        0x00,             // 按键状态（无按键）
        0x00, 0x00,       // X坐标（低字节在前，设为0不移动）
        0x00, 0x00,       // Y坐标（低字节在前，设为0不移动）999999999999999999
        0x01,             // 滚轮值：0x01表示向上1齿
        0x00              // 校验和占位
    };
    
    // 计算校验和（前10字节累加）
    uint8_t sum = 0;
    for (int i = 0; i < 12; i++) sum += scrollFrame[i];
    scrollFrame[12] = sum;  
    sendFrame(scrollFrame, sizeof(scrollFrame));
}

// 鼠标滚轮向下滚动（绝对模式，协议2.2.4）
void scrollWheelDown() {
 
    uint8_t scrollFrame[] = {
        0x57, 0xAB,       // 帧头
        0x00,             // 地址码
        0x04,             // 命令码 CMD_SEND_MS_ABS_DATA
        0x07,             // 后续数据长度=7字节
        0x02,             // 固定标识（协议要求）
        0x00,             // 按键状态（无按键）
        0x00, 0x00,       // X坐标（设为0不移动）
        0x00, 0x00,       // Y坐标（设为0不移动）
        0xFA,             // 滚轮值：0xFF表示向下1齿
        0x00              // 校验和占位
    };
    
    // 计算校验和（前10字节累加）
    uint8_t sum = 0;
    for (int i = 0; i < 12; i++) sum += scrollFrame[i];
    scrollFrame[12] = sum;
    
    sendFrame(scrollFrame, sizeof(scrollFrame));
    delayMicroseconds(500);
}

// 发送绝对坐标移动指令
void sendAbsMouse(uint16_t x, uint16_t y) {

    uint16_t chipX = (x * 4096) / 1920;
    uint16_t chipY = (y * 4096) / 1080;

    uint8_t frame[] = {
        0x57, 0xAB, 0x00, 0x04, 0x07, 0x02, 0x00,
        lowByte(chipX), highByte(chipX),
        lowByte(chipY), highByte(chipY),
        0x00, 0x00
    };

    uint8_t sum = 0;
    for (int i = 0; i < 12; i++) sum += frame[i];
    frame[12] = sum;

    sendFrame(frame, sizeof(frame));
}

// 发送相对坐标移动指令
void sendRelMouse(int8_t dx, int8_t dy) {

    uint8_t frame[] = {
        0x57, 0xAB, 0x00, 0x05, 0x05, 0x01, 0x00,
        static_cast<uint8_t>(dx),
        static_cast<uint8_t>(dy),
        0x00, 0x00
    };

    uint8_t sum = 0;
    for (int i = 0; i < 10; i++) sum += frame[i];
    frame[10] = sum;

    sendFrame(frame, sizeof(frame));
}


// 基础通信函数
void sendFrame(uint8_t* frame, size_t length) {
    mySerial.write(frame, length);
}

// 执行扫描操作
void performScan() {
  // 初始化绝对坐标
  sendAbsMouse(SCAN_START_X, SCAN_START_Y);
  delay(1);

  for(int y = 0; y < SCAN_HEIGHT; y += Y_STEP) {
    // 移动到当前行起始点（绝对坐标）
    uint16_t currentY = SCAN_START_Y + y;
    sendAbsMouse(SCAN_START_X, currentY);
        delay(CLICK_DELAY); 
  scrollWheelUp();
      delay(CLICK_DELAY);
    // 处理X轴移动（绝对坐标）
    if(y % 2 == 0) {
      for(int x = 0; x < SCAN_WIDTH; x += X_STEP) {
        uint16_t currentX = SCAN_START_X + x;
        sendAbsMouse(currentX, currentY);
     delay(CLICK_DELAY);
       scrollWheelUp();
        delay(CLICK_DELAY);
    
      }
    } else {
      for(int x = 0; x < SCAN_WIDTH; x += X_STEP) {
        uint16_t currentX = SCAN_START_X - x;
        sendAbsMouse(currentX, currentY);

        delay(CLICK_DELAY);
    
      scrollWheelUp();
        delay(CLICK_DELAY);
  
      }
    }
  }
  GR_D = false; //扫描结束后返回一个值
}


// 通用键盘按键发送函数
void sendKey(uint8_t keyCode) {
    // 构建按键按下帧（14字节）
    uint8_t pressFrame[14] = {
        0x57, 0xAB, 0x00, 0x02, 0x08, 0x00, 0x00, // 固定前7字节
        keyCode,                                   // 键编码（第8字节）
        0x00, 0x00, 0x00, 0x00, 0x00                // 中间5字节固定0
    };
    // 计算校验和（前13字节求和）
    uint8_t sum = 0;
    for (int i = 0; i < 13; i++) sum += pressFrame[i];
    pressFrame[13] = sum;
    sendFrame(pressFrame, sizeof(pressFrame));
 
}

// 通用键盘按键释放函数
void releaseKey(uint8_t keyCode) {
    // 构建按键释放帧（14字节）
    uint8_t releaseFrame[14] = {
        0x57, 0xAB, 0x00, 0x02, 0x08, 0x00, 0x00, // 固定前7字节
        0x00,                                      // 键编码为0（释放按键）
        0x00, 0x00, 0x00, 0x00, 0x00                // 中间5字节固定0
    };
    // 计算校验和（前13字节求和）
    uint8_t sum = 0;
    for (int i = 0; i < 13; i++) sum += releaseFrame[i];
    releaseFrame[13] = sum;
    sendFrame(releaseFrame, sizeof(releaseFrame));
}

// 释放所有按键（包括修饰键）
void releaseAllKeys() {
    uint8_t frame[14] = {0x57, 0xAB, 0x00, 0x02, 0x08, 0x00, 0x00,
                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    // 计算校验和（前13字节累加）
    uint8_t sum = 0;
    for (int i = 0; i < 13; i++) sum += frame[i];
    frame[13] = sum;
    sendFrame(frame, sizeof(frame)); // 发送帧
    delay(50); // 释放延迟（根据设备调整）
}

// 鼠标移动到指定坐标 X1278 Y1008 的函数
void moveMouseTo1278_1008() {
    // 定义目标坐标
    const uint16_t TARGET_X = 1278;
    const uint16_t TARGET_Y = 1008;
    
    // 调用绝对坐标移动函数
    sendAbsMouse(TARGET_X, TARGET_Y);
    
    // 等待移动完成（可根据实际情况调整延迟时间）
    delay(CLICK_DELAY);
}
void moveMouseTo956_573() {
    // 定义目标坐标
    const uint16_t TARGET_X = 956;
    const uint16_t TARGET_Y = 573;
    
    // 调用绝对坐标移动函数
    sendAbsMouse(TARGET_X, TARGET_Y);
    
    // 等待移动完成（可根据实际情况调整延迟时间）
    delay(CLICK_DELAY);
}

void moveMouseTo1670_93() {
    // 定义目标坐标
    const uint16_t TARGET_X = 1670;
    const uint16_t TARGET_Y = 93;
    
    // 调用绝对坐标移动函数
    sendAbsMouse(TARGET_X, TARGET_Y);
    
    // 等待移动完成（可根据实际情况调整延迟时间）
    delay(CLICK_DELAY);
}

void moveMouseTo645_302() {
    // 定义目标坐标
    const uint16_t TARGET_X = 645;
    const uint16_t TARGET_Y = 302;
    
    // 调用绝对坐标移动函数
    sendAbsMouse(TARGET_X, TARGET_Y);
    
    // 等待移动完成（可根据实际情况调整延迟时间）
    delay(CLICK_DELAY);
}

void moveMouseTo645_329() {
    // 定义目标坐标
    const uint16_t TARGET_X = 645;
    const uint16_t TARGET_Y = 329;
    
    // 调用绝对坐标移动函数
    sendAbsMouse(TARGET_X, TARGET_Y);
    
    // 等待移动完成（可根据实际情况调整延迟时间）
    delay(CLICK_DELAY);
}

void moveMouseTo1115_820() {
    // 定义目标坐标
    const uint16_t TARGET_X = 1115;
    const uint16_t TARGET_Y = 820;
    
    // 调用绝对坐标移动函数
    sendAbsMouse(TARGET_X, TARGET_Y);
    
    // 等待移动完成（可根据实际情况调整延迟时间）
    delay(CLICK_DELAY);
}

void moveMouseTo944_944() {
    // 定义目标坐标
    const uint16_t TARGET_X = 944;
    const uint16_t TARGET_Y = 944;
    
    // 调用绝对坐标移动函数
    sendAbsMouse(TARGET_X, TARGET_Y);
    
    // 等待移动完成（可根据实际情况调整延迟时间）
    delay(CLICK_DELAY);
}

void moveMouseTo674_334() {
    // 定义目标坐标
    const uint16_t TARGET_X = 674;
    const uint16_t TARGET_Y = 334;
    
    // 调用绝对坐标移动函数
    sendAbsMouse(TARGET_X, TARGET_Y);
    
    // 等待移动完成（可根据实际情况调整延迟时间）
    delay(CLICK_DELAY);
}