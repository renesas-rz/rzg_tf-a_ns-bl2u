/*
 * Copyright (c) 2021, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef PLATFORM_DEF_H
#define PLATFORM_DEF_H

#include <arch.h>

/*******************************************************************************
 * Platform binary types for linking
 ******************************************************************************/
#define PLATFORM_LINKER_FORMAT          "elf64-littleaarch64"
#define PLATFORM_LINKER_ARCH            aarch64

/* Size of cacheable stacks */

#define PLATFORM_STACK_SIZE 	U(0x1000)

#define PLATFORM_CLUSTER0_CORE_COUNT	U(4)
#define PLATFORM_CLUSTER1_CORE_COUNT	U(4)
#define PLATFORM_CORE_COUNT		(PLATFORM_CLUSTER1_CORE_COUNT + \
					 PLATFORM_CLUSTER0_CORE_COUNT)

#define MAX_IO_DEVICES			U(1)
#define MAX_IO_HANDLES			U(1)
/*******************************************************************************
 * Platform specific page table and MMU setup constants
 ******************************************************************************/
#define MAX_XLAT_TABLES		U(4)
#define PLAT_PHY_ADDR_SPACE_SIZE	(ULL(1) << 32)
#define PLAT_VIRT_ADDR_SPACE_SIZE	(ULL(1) << 32)
#define MAX_MMAP_REGIONS	U(7)

/*******************************************************************************
 * Platform memory map related constants
 ******************************************************************************/
#define DEVICE0_BASE		(0xE6000000)
#define DEVICE0_SIZE		(0x00300000)
#define DEVICE1_BASE		(0xE6360000)
#define DEVICE1_SIZE		(0x19CA0000)


/*******************************************************************************
 * Base address and size of external NVM flash
 ******************************************************************************/
#define FLASH_BASE			(0x08000000)
#define FLASH_SIZE			(0x04000000)

#define SPI_FLASH_BLOCK_SIZE		(0x10000) /* 64KB */

/*******************************************************************************
 * This offset is used to corrupt data in fip.bin
 * This is the base address for backup fip.bin image in NVM
 * which is used for replacing original fip.bin
 * This address is chosen such that it can stay with all
 * the other images in the NVM.
 ******************************************************************************/
#define FIP_BKP_OFFSET	(0x3000000)
#define FIP_BKP_ADDRESS	(FLASH_BASE + FIP_BKP_OFFSET)

/*******************************************************************************
 * Run-time address of the image.
 * It has to match the location where the Trusted Firmware-A loads the BL33
 * image.
 ******************************************************************************/
#define RZG_DRAM1_BASE		(0x40000000)
#define DRAM_BASE			RZG_DRAM1_BASE
#define DRAM_SIZE			(0x80000000)

/*******************************************************************************
 * Base address and limit for NS_BL2U image.
 ******************************************************************************/
#define NS_BL2U_BASE		(DRAM_BASE + 0x8000000)
#define NS_BL2U_LIMIT		(NS_BL2U_BASE + 0x4D000)

/*******************************************************************************
 * This is the temporary DDR address for loading backup fip.bin
 * image from NVM which is used for replacing original fip.bin
 * This address is chosen such that the NS_BL2U can be expanded
 * in future and also considering the large size of fip.bin.
 ******************************************************************************/
#define FIP_IMAGE_TMP_DDR_ADDRESS	(DRAM_BASE + 0x20000000)

/*******************************************************************************
 * Declarations and constants to access the mailboxes safely. Each mailbox is
 * aligned on the biggest cache line size in the platform. This is known only
 * to the platform as it might have a combination of integrated and external
 * caches. Such alignment ensures that two maiboxes do not sit on the same cache
 * line at any cache level. They could belong to different cpus/clusters &
 * get written while being protected by different locks causing corruption of
 * a valid mailbox address.
 ******************************************************************************/
#define CACHE_WRITEBACK_SHIFT   (6)
#define CACHE_WRITEBACK_GRANULE (1 << CACHE_WRITEBACK_SHIFT)

#endif /* PLATFORM_DEF_H */
