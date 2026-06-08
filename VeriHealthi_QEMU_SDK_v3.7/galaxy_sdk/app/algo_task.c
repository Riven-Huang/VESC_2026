/*
 * Copyright (c) 2026, VeriSilicon Holdings Co., Ltd. All rights reserved
 */

#include "algo_task.h"

#include <stdbool.h>
#include <stdint.h>
#include "gesture_algo.h"
#include "imu_sample.h"
#include "osal.h"
#include "uart_printf.h"
#include "vpi_event.h"

static volatile bool g_algo_event_ready;

bool algo_task_is_ready(void)
{
    return g_algo_event_ready;
}

static int handle_algo_event(EventManager manager, EventId event_id, EventParam param)
{
    GestureResult result;
    ImuSampleMessage *msg = (ImuSampleMessage *)param;

    (void)manager;

    if (event_id != EVENT_SEN_DATA_READY || !msg) {
        return EVENT_OK;
    }

    if (gesture_algo_process_window(msg, &result)) {
        uart_printf("%dms, %s\r\n",
                    (int)result.timestamp_ms,
                    gesture_algo_name(result.type));
    }

    return EVENT_OK;
}

void task_algo(void *param)
{
    EventManager algo_manager;

    (void)param;

    gesture_algo_init();

    algo_manager = vpi_event_new_manager(EVENT_MGR_ALGO, handle_algo_event);
    if (!algo_manager) {
        uart_printf("create algo event manager failed\r\n");
        goto exit;
    }

    if (vpi_event_register(EVENT_SEN_DATA_READY, algo_manager) != EVENT_OK) {
        uart_printf("register algo event failed\r\n");
        goto exit;
    }

    g_algo_event_ready = true;
    while (1) {
        vpi_event_listen(algo_manager);
    }

exit:
    osal_delete_task(NULL);
}
