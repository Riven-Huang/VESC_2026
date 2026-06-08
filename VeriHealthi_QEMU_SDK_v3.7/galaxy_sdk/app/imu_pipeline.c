/*
 * Copyright (c) 2026, VeriSilicon Holdings Co., Ltd. All rights reserved
 */

#include "imu_pipeline.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "algo_task.h"
#include "hal_imu.h"
#include "imu_sample.h"
#include "osal.h"
#include "uart_printf.h"
#include "vpi_event.h"
#include "vsd_error.h"

#define IMU_EVENT_POOL_SIZE 4
#define IMU_IRQ_WAIT_TIMEOUT_MS IMU_SAMPLE_PERIOD_MS

static ImuDevice *g_imu_dev;
static ImuSampleMessage g_imu_window_pool[IMU_EVENT_POOL_SIZE];
static ImuSample g_imu_sample_ring[IMU_ALGO_WINDOW_SAMPLES];
static volatile uint32_t g_imu_data_ready_count;
static uint32_t g_imu_window_write_index;
static uint32_t g_crc_byte_count;
static uint32_t g_crc32_value = 0xFFFFFFFFU;
static bool g_crc_printed;

static void imu_data_ready_callback(void)
{
    g_imu_data_ready_count++;
}

static int check_vsd_ret(const char *name, int ret)
{
    if (ret != VSD_SUCCESS) {
        uart_printf("%s failed %d\r\n", name, ret);
        return ret;
    }

    return VSD_SUCCESS;
}

static uint32_t crc32_update(uint32_t crc, const uint8_t *data, uint32_t length)
{
    uint32_t i;

    while (length > 0) {
        crc ^= *data;
        for (i = 0; i < 8; i++) {
            if ((crc & 1U) != 0U) {
                crc = (crc >> 1) ^ 0xEDB88320UL;
            } else {
                crc >>= 1;
            }
        }
        data++;
        length--;
    }

    return crc;
}

static void update_first_imu_crc(const ImuGyroAccelData *data)
{
    uint32_t bytes_left;
    uint32_t bytes_to_use;

    if (g_crc_printed) {
        return;
    }

    bytes_left = IMU_CRC_TARGET_BYTES - g_crc_byte_count;
    bytes_to_use = sizeof(*data);
    if (bytes_to_use > bytes_left) {
        bytes_to_use = bytes_left;
    }

    g_crc32_value = crc32_update(g_crc32_value, (const uint8_t *)data, bytes_to_use);
    g_crc_byte_count += bytes_to_use;

    if (g_crc_byte_count >= IMU_CRC_TARGET_BYTES) {
        uart_printf("imu crc32 %d bytes: 0x%x\r\n",
                    (int)IMU_CRC_TARGET_BYTES,
                    (unsigned int)(g_crc32_value ^ 0xFFFFFFFFU));
        g_crc_printed = true;
    }
}

static void notify_algo_window(ImuSampleMessage *window)
{
    int ret;

    ret = vpi_event_notify(EVENT_SEN_DATA_READY, window);
    if (ret != EVENT_OK) {
        uart_printf("notify imu window failed %d\r\n", ret);
    }
}

static void fill_window_snapshot(ImuSampleMessage *window,
                                 uint32_t window_index,
                                 uint32_t end_sample_index)
{
    uint32_t i;
    uint32_t start_sample_index = end_sample_index + 1U - IMU_ALGO_WINDOW_SAMPLES;

    window->window_index = window_index;
    window->sample_count = IMU_ALGO_WINDOW_SAMPLES;
    window->start_timestamp_ms = start_sample_index * IMU_SAMPLE_PERIOD_MS;
    window->end_timestamp_ms = end_sample_index * IMU_SAMPLE_PERIOD_MS;

    for (i = 0; i < IMU_ALGO_WINDOW_SAMPLES; i++) {
        uint32_t sample_index = start_sample_index + i;

        window->samples[i] = g_imu_sample_ring[sample_index % IMU_ALGO_WINDOW_SAMPLES];
    }
}

static void append_sample_to_window(uint32_t sample_index, const ImuGyroAccelData *data)
{
    ImuSampleMessage *window;
    ImuSample *sample = &g_imu_sample_ring[sample_index % IMU_ALGO_WINDOW_SAMPLES];
    uint32_t collected_samples = sample_index + 1U;

    sample->sample_index = sample_index;
    sample->timestamp_ms = sample_index * IMU_SAMPLE_PERIOD_MS;
    sample->data = *data;

    if (collected_samples < IMU_ALGO_WINDOW_SAMPLES) {
        return;
    }

    if (((collected_samples - IMU_ALGO_WINDOW_SAMPLES) % IMU_ALGO_WINDOW_STEP_SAMPLES) != 0U) {
        return;
    }

    window = &g_imu_window_pool[g_imu_window_write_index % IMU_EVENT_POOL_SIZE];
    fill_window_snapshot(window, g_imu_window_write_index, sample_index);
    notify_algo_window(window);
    g_imu_window_write_index++;
}

static bool wait_for_imu_data_ready(void)
{
    uint32_t waited_ms = 0;

    while (g_imu_data_ready_count == 0U && waited_ms < IMU_IRQ_WAIT_TIMEOUT_MS) {
        osal_sleep(1);
        waited_ms++;
    }

    if (g_imu_data_ready_count > 0U) {
        g_imu_data_ready_count--;
        return true;
    }

    return true;
}

int init_imu_pipeline(void)
{
    int ret;

    g_imu_dev = hal_imu_get_device(IMU_DEV_ID_0);
    if (!g_imu_dev) {
        uart_printf("get imu device failed\r\n");
        return VSD_ERR_INVALID_POINTER;
    }

    ret = check_vsd_ret("hal_imu_init", hal_imu_init(g_imu_dev));
    if (ret != VSD_SUCCESS) {
        return ret;
    }

    ret = check_vsd_ret("hal_imu_enable_power", hal_imu_enable_power(g_imu_dev, true));
    if (ret != VSD_SUCCESS) {
        return ret;
    }

    ret = check_vsd_ret("hal_imu_set_sensor_default_cfg", hal_imu_set_sensor_default_cfg(g_imu_dev));
    if (ret != VSD_SUCCESS) {
        return ret;
    }

    ret = check_vsd_ret("hal_imu_set_accel_cfg",
                        hal_imu_set_accel_cfg(g_imu_dev,
                                              8,
                                              0,
                                              IMU_SAMPLE_RATE_HZ,
                                              IMU_SENSOR_RANGE | IMU_SENSOR_ODR));
    if (ret != VSD_SUCCESS) {
        return ret;
    }

    ret = check_vsd_ret("hal_imu_set_gyro_cfg",
                        hal_imu_set_gyro_cfg(g_imu_dev,
                                             2000,
                                             0,
                                             IMU_SAMPLE_RATE_HZ,
                                             IMU_SENSOR_RANGE | IMU_SENSOR_ODR));
    if (ret != VSD_SUCCESS) {
        return ret;
    }

    ret = check_vsd_ret("hal_imu_set_work_mode",
                        hal_imu_set_work_mode(g_imu_dev, IMU_ACCEL_GYRO, IMU_SEN_MODE_NORMAL));
    if (ret != VSD_SUCCESS) {
        return ret;
    }

    ret = check_vsd_ret("hal_imu_set_fifo_wm",
                        hal_imu_set_fifo_wm(g_imu_dev, FIFO_WATERMARK_LEVEL));
    if (ret != VSD_SUCCESS) {
        return ret;
    }

    ret = check_vsd_ret("hal_imu_set_fifo_cfg",
                        hal_imu_set_fifo_cfg(g_imu_dev, IMU_FIFO_GYRO | IMU_FIFO_ACCEL, true));
    if (ret != VSD_SUCCESS) {
        return ret;
    }

    ret = check_vsd_ret("hal_imu_cfg_interrupt",
                        hal_imu_cfg_interrupt(g_imu_dev,
                                              true,
                                              IMU_ACC_GYRO_FIFO_WATERMARK_INTERRUPT,
                                              NULL));
    if (ret != VSD_SUCCESS) {
        return ret;
    }

    ret = check_vsd_ret("hal_imu_enable_interrupt",
                        hal_imu_enable_interrupt(g_imu_dev,
                                                 IMU_DATA_PIN,
                                                 true,
                                                 imu_data_ready_callback));
    if (ret != VSD_SUCCESS) {
        return ret;
    }

    ret = check_vsd_ret("hal_imu_flush_fifo", hal_imu_flush_fifo(g_imu_dev));
    if (ret != VSD_SUCCESS) {
        return ret;
    }

    return VSD_SUCCESS;
}

void task_imu(void *param)
{
    uint32_t sample_index = 0;

    (void)param;

    while (!algo_task_is_ready()) {
        osal_sleep(10);
    }

    while (1) {
        int ret;
        uint16_t available_frame = 0;
        ImuGyroAccelData data;

        (void)wait_for_imu_data_ready();

        ret = hal_imu_read_gyro_accel(g_imu_dev, &data, 1, &available_frame);
        if (ret == VSD_SUCCESS && available_frame > 0) {
            update_first_imu_crc(&data);
            append_sample_to_window(sample_index, &data);
            sample_index++;
            (void)hal_imu_enable_interrupt(g_imu_dev, IMU_DATA_PIN, true, imu_data_ready_callback);
        } else if (ret != VSD_SUCCESS && ret != VSD_ERR_EMPTY) {
            uart_printf("imu read failed %d\r\n", ret);
        }
    }
}
