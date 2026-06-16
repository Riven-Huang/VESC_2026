/*
 * Copyright (c) 2026, VeriSilicon Holdings Co., Ltd. All rights reserved
 */

#include "gesture_algo.h"

#include <stddef.h>
#include <stdint.h>
#include "gesture_rf_model.h"

#define GESTURE_CONFIRM_WINDOWS 3U
#define GESTURE_OUTPUT_COOLDOWN_MS 600U
#define GESTURE_RELEASE_WINDOWS 2U
#define GESTURE_SEGMENT_COUNT 5U
#define GESTURE_SEGMENT_SAMPLES (IMU_ALGO_WINDOW_SAMPLES / GESTURE_SEGMENT_COUNT)
#define GESTURE_QUARTER_SAMPLES (IMU_ALGO_WINDOW_SAMPLES / 4U)

#if IMU_ALGO_WINDOW_SAMPLES != 50U
#error "The gesture model requires a 50-sample window"
#endif

#if GESTURE_RF_FEATURE_COUNT != 123U
#error "The gesture model requires 123 input features"
#endif

typedef struct GestureStatistics_st {
    int32_t min[6];
    int32_t max[6];
    int32_t sum[6];
    int32_t first[6];
    int32_t last[6];
    int32_t segment_min[GESTURE_SEGMENT_COUNT][6];
    int32_t segment_max[GESTURE_SEGMENT_COUNT][6];
    int32_t segment_sum[GESTURE_SEGMENT_COUNT][6];
} GestureStatistics;

static GestureType g_last_output_type;
static uint32_t g_last_output_timestamp_ms;
static GestureType g_pending_type;
static uint32_t g_pending_count;
static GestureType g_blocked_type;
static uint32_t g_blocked_release_count;
static bool g_arm_raised;

static int32_t value_abs32(int32_t value)
{
    return value < 0 ? -value : value;
}

static int32_t value_min32(int32_t a, int32_t b)
{
    return a < b ? a : b;
}

static int32_t value_max32(int32_t a, int32_t b)
{
    return a > b ? a : b;
}

static int32_t round_div_i64(int64_t value, int32_t divisor)
{
    if (value >= 0) {
        return (int32_t)((value + divisor / 2) / divisor);
    }
    return (int32_t)(-((-value + divisor / 2) / divisor));
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

static void collect_statistics(const ImuSampleMessage *window, GestureStatistics *stats)
{
    uint32_t axis_index;
    uint32_t sample_index;
    int32_t axis[6];

    sample_to_axis_values(&window->samples[0].data, axis);
    for (axis_index = 0U; axis_index < 6U; axis_index++) {
        stats->min[axis_index] = axis[axis_index];
        stats->max[axis_index] = axis[axis_index];
        stats->sum[axis_index] = 0;
        stats->first[axis_index] = axis[axis_index];
        stats->last[axis_index] = axis[axis_index];
    }

    for (sample_index = 0U; sample_index < window->sample_count; sample_index++) {
        uint32_t segment_index = sample_index / GESTURE_SEGMENT_SAMPLES;

        sample_to_axis_values(&window->samples[sample_index].data, axis);
        for (axis_index = 0U; axis_index < 6U; axis_index++) {
            int32_t value = axis[axis_index];

            stats->min[axis_index] = value_min32(stats->min[axis_index], value);
            stats->max[axis_index] = value_max32(stats->max[axis_index], value);
            stats->sum[axis_index] += value;
            stats->last[axis_index] = value;

            if ((sample_index % GESTURE_SEGMENT_SAMPLES) == 0U) {
                stats->segment_min[segment_index][axis_index] = value;
                stats->segment_max[segment_index][axis_index] = value;
                stats->segment_sum[segment_index][axis_index] = 0;
            }
            stats->segment_min[segment_index][axis_index] =
                value_min32(stats->segment_min[segment_index][axis_index], value);
            stats->segment_max[segment_index][axis_index] =
                value_max32(stats->segment_max[segment_index][axis_index], value);
            stats->segment_sum[segment_index][axis_index] += value;
        }
    }
}

static void fill_rf_features(const ImuSampleMessage *window,
                             const GestureStatistics *stats,
                             GestureRfFeatures *features)
{
    uint32_t axis_index;
    uint32_t sample_index;
    uint32_t segment_index;
    uint32_t out_index = 0U;
    int32_t mean[6];
    int64_t deviation_sum[6] = {0};
    int32_t max_deviation[6] = {0};
    int64_t first_half_sum[6] = {0};
    int64_t second_half_sum[6] = {0};
    int64_t first_quarter_sum[6] = {0};
    int64_t last_quarter_sum[6] = {0};
    int64_t segment_gyro_activity_sum[GESTURE_SEGMENT_COUNT] = {0};
    int64_t segment_accel_activity_sum[GESTURE_SEGMENT_COUNT] = {0};
    int32_t segment_gyro_activity_max[GESTURE_SEGMENT_COUNT] = {0};
    int32_t segment_accel_activity_max[GESTURE_SEGMENT_COUNT] = {0};

    for (axis_index = 0U; axis_index < 6U; axis_index++) {
        mean[axis_index] = round_div_i64(stats->sum[axis_index], window->sample_count);
        features->value[out_index++] = stats->max[axis_index] - stats->min[axis_index];
    }
    for (axis_index = 3U; axis_index < 6U; axis_index++) {
        features->value[out_index++] = mean[axis_index];
    }
    for (axis_index = 0U; axis_index < 6U; axis_index++) {
        features->value[out_index++] = stats->last[axis_index] - stats->first[axis_index];
    }

    for (sample_index = 0U; sample_index < window->sample_count; sample_index++) {
        uint32_t segment = sample_index / GESTURE_SEGMENT_SAMPLES;
        int32_t axis[6];
        int32_t gyro_activity = 0;
        int32_t accel_activity = 0;

        sample_to_axis_values(&window->samples[sample_index].data, axis);
        for (axis_index = 0U; axis_index < 6U; axis_index++) {
            int32_t deviation = value_abs32(axis[axis_index] - mean[axis_index]);

            deviation_sum[axis_index] += deviation;
            max_deviation[axis_index] = value_max32(max_deviation[axis_index], deviation);
            if (sample_index < window->sample_count / 2U) {
                first_half_sum[axis_index] += axis[axis_index];
            } else {
                second_half_sum[axis_index] += axis[axis_index];
            }
            if (sample_index < GESTURE_QUARTER_SAMPLES) {
                first_quarter_sum[axis_index] += axis[axis_index];
            }
            if (sample_index >= window->sample_count - GESTURE_QUARTER_SAMPLES) {
                last_quarter_sum[axis_index] += axis[axis_index];
            }
            if (axis_index < 3U) {
                gyro_activity += deviation;
            } else {
                accel_activity += deviation;
            }
        }
        segment_gyro_activity_sum[segment] += gyro_activity;
        segment_accel_activity_sum[segment] += accel_activity;
        segment_gyro_activity_max[segment] =
            value_max32(segment_gyro_activity_max[segment], gyro_activity);
        segment_accel_activity_max[segment] =
            value_max32(segment_accel_activity_max[segment], accel_activity);
    }

    features->value[out_index++] = round_div_i64(
        segment_gyro_activity_sum[0] + segment_gyro_activity_sum[1] +
            segment_gyro_activity_sum[2] + segment_gyro_activity_sum[3] +
            segment_gyro_activity_sum[4],
        window->sample_count);
    features->value[out_index++] = round_div_i64(
        segment_accel_activity_sum[0] + segment_accel_activity_sum[1] +
            segment_accel_activity_sum[2] + segment_accel_activity_sum[3] +
            segment_accel_activity_sum[4],
        window->sample_count);
    features->value[out_index++] =
        (stats->max[0] - stats->min[0]) + (stats->max[1] - stats->min[1]) +
        (stats->max[2] - stats->min[2]);
    features->value[out_index++] =
        (stats->max[3] - stats->min[3]) + (stats->max[4] - stats->min[4]) +
        (stats->max[5] - stats->min[5]);

    for (axis_index = 0U; axis_index < 6U; axis_index++) {
        features->value[out_index++] = round_div_i64(deviation_sum[axis_index], window->sample_count);
    }
    for (axis_index = 0U; axis_index < 6U; axis_index++) {
        features->value[out_index++] =
            round_div_i64(second_half_sum[axis_index], window->sample_count / 2U) -
            round_div_i64(first_half_sum[axis_index], window->sample_count / 2U);
    }
    for (axis_index = 0U; axis_index < 6U; axis_index++) {
        features->value[out_index++] =
            round_div_i64(last_quarter_sum[axis_index], GESTURE_QUARTER_SAMPLES) -
            round_div_i64(first_quarter_sum[axis_index], GESTURE_QUARTER_SAMPLES);
    }
    for (axis_index = 0U; axis_index < 6U; axis_index++) {
        features->value[out_index++] = max_deviation[axis_index];
    }
    for (segment_index = 0U; segment_index < GESTURE_SEGMENT_COUNT; segment_index++) {
        for (axis_index = 0U; axis_index < 6U; axis_index++) {
            features->value[out_index++] =
                round_div_i64(stats->segment_sum[segment_index][axis_index],
                              GESTURE_SEGMENT_SAMPLES) -
                mean[axis_index];
        }
    }
    for (segment_index = 0U; segment_index < GESTURE_SEGMENT_COUNT; segment_index++) {
        for (axis_index = 0U; axis_index < 6U; axis_index++) {
            features->value[out_index++] =
                stats->segment_max[segment_index][axis_index] -
                stats->segment_min[segment_index][axis_index];
        }
    }
    for (segment_index = 0U; segment_index < GESTURE_SEGMENT_COUNT; segment_index++) {
        features->value[out_index++] =
            round_div_i64(segment_gyro_activity_sum[segment_index], GESTURE_SEGMENT_SAMPLES);
        features->value[out_index++] = segment_gyro_activity_max[segment_index];
        features->value[out_index++] =
            round_div_i64(segment_accel_activity_sum[segment_index], GESTURE_SEGMENT_SAMPLES);
        features->value[out_index++] = segment_accel_activity_max[segment_index];
    }

    (void)out_index;
}

static uint8_t vote_threshold(GestureType type)
{
    switch (type) {
    case GESTURE_PINCH:
        return 93U;
    case GESTURE_CLENCH:
        return 88U;
    case GESTURE_UP:
    case GESTURE_DOWN:
        return 65U;
    default:
        return 121U;
    }
}

static bool arm_state_allows(GestureType type)
{
    if (type == GESTURE_UP) {
        return !g_arm_raised;
    }
    if (type == GESTURE_DOWN) {
        return g_arm_raised;
    }
    return true;
}

static bool confirm_prediction(GestureType type)
{
    if (type == GESTURE_NONE) {
        g_pending_type = GESTURE_NONE;
        g_pending_count = 0U;
        return false;
    }
    if (type == g_pending_type) {
        g_pending_count++;
    } else {
        g_pending_type = type;
        g_pending_count = 1U;
    }
    return g_pending_count >= GESTURE_CONFIRM_WINDOWS;
}

static bool is_in_cooldown(uint32_t timestamp_ms)
{
    if (g_last_output_type == GESTURE_NONE || timestamp_ms < g_last_output_timestamp_ms) {
        return false;
    }
    return timestamp_ms - g_last_output_timestamp_ms < GESTURE_OUTPUT_COOLDOWN_MS;
}

void gesture_algo_init(void)
{
    g_last_output_type = GESTURE_NONE;
    g_last_output_timestamp_ms = 0U;
    g_pending_type = GESTURE_NONE;
    g_pending_count = 0U;
    g_blocked_type = GESTURE_NONE;
    g_blocked_release_count = 0U;
    g_arm_raised = false;
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

bool gesture_algo_process_window(const ImuSampleMessage *window, GestureResult *result)
{
    GestureStatistics stats;
    GestureRfFeatures features;
    GestureType type;
    uint8_t votes[5];

    if (!window || !result || window->sample_count != IMU_ALGO_WINDOW_SAMPLES) {
        return false;
    }

    collect_statistics(window, &stats);
    fill_rf_features(window, &stats, &features);
    type = gesture_rf_predict_with_votes(&features, votes);
    if (type == GESTURE_NONE || votes[type] < vote_threshold(type)) {
        type = GESTURE_NONE;
    }

    if (g_blocked_type != GESTURE_NONE) {
        if (type == g_blocked_type) {
            g_blocked_release_count = 0U;
            (void)confirm_prediction(GESTURE_NONE);
            return false;
        }
        g_blocked_release_count++;
        if (g_blocked_release_count < GESTURE_RELEASE_WINDOWS) {
            (void)confirm_prediction(GESTURE_NONE);
            return false;
        }
        g_blocked_type = GESTURE_NONE;
        g_blocked_release_count = 0U;
    }

    if (type == GESTURE_NONE) {
        (void)confirm_prediction(GESTURE_NONE);
        return false;
    }

    if (!arm_state_allows(type)) {
        (void)confirm_prediction(GESTURE_NONE);
        return false;
    }
    if (!confirm_prediction(type) || is_in_cooldown(window->end_timestamp_ms)) {
        return false;
    }

    result->timestamp_ms = window->end_timestamp_ms;
    result->type = type;
    g_last_output_type = type;
    g_last_output_timestamp_ms = result->timestamp_ms;
    g_blocked_type = type;
    g_blocked_release_count = 0U;
    g_pending_type = GESTURE_NONE;
    g_pending_count = 0U;
    if (type == GESTURE_UP) {
        g_arm_raised = true;
    } else if (type == GESTURE_DOWN) {
        g_arm_raised = false;
    }
    return true;
}
