/*
 * Copyright (c) 2021, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <string.h>

#define SPI_RET_OK 0
#define SPI_RET_NG	1

#define FLASH_MEMORY_SIZE 0x04000000
#define SPI_IOADDRESS_TOP 0x08000000
#define QSPI_BLOCK_SIZE 0x10000
#define QSPI_END_ADDR 0x04000000

int32_t rzg_spi_erase_sectors(uint32_t spi_addr, uint32_t len);
int32_t rzg_spi_write_buff(uint32_t spi_addr, uint64_t buff, uint32_t len);

#endif /* SPI_H */