/*
 * Copyright (c) 2026, VeriSilicon Holdings Co., Ltd. All rights reserved
 */

#ifndef APP_IMU_SAMPLE_H
#define APP_IMU_SAMPLE_H

#include <stdint.h>
#include "hal_imu.h"

#define IMU_SAMPLE_RATE_HZ 50U
#define IMU_SAMPLE_PERIOD_MS (1000U / IMU_SAMPLE_RATE_HZ)
#define IMU_ALGO_WINDOW_SAMPLES IMU_SAMPLE_RATE_HZ
#define IMU_ALGO_WINDOW_STEP_SAMPLES 10U
#define IMU_CRC_TARGET_BYTES 320000UL

typedef struct ImuSample_st {
    uint32_t sample_index;
    uint32_t timestamp_ms;
    ImuGyroAccelData data;
} ImuSample;

typedef struct ImuSampleMessage_st {
    uint32_t window_index;
    uint32_t sample_count;
    uint32_t start_timestamp_ms;
    uint32_t end_timestamp_ms;
    ImuSample samples[IMU_ALGO_WINDOW_SAMPLES];
} ImuSampleMessage;

#endif
