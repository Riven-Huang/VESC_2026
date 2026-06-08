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

#ifndef BSP_H
#define BSP_H

#include <stdint.h>

/**
 * @addtogroup BSP
 * Board Software Package
 * @{
 */

/**
 * @brief Delay us
 * @param us Delay time.
 */
void delay_us(uint32_t us);

/**
 * @brief Delay ms
 * @param ms Delay time.
 */
void delay_ms(uint32_t ms);

/**
 * @brief Get the number of microseconds since the system started
 *
 * @return uptime in microseconds
 */
uint64_t bsp_get_uptime_us(void);

/** @} */

#endif // BSP_H
