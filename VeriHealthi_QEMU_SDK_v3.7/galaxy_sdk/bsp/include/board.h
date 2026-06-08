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

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include "device.h"
#include "soc_pin_define.h"

/**
 * @addtogroup BSP_BOARD
 * Board API and definition
 * @ingroup BSP
 * @{
 */

/**
 * @brief Structure of board handler
 */
typedef struct BoardDevice_st {
    const char *name;  /**< Name of board */
    uint32_t reserved; /**< Reserved */
    void *priv;        /**< Private data */
} BoardDevice;

/**
 * @brief Board device Info
 */
typedef union BoardDeviceInfo_un {
    uint8_t device_id;
    /** PWM info struct */
    struct {
        uint8_t channel;
    } pwm;
} BoardDeviceInfo;

/**
 * @brief Structure of operations for board
 */
typedef struct BoardOperations_st {
    int (*init)(BoardDevice *board);
    void *(*find_device)(uint8_t device_id);
    void (*get_wakeup_src)(uint8_t port, uint8_t *wakeup_src);
} BoardOperations;

#define GPIO_PORT_INIT(pin_val, trigger_val, type_val, reload_val, invert_val) \
    {.pin_id     = (pin_val),                                                  \
     .group      = GET_GPIO_GROUP_ID(pin_val),                                 \
     .port       = PIN_TO_PORT_ID(pin_val),                                    \
     .trigger    = (trigger_val),                                              \
     .type       = (type_val),                                                 \
     .irq_reload = (reload_val),                                               \
     .invert     = (invert_val)}

/**
 * @brief Get board ops
 * @return Return result
 * @retval Board ops
 */
const BoardOperations *board_get_ops(void);

/**
 * @brief Register the board
 * @param ops Operations for board
 * @note This api must be called before all of board api
 * @return Return result
 * @retval VSD_SUCCESS on succeed, others on failure
 */
int board_register(const BoardOperations *ops);

/**
 * @brief Init the board
 * @param board_cfg Configuration of board
 * @return Return result
 * @retval VSD_SUCCESS on succeed, others on failure
 */
int board_init(void *board_cfg);

/**
 * @brief Get device from board
 * @param device_id ID of device
 * @return Return result
 * @retval Pointor to device which was found, NULL for no device
 */
void *board_find_device_by_id(uint8_t device_id);

/**
 * @brief Get wakeup source from board
 * @param port GPIO port id of power key
 * @param wakeup_src Wake up source from deepsleep mode
 */
void get_wakeup_src(uint8_t port, uint8_t *wakeup_src);

/** @} */

#endif // BOARD_H
