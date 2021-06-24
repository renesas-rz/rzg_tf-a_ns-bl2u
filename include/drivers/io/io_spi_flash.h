/*
 * Copyright (c) 2021, Renesas Electronics Corporation. All rights reserved.
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __IO_SPI_FLASH_H__
#define __IO_SPI_FLASH_H__

#include <platform_def.h>

#ifndef SPI_FLASH_BLOCK_SIZE
	#error SPI_FLASH_BLOCK_SIZE must be defined as the block \
		 size of the SPI Flash seen by the software
#endif

/* IO SPI Flash specification - used to refer to data on a memory map device
 * supporting block-like entities */
typedef struct io_spi_flash_spec {
	/* Base Address of the SPI Flash device - it is required to program
	 * the flash */
	uintptr_t device_address;
	uintptr_t region_address;
	uint32_t block_size;
	uint32_t block_count;
} io_spi_flash_spec_t;

struct io_dev_connector;

int register_io_dev_spi_flash(const struct io_dev_connector **dev_con);

#endif /* __IO_SPI_FLASH_H__ */