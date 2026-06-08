/** @file
 *  @brief Bluetooth subsystem logging helpers.
 */

/*
 * Copyright (C) 2025. VeriSilicon Holdings Co., Ltd. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __VSBT_LOG_H
#define __VSBT_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * @defgroup vsbt_log Vendor Logging Interface
 * @brief Vendor-specific logging interface for Bluetooth subsystem
 * @{
 */

/**
 * @brief Logging level enumeration
 * @details Defines different logging levels for controlling verbosity
 */
typedef enum VSBT_LOG_LEVEL {
    VSBT_LOG_LEVEL_NONE,    /**< No logging */
    VSBT_LOG_LEVEL_ERR,     /**< Error level logging */
    VSBT_LOG_LEVEL_WRN,     /**< Warning level logging */
    VSBT_LOG_LEVEL_INF,     /**< Informational level logging */
    VSBT_LOG_LEVEL_DBG,     /**< Debug level logging */
} VSBT_LOG_LEVEL;

/**
 * @brief Print a formatted log message
 * @details This function prints a formatted log message with the specified logging level.
 *          If the logging level is below the currently set level, the message is not printed.
 *
 * @param log_lvl Logging level for this message
 * @param format C string that contains the text to be written, followed by additional arguments
 * @return On success, returns the number of characters written; -1 if the logging level is below the set level
 */
int vsbt_log_printf(VSBT_LOG_LEVEL log_lvl, const char *format, ...);

/**
 * @brief Print the name of the currently running task
 * @details Displays the name of the currently executing task for debugging purposes.
 *
 * @param log_lvl Logging level for this operation
 */
void vsbt_log_show_task(VSBT_LOG_LEVEL log_lvl);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
