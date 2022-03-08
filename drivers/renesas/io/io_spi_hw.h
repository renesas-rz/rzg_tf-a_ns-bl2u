/*
 * Copyright (c) 2021, Renesas Electronics Corporation. All rights reserved.
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef __IO_SPI_HW_H__
#define __IO_SPI_HW_H__

#include <drivers/io/io_driver.h>
#include <io_storage.h>

#include <drivers/io/io_spi_flash.h>

#define IS_FLASH_ADDRESS_BLOCK_ALIGNED(fp, addr) \
	(((addr) & ((fp)->block_spec->block_size - 1)) == 0)

/* As we need to be able to keep state for seek, only one file can be open
 * at a time. Make this a structure and point to the entity->info. When we
 * can malloc memory we can change this to support more open files.
 */
typedef struct {
	/* Use the 'in_use' flag as any value for base and file_pos could be
	 * valid.
	 */
	int		in_use;
	uintptr_t	base;
	size_t		file_pos;

	/* Definition of the flash block device */
	const io_spi_flash_spec_t *block_spec;
} file_state_t;

void flash_init(void);

int flash_block_write(file_state_t *fp, uint32_t address,
		const uintptr_t buffer, size_t *written);

int flash_partial_write(file_state_t *fp, uint32_t address,
		const uintptr_t buffer, size_t length, size_t *written);

#endif /* __IO_SPI_HW_H__ */
