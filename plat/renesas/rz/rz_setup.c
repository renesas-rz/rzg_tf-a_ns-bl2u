/*
 * Copyright (c) 2022, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <debug.h>
// #include <drivers/console.h>
#include <io_storage.h>
#include <platform.h>
#include <platform_def.h>
#include <arch_helpers.h>
#include <xlat_tables_v2.h>
#include <rz_private.h>

#include <scifa.h>
#include <console/console.h>

#define IMAGE_TEXT_BASE		NS_BL2U_BASE
IMPORT_SYM(uintptr_t,		__TEXT_END__,		IMAGE_TEXT_END);

#define IMAGE_RODATA_BASE	IMAGE_TEXT_END
IMPORT_SYM(uintptr_t,		__RODATA_END__,		IMAGE_RODATA_END);

#define IMAGE_RW_BASE		IMAGE_RODATA_END
IMPORT_SYM(uintptr_t,		__NS_BL2U_END__,	IMAGE_RW_END_UNALIGNED);
#define IMAGE_RW_END		round_up(IMAGE_RW_END_UNALIGNED, PAGE_SIZE)


static const mmap_region_t rz_mmap[] = {
	MAP_REGION_FLAT(RZG2L_DEVICE_BASE, RZG2L_DEVICE_SIZE, MT_DEVICE | MT_RW | MT_NS),
	MAP_REGION_FLAT(RZG2L_SPIROM_BASE, RZG2L_SPIROM_SIZE, MT_MEMORY | MT_RO | MT_NS),
	MAP_REGION_FLAT(RZG2L_DDR1_BASE, RZG2L_DDR1_SIZE, MT_MEMORY | MT_RW | MT_NS),
	{0}
};

static console_t rz_console;

void rz_platform_setup(void)
{
	int ret; 

	ret = rz_io_setup();
	if (ret != IO_SUCCESS)
		WARN("IO setup failed : 0x%x\n", ret);
}

void rz_plat_enable_mmu(void)
{
	if (IS_IN_EL1())
		enable_mmu_el1(0);
	else if (IS_IN_EL2())
		enable_mmu_el2(0);
	else
		panic();
}

void rz_plat_configure_mmu(void)
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

	mmap_add(rz_mmap);
	init_xlat_tables();
	rz_plat_enable_mmu();
}

void early_platform_setup(void)
{
	int ret;

	/* initialize console driver */
	ret = console_rzg2l_register(
							RZG2L_SCIF0_BASE,
							RZG2L_UART_INCK_HZ,
							RZG2L_UART_BARDRATE,
							&rz_console);

	if (!ret)
		panic();

	console_set_scope(&rz_console,
			CONSOLE_FLAG_BOOT | CONSOLE_FLAG_RUNTIME | CONSOLE_FLAG_CRASH);

}

void platform_setup(void)
{
	rz_platform_setup();
}

void plat_arch_setup(void)
{
	rz_plat_configure_mmu();
}
