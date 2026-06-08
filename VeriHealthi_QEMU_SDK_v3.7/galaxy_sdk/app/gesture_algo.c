/*
 * Copyright (c) 2026, VeriSilicon Holdings Co., Ltd. All rights reserved
 */

#include "gesture_algo.h"

#include <stddef.h>
#include <stdint.h>
#include "gesture_rf_model.h"

#define GESTURE_CONFIRM_WINDOWS 2U
#define GESTURE_OUTPUT_COOLDOWN_MS 1600U
#define GESTURE_RF_MIN_VOTES 12U

typedef struct GestureFeatures_st {
    int32_t min[6];
    int32_t max[6];
    int32_t sum[6];
    int32_t first[6];
    int32_t last[6];
    int32_t gyro_abs_sum;
    int32_t accel_abs_sum;
} GestureFeatures;

static GestureType g_last_output_type;
static uint32_t g_last_output_timestamp_ms;
static GestureType g_pending_type;
static uint32_t g_pending_count;

void gesture_algo_init(void)
{
    g_last_output_type = GESTURE_NONE;
    g_last_output_timestamp_ms = 0;
    g_pending_type = GESTURE_NONE;
    g_pending_count = 0;
}

const char *gesture_algo_name(GestureType type)
{
    switch (type) {
    case GESTURE_PINCH:
        return "pinch";
    case GESTURE_CLENCH:
        return "clench";
    case GESTURE_UP:
        return "up";
    case GESTURE_DOWN:
        return "down";
    default:
        return "none";
    }
}

static int32_t value_min32(int32_t a, int32_t b)
{
    return a < b ? a : b;
}

static int32_t value_max32(int32_t a, int32_t b)
{
    return a > b ? a : b;
}

static int32_t value_abs32(int32_t value)
{
    return value < 0 ? -value : value;
}

static int32_t round_div_i32(int32_t value, int32_t divisor)
{
    if (value >= 0) {
        return (value + divisor / 2) / divisor;
    }

    return (value - divisor / 2) / divisor;
}

static void sample_to_axis_values(const ImuGyroAccelData *data, int32_t axis[6])
{
    axis[0] = data->gx;
    axis[1] = data->gy;
    axis[2] = data->gz;
    axis[3] = data->ax;
    axis[4] = data->ay;
    axis[5] = data->az;
}

static void extract_features(const ImuSampleMessage *window, GestureFeatures *features)
{
    uint32_t i;
    uint32_t axis_index;
    int32_t axis[6];

    sample_to_axis_values(&window->samples[0].data, axis);
    for (axis_index = 0; axis_index < 6; axis_index++) {
        features->min[axis_index] = axis[axis_index];
        features->max[axis_index] = axis[axis_index];
        features->sum[axis_index] = 0;
        features->first[axis_index] = axis[axis_index];
        features->last[axis_index] = axis[axis_index];
    }
    features->gyro_abs_sum = 0;
    features->accel_abs_sum = 0;

    for (i = 0; i < window->sample_count; i++) {
        const ImuGyroAccelData *data = &window->samples[i].data;

        sample_to_axis_values(data, axis);
        for (axis_index = 0; axis_index < 6; axis_index++) {
            features->min[axis_index] = value_min32(features->min[axis_index], axis[axis_index]);
            features->max[axis_index] = value_max32(features->max[axis_index], axis[axis_index]);
            features->sum[axis_index] += axis[axis_index];
            features->last[axis_index] = axis[axis_index];
        }
        features->gyro_abs_sum += value_abs32(axis[0]) + value_abs32(axis[1]) + value_abs32(axis[2]);
        features->accel_abs_sum += value_abs32(axis[3]) + value_abs32(axis[4]) + value_abs32(axis[5]);
    }
}

static void fill_rf_features(const GestureFeatures *features,
                             uint32_t sample_count,
                             GestureRfFeatures *rf_features)
{
    uint32_t axis_index;
    uint32_t out_index = 0;

    for (axis_index = 0; axis_index < 6; axis_index++) {
        rf_features->value[out_index++] = features->max[axis_index] - features->min[axis_index];
    }

    for (axis_index = 0; axis_index < 6; axis_index++) {
        rf_features->value[out_index++] =
            round_div_i32(features->sum[axis_index], (int32_t)sample_count);
    }

    for (axis_index = 0; axis_index < 6; axis_index++) {
        rf_features->value[out_index++] = features->last[axis_index] - features->first[axis_index];
    }

    rf_features->value[out_index++] =
        round_div_i32(features->gyro_abs_sum, (int32_t)sample_count);
    rf_features->value[out_index++] =
        round_div_i32(features->accel_abs_sum, (int32_t)sample_count);
    rf_features->value[out_index++] =
        (features->max[0] - features->min[0]) +
        (features->max[1] - features->min[1]) +
        (features->max[2] - features->min[2]);
    rf_features->value[out_index++] =
        (features->max[3] - features->min[3]) +
        (features->max[4] - features->min[4]) +
        (features->max[5] - features->min[5]);
}

static bool is_in_cooldown(GestureType type, uint32_t timestamp_ms)
{
    if (type == GESTURE_NONE) {
        return true;
    }

    if (g_last_output_type == GESTURE_NONE) {
        return false;
    }

    if (timestamp_ms < g_last_output_timestamp_ms) {
        return false;
    }

    if ((timestamp_ms - g_last_output_timestamp_ms) < GESTURE_OUTPUT_COOLDOWN_MS) {
        return true;
    }

    return false;
}

static bool confirm_prediction(GestureType type)
{
    if (type == GESTURE_NONE) {
        g_pending_type = GESTURE_NONE;
        g_pending_count = 0;
        return false;
    }

    if (type == g_pending_type) {
        g_pending_count++;
    } else {
        g_pending_type = type;
        g_pending_count = 1;
    }

    return g_pending_count >= GESTURE_CONFIRM_WINDOWS;
}

bool gesture_algo_process_window(const ImuSampleMessage *window, GestureResult *result)
{
    GestureFeatures features;
    GestureRfFeatures rf_features;
    GestureType type;
    uint8_t votes;

    if (!window || !result || window->sample_count == 0) {
        return false;
    }

    extract_features(window, &features);
    fill_rf_features(&features, window->sample_count, &rf_features);
    type = gesture_rf_predict_with_votes(&rf_features, &votes);
    if (votes < GESTURE_RF_MIN_VOTES) {
        g_pending_type = GESTURE_NONE;
        g_pending_count = 0;
        return false;
    }

    if (!confirm_prediction(type)) {
        return false;
    }

    if (is_in_cooldown(type, window->end_timestamp_ms)) {
        return false;
    }

    result->timestamp_ms = window->end_timestamp_ms;
    result->type = type;
    g_last_output_type = type;
    g_last_output_timestamp_ms = result->timestamp_ms;
    g_pending_count = 0;

    return true;
}
