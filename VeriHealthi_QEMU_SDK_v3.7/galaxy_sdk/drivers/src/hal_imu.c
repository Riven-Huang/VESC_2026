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

#include <stdint.h>
#include <stddef.h>
#include "hal_imu.h"
#include "hal_api.h"
#include "vsd_error.h"

#define HAL_IMU_NAME imu

HAL_DEFINE_DEVICE_VARIABLES(HAL_IMU_NAME)
HAL_DEFINE_DEVICE_INIT(HAL_IMU_NAME)
HAL_DEFINE_GET_OPS(HAL_IMU_NAME)
HAL_DEFINE_GET_DEVICE(HAL_IMU_NAME)

int hal_imu_init(ImuDevice *dev)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->init)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->init(dev);
}

int hal_imu_deinit(ImuDevice *dev)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->deinit)) {
        return VSD_ERR_UNSUPPORTED;
    }

    return get_ops(dev)->deinit(dev);
}

int hal_imu_soft_reset(ImuDevice *dev)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->soft_reset)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->soft_reset(dev);
}

int hal_imu_read_reg(ImuDevice *dev, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    if (!dev || !data) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->read_reg)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->read_reg(dev, reg_addr, data, len);
}

int hal_imu_write_reg(ImuDevice *dev, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
    if (!dev || !data) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->write_reg)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->write_reg(dev, reg_addr, data, len);
}

int hal_imu_set_sensor_default_cfg(ImuDevice *dev)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->set_sensor_default_cfg)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->set_sensor_default_cfg(dev);
}

int hal_imu_set_accel_cfg(ImuDevice *dev, uint8_t range, uint8_t bwp, uint16_t odr,
                          uint8_t select_type)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->set_accel_cfg)) {
        return VSD_ERR_UNSUPPORTED;
    }

    return get_ops(dev)->set_accel_cfg(dev, range, bwp, odr, select_type);
}

int hal_imu_set_gyro_cfg(ImuDevice *dev, uint16_t range, uint8_t bwp, uint16_t odr,
                         uint8_t select_type)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->set_gyro_cfg)) {
        return VSD_ERR_UNSUPPORTED;
    }

    return get_ops(dev)->set_gyro_cfg(dev, range, bwp, odr, select_type);
}

int hal_imu_set_sensor_cfg(ImuDevice *dev, ImuSensorConfig *accel_cfg, ImuSensorConfig *gyro_cfg,
                           ImuAuxCfg *aux_cfg)
{
    if (!dev || (!accel_cfg && !gyro_cfg && !aux_cfg)) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->set_sensor_cfg)) {
        return VSD_ERR_UNSUPPORTED;
    }

    return get_ops(dev)->set_sensor_cfg(dev, accel_cfg, gyro_cfg, aux_cfg);
}

int hal_imu_get_sens_cfg(ImuDevice *dev, ImuSensorConfig *accel_cfg, ImuSensorConfig *gyro_cfg,
                         ImuAuxCfg *aux_cfg)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->get_sensor_cfg)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->get_sensor_cfg(dev, accel_cfg, gyro_cfg, aux_cfg);
}

int hal_imu_get_accel_range(ImuDevice *dev, uint8_t *range)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->get_accel_range)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->get_accel_range(dev, range);
}

int hal_imu_set_work_mode(ImuDevice *dev, uint8_t sensor, uint8_t mode)
{
    if (!dev) {
        return VSD_ERR_INVALID_POINTER;
    }

    switch (mode) {
    case IMU_SEN_MODE_NORMAL:
        if (get_ops(dev)->set_normal_mode != NULL) {
            return get_ops(dev)->set_normal_mode(dev, sensor);
        }
        break;

    case IMU_SEN_MODE_LOW_PWR:
        if (get_ops(dev)->set_low_power != NULL) {
            return get_ops(dev)->set_low_power(dev, sensor);
        }
        break;

    case IMU_SEN_MODE_OFF:
        if (get_ops(dev)->set_stop_mode != NULL) {
            return get_ops(dev)->set_stop_mode(dev, sensor);
        }
        break;

    default:
        return VSD_ERR_INVALID_PARAM;
    }
    return VSD_ERR_UNSUPPORTED;
}

int hal_imu_get_power_mode(ImuDevice *dev, ImuPmuStatus *pmu_status)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->get_power_mode)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->get_power_mode(dev, pmu_status);
}

int hal_imu_set_fifo_wm(ImuDevice *dev, uint8_t fifo_wm)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->set_fifo_wm)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->set_fifo_wm(dev, fifo_wm);
}

int hal_imu_set_fifo_down(ImuDevice *dev, uint8_t fifo_down)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->set_fifo_down)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->set_fifo_down(dev, fifo_down);
}

int hal_imu_set_fifo_cfg(ImuDevice *dev, uint8_t config, bool enable)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->set_fifo_cfg)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->set_fifo_cfg(dev, config, enable);
}

int hal_imu_flush_fifo(ImuDevice *dev)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->flush_fifo)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->flush_fifo(dev);
}

int hal_imu_get_fifo_data(ImuDevice *dev)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->get_fifo_data)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->get_fifo_data(dev);
}

int hal_imu_read_accel(ImuDevice *dev, ImuSensorData *accel_data, uint16_t frame_request,
                       uint16_t *available_frame)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->fifo_parsing_accel)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->fifo_parsing_accel(dev, accel_data, frame_request, available_frame);
}

int hal_imu_read_gyro_accel(ImuDevice *dev, ImuGyroAccelData *gyro_accel_data,
                            uint16_t frame_request, uint16_t *available_frame)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->fifo_parsing_gyro_accel)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->fifo_parsing_gyro_accel(dev, gyro_accel_data, frame_request,
                                                 available_frame);
}

int hal_imu_cfg_interrupt(ImuDevice *dev, bool enable, uint8_t irq_type,
                          IMUInterruptSetting *irq_settings)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->set_interrupt_cfg)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->set_interrupt_cfg(dev, enable, irq_type, irq_settings);
}

int hal_imu_check_interrupt_status(ImuDevice *dev, uint8_t *irq_type)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->check_interrupt_status)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->check_interrupt_status(dev, irq_type);
}

int hal_imu_set_highg_threshold(ImuDevice *dev, IMUHighGCondition highg_condition)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->set_highg_threshold)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->set_highg_threshold(dev, highg_condition);
}

int hal_imu_reset_step_counter(ImuDevice *dev)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->reset_step_counter)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->reset_step_counter(dev);
}

int hal_imu_set_step_counter(ImuDevice *dev, uint8_t step_cnt_enable)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->set_step_counter)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->set_step_counter(dev, step_cnt_enable);
}

int hal_imu_get_step_counter(ImuDevice *dev, uint16_t *step_val)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->get_step_counter)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->get_step_counter(dev, step_val);
}

int hal_imu_enable_interrupt(ImuDevice *dev, uint8_t pin, bool enable,
                             ImuDataReadyHandler data_ready_callback)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->enable_interrupt)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->enable_interrupt(dev, pin, enable, data_ready_callback);
}

int hal_imu_enable_power(ImuDevice *dev, bool enable)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->enable_power)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->enable_power(dev, enable);
}

int hal_imu_perform_self_test(ImuDevice *dev, uint8_t select_sensor)
{
    if (!dev) {
        return VSD_ERR_INVALID_PARAM;
    }
    if (!(get_ops(dev)->selftest)) {
        return VSD_ERR_UNSUPPORTED;
    }
    return get_ops(dev)->selftest(dev, select_sensor);
}
