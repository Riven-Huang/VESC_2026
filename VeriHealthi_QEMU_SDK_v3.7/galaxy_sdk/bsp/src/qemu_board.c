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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "vs_conf.h"
#include "qemu_board.h"
#include "sys_common.h"
#include "vsd_error.h"
#include "bsp_common.h"
#include "hal_imu.h"
#include "imu_simulator.h"

static void *device_list[MAX_DEVICE_ID];

static const ImuHwConfig imu_sensor_config = {
    .power_pin = NULL,
    .data_pin  = NULL,
    .wake_pin  = NULL,
};

static ImuConfig imu_cfg = {
    .device_id = IMU_DEV_ID_0,
    .bus =
        {
            .bus_device =
                {
                    .type    = BUS_TYPE_I2C,
                    .port_id = 0,
                    .i2c     = NULL,
                },
            .bus_config =
                {
                    .i2c = NULL,
                },
        },
};

static ImuDevice imu_dev_list[] = {
    {
        .base =
            {
                .ops = (void *)&imu_simulator_ops,
                .ctx = NULL,
            },
        .cfg        = &imu_cfg,
        .flag       = 0x38,
        .capability = 0,
        .fifo_buff  = NULL,
        .hw_cfg     = &imu_sensor_config,
    },
};

static inline int create_imu_device(void)
{
    return hal_imu_device_init(imu_dev_list, ARRAY_SIZE(imu_dev_list));
}

NON_XIP_TEXT
static int qemu_board_init(BoardDevice *board)
{
    int ret;

    board->name = qemu_board_name;
    ret         = create_imu_device();

    return ret;
}

void *qemu_board_find_device(uint8_t device_id)
{
    return device_list[device_id];
}

const BoardOperations qemu_board_ops = {
    .init        = qemu_board_init,
    .find_device = qemu_board_find_device,
};

const BoardOperations *board_get_ops(void)
{
    return &qemu_board_ops;
}
