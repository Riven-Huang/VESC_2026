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

#ifndef BASE_DEVICE_H
#define BASE_DEVICE_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @ingroup DRIVER
 * base device definitions
 * @addtogroup DRIVER_BASE_DEV
 * @{
 */

typedef struct BaseDevice_st {
    uintptr_t base_addr;   /**< base address of register region */
    const uint8_t *clocks; /**< pointer of clock map list */
    const uint8_t *irqs;   /**< pointer of interrupt map list */
    const void *ops;       /**< operations of one device */
    void *ctx;             /**< context of one device */
    const void *hw_ext;    /**< hardware extension */
    uint8_t clock_count;   /**< how many clock's map in list */
    uint8_t irq_count;     /**< how many interrupt's map in list */
    uint8_t pwr_id;        /**< power id */
    uint8_t rst_id;        /**< reset id */
} BaseDevice;

/** @} */

#endif // BASE_DEVICE_H
