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

#ifndef OSAL_SEMAPHORE_H
#define OSAL_SEMAPHORE_H

#include <stdint.h>
#include "osal_define.h"

/**
 * @addtogroup OSAL_SEMAPHORE
 * OSAL Semaphore API and definitions
 * @ingroup OSAL
 * @{
 */

typedef struct OsalSemaphore_st OsalSemaphore;

/**
 * @brief Create a binary semaphore
 *
 * @param sem The semaphore being created
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_create_sem(OsalSemaphore **sem);

/**
 * @brief Create a counting semaphore
 *
 * @param sem The semaphore being created
 * @param max_count The semaphore max counting value
 * @param init_count The semaphore init counting value
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_create_counting_sem(OsalSemaphore **sem, uint8_t max_count, uint8_t init_count);

/**
 * @brief Delete a binary/counting semaphore
 *
 * @note Do not delete a semaphore that has tasks blocked on it (tasks that are
 * in the blocked state waiting for the semaphore to become available)
 * @param sem The semaphore being deleted
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_delete_sem(OsalSemaphore *sem);

/**
 * @brief Wait for or take a semaphore
 *
 * @note The semaphore can be taken only if it is available. This function must
 * not be used from an interrupt service routine.
 * @param sem The semaphore being taking
 * @param timeout The maximum amount of time the task should remain in the
 * blocked state to wait for the semaphore if it is not already available, in
 * millisecond
 * 1) If timeout is zero, then osal_sem_wait will return immediately if the
 * semaphore is not available.
 * 2) Setting timeout to OSAL_WAIT_FOREVER will cause the task to wait
 * indefinitely (without a timeout)
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_sem_wait(OsalSemaphore *sem, uint32_t timeout);

/**
 * @brief Post or give a semaphore
 *
 * @param sem The semaphore being given
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_sem_post(OsalSemaphore *sem);

/**
 * @brief Post or give a semaphore from ISR
 *
 * @note osal_sem_post_isr is the interrupt safe version of osal_sem_post
 * @param sem The semaphore being given
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_sem_post_isr(OsalSemaphore *sem);

/** @} */

#endif // OSAL_SEMAPHORE_H
