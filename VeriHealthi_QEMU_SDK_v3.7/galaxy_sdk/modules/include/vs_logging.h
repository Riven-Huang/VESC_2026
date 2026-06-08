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

#ifndef VS_LOGGING_H
#define VS_LOGGING_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include "vs_conf.h"
#include "uart_printf.h"

/**
 * @addtogroup MODULE
 * @{
 * @addtogroup MODULE_LOGGING
 * @{
 */

/**
 * @addtogroup LOGGING_VS
 * logging API and definition.
 * @{
 */

#define LOG_LVL_NONE     0
#define LOG_LVL_ERROR    1
#define LOG_LVL_CRITICAL 2
#define LOG_LVL_WARNING  3
#define LOG_LVL_INFO     4
#define LOG_LVL_DEBUG    5

extern int vs_logging_level;

#if CONFIG_USING_HOST_CONSOLE
#if CONFIG_HOST_CONSOLE_VERBOSE
#include <time.h>
#include <sys/time.h>
#define vs_logging_inner(level, fmt, ...)                                                         \
    do {                                                                                          \
        struct tm st;                                                                             \
        struct timeval tv;                                                                        \
        gettimeofday(&tv, NULL);                                                                  \
        localtime_r(&tv.tv_sec, &st);                                                             \
        if ((level) <= vs_logging_level) {                                                        \
            if ((level) == LOG_LVL_ERROR) {                                                       \
                printf("\033[31m[%02d:%02d:%02d.%06lu]\033[0m", st.tm_hour, st.tm_min, st.tm_sec, \
                       tv.tv_usec);                                                               \
                printf("\033[31m" fmt "\033[0m", ##__VA_ARGS__);                                  \
            } else if ((level) == LOG_LVL_CRITICAL) {                                             \
                printf("[%02d:%02d:%02d.%06lu]", st.tm_hour, st.tm_min, st.tm_sec, tv.tv_usec);   \
                printf(fmt, ##__VA_ARGS__);                                                       \
            } else if ((level) == LOG_LVL_WARNING) {                                              \
                printf("\033[33m[%02d:%02d:%02d.%06lu]\033[0m", st.tm_hour, st.tm_min, st.tm_sec, \
                       tv.tv_usec);                                                               \
                printf("\033[33m" fmt "\033[0m", ##__VA_ARGS__);                                  \
            } else {                                                                              \
                printf("[%02d:%02d:%02d.%06lu]", st.tm_hour, st.tm_min, st.tm_sec, tv.tv_usec);   \
                printf(fmt, ##__VA_ARGS__);                                                       \
            }                                                                                     \
            printf("\r\n");                                                                       \
        }                                                                                         \
    } while (0)
#else
#define vs_logging_inner(level, fmt, ...)  \
    do {                                   \
        if ((level) <= vs_logging_level) { \
            printf(fmt, ##__VA_ARGS__);    \
            printf("\r\n");                \
        }                                  \
    } while (0)
#endif
#else
#define vs_logging_inner(level, fmt, ...)    \
    do {                                     \
        if ((level) <= vs_logging_level) {   \
            uart_printf(fmt, ##__VA_ARGS__); \
        }                                    \
    } while (0)
#endif

#define logging_none(level, fmt, ...)

#define vs_log_LOG_LVL_DEBUG    logging_none
#define vs_log_LOG_LVL_INFO     logging_none
#define vs_log_LOG_LVL_WARNING  logging_none
#define vs_log_LOG_LVL_CRITICAL logging_none
#define vs_log_LOG_LVL_ERROR    logging_none
#define vs_log_LOG_LVL_NONE     logging_none

#define VS_LOG_LEVEL CONFIG_LOG_LEVEL

#if (VS_LOG_LEVEL >= LOG_LVL_ERROR)
#undef vs_log_LOG_LVL_ERROR
#define vs_log_LOG_LVL_ERROR vs_logging_inner
#endif

#if (VS_LOG_LEVEL >= LOG_LVL_CRITICAL)
#undef vs_log_LOG_LVL_CRITICAL
#define vs_log_LOG_LVL_CRITICAL vs_logging_inner
#endif

#if (VS_LOG_LEVEL >= LOG_LVL_WARNING)
#undef vs_log_LOG_LVL_WARNING
#define vs_log_LOG_LVL_WARNING vs_logging_inner
#endif

#if (VS_LOG_LEVEL >= LOG_LVL_INFO)
#undef vs_log_LOG_LVL_INFO
#define vs_log_LOG_LVL_INFO vs_logging_inner
#endif

#if (VS_LOG_LEVEL >= LOG_LVL_DEBUG)
#undef vs_log_LOG_LVL_DEBUG
#define vs_log_LOG_LVL_DEBUG vs_logging_inner
#endif

#define vs_logging(level, fmt, ...) vs_log_##level(level, fmt, ##__VA_ARGS__)

/**
 * @brief Set logging level
 * @param log_level log level
 */
void set_logging_level(int log_level);

/**
 * @brief print to memory
 * @param args log string
 */
void mem_printf(char *args, ...);

/**
 * @brief get print memory pointer
 * @return  memory pointer: on success,  NULL : error
 */
char *get_log_buffer(void);

/** @} */

/** @} */

/** @} */

#endif // VS_LOGGING_H
