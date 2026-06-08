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

#ifndef OSAL_LOCK_H
#define OSAL_LOCK_H

#include <stdint.h>
#include "osal_define.h"

/**
 * @addtogroup OSAL_LOCK
 * OSAL Lock API and definitions
 * @ingroup OSAL
 * @{
 */

typedef struct OsalMutex_st OsalMutex;

/**
 * @brief Create a mutex
 *
 * @param mu The mutex being created
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_create_mutex(OsalMutex **mu);

/**
 * @brief Delete a mutex
 *
 * @note Do not delete a mutex that has tasks blocked on it (tasks that are
 * in the blocked state waiting for the mutex to become available)
 * @param mu The mutex being deleted
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_delete_mutex(OsalMutex *mu);

/**
 * @brief Unlock or release a mutex
 *
 * @param mu The mutex to be unlocked
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_unlock_mutex(OsalMutex *mu);

/**
 * @brief Lock or take a mutex
 *
 * @param mu The mutex to be locked
 * @param timeout The maximum amount of time the task should remain in the
 * blocked state to wait for the mutex if it is not already available, in
 * millisecond.
 * 1) If timeout is zero, then osal_lock_mutex will return immediately if the
 * mutex is not available.
 * 2) Setting timeout to OSAL_WAIT_FOREVER will cause the task to wait
 * indefinitely (without a timeout)
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_lock_mutex(OsalMutex *mu, uint32_t timeout);

/** @} */

#endif // OSAL_LOCK_H
