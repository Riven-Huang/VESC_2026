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

#ifndef UART_PRINTF_H
#define UART_PRINTF_H

/**
 * @addtogroup MODULE
 * @{
 * @addtogroup MODULE_LOGGING
 * @{
 */

/**
 * @addtogroup LOGGING_UART
 * uart printf API and definition.
 * @{
 */

/*Check your function's parameter a & b by following function f's rule*/
#define CHECK_PARAM_FORMAT(f, a, b) __attribute__((__format__(f, a, b)))

/**
 * @brief Print string to UART
 * @param format C string that contains the text to be written to UART
 * @return On success, total number of characters written is returned
 */
int uart_printf(const char *format, ...) CHECK_PARAM_FORMAT(printf, 1, 2);
int uart_sprintf(char *out, const char *format, ...) CHECK_PARAM_FORMAT(printf, 2, 3);

/**
 * @brief API to init the UART interface for debug
 * @return return 0 for success
 */
int uart_debug_init(void);

/** @} */

/** @} */

/** @} */

#endif // UART_PRINTF_H
