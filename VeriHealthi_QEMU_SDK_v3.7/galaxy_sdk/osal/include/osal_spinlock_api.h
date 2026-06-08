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

#ifndef OSAL_SPINLOCK_H
#define OSAL_SPINLOCK_H

#include <stdint.h>
#include "osal_define.h"

/**
 * @addtogroup OSAL_SPINLOCK
 * OSAL Spinlock API and definitions
 * @ingroup OSAL
 * @{
 */

typedef struct OsalSpinlock_st OsalSpinlock;

/**
 * @brief Create a spinlock
 *
 * @param spinlock The spinlock being created
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_create_spinlock(OsalSpinlock **spinlock);

/**
 * @brief Delete a spinlock
 *
 * @param spinlock The spinlock being deleted
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_delete_spinlock(OsalSpinlock *spinlock);

/**
 * @brief Unlock spinlock
 *
 * @param spinlock The spinlock to be unlocked
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_unlock_spinlock(OsalSpinlock *spinlock);

/**
 * @brief Lock spinlock
 *
 * @param spinlock The spinlock to be locked
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_lock_spinlock(OsalSpinlock *spinlock);

/** @} */

#endif // OSAL_SPINLOCK_H
