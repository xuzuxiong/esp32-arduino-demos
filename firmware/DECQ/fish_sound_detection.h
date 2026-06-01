#ifndef FISH_SOUND_DETECTION_H
#define FISH_SOUND_DETECTION_H

#include <algorithm>

#include <Arduino.h>
#include <arduinoFFT.h>
#include <driver/i2s.h>

#define SAMPLES 1024
#define SAMPLE_RATE 48000
#define I2S_WS 25
#define I2S_SD 32
#define I2S_SCK 26
#define I2S_PORT I2S_NUM_0

#define HISTORY_SIZE 50
#define ENERGY_THRESHOLD 8.0
#define ABSOLUTE_THRESHOLD 4.0
#define MIN_VOLUME 0.5
#define MAX_VOLUME 50.0
#define DMA_CHANNEL_FOR_I2S 1

extern float vReal[SAMPLES];
extern float vImag[SAMPLES];
extern ArduinoFFT<float> FFT;
extern float energyHistory[HISTORY_SIZE];
extern int historyIndex;

void setupI2S();
void readI2SData();
float computeLowFreqEnergy();
float computeBaselineEnergy();
void detectFishSound();
void setFishSoundDetectionEnabled(bool enabled);
bool isFishSoundDetectionEnabled();
bool consumeFishSoundEvent();
void clearFishSoundEvent();

#endif
