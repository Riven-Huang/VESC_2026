/*
 * Copyright (c) 2024 VeriSilicon
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Public API for VSBT FLASH drivers
 */

#ifndef VSBT_FLASH_H_
#define VSBT_FLASH_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup vsbt_flash Vendor NVM Interface
 * @brief Vendor-specific NVM interface for Bluetooth subsystem
 * @{
 */

/**
 * Flash memory parameters. Contents of this structure suppose to be
 * filled in during flash device initialization and stay constant
 * through a runtime.
 */
struct vsbt_flash_param {
	const size_t write_block_size;
	uint8_t erase_value; /* Byte value of erased flash */
    const size_t sector_size;
    const size_t sector_count;
};

/**
 * @brief Allocate a continuous space on Non-volatile storage
 * @details The storage space should contain 2 or more sectors.
 *          Those sectors are dedicated to Bluetooth usage,
 *          so the start address and size should be aligned
 *          with the physical sector of the NVS device.
 *
 * @param bank_id ID used to identify the storage space in all APIs
 * @return Size of the allocated space on success, negative errno code on fail
 */
int vsbt_flash_open_bank(unsigned int *bank_id);

/**
 * @brief Read data from flash
 * @details All flash drivers support reads without alignment restrictions on
 *          the read offset, the read size, or the destination address.
 *
 * @param bank_id ID of the storage space
 * @param offset Offset (byte aligned) to read
 * @param data Buffer to store read data
 * @param len Number of bytes to read
 * @return 0 on success, negative errno code on fail
 */
int vsbt_flash_read(unsigned int bank_id, unsigned int offset, void *data,
			 size_t len);

/**
 * @brief Write buffer into flash memory
 * @details All flash drivers support a source buffer located either in RAM or
 *          SoC flash, without alignment restrictions on the source address.
 *          Write size and offset must be multiples of the minimum write block size
 *          supported by the driver.
 *
 * @param bank_id ID of the storage space
 * @param offset Starting offset for the write
 * @param data Data to write
 * @param len Number of bytes to write
 * @return 0 on success, negative errno code on fail
 */
int vsbt_flash_write(unsigned int bank_id, unsigned int offset,
			  const void *data,
			  size_t len);

/**
 * @brief Erase part or all of a flash memory
 * @details Offset and size is aligned to the sector_size from
 *          struct flash_parameters
 *
 * @param bank_id ID of the storage space
 * @param offset Erase area starting offset
 * @param size Size of area to be erased
 * @return 0 on success, negative errno code on fail
 */
int vsbt_flash_erase(unsigned int bank_id, unsigned int offset, size_t size);

/**
 * @brief Get pointer to vsbt_flash_param structure
 * @details Returned pointer points to a structure that should be considered
 *          constant through a runtime, regardless if it is defined in RAM or
 *          Flash. Developer is free to cache the structure pointer or copy its contents.
 *
 * @param bank_id ID of the storage space
 * @return Pointer to vsbt_flash_param structure characteristic for the device
 */
const struct vsbt_flash_param *vsbt_flash_get_parameters(unsigned int bank_id);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* VSBT_FLASH_H_*/
