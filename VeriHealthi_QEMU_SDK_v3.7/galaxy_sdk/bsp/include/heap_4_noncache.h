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

#ifndef HEAP_4_NONCACHE_H
#define HEAP_4_NONCACHE_H

#include <stddef.h>
#include "mpu_wrappers.h"
#include "portable.h"

/**
 * @brief Gets the current state of the non cacheable heap
 * @param pxHeapStats Pointer to record heap state, @see HeapStats_t
 */
void vPortGetNonCacheHeapStats(HeapStats_t *pxHeapStats);

/**
 * @brief Allocate memory in the non cacheable heap region
 * @param xSize memory size
 * @return void* Pointer to the memory block
 */
void *pvPortMallocNonCache(size_t xSize) PRIVILEGED_FUNCTION;

/**
 * @brief Free memory in the non cacheable heap region
 * @param pv Pointer to the memory block
 */
void vPortFreeNonCache(void *pv) PRIVILEGED_FUNCTION;

/**
 * @brief Initialize the non cacheable memory block
 */
void vPortInitialiseNonCacheBlocks(void) PRIVILEGED_FUNCTION;

/**
 * @brief Get the total amount of non cacheable heap space that remains
 * unallocated when the function is called
 * @return size_t Unallocated heap size
 */
size_t xPortGetFreeNonCacheHeapSize(void) PRIVILEGED_FUNCTION;

/**
 * @brief Get the lowest amount of free non cacheable heap space that has
 * existed system the FreeRTOS application booted
 * @return size_t The lowest heap space size
 */
size_t xPortGetMinimumEverFreeNonCacheHeapSize(void) PRIVILEGED_FUNCTION;

#endif // HEAP_4_NONCACHE_H
