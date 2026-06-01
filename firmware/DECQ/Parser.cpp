#include "Parser.h"
#include <ctype.h>
#include <string.h>
#include <stdio.h>

static void normalize_line(char* s) {
  for (char* p = s; *p; ++p) {
    if (*p == '\r' || *p == '\n') { *p = '\0'; break; }
    if (*p == ',') *p = ' ';
  }
}
static void to_upper_inplace(char* s) {
  for (char* p = s; *p; ++p) *p = (char)toupper((unsigned char)*p);
}

bool parse_cmd_xy(char* line, Parsed& out) {
  if (!line) return false;
  normalize_line(line);

  char cmd[16] = {0};
  int x = 0, y = 0;
  int n = sscanf(line, " %15s %d %d", cmd, &x, &y);
  if (n <= 0) return false;

  to_upper_inplace(cmd);

  if (!strcmp(cmd, "NONE")) {
    out.type = CMD_NONE; out.x = -1; out.y = -1; return true;
  }
  if (!strcmp(cmd, "MON")) {
    if (n == 3) { out.type = CMD_MON;  out.x = x; out.y = y; return true; }
    return false;
  }
  if (!strcmp(cmd, "WALK")) {
    if (n == 3) { out.type = CMD_WALK; out.x = x; out.y = y; return true; }
    return false;
  }
  if (!strcmp(cmd, "LOOT")) {                 // ← 新增
    if (n == 3) { out.type = CMD_LOOT; out.x = x; out.y = y; return true; }
    return false;
  }

  return false;
}
