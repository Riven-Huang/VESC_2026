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

#ifndef OSAL_TIMER_H
#define OSAL_TIMER_H

#include <stdint.h>
#include <stdbool.h>
#include "osal_define.h"

/**
 * @addtogroup OSAL_TIMER
 * OSAL Timer API and definitions
 * @ingroup OSAL
 * @{
 */

typedef struct OsalTimer_st OsalTimer;

/**
 * @brief OSAL timer timeout callback function
 * @param timer The timer itself
 * @param param The param of timer timeout callback
 */
typedef void (*OsalTimerCallback)(OsalTimer *timer, void *param);

/**
 * @brief Create a timer
 * @param timer The timer to be created
 * @param name The name of timer
 * @param period The period of timer, in millisecond
 * @param auto_reload Is the count value automatically reloaded
 * @param cb The timeout callback of timer
 * @param param The param of timer timeout callback
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_timer_create(OsalTimer **timer, char *name, int period, bool auto_reload,
                      OsalTimerCallback cb, void *param);

/**
 * @brief Delete a timer
 *
 * @param timer The timer being deleted
 * @param timeout The maximum amount of wait time, in millisecond
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_timer_delete(OsalTimer *timer, uint32_t timeout);

/**
 * @brief Changed a timer period
 *
 * @param timer The timer being changed
 * @param period The new period of timer, in millisecond
 * @param timeout The maximum amount of wait time, in millisecond
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_timer_change_period(OsalTimer *timer, uint32_t period, uint32_t timeout);

/**
 * @brief Changed a timer period from interrupt
 *
 * @param timer The timer being changed
 * @param period The new period of timer, in millisecond
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_timer_change_period_isr(OsalTimer *timer, uint32_t period);

/**
 * @brief Start a timer
 *
 * @param timer The timer being started
 * @param timeout The maximum amount of wait time, in millisecond
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_timer_start(OsalTimer *timer, uint32_t timeout);

/**
 * @brief Start a timer from interrupt
 *
 * @param timer The timer being started
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_timer_start_isr(OsalTimer *timer);

/**
 * @brief Reset a timer
 *
 * @param timer The timer being reset
 * @param timeout The maximum amount of wait time, in millisecond
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_timer_reset(OsalTimer *timer, uint32_t timeout);

/**
 * @brief Reset a timer from interrupt
 *
 * @param timer The timer being reset
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_timer_reset_isr(OsalTimer *timer);

/**
 * @brief Stop a timer
 *
 * @param timer The timer being stopped
 * @param timeout The maximum amount of wait time, in millisecond
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_timer_stop(OsalTimer *timer, uint32_t timeout);

/**
 * @brief Stop a timer from interrupt
 *
 * @param timer The timer being stopped
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_timer_stop_isr(OsalTimer *timer);

/** @} */

#endif // OSAL_TIMER_H
