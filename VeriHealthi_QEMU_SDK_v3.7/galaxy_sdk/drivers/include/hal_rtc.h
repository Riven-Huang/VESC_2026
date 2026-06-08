/*
 * Copyright (c) 2026, VeriSilicon Holdings Co., Ltd. All rights reserved
 *
 * 1. Redistributing the source code of this software is only allowed after
 * receiving explicit, written permission from VeriSilicon. The copyright notice,
 * this list of conditions and the following disclaimer must be retained in all
 * source code distributions.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 */

#ifndef HAL_RTC_H
#define HAL_RTC_H

#include <stdint.h>
#include <stdbool.h>
#include "base_device.h"

/**
 * @addtogroup HAL_RTC
 * RTC HAL API and definition
 * @ingroup DRIVER_HAL
 * Hardware Abstraction Layer
 * @{
 */

/**
 * @brief RTC clock source
 */
typedef enum RtcOsc {
    RTC_OSC_ROSC = 0, /**< Select ROSC as RTC clock source */
    RTC_OSC_XOSC = 1, /**< Select XOSC as RTC clock source */
} RtcOsc;

/**
 * @brief Structure of RTC hardware configuration
 */
typedef struct RtcHwConfig_st {
    uint32_t tclk_freq; /**< Target clk config, when need different accuracy of
                             RTC, tclk_freq is set*/
    uint8_t clk_source; /**< RTC clock source */
} RtcHwConfig;

/**
 * @brief Structure of RTC time
 */
typedef struct RtcTime_st {
    uint16_t tm_year; /**< Year, need when set*/
    uint8_t tm_mon;   /**< Month, need when set*/
    uint8_t tm_day;   /**< Day in the month, need when set*/
    uint8_t tm_week;  /**< Day of the week, need when set*/
    uint8_t tm_hour;  /**< Hour, need when set*/
    uint8_t tm_min;   /**< Minute, need when set*/
    uint8_t tm_sec;   /**< Second, need when set*/
    uint16_t tm_ms;   /**< Millisecond, need when set*/
} RtcTime;

/**
 * @brief RTC interrupt callback handler
 * @param cb_context Callback context
 */
typedef void (*AlarmIrqHandler)(void *cb_context);

/**
 * @brief This struct define RTC hardware
 */
typedef struct RtcDevice_st {
    BaseDevice base; /**< Base device for RTC */
    const RtcHwConfig *hw_config;
} RtcDevice;
typedef RtcDevice rtcDevice;

/**
 * @brief Structure of operations for HAL of RTC
 */
typedef struct RtcOperations_st {
    int (*device_init)(RtcDevice *device);
    int (*init)(const RtcDevice *device);
    int (*rtc_enable)(const RtcDevice *device, bool enable);
    int (*get_time)(const RtcDevice *device, RtcTime *time);
    int (*get_utc_time)(const RtcDevice *device, uint64_t *utc);
    int (*set_utc_time)(const RtcDevice *device, uint64_t utc);
    int (*get_alarm)(const RtcDevice *device, uint64_t *time);
    int (*set_alarm)(const RtcDevice *device, uint32_t period, AlarmIrqHandler handler,
                     void *cb_ctx);
    int (*get_boot_time)(const RtcDevice *device, uint64_t *time, bool unit_us);
    int (*get_temperature)(const RtcDevice *device, int *temp_val);
    bool (*get_synced)(const RtcDevice *device);
    void (*irq_handler)(const RtcDevice *device);
} RtcOperations;
typedef RtcOperations rtcOperations;

/**
 * @brief Initialize RTC device instance
 * @param[in] list Device list of RTC
 * @param[in] count Count number of RTC device
 * @return  VSD_SUCCESS on success, others on failure
 */
int hal_rtc_device_init(RtcDevice *list, uint8_t count);

/**
 * @brief Get RTC device instance
 * @param dev_id Device id of a RTC device
 * @return Return result
 * @retval RtcDevice* a RtcDevice instance on success, NULL on failure
 */
RtcDevice *hal_rtc_get_device(uint8_t dev_id);

/**
 * @brief Initialize the real time clock
 * @param device Pointer to the RTC hardware device structure
 * @return Return result
 * @retval VSD_SUCCESS on success, others on failure
 */
int hal_rtc_init(const RtcDevice *device);

/**
 * @brief Enable the RTC
 * @param  device Pointer to the RTC hardware device structure
 * @param  enable Determine if the RTC is enabled
 * @return Return result
 * @retval VSD_SUCCESS on success, others on failure
 */
int hal_rtc_enable(const RtcDevice *device, bool enable);

/**
 * @brief Get time from the RTC
 * @param device Pointer to the RTC hardware device structure
 * @param time Pointer to a time structure
 * @return Return result
 * @retval VSD_SUCCESS on success, others on failure
 */
int hal_rtc_get_time(const RtcDevice *device, RtcTime *time);

/**
 * @brief Get UTC time in millisecond to the RTC
 * @param device Pointer to the RTC hardware device structure
 * @param utc Pointer to UTC time
 * @return Return result
 * @retval VSD_SUCCESS on success, others on failure
 */
int hal_get_utc_time(const RtcDevice *device, uint64_t *utc);

/**
 * @brief Set UTC time in millisecond to the RTC
 * @param device Pointer to the RTC hardware device structure
 * @param utc New UTC time
 * @return Return result
 * @retval VSD_SUCCESS on success, others on failure
 */
int hal_set_utc_time(const RtcDevice *device, uint64_t utc);

/**
 * @brief Get alarm time in millisecond from the RTC
 * @param device Pointer to the RTC hardware device structure
 * @param time Pointer to a alarm time
 * @return Return result
 * @retval VSD_SUCCESS on success, others on failure
 */
int hal_rtc_get_alarm(const RtcDevice *device, uint64_t *time);

/**
 * @brief Set alarm time in millisecond from the RTC
 * @param device Pointer to the RTC hardware device structure
 * @param period An alarm clock will be generated after the period
 * @param handler A function pointer to the interrupt handler
 * @param cb_ctx Callback context
 * @return Return result
 * @retval VSD_SUCCESS on success, others on failure
 */
int hal_rtc_set_alarm(const RtcDevice *device, uint32_t period, AlarmIrqHandler handler,
                      void *cb_ctx);

/**
 * @brief Get boot time in millisecond from the RTC
 * @param device Pointer to the RTC hardware device structure
 * @param time Pointer to boot time (ms)
 * @return Return result
 * @retval VSD_SUCCESS on success, others on failure
 */
int hal_get_boot_time(const RtcDevice *device, uint64_t *time);

/**
 * @brief Get boot time in microsecond(us) from the RTC
 * @param device Pointer to the RTC hardware device structure
 * @param time Pointer to boot time (us)
 * @return Return result
 * @retval VSD_SUCCESS on success, others on failure
 */
int hal_get_boot_time_us(const RtcDevice *device, uint64_t *time);

/**
 * @brief Get RTC synced flag
 * @param device Pointer to the RTC hardware device structure
 * @return Return synced flag
 */
bool hal_rtc_has_synced(const RtcDevice *device);

/**
 * @brief Get hundredfold temperature value from the RTC
 * @param device Pointer to the RTC hardware device structure
 * @param temp_val Pointer to hundredfold temperature value
 * @return Return result
 * @retval VSD_SUCCESS on success, others on failure
 */
int hal_rtc_get_temp(const RtcDevice *device, int *temp_val);

/**
 * @brief RTC interrupt handler
 * @param device An instance of RTC device
 */
void hal_rtc_irq_handler(const RtcDevice *device);

/** @} */

#endif // HAL_RTC_H
