#include "fish_sound_detection.h"

#include <cstring>

#include "keypad.h"

float vReal[SAMPLES];
float vImag[SAMPLES];
ArduinoFFT<float> FFT(vReal, vImag, SAMPLES, SAMPLE_RATE);

float energyHistory[HISTORY_SIZE] = {0};
int historyIndex = 0;
static bool sFishSoundDetectionEnabled = false;
static bool sFishSoundEventPending = false;
static unsigned long sLastFishSoundTriggerMs = 0;
static const unsigned long SOUND_TRIGGER_DEBOUNCE_MS = 600UL;

static void processI2SData(int32_t* rawData, float& dcOffset) {
  int64_t sum = 0;
  for (int i = 0; i < SAMPLES; ++i) {
    rawData[i] >>= 8;
    sum += rawData[i];
  }

  dcOffset = static_cast<float>(sum) / static_cast<float>(SAMPLES);
  for (int i = 0; i < SAMPLES; ++i) {
    vReal[i] = (rawData[i] - dcOffset) / 8388608.0f;
    vImag[i] = 0.0f;
  }
}

void setupI2S() {
  i2s_config_t i2sConfig = {};
  i2sConfig.mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_RX);
  i2sConfig.sample_rate = SAMPLE_RATE;
  i2sConfig.bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT;
  i2sConfig.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
  i2sConfig.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  i2sConfig.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
  i2sConfig.dma_buf_count = 4;
  i2sConfig.dma_buf_len = SAMPLES;
  i2sConfig.use_apll = false;
  i2sConfig.tx_desc_auto_clear = false;
  i2sConfig.fixed_mclk = 0;

  if (i2s_driver_install(I2S_PORT, &i2sConfig, 0, nullptr) != ESP_OK) {
    Serial.println("I2S driver install failed");
    return;
  }

  i2s_pin_config_t pinConfig = {};
  pinConfig.bck_io_num = static_cast<gpio_num_t>(I2S_SCK);
  pinConfig.ws_io_num = static_cast<gpio_num_t>(I2S_WS);
  pinConfig.data_out_num = I2S_PIN_NO_CHANGE;
  pinConfig.data_in_num = static_cast<gpio_num_t>(I2S_SD);

  if (i2s_set_pin(I2S_PORT, &pinConfig) != ESP_OK) {
    Serial.println("I2S pin setup failed");
    return;
  }

  i2s_zero_dma_buffer(I2S_PORT);
  i2s_start(I2S_PORT);
}

void readI2SData() {
  int32_t rawData[SAMPLES];
  size_t bytesRead = 0;
  i2s_read(I2S_PORT, rawData, sizeof(rawData), &bytesRead, portMAX_DELAY);
  if (bytesRead < sizeof(rawData)) {
    return;
  }

  float dcOffset = 0.0f;
  processI2SData(rawData, dcOffset);
}

float computeLowFreqEnergy() {
  float lowFreqEnergy = 0.0f;
  const int lowFreqStart = 1;
  const int lowFreqEnd = 50;

  for (int i = lowFreqStart; i < lowFreqEnd; ++i) {
    lowFreqEnergy += vReal[i] * vReal[i];
  }

  return lowFreqEnergy;
}

float computeBaselineEnergy() {
  float sortedHistory[HISTORY_SIZE];
  memcpy(sortedHistory, energyHistory, sizeof(energyHistory));
  std::sort(sortedHistory, sortedHistory + HISTORY_SIZE);
  return sortedHistory[HISTORY_SIZE / 2];
}

void setFishSoundDetectionEnabled(bool enabled) {
  sFishSoundDetectionEnabled = enabled;
  if (!enabled) {
    sFishSoundEventPending = false;
  }
}

bool isFishSoundDetectionEnabled() {
  return sFishSoundDetectionEnabled;
}

bool consumeFishSoundEvent() {
  if (!sFishSoundEventPending) {
    return false;
  }

  sFishSoundEventPending = false;
  return true;
}

void clearFishSoundEvent() {
  sFishSoundEventPending = false;
}

void detectFishSound() {
  readI2SData();

  FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(FFT_FORWARD);
  FFT.complexToMagnitude();

  const float lowFreqEnergy = computeLowFreqEnergy();
  const float baselineEnergy = computeBaselineEnergy();

  energyHistory[historyIndex] = lowFreqEnergy;
  historyIndex = (historyIndex + 1) % HISTORY_SIZE;

  const float energyRatio = lowFreqEnergy / (baselineEnergy + 0.01f);
  const bool triggered =
      (energyRatio >= ENERGY_THRESHOLD || lowFreqEnergy >= ABSOLUTE_THRESHOLD) &&
      lowFreqEnergy >= MIN_VOLUME && lowFreqEnergy <= MAX_VOLUME;

  if (!triggered) {
    return;
  }

  if (lowFreqEnergy <= 10.0f) {
    return;
  }

  if (!sFishSoundDetectionEnabled) {
    return;
  }

  const unsigned long now = millis();
  if (now - sLastFishSoundTriggerMs < SOUND_TRIGGER_DEBOUNCE_MS) {
    return;
  }

  sLastFishSoundTriggerMs = now;
  sFishSoundEventPending = true;
  digitalWrite(Audio, !digitalRead(Audio));
}
