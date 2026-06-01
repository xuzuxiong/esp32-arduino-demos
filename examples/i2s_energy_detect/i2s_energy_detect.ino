/**
 * Minimal example: PCM1808 I2S + FFT low-frequency energy detection
 *
 * Hardware I2S pins (same as DECQ main firmware):
 *   BCK/SCK = GPIO26, WS = GPIO25, SD = GPIO32
 *
 * Serial 115200 prints energy ratio once per second when above threshold.
 */
#include <Arduino.h>
#include <algorithm>
#include <arduinoFFT.h>
#include <driver/i2s.h>

static const int SAMPLES = 1024;
static const int SAMPLE_RATE = 48000;
static const int I2S_WS = 25;
static const int I2S_SD = 32;
static const int I2S_SCK = 26;
static const i2s_port_t I2S_PORT = I2S_NUM_0;

static const int HISTORY_SIZE = 50;
static const float ENERGY_THRESHOLD = 8.0f;
static const float ABSOLUTE_THRESHOLD = 4.0f;

static float vReal[SAMPLES];
static float vImag[SAMPLES];
static ArduinoFFT<float> FFT(vReal, vImag, SAMPLES, SAMPLE_RATE);
static float energyHistory[HISTORY_SIZE] = {0};
static int historyIndex = 0;

static void setupI2S() {
  i2s_config_t cfg = {};
  cfg.mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_RX);
  cfg.sample_rate = SAMPLE_RATE;
  cfg.bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT;
  cfg.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
  cfg.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  cfg.intr_alloc_flags = ESP_INTR_FLAG_LEVEL1;
  cfg.dma_buf_count = 4;
  cfg.dma_buf_len = SAMPLES;

  i2s_driver_install(I2S_PORT, &cfg, 0, nullptr);

  i2s_pin_config_t pins = {};
  pins.bck_io_num = static_cast<gpio_num_t>(I2S_SCK);
  pins.ws_io_num = static_cast<gpio_num_t>(I2S_WS);
  pins.data_out_num = I2S_PIN_NO_CHANGE;
  pins.data_in_num = static_cast<gpio_num_t>(I2S_SD);
  i2s_set_pin(I2S_PORT, &pins);
  i2s_zero_dma_buffer(I2S_PORT);
  i2s_start(I2S_PORT);
}

static void readI2SData() {
  int32_t raw[SAMPLES];
  size_t bytesRead = 0;
  i2s_read(I2S_PORT, raw, sizeof(raw), &bytesRead, portMAX_DELAY);
  if (bytesRead < sizeof(raw)) return;

  int64_t sum = 0;
  for (int i = 0; i < SAMPLES; ++i) {
    raw[i] >>= 8;
    sum += raw[i];
  }
  const float dc = static_cast<float>(sum) / static_cast<float>(SAMPLES);
  for (int i = 0; i < SAMPLES; ++i) {
    vReal[i] = (raw[i] - dc) / 8388608.0f;
    vImag[i] = 0.0f;
  }
}

static float computeLowFreqEnergy() {
  float energy = 0.0f;
  for (int i = 1; i < 50; ++i) energy += vReal[i] * vReal[i];
  return energy;
}

static float computeBaselineEnergy() {
  float sorted[HISTORY_SIZE];
  memcpy(sorted, energyHistory, sizeof(energyHistory));
  std::sort(sorted, sorted + HISTORY_SIZE);
  return sorted[HISTORY_SIZE / 2];
}

void setup() {
  Serial.begin(115200);
  delay(500);
  setupI2S();
  Serial.println("I2S energy detect example ready");
}

void loop() {
  readI2SData();
  FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(FFT_FORWARD);
  FFT.complexToMagnitude();

  const float lowEnergy = computeLowFreqEnergy();
  const float baseline = computeBaselineEnergy();
  energyHistory[historyIndex] = lowEnergy;
  historyIndex = (historyIndex + 1) % HISTORY_SIZE;

  const float ratio = lowEnergy / (baseline + 0.01f);
  if (ratio >= ENERGY_THRESHOLD || lowEnergy >= ABSOLUTE_THRESHOLD) {
    Serial.printf("EVENT energy=%.2f baseline=%.2f ratio=%.2f\n", lowEnergy, baseline, ratio);
  }

  delay(200);
}
