/*
 * Copyright (c) 2021, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <debug.h>
#include <drivers/console.h>
#include <io_storage.h>
#include <platform.h>
#include <platform_def.h>
#include <arch_helpers.h>
#include <xlat_tables_v2.h>
#include <rzg_private.h>

extern void rzg_rpc_read_ext_mode_init(void);

#define IMAGE_TEXT_BASE		NS_BL2U_BASE
IMPORT_SYM(uintptr_t,		__TEXT_END__,		IMAGE_TEXT_END);

#define IMAGE_RODATA_BASE	IMAGE_TEXT_END
IMPORT_SYM(uintptr_t,		__RODATA_END__,		IMAGE_RODATA_END);

#define IMAGE_RW_BASE		IMAGE_RODATA_END
IMPORT_SYM(uintptr_t,		__NS_BL2U_END__,	IMAGE_RW_END_UNALIGNED);
#define IMAGE_RW_END		round_up(IMAGE_RW_END_UNALIGNED, PAGE_SIZE)


static const mmap_region_t rzg_mmap[] = {
	MAP_REGION_FLAT(DEVICE0_BASE, DEVICE0_SIZE, MT_DEVICE | MT_RW | MT_NS),
	MAP_REGION_FLAT(DEVICE1_BASE, DEVICE1_SIZE, MT_DEVICE | MT_RW | MT_NS),
	MAP_REGION_FLAT(FLASH_BASE, FLASH_SIZE, MT_DEVICE | MT_RW | MT_NS),
	MAP_REGION_FLAT(DRAM_BASE, DRAM_SIZE, MT_MEMORY | MT_RW | MT_NS),
	{0}
};

void rzg_platform_setup(void)
{
	int ret;

	ret = rzg_io_setup();
	if (ret != IO_SUCCESS)
		WARN("IO setup failed : 0x%x\n", ret);

}

void rzg_plat_enable_mmu(void)
{
#ifdef __aarch64__
	if (IS_IN_EL1())
		enable_mmu_el1(0);
	else if (IS_IN_EL2())
		enable_mmu_el2(0);
	else
		panic();
#else
	if (IS_IN_HYP())
		enable_mmu_hyp(0);
	else
		enable_mmu_svc_mon(0);
#endif
}

void rzg_plat_configure_mmu(void)
{
	/* Code */
	mmap_add_region(IMAGE_TEXT_BASE, IMAGE_TEXT_BASE,
			IMAGE_TEXT_END - IMAGE_TEXT_BASE, MT_CODE | MT_NS);

	/* RO data */
	mmap_add_region(IMAGE_RODATA_BASE, IMAGE_RODATA_BASE,
			IMAGE_RODATA_END - IMAGE_RODATA_BASE, MT_RO_DATA | MT_NS);

	/* Data + BSS */
	mmap_add_region(IMAGE_RW_BASE, IMAGE_RW_BASE,
			IMAGE_RW_END - IMAGE_RW_BASE, MT_RW_DATA | MT_NS);

	mmap_add(rzg_mmap);
	init_xlat_tables();

	rzg_plat_enable_mmu();
}

void platform_setup(void)
{
	rzg_platform_setup();
}

void plat_arch_setup(void)
{
	rzg_plat_configure_mmu();
}

void early_platform_setup(void)
{
    /* initialize the console */
	console_init(0, 0, 0);
}
