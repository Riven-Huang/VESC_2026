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

#ifndef OSAL_DEFINE
#define OSAL_DEFINE

/**
 * @addtogroup OSAL_COMMON
 * OSAL common definitions
 * @ingroup OSAL
 * @{
 */

/** Setting the wait time to OSAL_WAIT_FOREVER will cause the task to wait
 * indefinitely (without a timeout) */
#define OSAL_WAIT_FOREVER (0xFFFFFFFF)
/** Definition of success of OSAL */
#define OSAL_SUCCESS 0
/** Definition of error of OSAL */
#define OSAL_ERROR 1

/** @} */

#endif // OSAL_DEFINE
