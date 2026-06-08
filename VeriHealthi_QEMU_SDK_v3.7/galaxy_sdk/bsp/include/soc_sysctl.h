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

#ifndef SOC_SYSCTL_H
#define SOC_SYSCTL_H

#include <stdint.h>
#include <stdbool.h>
#include "bsp.h"

/**
 * @ingroup BSP_SOC
 * @addtogroup SOC_PEGASUS
 * @{
 */

/**
 * @addtogroup PEGASUS_SYSCTL
 * System Ctrl APIs
 * @{
 */

/**
 * @brief SOC software reset
 */
void soc_sw_reset(void);

/**
 * @brief Software reset SOC by wdt
 */
void soc_sw_reset_wdt(void);

/**
 * @brief Software stop ZSP
 * @return VSD_SUCCESS on success, others on error
 */
int soc_sw_stop_zsp(void);

/**
 * @brief Software load ZSP
 * @return VSD_SUCCESS on success, others on error
 */
int soc_sw_load_zsp(void);

/**
 * @brief Software start ZSP
 * @return VSD_SUCCESS on success, others on error
 */
int soc_sw_start_zsp(void);

/**
 * @brief Get ZSP boot status
 * @return The status of ZSP boot
 */
int soc_get_zsp_boot_status(void);

/**
 * @brief Get CPU clock frequency
 * @return The clock frequency of CPU
 */
uint32_t soc_cpu_clock_get_freq(void);

/**
 * @brief Get RTC clock frequency
 * @return The clock frequency of RTC
 */
uint32_t soc_rtc_clock_get_freq(void);

/**
 * @brief Prepare for boot environment
 */
void soc_prepare_boot(void);

/**
 * @brief Check whether platform is processing IRQ
 * @return True for IRQ, false otherwise.
 */
bool soc_platform_in_isr(void);

/**
 * @brief Reinit xip divisor
 */
void soc_qspi_set_divisor(uint32_t divisor);

/**
 * @brief Set interrupt level
 */
/**
 * @brief Set interrupt level
 * @param irqn IRQn type; @see IRQn_Type
 * @param level 0 <= level <= 127
 */
void soc_set_interrupt_level(int irqn, uint8_t level);

/** @} */

/** @} */

#endif // SOC_SYSCTL_H
