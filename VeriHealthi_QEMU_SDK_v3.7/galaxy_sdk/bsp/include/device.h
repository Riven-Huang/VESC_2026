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

#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>

/**
 * @addtogroup BOARD_DEVICE
 * Device definition
 * @ingroup BSP_BOARD
 * @{
 */

/**
 * @brief Device ID definitions for board
 */
typedef enum DeviceID {
    PWR_KEY_DEV_ID = 0, /**< Power key */
    CAMERA_SENSOR_ID,   /**< CAMERA sensor */
    LED_PWM_DEV_ID,     /**< PWM for LED */
    LED_GPIO_DEV0_ID,   /**< GPIO for LED0 */
    LED_GPIO_DEV1_ID,   /**< GPIO for LED1 */
    FUN_KEY_DEV0_ID,    /**< GPIO for KEY0 to trigger function */
    FUN_KEY_DEV1_ID,    /**< GPIO for KEY1 to trigger function */
    FUN_KEY_DEV2_ID,    /**< GPIO for KEY2 to trigger function */
    MAX_DEVICE_ID,
} DeviceID;

/** @} */

#endif // DEVICE_H
