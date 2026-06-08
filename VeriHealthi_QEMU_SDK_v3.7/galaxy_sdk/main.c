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

#include <stddef.h>
#include "vs_conf.h"
#include "soc_init.h"
#include "soc_sysctl.h"
#include "bsp.h"
#include "uart_printf.h"
#include "board.h"
#include "osal.h"
#include "app/algo_task.h"
#include "app/imu_pipeline.h"
#include "vsd_error.h"
#include "vpi_error.h"
#include "main.h"

static void task_init_app(void *param)
{
    int ret;
    BoardDevice board_dev;

    ret = board_register(board_get_ops());
    ret = vsd_to_vpi(ret);
    if (ret != VPI_SUCCESS) {
        uart_printf("board register failed %d", ret);
        goto exit;
    }
    ret = board_init((void *)&board_dev);
    ret = vsd_to_vpi(ret);
    if (ret != VPI_SUCCESS) {
        uart_printf("board init failed %d", ret);
        goto exit;
    }
    if (board_dev.name) {
        uart_printf("Board: %s", board_dev.name);
    }

    uart_printf("Hello VeriHealthi!\r\n");

    ret = init_imu_pipeline();
    if (ret != VSD_SUCCESS) {
        goto exit;
    }

    osal_create_task(task_imu, "imu_task", 1024, 4, NULL);
    osal_create_task(task_algo, "algo_task", 1024, 3, NULL);
exit:
    osal_delete_task(NULL);
}

int main(void)
{
    int ret;

    ret = soc_init();
    ret = vsd_to_vpi(ret);
    if (ret != VPI_SUCCESS) {
        uart_printf("soc init error %d", ret);
        goto exit;
    } else {
        uart_printf("soc init done");
    }
    osal_pre_start_scheduler();
    osal_create_task(task_init_app, "init_app", 512, 1, NULL);
    osal_start_scheduler();
exit:
    goto exit;
}
