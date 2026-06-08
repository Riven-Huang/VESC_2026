/*
 * Copyright (c) 2025, VeriSilicon Holdings Co., Ltd. All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef HAL_API_H
#define HAL_API_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hal_common.h"

/**
 * @ingroup DRIVER
 * @addtogroup DRIVER_HAL
 * @{
 */

/**
 * @addtogroup HAL_API
 * Hardware Abstraction Layer Common
 * @{
 */

/**
 * @brief Declare HAL device type which asks that it should
 * be same as your XXXDevice which is defined in hal_xxx.h
 *
 * HAL_DEVICE_NAME(uart) will be : uartDevice
 */
#define HAL_DEVICE_NAME_INNER(name) name##Device
#define HAL_DEVICE_NAME(name)       HAL_DEVICE_NAME_INNER(name)

/**
 * @brief Declare HAL operations type which asks that it should
 * be same as your XXXOperations which is defined in hal_xxx.h
 *
 * HAL_DEVICE_OPS(uart) will be : uartOperations
 */
#define HAL_DEVICE_OPS_INNER(name) name##Operations
#define HAL_DEVICE_OPS(name)       HAL_DEVICE_OPS_INNER(name)

/**
 * @brief Define HAL device list's pointer and count, it should
 * only be called in hal_xxx.c when you add your HAL
 *
 * HAL_DEFINE_DEVICE_VARIABLES(uart) will define two variables
 * in hal_uart.c
 * static uartDevice *uart_dev_list;
 * static uint8_t uart_dev_count;
 */
#define HAL_DEFINE_DEVICE_VARIABLES_INNER(name)     \
    static HAL_DEVICE_NAME(name) * name##_dev_list; \
    static uint8_t name##_dev_count;
#define HAL_DEFINE_DEVICE_VARIABLES(name) HAL_DEFINE_DEVICE_VARIABLES_INNER(name)

/**
 * @brief Define get operations function for every HAL driver
 * type, such as Uart, Spi, I2c
 *
 * HAL_DEFINE_GET_OPS(uart) will be :
 * static inline uartOperations *get_ops(const uartDevice *dev)
 * {
 *     return dev->base.ops;
 * }
 */
#define HAL_DEFINE_GET_OPS_INNER(name)                                     \
    static inline const name##Operations *get_ops(const name##Device *dev) \
    {                                                                      \
        return dev->base.ops;                                              \
    }
#define HAL_DEFINE_GET_OPS(name) HAL_DEFINE_GET_OPS_INNER(name)

/**
 * @brief Define hal_xxx_device_init for every HAL driver type and it
 * should be called in hal_xxx.c to define this function
 *
 * HAL_DEFINE_DEVICE_INIT(uart) will be :
 * int hal_uart_device_init(uartDevice *list, uint8_t count)
 * {
 *     ...
 * }
 */
#define HAL_DEFINE_DEVICE_INIT_INNER(name)                                    \
    int hal_##name##_device_init(HAL_DEVICE_NAME(name) * list, uint8_t count) \
    {                                                                         \
        int ret;                                                              \
        uint8_t i;                                                            \
        const HAL_DEVICE_OPS(name) * drv_ops;                                 \
                                                                              \
        if (!list || !count) {                                                \
            return VSD_ERR_INVALID_PARAM;                                     \
        }                                                                     \
        if (name##_dev_list || name##_dev_count) {                            \
            return VSD_ERR_GENERIC;                                           \
        }                                                                     \
        name##_dev_list  = list;                                              \
        name##_dev_count = count;                                             \
        for (i = 0; i < name##_dev_count; ++i) {                              \
            drv_ops = (name##_dev_list + i)->base.ops;                        \
            if (!drv_ops || !(drv_ops->device_init)) {                        \
                return VSD_ERR_INVALID_POINTER;                               \
            }                                                                 \
            ret = drv_ops->device_init(name##_dev_list + i);                  \
            if (ret != VSD_SUCCESS) {                                         \
                return ret;                                                   \
            }                                                                 \
        }                                                                     \
        return VSD_SUCCESS;                                                   \
    }
#define HAL_DEFINE_DEVICE_INIT(name) HAL_DEFINE_DEVICE_INIT_INNER(name)

/**
 * @brief Define hal_xxx_get_device for every HAL driver type and it
 * should be called in hal_xxx.c to define this function
 *
 * HAL_DEFINE_GET_DEVICE(uart) will be :
 * uartDevice *hal_uart_get_device(uint8_t dev_id)
 * {
 *     ...
 * }
 */
#define HAL_DEFINE_GET_DEVICE_INNER(name)                 \
    name##Device *hal_##name##_get_device(uint8_t dev_id) \
    {                                                     \
        if (dev_id >= name##_dev_count) {                 \
            return NULL;                                  \
        }                                                 \
                                                          \
        return name##_dev_list + dev_id;                  \
    }
#define HAL_DEFINE_GET_DEVICE(name) HAL_DEFINE_GET_DEVICE_INNER(name)

/**
 * @brief Define hal_xxx_irq_handler for every HAL driver type and it
 * should be called in hal_xxx.c to define this function
 *
 * HAL_DEFINE_DEV_IRQ_HANDLER(uart) will be :
 * HAL_ISR_SECTION void hal_uart_irq_handler(const uartDevice *dev)
 * {
 *     ...
 * }
 */
#define HAL_DEFINE_DEV_IRQ_HANDLER_INNER(name)                                       \
    HAL_ISR_SECTION void hal_##name##_irq_handler(const HAL_DEVICE_NAME(name) * dev) \
    {                                                                                \
        const HAL_DEVICE_OPS(name) * drv_ops;                                        \
                                                                                     \
        if (!dev) {                                                                  \
            return;                                                                  \
        }                                                                            \
        drv_ops = get_ops(dev);                                                      \
        if (!drv_ops || !drv_ops->irq_handler) {                                     \
            return;                                                                  \
        }                                                                            \
        drv_ops->irq_handler(dev);                                                   \
    }
#define HAL_DEFINE_DEV_IRQ_HANDLER(name) HAL_DEFINE_DEV_IRQ_HANDLER_INNER(name)

/**
 * @brief Define hal_xxx_device_deinit for every HAL driver type and it
 * should be called in hal_xxx.c to define this function
 *
 *
 * HAL_DEFINE_DEVICE_DEINIT(uart) will be :
 * int hal_uart_device_deinit(void)
 * {
 *     ...
 * }
 */
#define HAL_DEFINE_DEVICE_DEINIT_INNER(name)                                  \
    int hal_##name##_device_deinit(void)                                      \
    {                                                                         \
        int ret;                                                               \
        uint8_t i;                                                            \
        const HAL_DEVICE_OPS(name) * drv_ops;                                 \
                                                                              \
        if (!name##_dev_list || !name##_dev_count) {                          \
            return VSD_SUCCESS;                                               \
        }                                                                     \
                                                                              \
        for (i = 0; i < name##_dev_count; ++i) {                              \
            drv_ops = (name##_dev_list + i)->base.ops;                        \
            if (!drv_ops || !(drv_ops->device_deinit)) {                               \
                return VSD_ERR_INVALID_POINTER;                                 \
            }                                                                 \
            ret = drv_ops->device_deinit(name##_dev_list + i);                  \
            if (ret != VSD_SUCCESS) {                                         \
                return ret;                                                   \
            }                                                                  \
        }                                                                     \
                                                                              \
        name##_dev_list  = NULL;                                              \
        name##_dev_count = 0;                                                 \
                                                                              \
        return VSD_SUCCESS;                                                           \
    }
#define HAL_DEFINE_DEVICE_DEINIT(name) HAL_DEFINE_DEVICE_DEINIT_INNER(name)
/** @} */

/** @} */

#endif /* HAL_API_H */
