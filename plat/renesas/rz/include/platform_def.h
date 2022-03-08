/*
 * Copyright (c) 2022, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef PLATFORM_DEF_H
#define PLATFORM_DEF_H

#include <arch.h>
#include <rzg2l_def.h>

/*******************************************************************************
 * Platform binary types for linking
 ******************************************************************************/
#define PLATFORM_LINKER_FORMAT	"elf64-littleaarch64"
#define PLATFORM_LINKER_ARCH	aarch64

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
 * Base address and size of external NVM flash
 ******************************************************************************/
#define FLASH_BASE			(RZG2L_SPIROM_BASE)
#define FLASH_SIZE			(0x10000000)

#define BP_BL2_OFFSET		(0x00000000)
#define FW_FIP_OFFSET		(0x0001D200)
#define FIP_BKP_OFFSET		(0x0021D200)
#define FIP_BKP_ADDRESS		(FLASH_BASE + FIP_BKP_OFFSET)

#define SPI_FLASH_BLOCK_SIZE	(0x01000) /* 4KB */

/*******************************************************************************
 * Base address and limit for NS_BL2U image.
 ******************************************************************************/
#define NS_BL2U_BASE		(0x48000000)
#define NS_BL2U_LIMIT		(NS_BL2U_BASE + 0x20000)

/*******************************************************************************
 * This is the temporary DDR address for loading backup fip.bin
 * image from NVM which is used for replacing original fip.bin
 * This address is chosen such that the NS_BL2U can be expanded
 * in future and also considering the large size of fip.bin.
 ******************************************************************************/
#define FIP_IMAGE_TMP_DDR_ADDRESS	(RZG2L_DDR1_BASE + 0x20000000)

/*******************************************************************************
 * Declarations and constants to access the mailboxes safely. Each mailbox is
 * aligned on the biggest cache line size in the platform. This is known only
 * to the platform as it might have a combination of integrated and external
 * caches. Such alignment ensures that two maiboxes do not sit on the same cache
 * line at any cache level. They could belong to different cpus/clusters &
 * get written while being protected by different locks causing corruption of
 * a valid mailbox address.
 ******************************************************************************/
#define CACHE_WRITEBACK_SHIFT	(6)
#define CACHE_WRITEBACK_GRANULE	(1 << CACHE_WRITEBACK_SHIFT)

#endif /* PLATFORM_DEF_H */