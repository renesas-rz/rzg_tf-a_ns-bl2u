
/*
 * Copyright (c) 2021, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdint.h>
#include <string.h>
#include <lib/mmio.h>

#include "rpc_accessor.h"

static void rpc_wait_trans_end(void);

void rzg_rpc_manual_mode_init(void)
{
	uint32_t data = 0;

	// power on rpc
	data = *((volatile uint32_t *)CPG_MSTPSR9);
	if (data & BIT17)
	{
		// case RPC(QSPI) Standby
		data &= ~BIT17;
		mmio_write_32(CPG_CPGWPR, ~data);
		mmio_write_32(CPG_SMSTPCR9, data);
		while (BIT17 & mmio_read_32(CPG_MSTPSR9))
			; // wait bit=0
	}

	// set SSL delay bit10-8  SLNDL[2:0] =  100 : 5.5 cycles of QSPIn_SPCLK
	mmio_write_32(RPC_SSLDR, 0x00000400);
}

void rzg_rpc_read_ext_mode_init(void)
{
	uint32_t product;
	uint32_t cut;

	product = mmio_read_32(PRR) & PRR_PRODUCT_MASK;
	cut = mmio_read_32(PRR) & PRR_CUT_MASK;

	if ((product == PRR_PRODUCT_G2M) && (cut < PRR_CUT_30))
	{
		mmio_write_32(RPC_PHYCNT, 0x00030260);
		mmio_write_32(RPC_PHYCNT, 0x80030260);
	}
	else
	{
		mmio_write_32(RPC_PHYCNT, 0x00038260);
		mmio_write_32(RPC_PHYCNT, 0x80030260);
	}

	mmio_write_32(RPC_CMNCR, 0x01FFF300);
	mmio_write_32(RPC_DRCR, 0x001F0100);
		//bit20-16 RBURST[4:0] = 11111 : 32 continuous data unit
		//bit8     RBE         =     1 : Burst read
	mmio_write_32(RPC_DRCMR, 0x000C0000);
		//bit23-16 CMD[7:0] = 0x0C : 4FAST_READ 0Ch Command 4-byte address command
	mmio_write_32(RPC_DREAR, 0x00000001);
		//bit23-16 EAV[7:0]   = 0    : ADR[32:26] output set0
		//bit2-0   EAC[2:0]   = 001  : ADR[25:0 ] Enable
	mmio_write_32(RPC_DRENR, 0x0000CF00);
		//bit31-30 CDB[1:0]   =   00 : 1bit width command (QSPI0_MOSI)
		//bit25-24 ADB[1:0]   =   00 : 1bit width address (QSPI0_MOSI)
		//bit17-16 DRDB[1:0]  =   00 : 1bit width transfer data (QSPI0_IO0)
		//bit15    DME        =    1 : dummy cycle enable
		//bit14    CDE        =    1 : Command enable
		//bit11-8  ADE[3:0]   = 1111 : ADR[23:0] output (24 Bit Address)
	mmio_write_32(RPC_DRDMCR, 0x00000007);
		//bit2-0 DMCYC[2:0]   = 111 : 8 cycle dummy wait
	mmio_write_32(RPC_DRDRENR, 0x00000000);
		//bit8 ADDRE  = 0 : Address SDR transfer
		//bit0 DRDRE  = 0 : DATA SDR transfer
}

void rzg_rpc_reset(void)
{
	mmio_write_32(CPG_CPGWPR, ~BIT17);
	mmio_write_32(CPG_SRCR9, BIT17);
	//wait: tRLRH Reset# low pulse width 10us
	waitus(20); // wait 20us

	mmio_write_32(CPG_CPGWPR, ~BIT17);
	mmio_write_32(CPG_SRSTCLR9, BIT17);
	//wait: tREADY1(35us) - tRHSL(10us) = 25us
	waitus(40); // wait 40us
}

void rzg_rpc_erase_sector(uint32_t sector_addr)
{
	uint32_t product;
	uint32_t cut;

	product = mmio_read_32(PRR) & PRR_PRODUCT_MASK;
	cut = mmio_read_32(PRR) & PRR_CUT_MASK;

	if ((product == PRR_PRODUCT_G2M) && (cut < PRR_CUT_30))
	{
		mmio_write_32(RPC_PHYCNT, 0x80030260);
	}
	else
	{
		mmio_write_32(RPC_PHYCNT, 0x80038260);
		//bit31  CAL         =  1 : PHY calibration
		//bit1-0 PHYMEM[1:0] = 00 : QSPI-SDR
	}
	mmio_write_32(RPC_CMNCR, 0x81FFF300);
	//bit31  MD       =  1 : Manual mode
	//bit1-0 BSZ[1:0] = 00 : QSPI Flash x 1
	mmio_write_32(RPC_SMCMR, 0x00DC0000);
	//bit23-16 CMD[7:0] = 0xDC : Sector Erase 4-byte address command
	mmio_write_32(RPC_SMADR, sector_addr);
	mmio_write_32(RPC_SMDRENR, 0x00000000);
	//bit8 ADDRE  = 0 : Address SDR transfer
	//bit0 SPIDRE = 0 : DATA SDR transfer
	mmio_write_32(RPC_SMENR, 0x00004F00);
	//bit31-30 CDB[1:0]   =   00 : 1bit width command (QSPI0_MOSI)
	//bit25-24 ADB[1:0]   =   00 : 1bit width address (QSPI0_MOSI)
	//bit17-16 SPIDB[1:0] =   00 : 1bit width transfer data (QSPI0_MISO)
	//bit15    DME        =    0 : No dummy cycle
	//bit14    CDE        =    1 : Command enable
	//bit11-8  ADE[3:0]   = 1111 : ADR[31:0] output (32 Bit Address)
	//bit3-0   SPIDE[3:0] = 0000 : No transfer
	mmio_write_32(RPC_SMCR, 0x00000001);
	//bit2     SPIRE      = 0 : Data read disable
	//bit1     SPIWE      = 0 : Data write disable
	//bit0     SPIE       = 1 : SPI transfer start

	rpc_wait_trans_end();
}

void rzg_rpc_write_buff(uint32_t addr, uint32_t source_addr)
{
	uintptr_t i = 0;
	uint32_t product;
	uint32_t cut;
	uint32_t data;

	mmio_write_32(RPC_DRCR, 0x011F0301);
	//bit9   RCF         =  1 : Read Cache Clear

	product = mmio_read_32(PRR) & PRR_PRODUCT_MASK;
	cut = mmio_read_32(PRR) & PRR_CUT_MASK;

	if ((product == PRR_PRODUCT_G2M) && (cut < PRR_CUT_30))
	{
		mmio_write_32(RPC_PHYCNT, 0x80030274);
	}
	else
	{
		mmio_write_32(RPC_PHYCNT, 0x80038274);
		//bit31  CAL         =  1 : PHY calibration
		//bit2   WBUF        =  1 : Write Buffer Enable
		//bit1-0 PHYMEM[1:0] = 00 : QSPI-SDR
	}

	for (i = 0; i < 256; i = (i + 0x4))
	{
		data = *(volatile uint32_t *)(source_addr + i);
		mmio_write_32((RPC_WRBUF + i), data);
	}
	//bit31  CAL         =  1 : PHY calibration
	//bit2   WBUF        =  1 : Write Buffer Enable
	//bit1-0 PHYMEM[1:0] = 00 : QSPI-SDR
	mmio_write_32(RPC_CMNCR, 0x81FFF300);
	//bit31  MD       =  1 : Manual mode
	//bit1-0 BSZ[1:0] = 00 : QSPI Flash x 1
	mmio_write_32(RPC_SMCMR, 0x00120000);
	//bit23-16 CMD[7:0] = 0x12 : Page Program 4-byte address
	mmio_write_32(RPC_SMADR, addr);
	mmio_write_32(RPC_SMDRENR, 0x00000000);
	//bit8 ADDRE  = 0 : Address SDR transfer
	//bit0 SPIDRE = 0 : DATA SDR transfer
	mmio_write_32(RPC_SMENR, 0x00004F0F);
	//bit31-30 CDB[1:0]   =   00 : 1bit width command (QSPI0_MOSI)
	//bit25-24 ADB[1:0]   =   00 : 1bit width address (QSPI0_MOSI)
	//bit17-16 SPIDB[1:0] =   00 : 1bit width transfer data (QSPI0_MISO)
	//bit15    DME        =    0 : No dummy cycle
	//bit14    CDE        =    1 : Command enable
	//bit11-8  ADE[3:0]   = 1111 : ADR[23:0] is output
	//bit3-0   SPIDE[3:0] = 1111 : 32bit transfer
	mmio_write_32(RPC_SMCR, 0x00000003);
	//bit2     SPIRE      = 0 : Data read disable
	//bit1     SPIWE      = 1 : Data write enable
	//bit0     SPIE       = 1 : SPI transfer start

	rpc_wait_trans_end();

	if ((product == PRR_PRODUCT_G2M) && (cut < PRR_CUT_30))
	{
		mmio_write_32(RPC_PHYCNT, 0x00030273);
	}
	else
	{
		mmio_write_32(RPC_PHYCNT, 0x00038273);
		//bit31  CAL         =  0 : No PHY calibration
		//bit2   WBUF        =  0 : Write Buffer Disable
		//bit1-0 PHYMEM[1:0] = 11 : HyperFlash
	}
	mmio_write_32(RPC_DRCR, 0x011F0301);
	//bit9   RCF         =  1 : Read Cache Clear
}

void rzg_rpc_read_status(uint32_t *readData)
{
	uint32_t product;
	uint32_t cut;

	product = mmio_read_32(PRR) & PRR_PRODUCT_MASK;
	cut = mmio_read_32(PRR) & PRR_CUT_MASK;

	if ((product == PRR_PRODUCT_G2M) && (cut < PRR_CUT_30))
	{
		mmio_write_32(RPC_PHYCNT, 0x00030260);
		mmio_write_32(RPC_PHYCNT, 0x80030260);
	}
	else
	{
		mmio_write_32(RPC_PHYCNT, 0x00038260);
		mmio_write_32(RPC_PHYCNT, 0x80038260);
		//bit31  CAL         =  1 : PHY calibration
		//bit1-0 PHYMEM[1:0] = 00 : QSPI-SDR
	}
	mmio_write_32(RPC_CMNCR, 0x81FFF300);
	//bit31  MD       =  1 : Manual mode
	//bit1-0 BSZ[1:0] = 00 : QSPI Flash x 1
	mmio_write_32(RPC_SMCMR, 0x00050000);
	//bit23-16 CMD[7:0] = 0x05 : Status Read command (for Palladium QSPI model)
	mmio_write_32(RPC_SMDRENR, 0x00000000);
	//bit8 ADDRE  = 0 : Address SDR transfer
	//bit0 SPIDRE = 0 : DATA SDR transfer
	mmio_write_32(RPC_SMENR, 0x0000400F);
	//bit31-30 CDB[1:0]   =   00 : 1bit width command (QSPI0_MOSI)
	//bit25-24 ADB[1:0]   =   00 : 1bit width address (QSPI0_MOSI)
	//bit17-16 SPIDB[1:0] =   00 : 1bit width transfer data (QSPI0_MISO)
	//bit15    DME        =    0 : No dummy cycle
	//bit14    CDE        =    1 : Command enable
	//bit11-8  ADE[3:0]   = 0000 : Address output disable
	//bit3-0   SPIDE[3:0] = 1111 : 32bit transfer
	mmio_write_32(RPC_SMCR, 0x00000005);
	//bit2     SPIRE      = 1 : Data read enable
	//bit1     SPIWE      = 0 : Data write disable
	//bit0     SPIE       = 1 : SPI transfer start

	rpc_wait_trans_end();

	readData[0] = mmio_read_32(RPC_SMRDR0); //read data[31:0]
}

void rzg_rpc_write_cmd(uint32_t command)
{
	uint32_t product;
	uint32_t cut;

	product = mmio_read_32(PRR) & PRR_PRODUCT_MASK;
	cut = mmio_read_32(PRR) & PRR_CUT_MASK;

	if ((product == PRR_PRODUCT_G2M) && (cut < PRR_CUT_30))
	{
		mmio_write_32(RPC_PHYCNT, 0x80030260);
	}
	else
	{
		mmio_write_32(RPC_PHYCNT, 0x80038260);
		//bit31  CAL         =  1 : PHY calibration
		//bit1-0 PHYMEM[1:0] = 00 : QSPI-SDR
	}
	mmio_write_32(RPC_CMNCR, 0x81FFF300);
	//bit31  MD       =  1 : Manual mode
	//bit1-0 BSZ[1:0] = 00 : QSPI Flash x 1
	mmio_write_32(RPC_SMCMR, command);
	//bit23-16 CMD[7:0] : command
	mmio_write_32(RPC_SMDRENR, 0x00000000);
	//bit8 ADDRE  = 0 : Address SDR transfer
	//bit0 SPIDRE = 0 : DATA SDR transfer
	mmio_write_32(RPC_SMENR, 0x00004000);
	//bit31-30 CDB[1:0]   =   00 : 1bit width command (QSPI0_MOSI)
	//bit25-24 ADB[1:0]   =   00 : 1bit width address (QSPI0_MOSI)
	//bit17-16 SPIDB[1:0] =   00 : 1bit width transfer data (QSPI0_MISO)
	//bit15    DME        =    0 : No dummy cycle
	//bit14    CDE        =    1 : Command enable
	//bit11-8  ADE[3:0]   = 0000 : Address output disable
	//bit3-0   SPIDE[3:0] = 0000 : No transfer
	mmio_write_32(RPC_SMCR, 0x00000001);
	//bit2     SPIRE      = 0 : Data read disable
	//bit1     SPIWE      = 0 : Data write disable
	//bit0     SPIE       = 1 : SPI transfer start

	rpc_wait_trans_end();
}

static void rpc_wait_trans_end(void)
{
	uint32_t data = 0;

	while (1)
	{
		data = mmio_read_32(RPC_CMNSR);
		if (data & BIT0)
		{
			break;
		}
		// Wait for TEND = 1
	}
}
