/*
 * Copyright (c) 2021, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <string.h>

#include <platform_def.h>

#define SPI_RET_OK 0
#define SPI_RET_NG 1

#define FLASH_MEMORY_SIZE	FLASH_SIZE
#define SPI_IOADDRESS_TOP	FLASH_BASE
#define QSPI_BLOCK_SIZE 	SPI_FLASH_BLOCK_SIZE
#define QSPI_END_ADDR		(FLASH_BASE + FLASH_SIZE)

int32_t spi_multi_setup(void);
int32_t spi_multi_erase_sectors(uint32_t spi_addr, uint32_t len);
int32_t spi_multi_write(uint32_t spi_addr, uint64_t buff, uint32_t len);

#endif /* SPI_H */
