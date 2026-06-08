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

#ifndef OSAL_SYS_STATE_H
#define OSAL_SYS_STATE_H

#include <stdint.h>

/**
 * @addtogroup OSAL_STATE
 * OSAL System State API and definitions
 * @ingroup OSAL
 * @{
 */

/* Macros used to indicate state of task */
#define TASK_RUNNING_CHAR   ('X')
#define TASK_BLOCKED_CHAR   ('B')
#define TASK_READY_CHAR     ('R')
#define TASK_DELETED_CHAR   ('D')
#define TASK_SUSPENDED_CHAR ('S')
/* Macros used to indicate indicate whether the task is alive */
#define TASK_ALIVE_CHAR ('Y')
#define TASK_DEAD_CHAR  ('N')

#define EACH_TASK_INFO_SIZE 64

/**
 * @brief Dump current status of all tasks
 */
void osal_dump_os_state(void);

/**
 * @brief Dump current status of system heap
 */
void osal_dump_heap_size(void);

/**
 * @brief Get current task name
 *
 * @return task name
 */
char *osal_get_task_name(void);

/** @} */

#endif // OSAL_SYS_STATE_H
