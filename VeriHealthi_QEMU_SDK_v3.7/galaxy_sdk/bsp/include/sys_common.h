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

#ifndef SYS_COMMON_H
#define SYS_COMMON_H

/**
 * @addtogroup PEGASUS_SYS_COMMON
 * System Common definition
 * @ingroup SOC_PEGASUS
 * @{
 */

/** noncritical functions section */
#define NONCRITICAL_SECTION __attribute__((section(".stext")))
/** critical functions section */
#define CRITICAL_SECTION __attribute__((section(".itext")))
/** driver ISR functions section */
#define DRV_ISR_SECTION __attribute__((section(".itext")))
/** driver critical functions section */
#define DRV_CRITICAL_SECTION __attribute__((section(".itext")))

/** noncritical data section */
#define NONCRITICAL_DATA __attribute__((section(".sdram_data")))
/** sensor data section */
#define SENSOR_DATA_SECTION __attribute__((section(".sensor_data")))
/** noncache data section */
#define NONCACHE_DATA_SECTION __attribute__((section(".noncache_data")))

/** 2KB Bootloader reserved data section, can be reused for data */
#define RSV_DATA_SECTION __attribute__((section(".rsv_data")))

#define NON_XIP_TEXT __attribute__((section(".nonxip_text")))
#define NON_XIP_DATA __attribute__((section(".sdram_data")))

/** @} */

#endif // SYS_COMMON_H
