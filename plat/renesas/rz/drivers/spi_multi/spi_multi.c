/*
 * Copyright (c) 2022, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <lib/utils_def.h>
#include <lib/mmio.h>
#include <arch_helpers.h>

#include <spi.h>
#include <spi_multi_regs.h>
#include <spi_multi.h>
#include <spi_multi_reg_values.h>

uint8_t write_buffer[SPI_MULTI_BUF_SIZE] = {0};

void spi_multi_timing_set(void)
{

	/* Timing adjustment register setting */
	mmio_write_32(SPIM_PHYADJ2, 0xA5390000);
	mmio_write_32(SPIM_PHYADJ1, 0x80000000);
	mmio_write_32(SPIM_PHYADJ2, 0x00008080);
	mmio_write_32(SPIM_PHYADJ1, 0x80000022);
	mmio_write_32(SPIM_PHYADJ2, 0x00008080);
	mmio_write_32(SPIM_PHYADJ1, 0x80000024);

	/* SDR mode serial flash settings */
	mmio_write_32(SPIM_PHYCNT, SPIM_PHYCNT_SDR_TIM_ADJ_SET_VALUE);

	/* Timing adjustment register setting */
	mmio_write_32(SPIM_PHYADJ2, 0x00000030);
	mmio_write_32(SPIM_PHYADJ1, 0x80000032);

	dmbsy();
}

uint8_t spi_multi_cmd_read(uint8_t command)
{
	uint32_t val;
	uint8_t r_status;

	/* Set the QSPIn_SSL setting value & Manual Mode */
	mmio_write_32(SPIM_CMNCR, SPIM_CMNCR_MANUAL_SET_VALUE);

	val = command << SMCMR_CMD_BIT_SHIFT;
	mmio_write_32(SPIM_SMCMR, val);

	/* Set the Transfer Data size setting value &  command output enable */
	mmio_write_32(SPIM_SMENR, SMENR_CDE_ENABLED | SPI_MANUAL_COMMAND_SIZE_32_BIT);

	/* Set the SDR transfer & SPI flash mode setting value */
	mmio_write_32(SPIM_SMDRENR, SPIM_SMDRENR_SET_VALUE);

	mmio_write_32(SPIM_SMCR, SMCR_SPIE_TRANSFER_ENABLE | SMCR_SPIRE_ENABLE);

	/* Wait until the transfer is complete */
	do {
		val = mmio_read_32(SPIM_CMNSR);
	} while ((val & CMNSR_TEND) == 0);

	val = mmio_read_32(SPIM_SMRDR0);

	r_status = (uint8_t)val;
	return(r_status);
}

void spi_multi_cmd_write(uint8_t command,uint8_t size,uint32_t data)
{
	uint32_t val;

	/* SDR mode serial flash settings */
	mmio_write_32(SPIM_PHYCNT, SPIM_PHYCNT_SET_VALUE);

	/* Set the QSPIn_SSL setting value & Manual Mode */
	mmio_write_32(SPIM_CMNCR, SPIM_CMNCR_MANUAL_SET_VALUE);

	/* Set the Manual Mode Command */
	mmio_write_32(SPIM_SMCMR, command << SMCMR_CMD_BIT_SHIFT);

	/* Set the SDR transfer & SPI flash mode setting value */
	mmio_write_32(SPIM_SMDRENR, SPIM_SMDRENR_SET_VALUE);

	/* Set the Transfer Data size setting value &  command output enable */
	mmio_write_32(SPIM_SMENR, SMENR_CDE_ENABLED | size);

	/* Set the data transfer enable & data write enable	 */
	if (size == SPI_MANUAL_COMMAND_SIZE_0)
	{
		mmio_write_32(SPIM_SMCR, SMCR_SPIE_TRANSFER_ENABLE);
	} else {
		/* Set the write data in Manual mode */
		mmio_write_32(SPIM_SMWDR0, data);

		mmio_write_32(SPIM_SMCR, SMCR_SPIE_TRANSFER_ENABLE | SMCR_SPIWE_ENABLE);
	}

	/* Wait until the transfer is complete */
	do {
		val = mmio_read_32(SPIM_CMNSR);
	} while ((val & CMNSR_TEND) == 0);
}

int spi_multi_setup_read_ext(void )
{
	uint32_t val;

	/* Wait until the transfer is complete */
	do {
		val = mmio_read_32(SPIM_CMNSR);
	} while ((val & CMNSR_TEND) == 0);

	/* Device-specific settings */
	spi_multi_setup_device();
	/* SDR mode serial flash settings */
	mmio_write_32(SPIM_PHYCNT, SPIM_PHYCNT_SET_VALUE);

	/* Read timing setting */
	mmio_write_32(SPIM_PHYOFFSET1, SPIM_PHYOFFSET1_SET_VALUE);
	mmio_write_32(SPIM_PHYOFFSET2, SPIM_PHYOFFSET2_SET_VALUE);

	/* Set the QSPIn_SSL setting value */
	mmio_write_32(SPIM_CMNCR, SPIM_CMNCR_EXT_READ_SET_VALUE);
	/* Set SSL delay setting value */
	mmio_write_32(SPIM_SSLDR, SPIM_SSLDR_EXT_READ_SET_VALUE);

	/* Clear the RBE bit */
	mmio_write_32(SPIM_DRCR, SPIM_DRCR_SET_VALUE);
	mmio_read_32(SPIM_DRCR);

	/* Set the data read command */
	mmio_write_32(SPIM_DRCMR, SPIM_DRCMR_EXT_READ_SET_VALUE);

	/* Extended external address setting */
	mmio_write_32(SPIM_DREAR, SPIM_DREAR_SET_VALUE);

	/* Set the bit width of command and address output to 1 bit and	*/
	/* the address size to 4 byte									*/
	mmio_write_32(SPIM_DRENR, SPIM_DRENR_EXT_READ_SET_VALUE);

	/* Dummy cycle setting */
	mmio_write_32(SPIM_DRDMCR, SPIM_DRDMCR_EXT_READ_SET_VALUE);

	/* Change to SPI flash mode */
	mmio_write_32(SPIM_DRDRENR, SPIM_DRDRENR_SET_VALUE);

	/* Timing adjustment register setting */
	spi_multi_timing_set();

	return SPI_RET_OK;
}

int spi_multi_setup_manual(void )
{
	uint32_t val;

	/* Wait until the transfer is complete */
	do {
		val = mmio_read_32(SPIM_CMNSR);
	} while ((val & CMNSR_TEND) == 0);

	/* Device-specific settings */
	spi_multi_setup_device();

	/* SDR mode serial flash settings */
	mmio_write_32(SPIM_PHYCNT, SPIM_PHYCNT_SET_VALUE);

	/* Read timing setting */
	mmio_write_32(SPIM_PHYOFFSET1, SPIM_PHYOFFSET1_SET_VALUE);
	mmio_write_32(SPIM_PHYOFFSET2, SPIM_PHYOFFSET2_SET_VALUE);

	/* Set the QSPIn_SSL setting value */
	mmio_write_32(SPIM_CMNCR, SPIM_CMNCR_MANUAL_SET_VALUE);
	/* Set SSL delay setting value */
	mmio_write_32(SPIM_SSLDR, SPIM_SSLDR_MANUAL_SET_VALUE);

	/* Clear the RBE bit */
	mmio_write_32(SPIM_DRCR, SPIM_DRCR_SET_VALUE);
	mmio_read_32(SPIM_DRCR);

	/* Set the data read command */
	mmio_write_32(SPIM_DRCMR, SPIM_DRCMR_MANUAL_SET_VALUE);

	/* Extended external address setting */
	mmio_write_32(SPIM_DREAR, SPIM_DREAR_SET_VALUE);

	/* Set the bit width of command and address output to 1 bit and */
	/* the address size to 4 byte									*/
	mmio_write_32(SPIM_DRENR, SPIM_DRENR_MANUAL_SET_VALUE);

	/* Dummy cycle setting */
	mmio_write_32(SPIM_DRDMCR, SPIM_DRDMCR_MANUAL_SET_VALUE);

	/* Change to SPI flash mode */
	mmio_write_32(SPIM_DRDRENR, SPIM_DRDRENR_SET_VALUE);

	/* Timing adjustment register setting */
	spi_multi_timing_set();

	return SPI_RET_OK;
}

int spi_multi_setup( void ) {
	return spi_multi_setup_read_ext();
}

void spi_multi_erase_1sector (uint32_t sec_addr)
{
	uint32_t val;

	// Set erase instruction
	mmio_write_32(SPIM_SMCMR, SMCMR_CMD_BLOCK_ERASE << SMCMR_CMD_BIT_SHIFT);
	// Set erase sector address
	mmio_write_32(SPIM_SMADR, sec_addr);

	/* Set the SDR transfer & SPI flash mode setting value */
	mmio_write_32(SPIM_SMDRENR, SPIM_SMDRENR_SET_VALUE);

	mmio_write_32(SPIM_SMENR, SMENR_CDE_ENABLED | SPI_MANUAL_COMMAND_SIZE_0 |
		SMENR_ADE_ADR23_0_OUT);

	// Start erase
	mmio_write_32(SPIM_SMCR, SMCR_SPIE_TRANSFER_ENABLE | SMCR_SPIWE_DISABLE |
		SMCR_SPIRE_DISABLE);

	/* Wait until the transfer is complete */
	do {
		val = mmio_read_32(SPIM_CMNSR);
	} while ((val & CMNSR_TEND) == 0);

	return;
}

int32_t spi_multi_erase_sectors(uint32_t spi_addr, uint32_t len)
{
	uint32_t erase_sector_addr;
	uint32_t mask_sec_size;
	uint32_t erase_sec_top_addr, erase_sec_end_addr;
	uint32_t status;

	if ((spi_addr + len) > QSPI_END_ADDR)
	{
		return SPI_RET_NG;
	}

	mask_sec_size = (~QSPI_BLOCK_SIZE) + 1;
	erase_sec_top_addr = spi_addr & mask_sec_size;
	erase_sec_end_addr = (spi_addr + len) | ~(mask_sec_size);

	spi_multi_setup_manual();

	for (erase_sector_addr = erase_sec_top_addr;
		 erase_sector_addr <= erase_sec_end_addr;
		 erase_sector_addr = erase_sector_addr + QSPI_BLOCK_SIZE)
	{
		// Write enable
		spi_multi_cmd_write(SMCMR_CMD_WRITE_ENABLE, SPI_MANUAL_COMMAND_SIZE_0, 0);

		spi_multi_erase_1sector(erase_sector_addr);

		do {
			status = spi_multi_cmd_read(SMCMR_CMD_READ_STATUS);
		} while(STATUS_REGISTER_S0_READY != (status & STATUS_REGISTER_S0_MASK));
	}

	spi_multi_setup_read_ext();

	return SPI_RET_OK;
}

int32_t spi_multi_write_buff (uint32_t spi_addr, uint32_t src_addr)
{
	int32_t i;
	uint32_t val;
	uint32_t buffer;

	/* Read Cache Clear */
	mmio_write_32(SPIM_DRCR, SPIM_DRCR_SET_VALUE | DRCR_SSLE | DRCR_SSLN);

	/* Enbale Write Buffer */
	mmio_write_32(SPIM_PHYCNT, SPIM_PHYCNT_SET_VALUE | PHYCNT_CAL_PERFOMED |
		PHYCNT_WBUF_USED | PHYCNT_WBUF2_USED);

	// write to buffer
	for (i = 0; i < SPI_MULTI_BUF_SIZE; i = (i + sizeof(buffer)))
	{
		buffer = *(volatile uint32_t *)((uintptr_t)(src_addr + i));
		mmio_write_32((SPI_MULTI_BUF + i), buffer);
	}

	// Set write instruction
	mmio_write_32(SPIM_SMCMR, SMCMR_CMD_PAGE_PROGRAM << SMCMR_CMD_BIT_SHIFT);

	// set destination
	mmio_write_32(SPIM_SMADR, spi_addr);

	/* Set the SDR transfer & SPI flash mode setting value */
	mmio_write_32(SPIM_SMDRENR, SPIM_SMDRENR_SET_VALUE);

	// manual mode setting
	mmio_write_32(SPIM_SMENR, SMENR_CDE_ENABLED | SPI_MANUAL_COMMAND_SIZE_32_BIT |
		SMENR_ADE_ADR23_0_OUT);

	// Start write
	mmio_write_32(SPIM_SMCR, SMCR_SPIE_TRANSFER_ENABLE | SMCR_SPIWE_ENABLE |
		SMCR_SPIRE_DISABLE);

	/* Wait until the transfer is complete */
	do {
		val = mmio_read_32(SPIM_CMNSR);
	} while ((val & CMNSR_TEND) == 0);

	/* Disable Write Buffer */
	mmio_write_32(SPIM_PHYCNT, SPIM_PHYCNT_SET_VALUE);

	/* Read Cache Clear */
	mmio_write_32(SPIM_DRCR, SPIM_DRCR_SET_VALUE | DRCR_SSLE | DRCR_SSLN);

	return SPI_RET_OK;
}

int32_t spi_multi_write(uint32_t dest_addr, uint64_t src_addr, uint32_t data_len)
{
	int32_t i;
	uint32_t dest;
	uint32_t len;
	uint32_t status;

	if ((dest_addr + data_len) > QSPI_END_ADDR)
	{
		return SPI_RET_NG;
	}

	if (SPI_RET_OK != spi_multi_erase_sectors(dest_addr, data_len))
	{
		return SPI_RET_NG;
	}

	spi_multi_setup_manual();

	for (dest = dest_addr, len = data_len;
		 dest < (dest_addr + data_len);
		 dest += SPI_MULTI_BUF_SIZE, src_addr += SPI_MULTI_BUF_SIZE, len -=SPI_MULTI_BUF_SIZE)
	{
		// Write enable
		spi_multi_cmd_write(SMCMR_CMD_WRITE_ENABLE, SPI_MANUAL_COMMAND_SIZE_0, 0);

		if (SPI_MULTI_BUF_SIZE > len)
		{
			for (i = 0; i < len; i++)
			{
				write_buffer[i] = *(volatile uint8_t *)(src_addr + i);
			}
			// clear write buffer
			for (i = len; i < SPI_MULTI_BUF_SIZE; i++)
			{
				write_buffer[i] = 0xFF;
			}
			spi_multi_write_buff(dest, (uint64_t)write_buffer);
		}
		else
		{
			spi_multi_write_buff(dest, src_addr);
		}

		do {
			status = spi_multi_cmd_read(SMCMR_CMD_READ_STATUS);
		} while(STATUS_REGISTER_S0_READY != (status & STATUS_REGISTER_S0_MASK));
	}

	spi_multi_setup_read_ext();

	return SPI_RET_OK;
}
