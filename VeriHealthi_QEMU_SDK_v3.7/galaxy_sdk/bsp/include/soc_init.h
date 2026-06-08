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

#ifndef SOC_INIT_H
#define SOC_INIT_H

/**
 * @ingroup BSP
 * @addtogroup BSP_SOC
 * @{
 */

/**
 * @addtogroup SOC_INIT
 * SoC Init API
 * @{
 */

/**
 * @brief SoC init function, should be called before SoC accessing
 * @return Return result
 * @retval VSD_SUCCESS for succeed, others for failure
 */
int soc_init(void);

/** @} */

/** @} */

#endif // SOC_INIT_H
