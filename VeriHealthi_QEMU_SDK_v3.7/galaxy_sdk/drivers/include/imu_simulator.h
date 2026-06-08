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

#ifndef IMU_SIMULATOR_H
#define IMU_SIMULATOR_H

#include "hal_imu.h"

/**
 * @ingroup DRIVER
 * @addtogroup DRIVER_SENSOR
 * @{
 * @addtogroup SENSOR_IMU
 * @{
 */

/**
 * @addtogroup IMU_SIMULATOR
 * IMU Simulator Driver
 * @{
 */

/**
 * @brief Deinit the device for imu simulator
 *
 * @param imu_device handle of imu device
 * @return 0 for succeed, others for failure
 */
int imu_simulator_device_deinit(ImuDevice *imu_device);

extern const ImuOperations imu_simulator_ops;
/** @} */

/** @} */

/** @} */

#endif // IMU_SIMULATOR_H
