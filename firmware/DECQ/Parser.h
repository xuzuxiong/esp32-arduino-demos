#pragma once
#include <Arduino.h>

enum CommandType : uint8_t {
  CMD_UNKNOWN = 0,
  CMD_NONE,
  CMD_MON,
  CMD_WALK,
  CMD_LOOT          // ← 新增
};

struct Parsed {
  CommandType type{CMD_UNKNOWN};
  int x{-1};
  int y{-1};
};

// "MON,884,154" / "WALK,1588,348" / "LOOT,1028,147" / "NONE"
bool parse_cmd_xy(char* line, Parsed& out);
