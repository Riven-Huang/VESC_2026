/*
 * Copyright (c) 2025, VeriSilicon Holdings Co., Ltd. All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef BSP_COMMON_H
#define BSP_COMMON_H

#include <stdint.h>
#include <stddef.h>

/**
 * @addtogroup BSP_COMMON
 * BSP Common Definition
 * @ingroup BSP
 * @{
 */

#ifndef ROUND_DOWN
#define ROUND_DOWN(a, b) (((a) / (b)) * (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) > (b) ? (b) : (a))
#endif

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(a) (a) < 0 ? -(a) : (a)
#endif

#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#endif

#ifndef BIT
#define BIT(b) (1UL << (b))
#endif

#ifndef MASK
#define MASK(b_hi, b_lo) (((1ULL << ((b_hi) - (b_lo) + 1ULL)) - 1ULL) << (b_lo))
#endif

#ifndef SET_BIT
#define SET_BIT(x, b) ((x) |= (1 << (b)))
#endif

#ifndef SET_BITS
#define SET_BITS(x, b_hi, b_lo) ((x) | (MASK(b_hi, b_lo) << (b_lo)))
#endif

#ifndef GET_BIT
#define GET_BIT(x, b) (((x) & (1ULL << (b))) >> (b))
#endif

#ifndef GET_BITS
#define GET_BITS(x, b_hi, b_lo) (((x) & MASK(b_hi, b_lo)) >> (b_lo))
#endif

#ifndef CLEAR_BIT
#define CLEAR_BIT(x, b) ((x) &= ~(1 << (b)))
#endif

#ifndef CLEAR_BITS
#define CLEAR_BITS(x, b_hi, b_lo) ((x) &= ~(MASK(b_hi, b_lo)))
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

/**
 * @brief         cast a member of a structure out to the containing structure
 * @param ptr     the pointer to the member.
 * @param type    the type of the container struct this is embedded in.
 * @param member  the name of the member within the struct.
 *
 */
#ifndef container_of
#define container_of(ptr, type, member)                    \
    ({                                                     \
        const typeof(((type *)0)->member) *__mptr = (ptr); \
        (type *)((char *)__mptr - offsetof(type, member)); \
    })
#endif

#ifndef OPTIMIZE_FUNC
#define OPTIMIZE_FUNC(x) __attribute__((optimize(x)))
#endif

#ifndef ARG_UNUSED
#define ARG_UNUSED(x) ((void)(x))
#endif

#ifndef ATTR_UNUSED
#define ATTR_UNUSED __attribute__((unused))
#endif

/** @} */

#endif // BSP_COMMON_H
