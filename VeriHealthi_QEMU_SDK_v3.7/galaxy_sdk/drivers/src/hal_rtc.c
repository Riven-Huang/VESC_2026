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

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "vsd_error.h"
#include "hal_rtc.h"
#include "hal_api.h"
#include "hal_common.h"

#define HAL_RTC_NAME rtc

HAL_DEFINE_DEVICE_VARIABLES(HAL_RTC_NAME)
HAL_DEFINE_GET_OPS(HAL_RTC_NAME)
HAL_DEFINE_DEVICE_INIT(HAL_RTC_NAME)
HAL_DEFINE_GET_DEVICE(HAL_RTC_NAME)
HAL_DEFINE_DEV_IRQ_HANDLER(HAL_RTC_NAME)

int hal_rtc_init(const RtcDevice *device)
{
    if (!get_ops(device)) {
        return VSD_ERR_INVALID_POINTER;
    }
    if ((!get_ops(device)->init)) {
        return VSD_ERR_UNSUPPORTED;
    }

    return get_ops(device)->init(device);
}

int hal_rtc_enable(const RtcDevice *device, bool enable)
{
    if ((!get_ops(device)->rtc_enable)) {
        return VSD_ERR_UNSUPPORTED;
    }

    return get_ops(device)->rtc_enable(device, enable);
}

int hal_rtc_get_time(const RtcDevice *device, RtcTime *time)
{
    if (!get_ops(device)->get_time) {
        return VSD_ERR_UNSUPPORTED;
    }

    if (!time) {
        return VSD_ERR_INVALID_POINTER;
    }

    return get_ops(device)->get_time(device, time);
}

int hal_get_utc_time(const RtcDevice *device, uint64_t *utc)
{
    if (!device) {
        return VSD_ERR_GENERIC;
    }
    if (!get_ops(device)->get_utc_time) {
        return VSD_ERR_UNSUPPORTED;
    }

    if (!utc) {
        return VSD_ERR_INVALID_POINTER;
    }

    return get_ops(device)->get_utc_time(device, utc);
}

int hal_set_utc_time(const RtcDevice *device, uint64_t utc)
{
    if (!get_ops(device)->set_utc_time) {
        return VSD_ERR_UNSUPPORTED;
    }

    return get_ops(device)->set_utc_time(device, utc);
}

int hal_rtc_get_alarm(const RtcDevice *device, uint64_t *time)
{
    if (!get_ops(device)->get_alarm) {
        return VSD_ERR_UNSUPPORTED;
    }

    if (!time) {
        return VSD_ERR_INVALID_POINTER;
    }

    return get_ops(device)->get_alarm(device, time);
}

int hal_rtc_set_alarm(const RtcDevice *device, uint32_t period, AlarmIrqHandler handler,
                      void *cb_ctx)
{
    if ((!get_ops(device)->set_alarm)) {
        return VSD_ERR_UNSUPPORTED;
    }

    return get_ops(device)->set_alarm(device, period, handler, cb_ctx);
}

int hal_get_boot_time(const RtcDevice *device, uint64_t *time)
{
    if (!device) {
        return VSD_ERR_GENERIC;
    }
    if (!get_ops(device)->get_boot_time) {
        return VSD_ERR_UNSUPPORTED;
    }

    if (!time) {
        return VSD_ERR_INVALID_POINTER;
    }

    return get_ops(device)->get_boot_time(device, time, false);
}

int hal_get_boot_time_us(const RtcDevice *device, uint64_t *time)
{
    if (!device) {
        return VSD_ERR_GENERIC;
    }
    if (!get_ops(device)->get_boot_time) {
        return VSD_ERR_UNSUPPORTED;
    }

    if (!time) {
        return VSD_ERR_INVALID_POINTER;
    }

    return get_ops(device)->get_boot_time(device, time, true);
}

bool hal_rtc_has_synced(const RtcDevice *device)
{
    return get_ops(device)->get_synced(device);
}

int hal_rtc_get_temp(const RtcDevice *device, int *temp_val)
{
    if (!get_ops(device)->get_boot_time) {
        return VSD_ERR_UNSUPPORTED;
    }

    if (!temp_val) {
        return VSD_ERR_INVALID_POINTER;
    }

    return get_ops(device)->get_temperature(device, temp_val);
}
