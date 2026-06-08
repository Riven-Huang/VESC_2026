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

#ifndef OSAL_TIME_H
#define OSAL_TIME_H

#include <stdint.h>
#include "osal_define.h"

/**
 * @addtogroup OSAL_TIME
 * OSAL Time API and definitions
 * @ingroup OSAL
 * @{
 */

/**
 * @brief Get actual time a tick represents
 * @return a tick period in milliseconds
 */
uint64_t osal_tick_period_ms(void);

/**
 * @brief Get the number of milliseconds since the operating system(or
 * scheduler) started
 *
 * @note Invocation is not recommended when the SysTimer interrupt is masked.
 * For example, after entering the critical section through
 * osal_enter_critical(). This causes the RTOS to stop tick counting, which
 * affects the accuracy of the results returned by the function.
 */
uint64_t osal_get_uptime(void);

/**
 * @brief Setup the systick timer to generate the tick interrupts at the
 * required frequency
 */
void osal_setup_timer(void);

/**
 * @brief Update the systick timer configurations to ensure accurate timing when
 * the operating system's base clock changes
 */
void osal_update_systick(void);

/**
 * @brief Convert time in milliseconds to ticks
 *
 * @note A fraction less then one tick will be counted as one tick
 * @param duration A period of time in milliseconds
 * @return uint32_t Number of ticks
 */
uint32_t osal_ms_to_tick(uint32_t duration);

/** @} */

#endif // OSAL_TIME_H
