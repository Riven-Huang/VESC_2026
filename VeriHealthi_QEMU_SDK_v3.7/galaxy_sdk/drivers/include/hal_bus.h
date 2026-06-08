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

#ifndef HAL_BUS_H
#define HAL_BUS_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @addtogroup HAL_BUS
 * Bus HAL API and definition
 * @ingroup DRIVER_HAL
 * Hardware Abstraction Layer
 * @{
 */

/**
 * @brief SPI device struct declaration
 */
typedef struct SpiDevice_st SpiDevice;

/**
 * @brief I2C device struct declaration
 */
typedef struct I2cDevice_st I2cDevice;

/**
 * @brief I3C device struct declaration
 */
typedef struct I3cDevice_st I3cDevice;

/**
 * @brief SPI config struct declaration
 */
typedef struct SpiConfig_st SpiConfig;

/**
 * @brief I2C config struct declaration
 */
typedef struct I2cXferConfig_st I2cXferConfig;

/**
 * @brief I3C config struct declaration
 */
typedef struct I3cConfig_st I3cConfig;

typedef enum BusType {
    BUS_TYPE_I2C,
    BUS_TYPE_SPI,
    BUS_TYPE_I3C,
    BUS_TYPE_MAX,
} BusType;

/**
 * @struct BusDevice
 * @brief Define bus device struct
 */
typedef struct BusDevice_st {
    BusType type;    /**< Bus type, @see BusType */
    uint8_t port_id; /**< The port id of an interface */
    union {
        const SpiDevice *spi; /**< Handle of SPI device */
        const I2cDevice *i2c; /**< Handle of I2C device */
        const I3cDevice *i3c; /**< Handle of I3C device */
    };
} BusDevice;

/**
 * @struct BusConfig
 * @brief Define bus configuration struct
 */
typedef union BusConfig_un {
    const SpiConfig *spi;     /**< SPI port configuration */
    const I2cXferConfig *i2c; /**< I2C port configuration */
    const I3cConfig *i3c;     /**< I3C port configuration */
} BusConfig;

typedef struct DeviceBus_st {
    BusDevice bus_device;
    BusConfig bus_config;
} DeviceBus;

typedef struct DeviceConfig_st {
    uint8_t device_id; /**< Device id for multi instance, definition see
                            DevId on each sensor hal */
    DeviceBus bus;
} DeviceConfig;

/** @} */

#endif // HAL_BUS_H
