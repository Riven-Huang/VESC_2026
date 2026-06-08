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

#ifndef HAL_COMMON_H
#define HAL_COMMON_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * @ingroup DRIVER
 * @addtogroup DRIVER_HAL
 * @{
 */

/**
 * @addtogroup HAL_COMMON
 * Hardware Abstraction Layer Common
 * @{
 */

#ifndef BIT
#define BIT(b) (1UL << (b))
#endif

/** HAL interrupt service routine section */
#define HAL_ISR_SECTION __attribute__((section(".hal_isr")))

/** Define the wait forever timeout macro */
#define HAL_WAIT_FOREVER 0xFFFFFFFFU

/**
 * @brief Callback handler for transfer complete
 * @param param Parameter of callback API
 */
typedef void (*XferCallback)(void *param);

/**
 * @enum DeviceProperty
 * @brief Device property, indicate it's a HW or SW device
 */
typedef enum DeviceProperty {
    DEVICE_PROP_HW, /**< A hardware device */
    DEVICE_PROP_SW, /**< A software device */
} DeviceProperty;

/**
 * @enum DeviceWorkMode
 * @brief Device work mode, master or slave
 */
typedef enum DeviceWorkMode {
    DEVICE_MASTER_MODE, /**< Device works on master mode */
    DEVICE_SLAVE_MODE,  /**< Device works on slave mode */
} DeviceWorkMode;

/**
 * @enum XferModeDef
 * @brief Define common transfer modes
 */
typedef enum XferModeDef {
    XFER_MODE_POLLING = 0, /**< Polling mode */
    XFER_MODE_INTR    = 1, /**< Interrupt mode */
    XFER_MODE_DMA     = 2, /**< DMA mode */
    XFER_MODE_MAX,
    XFER_MODE_DEFAULT = XFER_MODE_MAX
} XferModeDef;

/**
 * @enum XferCapability
 * @brief Define the capability of transfer modes
 */
typedef enum XferCapability {
    XFER_CAP_POLLING = (0x1 << XFER_MODE_POLLING), /**< Capability of polling mode */
    XFER_CAP_INTR    = (0x1 << XFER_MODE_INTR),    /**< Capability of interrupt mode */
    XFER_CAP_DMA     = (0x1 << XFER_MODE_DMA),     /**< Capability of DMA mode */
} XferCapability;

/**
 * @enum FifoWidthDef
 * @brief Define common fifo width
 */
typedef enum FifoWidthDef {
    WIDTH_8_BITS_TYPE  = 0, /**< FIFO width is 8 bits */
    WIDTH_16_BITS_TYPE = 1, /**< FIFO width is 16 bits */
    WIDTH_32_BITS_TYPE = 2, /**< FIFO width is 32 bits */
    WIDTH_64_BITS_TYPE = 3, /**< FIFO width is 64 bits */
    WIDTH_TYPE_MAX,
    WIDTH_TYPE_DEFAULT = WIDTH_TYPE_MAX
} FifoWidthDef;

/**
 * @enum DmacWidthCapability
 * @brief Define the transfer width capability of DMAC
 */
typedef enum DmacWidthCapability {
    WIDTH_CAP_8_BIT  = (0X1 << WIDTH_8_BITS_TYPE),  /**< DMA supports 8bit read/write */
    WIDTH_CAP_16_BIT = (0X1 << WIDTH_16_BITS_TYPE), /**< DMA supports 16bit read/write */
    WIDTH_CAP_32_BIT = (0X1 << WIDTH_32_BITS_TYPE), /**< DMA supports 32bit read/write */
    WIDTH_CAP_64_BIT = (0X1 << WIDTH_64_BITS_TYPE), /**< DMA supports 64bit read/write */
} DmacWidthCapability;

typedef struct XferInfo_st {
    XferCallback callback; /**< Callback handler for transfer complete */
    bool is_async;         /**< Use asynchronous or synchronous mode */
} XferInfo;

typedef struct HalBuffer_st {
    void *mem;     /**< Buffer handle */
    uint8_t *addr; /**< Buffer address */
    uint32_t size; /**< Size of buffer in byte */
} HalBuffer;

typedef struct HalKernelBuffer_st {
    int fd;           /**< The buffer file descriptor */
    int index;        /**< The buffer index */
    void *vir_addr;   /**< The buffer virtual address */
    void *phy_addr;   /**< The buffer physical address */
    uint32_t bf_sz;   /**< The buffer size in byte */
    uint32_t data_sz; /**< The valid data size in byte */
} HalKernelBuffer;

/** @} */

/** @} */

#endif // HAL_COMMON_H
