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

#ifndef OSAL_NOTIFY_H
#define OSAL_NOTIFY_H

#include <stdint.h>
#include "osal_define.h"

/**
 * @addtogroup OSAL_NOTIFY
 * OSAL Notify API and definitions
 * @ingroup OSAL
 * @{
 */

typedef struct OsalNotify_st OsalNotify;

/**
 * @struct OsalNotifyWait
 * @brief Struct of parameters for notification waiting
 */
typedef struct OsalNotifyWait_St {
    unsigned long index_to_wait; /**< The index within the calling task's array
                                    of notification values on which the calling
                                    task will wait for a notification to be
                                    received */
    uint32_t bits_to_clr_on_in;  /**< Bit mask to clear on entrying */
    uint32_t bits_to_clr_on_out; /**< Bit mask to clear before exiting */
    uint32_t notify_value;       /**< Notification value to be passed out */
    uint32_t ticks_to_wait;      /**< The maximum time to wait in the blocked state
                                    for a notification to be received */
} OsalNotifyWait;

/**
 * @brief Send a notification to the specific task from task, potentially
 * removing the task from the blocked state, and update updating the task's
 * notification value
 *
 * @param notify Notification sending parameters @see OsalNotify
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_task_notify(OsalNotify *notify);

/**
 * @brief Send a notification to the specific task from ISR
 *
 * @note osal_task_notify_from_isr is the interrupt safe version of
 * osal_task_notify
 * @param notify Notification sending parameters @see OsalNotify
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_task_notify_from_isr(OsalNotify *notify);

/**
 * @brief Wait for the calling task to receive a notification with an optional
 * timeout
 *
 * @param notify_wait Notification waiting parameters @see OsalNotifyWait
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_task_notify_wait(OsalNotifyWait *notify_wait);

/** @} */

#endif // OSAL_NOTIFY_H
