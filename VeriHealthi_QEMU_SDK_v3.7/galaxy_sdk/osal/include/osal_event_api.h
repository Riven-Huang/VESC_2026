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

#ifndef OSAL_EVENT_H
#define OSAL_EVENT_H

#include <stdint.h>
#include "vpi_list.h"
#include "osal_define.h"

/**
 * @addtogroup OSAL_EVENT
 * OSAL Event API and definitions
 * @ingroup OSAL
 * @{
 */

typedef struct OsalEvent_st {
    uint32_t event_id;
    void *data;
} OsalEvent;

/**
 * @struct OsalEventEntry
 * @brief Event item in event system
 */
typedef struct OsalEventEntry_st {
    ListHead event_entry; /**< List hook */
    OsalEvent event_data; /**< Event */
    char *owner_name;
} OsalEventEntry;

/**
 * @brief Create an event queue and and return a handle to it for reference in
 * event system
 *
 * @param size Size of event queue
 * @param item_size Size of each event queue item
 * @return void* Queue handle is for success, NULL is for failure
 */
void *osal_create_event_queue(int size, uint32_t item_size);

/**
 * @brief Delete an event queue
 *
 * @param queue A handle to the queue to be deleted
 * @return int OSAL_SUCCESS for success, others for failure
 */
int osal_delete_event_queue(void *queue);

/**
 * @brief Send an event to the specific event queue
 *
 * @param queue Event queue handle
 * @param event_id Event ID
 * @param data Event data that needs to be sent
 * @param timeout Timeout time, in milliseconds
 * @return int OSAL_SUCCESS is for success, OSAL_ERROR is for failure
 */
int osal_send_event(void *queue, uint32_t event_id, void *data, uint32_t timeout);

/**
 * @brief Send an event to the specific event queue in isr
 *
 * @param queue Event queue handle
 * @param event_id Event ID
 * @param data Event data that needs to be sent
 * @param timeout Timeout time, in milliseconds
 * @return int OSAL_SUCCESS is for success, OSAL_ERROR is for failure
 */
int osal_send_event_from_isr(void *queue, uint32_t event_id, void *data, uint32_t timeout);

/**
 * @brief Wait for event data from the specific event queue
 *
 * @param queue Event queue handle
 * @param data Event data that needs to be received
 * @param timeout The maximum amount of time the task should remain in the
 * blocked state to wait for data to become available on the queue, in
 * milliseconds.
 * 1) If timeout is zero, then osal_wait_event will return immediately if the
 * queue is already empty
 * 2) Setting timeout to OSAL_WAIT_FOREVER will cause the
 * task to wait indefinitely (without a timeout)
 * @return int Event ID
 */
int osal_wait_event(void *queue, void **data, uint32_t timeout);

/**
 * @brief Get free buffer length of event pool
 *
 * @return uint32_t Free buffer length
 */
uint32_t osal_get_event_pool_length(void);

/** @} */

#endif // OSAL_EVENT_H
