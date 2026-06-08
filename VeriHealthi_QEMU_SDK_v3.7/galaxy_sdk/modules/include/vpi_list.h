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

#ifndef VPI_LIST_H
#define VPI_LIST_H

/**
 * @addtogroup VPI_LIST
 * LIST Definition
 * @ingroup MODULE_VPI
 * @{
 */

typedef struct ListHead_st {
    struct ListHead_st *next, *pre;
} ListHead;

/** @} */

#endif // VPI_LIST_H
