/*
 * Copyright (c) 2026, VeriSilicon Holdings Co., Ltd. All rights reserved
 */

#ifndef APP_GESTURE_ALGO_H
#define APP_GESTURE_ALGO_H

#include <stdbool.h>
#include <stdint.h>
#include "imu_sample.h"

typedef enum GestureType_en {
    GESTURE_NONE = 0,
    GESTURE_PINCH,
    GESTURE_CLENCH,
    GESTURE_UP,
    GESTURE_DOWN,
} GestureType;

typedef struct GestureResult_st {
    GestureType type;
    uint32_t timestamp_ms;
} GestureResult;

void gesture_algo_init(void);
bool gesture_algo_process_window(const ImuSampleMessage *window, GestureResult *result);
const char *gesture_algo_name(GestureType type);

#endif
