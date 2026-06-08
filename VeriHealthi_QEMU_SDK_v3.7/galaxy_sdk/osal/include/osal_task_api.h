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

#ifndef OSAL_TASK_H
#define OSAL_TASK_H

#include <stdint.h>
#include <stdbool.h>
#include "osal_define.h"

/** @addtogroup OSAL_TASK
 *  OSAL Task API and definitions
 *  @ingroup OSAL
 *  @{
 */

typedef void (*TaskHookCb)(void);

/**
 * @brief Create a task
 *
 * @param func A pointer to the function that implements the task
 * @param name A descriptive name for the task
 * @param stack_size The number of words the stack can hold, not the number of
 * bytes
 * @param task_priority The priority at which the task will execute
 * @param param A parameter for task function
 * @return void* Task handle for success, NULL for failure
 */
void *osal_create_task(void *func, char *name, uint32_t stack_size, uint32_t task_priority,
                       void *param);

/**
 * @brief Delete the specific task
 *
 * @param task Task handle
 */
void osal_delete_task(void *task);

/**
 * @brief Suspend a task
 *
 * @param task Task handle
 */
void osal_suspend_task(void *task);

/**
 * @brief Suspend the scheduler
 *
 * @note Suspending the scheduler prevents a context switch from occurring, but
 * leaves interrupts enabled
 * @note For FreeRTOS, API functions must not be called while the scheduler is
 * suspended
 */
void osal_suspend_all(void);

/**
 * @brief Resume a task
 *
 * @param task Task handle
 */
void osal_resume_task(void *task);

/**
 * @brief Resume the scheduler
 */
void osal_resume_all(void);

/**
 * @brief Place the calling task into the sleep(aka blocked) state until the
 * sleep period has expired
 *
 * @param ms Sleep period, in miniseconds
 */
void osal_sleep(int32_t ms);

/**
 * @brief do something before start scheduler
 */
void osal_pre_start_scheduler(void);

/**
 * @brief Start the scheduler
 */
void osal_start_scheduler(void);

/**
 * @brief End the scheduler
 */
void osal_end_scheduler(void);

/**
 * @brief Get the state of scheduler
 *
 * @return true Started
 * @return false Stopped
 */
bool osal_started(void);

/**
 * @brief Enter critical sections
 */
void osal_enter_critical(void);

/**
 * @brief Exit critical sections
 */
void osal_exit_critical(void);

/**
 * @brief Get os highest priority
 *
 * @return the highest priority of os
 */
int osal_highest_priority(void);

/**
 * @brief Set hook function for idle task

 * @param cb the idle hook function
 */
void osal_set_idle_hook(TaskHookCb cb);

/**
 * @brief Get os current task handle
 *
 * @return the current task handle
 */
void *osal_get_current_task_handle(void);

/**
 * @brief task yield
 */
void osal_task_yield(void);

/** @} */

#endif // OSAL_TASK_H
