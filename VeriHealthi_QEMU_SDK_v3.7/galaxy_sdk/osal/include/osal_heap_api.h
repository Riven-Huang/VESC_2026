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

#ifndef OSAL_HEAP_H
#define OSAL_HEAP_H

#include <stddef.h>
#include <stdint.h>
/**
 * @addtogroup OSAL_HEAP
 * OSAL Heap API
 * @ingroup OSAL
 * @{
 */

/**
 * @brief Allocate memory
 * @note Allocate cacheable memory for CPU which supports caching; Allocate
 * non-cacheable memory for CPU which does not support caching.
 *
 * @param len Memory size
 * @return void* Pointer of memory block, NULL for failure
 */
void *osal_malloc(size_t len);

/**
 * @brief Free the memory
 *
 * @note Free cacheable memory for CPU which supports caching; Free
 * non-cacheable memory for CPU which does not support caching.
 * @param pmem Pointer of memory block
 */
void osal_free(void *pmem);

/**
 * @brief Allocate non-cacheable memory
 * @param len Memory size
 * @return void* Pointer of memory block, NULL for failure
 */
void *osal_malloc_noncache(size_t len);

/**
 * @brief Free the non-cacheable memory
 * @param pmem Pointer of memory block
 */
void osal_free_noncache(void *pmem);

/**
 * @brief initialize OS heap
 * @return OSAL_SUCCESS for success, OSAL_ERROR for failure
 */
int osal_heap_init(void);

/**
 * @brief Get the total amount of heap space that remains unallocated
 *
 * @return Size of the free heap in bytes
 */
size_t osal_get_free_heap(void);
/** @} */

#endif // OSAL_HEAP_H
