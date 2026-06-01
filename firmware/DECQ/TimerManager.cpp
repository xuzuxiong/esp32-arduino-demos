#include "TimerManager.h"
#include "fish_sound_detection.h"
#include "menu.h"
#include "scan_controller.h"
#include <Arduino.h>

char combinedText_djs[64] = {0};

unsigned long currentMillis = 0;
unsigned long currentMillis_paogan = 0;
unsigned long currentMillis_guaer = 0;
unsigned long currentMillis_guaer5 = 0;
unsigned long currentMillis_guaer10 = 0;
unsigned long currentMillis_guaer15 = 0;
unsigned long currentMillis_PG = 0;
unsigned long currentMillis_GR = 0;
unsigned long currentMillis_GR_A = 0;
unsigned long currentMillis_laji = 0;
unsigned long currentMillis_bangke = 0;
bool PG = false;
bool GR = false;
bool GR_A = false;
bool GR_B = false;
bool GR_C = false;
bool GR_D = false;
bool GR_E = false;
bool YPJC = false;
bool JR_ZD = false;
bool XL_90 = false;
bool XL_60 = false;
bool XL_50 = false;
bool XL_20 = false;
int QH_1 = 0;
int QH_2 = 0;
bool QH_3 = false;
bool LL_90 = false;
bool LL_60 = false;
bool LL_50 = false;
bool LL_20 = false;
bool JH_JH = false;
bool XS = false;
bool LS = false;
bool XL = false;
bool LL = false;
unsigned long previousMillis_XL = 0;
unsigned long previousMillis_LL = 0;
const long interval_XL = 20000;
const long interval_LL = 20000;
unsigned long variableValue_AX_B = 0;
unsigned long variableValue_AX_A = 0;
unsigned long variableValue_AJ1_A = 0;
unsigned long variableValue_AJ1_B = 0;
unsigned long variableValue_AJ2_A = 0;
unsigned long variableValue_AJ2_B = 0;
unsigned long variableValue_AJ3_A = 0;
unsigned long variableValue_AJ3_B = 0;
unsigned long variableValue_AJ4_A = 0;
unsigned long variableValue_AJ4_B = 0;
unsigned long variableValue_AJ5_A = 0;
unsigned long variableValue_AJ5_B = 0;
unsigned long variableValue_AJ6_A = 0;
unsigned long variableValue_AJ6_B = 0;
unsigned long variableValue_JN1_A = 0;
unsigned long variableValue_JN1_B = 0;
unsigned long variableValue_JN2_A = 0;
unsigned long variableValue_JN2_B = 0;
unsigned long variableValue_JN3_A = 0;
unsigned long variableValue_JN3_B = 0;
unsigned long variableValue_JN4_A = 0;
unsigned long variableValue_JN4_B = 0;
unsigned long variableValue_JN5_A = 0;
unsigned long variableValue_JN5_B = 0;
unsigned long variableValue_BF1_A = 0;
unsigned long variableValue_BF1_B = 0;
unsigned long variableValue_BF2_A = 0;
unsigned long variableValue_BF2_B = 0;
unsigned long variableValue_BF3_A = 0;
unsigned long variableValue_BF3_B = 0;
unsigned long variableValue_BF4_A = 0;
unsigned long variableValue_BF4_B = 0;
unsigned long variableValue_BF5_A = 0;
unsigned long variableValue_BF5_B = 0;
unsigned long variableValue_GH1_A = 0;
unsigned long variableValue_GH1_B = 0;
unsigned long variableValue_GH2_A = 0;
unsigned long variableValue_GH2_B = 0;
unsigned long variableValue_GH3_A = 0;
unsigned long variableValue_GH3_B = 0;
unsigned long variableValue_GH3_D = 0;
unsigned long variableValue_HC_D = 0;
unsigned long variableValue_S_A = 0;
unsigned long variableValue_S_B = 0;
unsigned long variableValue_X_A = 0;
unsigned long variableValue_X_B = 0;
unsigned long variableValue_Z_A = 0;
unsigned long variableValue_Z_B = 0;
unsigned long variableValue_Y_A = 0;
unsigned long variableValue_Y_B = 0;
unsigned long variableValue_HC_A = 50;
unsigned long variableValue_HC_B = 0;
unsigned long variableValue_HC_c = 0;
bool DYHC = false;
bool paogan = false;
bool paogan_A = false;
bool AJ_A = false;
bool DY_A = 0;
bool DY_B = 0;
bool DY_C = 0;
bool DY_D = 0;
bool DY_E = 0;
bool DY_F = 0;
bool YM = 0;
uint8_t DY1 = 0;
uint8_t DY2 = 0;
uint8_t DY3 = 0;
uint8_t DY4 = 0;
uint8_t DY5 = 0;
uint8_t DY6 = 0;
uint8_t DYHC1 = 0;
uint8_t GR1 = 0;
uint8_t PG1 = 0;
uint8_t JN1 = 0;
uint8_t JN2 = 0;
uint8_t JN3 = 0;
uint8_t JN4 = 0;
uint8_t JN5 = 0;
uint8_t BUF1 = 0;
uint8_t BUF2 = 0;
uint8_t BUF3 = 0;
uint8_t BUF4 = 0;
uint8_t BUF5 = 0;
uint8_t GH1 = 0;
uint8_t GH2 = 0;
uint8_t GH3 = 0;
uint8_t S = 0;
uint8_t X = 0;
uint8_t Z = 0;
uint8_t Y = 0;
uint8_t HC = 0;
bool DG_A = 0;
bool DG_B = 0;
bool DG_C = 0;
bool DG_D = 0;
bool DG_E = 0;
bool DG_F = 0;
bool DG_G = 0;
bool DG_H = 0;
bool DG_I = 0;
bool DG_J = 0;
bool DG_K = 0;
bool DG_Le = 0;
bool DG_M = 0;
bool YM_A = 0;
bool AJSF_A = 0;
bool AJSF_B = 0;
unsigned long JZSJ = 0;
unsigned long DY_Millis[6] = {0};
unsigned long DY_Millis_A[6] = {0};
unsigned long DD_Millis[5] = {0};
unsigned long DD_Millis_A[5] = {0};
unsigned long BF_Millis[5] = {0};
unsigned long BF_Millis_A[5] = {0};
unsigned long GH_Millis[3] = {0};
unsigned long GH_Millis_A[3] = {0};
unsigned long FX_Millis[4] = {0};
unsigned long FX_Millis_A[4] = {0};
unsigned long key49_TriggerTime = 0;
unsigned long key50_TriggerTime = 0;
unsigned long key46_TriggerTime = 0;
const unsigned long KEY49_INTERVAL = 180000;
const unsigned long KEY50_INTERVAL = 240000;
const unsigned long KEY46_INTERVAL = 300000;
bool key49_50_46_3min_flag = false;
bool key49_50_46_4min_flag = false;
bool key49_50_46_5min_flag = false;
unsigned long key345_start_ms = 0;
bool key345_timer_running = false;
bool DY_AX = false;
bool DD_AX = false;
bool BF_AX = false;
bool GH_AX = false;
bool FX_AX = false;
bool enabled = false;
bool enabled_A = false;
bool DD_A1 = false;
bool DD_A2 = false;
bool DD_A3 = false;
bool DD_A4 = false;
bool DD_A5 = false;
bool BF_A1 = false;
bool BF_A2 = false;
bool BF_A3 = false;
bool BF_A4 = false;
bool BF_A5 = false;
bool GH_A1 = false;
bool GH_A2 = false;
bool GH_A3 = false;
bool S_A1 = false;
bool X_A1 = false;
bool Y_A1 = false;
bool Z_A1 = false;
bool GR_F = false;
bool QH = false;
bool DYS_A1 = false;
bool DDS_A1 = false;
bool YDYD = false;
unsigned long elapsedTime = 0;
unsigned long elapsedTime_A = 0;
const int TOTAL_ROLES = 6;
int currentRole = 0;
bool roleSwitchEnabled = false;
int roleSequence[6] = {0};
int currentSequenceIndex = 0;
int lastEndRole = 0;
unsigned long lastPublishTime_A = 0;
const unsigned long PUBLISH_INTERVAL = 60000;

static const uint8_t MAX_KEY_INDEX = 102;
static const unsigned long DEFAULT_RETURN_PRESS_MS = 50;
static const uint8_t FISH_BAIT_KEY_INDEX = 42;          // '='
static const uint8_t FISH_CAST_KEY_INDEX = 41;          // '-'
static const uint8_t FISH_REEL_FALLBACK_KEY_INDEX = 35; // '0'
static const unsigned long FISH_BAIT_DELAY_MS = 4200UL;
static const unsigned long FISH_SOUND_ARM_DELAY_MS = 1031UL;
static const unsigned long FISH_MIN_SPLASH_AGE_MS = 4800UL;
static const unsigned long FISH_CAST_TIMEOUT_MS = 30300UL;
static const unsigned long FISH_BAIT_DURATION_MS = 285000UL;
static const unsigned long FISH_POST_REEL_DELAY_MS = 905UL;
static const unsigned long FISH_TIMEOUT_RECOVER_DELAY_MS = 250UL;
static const unsigned long FISH_REACTION_DELAY_MS = 120UL;
static const unsigned long FISH_REEL_FALLBACK_DELAY_MS = 80UL;
static byte sLastMenu = 255;
static bool sLastPaused = true;
static bool sNeedFishPrep = false;

enum FishingState : uint8_t {
  FISHING_IDLE = 0,
  FISHING_APPLY_BAIT,
  FISHING_WAIT_AFTER_BAIT,
  FISHING_CAST,
  FISHING_WAIT_BITE,
  FISHING_REEL_IN,
  FISHING_RECOVER
};

static FishingState sFishingState = FISHING_IDLE;
static unsigned long sFishingStateStartedAt = 0;
static unsigned long sFishingCastAt = 0;
static unsigned long sFishingBaitExpiresAt = 0;
static bool sFishingNeedsBait = true;
static bool sFishingRecoveredFromTimeout = false;

static unsigned long clampHoldMs(unsigned long holdMs) {
  if (holdMs < 20) return 20;
  if (holdMs > 5000) return 5000;
  return holdMs;
}

static unsigned long periodicSecondsMs(unsigned int base) {
  if (base == 0 || base >= 99999U) return 0;
  return (unsigned long)base * 1000UL;
}

static void setFishingState(FishingState nextState) {
  sFishingState = nextState;
  sFishingStateStartedAt = millis();
}

static void resetFishingState() {
  sFishingState = FISHING_IDLE;
  sFishingStateStartedAt = 0;
  sFishingCastAt = 0;
  sFishingBaitExpiresAt = 0;
  sFishingNeedsBait = true;
  sFishingRecoveredFromTimeout = false;
  setFishSoundDetectionEnabled(false);
  clearFishSoundEvent();
}

static void updateFishingBaitDeadline(unsigned long now) {
  if (sFishingBaitExpiresAt != 0 && now >= sFishingBaitExpiresAt) {
    sFishingNeedsBait = true;
  }
}

static bool maybeRunSlot(uint8_t keyNumber,
                         unsigned int intervalValue,
                         bool valueInSeconds,
                         unsigned long &lastMillis,
                         unsigned long holdMs);

static bool runFishingTimedActions(unsigned long holdMs) {
  if (maybeRunSlot(DY1, variableValue_AJ1, true, DY_Millis[0], holdMs)) return true;
  if (maybeRunSlot(DY2, variableValue_AJ2, true, DY_Millis[1], holdMs)) return true;
  if (maybeRunSlot(DY3, variableValue_AJ3, true, DY_Millis[2], holdMs)) return true;
  if (maybeRunSlot(DY4, variableValue_AJ4, true, DY_Millis[3], holdMs)) return true;
  if (maybeRunSlot(DY5, variableValue_AJ5, true, DY_Millis[4], holdMs)) return true;
  return maybeRunSlot(DY6, variableValue_AJ6, true, DY_Millis[5], holdMs);
}

static void resetScheduleArrays() {
  memset(DY_Millis, 0, sizeof(DY_Millis));
  memset(DD_Millis, 0, sizeof(DD_Millis));
  memset(BF_Millis, 0, sizeof(BF_Millis));
  memset(GH_Millis, 0, sizeof(GH_Millis));
  memset(FX_Millis, 0, sizeof(FX_Millis));
  variableValue_HC_c = 0;
}

static void tapKeyNumber(uint8_t keyNumber, unsigned long holdMs) {
  if (keyNumber > MAX_KEY_INDEX) return;
  sendKeyByNumber((int)keyNumber);
  delay(clampHoldMs(holdMs));
  releaseAllKeys();
}

static bool maybeRunSlot(uint8_t keyNumber,
                         unsigned int intervalValue,
                         bool valueInSeconds,
                         unsigned long &lastMillis,
                         unsigned long holdMs) {
  const unsigned long intervalMs = valueInSeconds
      ? periodicSecondsMs(intervalValue)
      : (intervalValue == 0 || intervalValue >= 99999U)
            ? 0
            : (unsigned long)intervalValue;
  if (intervalMs == 0) return false;

  const unsigned long now = millis();
  if (lastMillis == 0) {
    lastMillis = now - intervalMs;
  }
  if (now - lastMillis < intervalMs) return false;

  lastMillis = now;
  tapKeyNumber(keyNumber, holdMs);
  return true;
}

static void handleRunTimer() {
  if (!enabled || variableValue_HC_B == 0) return;
  if (variableValue_HC_D == 0) {
    variableValue_HC_D = millis();
    return;
  }

  const unsigned long elapsed = millis() - variableValue_HC_D;
  if (elapsed >= variableValue_HC_B) {
    enabled = false;
    isPaused = true;
    releaseAllKeys();
  }
}

void qiehuan() {
  if (sLastMenu != Menu || sLastPaused != isPaused) {
    resetScheduleArrays();
    resetFishingState();
    sNeedFishPrep = (!isPaused && Menu == 0);
    sLastMenu = Menu;
    sLastPaused = isPaused;
  }

  if (isPaused) {
    resetFishingState();
    releaseAllKeys();
  }
}

void SUJI() {
  qiehuan();
  handleRunTimer();

  if (sNeedFishPrep && !isPaused && Menu == 0) {
    startBaitAndPrepareCast();
    sNeedFishPrep = false;
  }
}

void DIAOYU() {
  if (isPaused || Menu != 0) return;

  const unsigned long now = millis();
  const unsigned long holdMs = clampHoldMs(variableValue_AX);
  updateFishingBaitDeadline(now);

  switch (sFishingState) {
    case FISHING_IDLE:
      if (!sNeedFishPrep) {
        startBaitAndPrepareCast();
      }
      return;

    case FISHING_APPLY_BAIT: {
      setFishSoundDetectionEnabled(false);
      clearFishSoundEvent();
      tapKeyNumber(FISH_BAIT_KEY_INDEX, holdMs);
      sFishingNeedsBait = false;
      sFishingRecoveredFromTimeout = false;
      sFishingBaitExpiresAt = millis() + FISH_BAIT_DURATION_MS;
      setFishingState(FISHING_WAIT_AFTER_BAIT);
      return;
    }

    case FISHING_WAIT_AFTER_BAIT:
      if (runFishingTimedActions(holdMs)) return;
      if (now - sFishingStateStartedAt >= FISH_BAIT_DELAY_MS) {
        setFishingState(FISHING_CAST);
      }
      return;

    case FISHING_CAST:
      tapKeyNumber(FISH_CAST_KEY_INDEX, holdMs);
      sFishingCastAt = millis();
      sFishingRecoveredFromTimeout = false;
      setFishSoundDetectionEnabled(false);
      clearFishSoundEvent();
      setFishingState(FISHING_WAIT_BITE);
      return;

    case FISHING_WAIT_BITE:
      if (runFishingTimedActions(holdMs)) return;
      if (!isFishSoundDetectionEnabled() &&
          now - sFishingCastAt >= FISH_SOUND_ARM_DELAY_MS) {
        clearFishSoundEvent();
        setFishSoundDetectionEnabled(true);
      }

      if (consumeFishSoundEvent()) {
        if (now - sFishingCastAt < FISH_MIN_SPLASH_AGE_MS) {
          return;
        }

        setFishSoundDetectionEnabled(false);
        setFishingState(FISHING_REEL_IN);
        return;
      }

      if (now - sFishingCastAt >= FISH_CAST_TIMEOUT_MS) {
        setFishSoundDetectionEnabled(false);
        clearFishSoundEvent();
        sFishingRecoveredFromTimeout = true;
        setFishingState(FISHING_RECOVER);
      }
      return;

    case FISHING_REEL_IN:
      if (now - sFishingStateStartedAt < FISH_REACTION_DELAY_MS) {
        return;
      }

      performScan();
      delay(FISH_REEL_FALLBACK_DELAY_MS);
      tapKeyNumber(FISH_REEL_FALLBACK_KEY_INDEX, holdMs);
      setFishSoundDetectionEnabled(false);
      clearFishSoundEvent();
      sFishingRecoveredFromTimeout = false;
      setFishingState(FISHING_RECOVER);
      return;

    case FISHING_RECOVER: {
      const unsigned long recoverDelayMs =
          sFishingRecoveredFromTimeout ? FISH_TIMEOUT_RECOVER_DELAY_MS
                                       : FISH_POST_REEL_DELAY_MS;
      if (runFishingTimedActions(holdMs)) return;
      if (now - sFishingStateStartedAt < recoverDelayMs) {
        return;
      }

      if (sFishingNeedsBait) {
        setFishingState(FISHING_APPLY_BAIT);
      } else {
        setFishingState(FISHING_CAST);
      }
      return;
    }
  }
}

void DAGUAI() {
  if (isPaused || Menu != 1) return;

  const unsigned long holdMs = clampHoldMs(variableValue_AX);
  const unsigned long returnHoldMs = clampHoldMs(
      variableValue_HC_A ? variableValue_HC_A : DEFAULT_RETURN_PRESS_MS);

  if (maybeRunSlot(JN1, variableValue_JN1, true, DD_Millis[0], holdMs)) return;
  if (maybeRunSlot(JN2, variableValue_JN2, true, DD_Millis[1], holdMs)) return;
  if (maybeRunSlot(JN3, variableValue_JN3, true, DD_Millis[2], holdMs)) return;
  if (maybeRunSlot(JN4, variableValue_JN4, true, DD_Millis[3], holdMs)) return;
  if (maybeRunSlot(JN5, variableValue_JN5, true, DD_Millis[4], holdMs)) return;

  if (maybeRunSlot(BUF1, variableValue_BF1, true, BF_Millis[0], holdMs)) return;
  if (maybeRunSlot(BUF2, variableValue_BF2, true, BF_Millis[1], holdMs)) return;
  if (maybeRunSlot(BUF3, variableValue_BF3, true, BF_Millis[2], holdMs)) return;
  if (maybeRunSlot(BUF4, variableValue_BF4, true, BF_Millis[3], holdMs)) return;
  if (maybeRunSlot(BUF5, variableValue_BF5, true, BF_Millis[4], holdMs)) return;

  if (maybeRunSlot(GH1, variableValue_GH1, true, GH_Millis[0], holdMs)) return;
  if (maybeRunSlot(GH2, variableValue_GH2, true, GH_Millis[1], holdMs)) return;
  if (maybeRunSlot(GH3, variableValue_GH3, true, GH_Millis[2], holdMs)) return;

  if (maybeRunSlot(S, variableValue_S, true, FX_Millis[0], holdMs)) return;
  if (maybeRunSlot(X, variableValue_X, true, FX_Millis[1], holdMs)) return;
  if (maybeRunSlot(Z, variableValue_Z, true, FX_Millis[2], holdMs)) return;
  if (maybeRunSlot(Y, variableValue_Y, true, FX_Millis[3], holdMs)) return;

  maybeRunSlot(HC, variableValue_HC, true, variableValue_HC_c, returnHoldMs);
}

void executeRoleSwitch() {
  if (!roleSwitchEnabled) return;
  currentRole = (currentRole + 1) % TOTAL_ROLES;
}

void shuffleRoleSequence() {
  for (int i = 0; i < TOTAL_ROLES; ++i) {
    roleSequence[i] = i;
  }
}

void startBaitAndPrepareCast() {
  if (Menu != 0 || isPaused) return;

  resetFishingState();
  setFishingState(FISHING_APPLY_BAIT);
}

const char* getFishingStateLabel() {
  switch (sFishingState) {
    case FISHING_IDLE:
      return "IDLE";
    case FISHING_APPLY_BAIT:
      return "BAIT";
    case FISHING_WAIT_AFTER_BAIT:
      return "WAIT_CAST";
    case FISHING_CAST:
      return "CAST";
    case FISHING_WAIT_BITE:
      return "WAIT_BITE";
    case FISHING_REEL_IN:
      return "REEL";
    case FISHING_RECOVER:
      return "RECOVER";
  }
  return "UNKNOWN";
}

unsigned long getFishingBaitRemainingMs() {
  if (sFishingBaitExpiresAt == 0) {
    return 0;
  }

  const unsigned long now = millis();
  if (now >= sFishingBaitExpiresAt) {
    return 0;
  }

  return sFishingBaitExpiresAt - now;
}
