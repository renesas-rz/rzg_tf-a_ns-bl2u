/*
 * Copyright (c) 2018, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <debug.h>
#include <rzg_firmware_image_package.h>
#include <fwu_nvm.h>
#include <io_storage.h>
#include <platform.h>
#include <platform_def.h>
#include <status.h>
#include <string.h>
#include <uuid_utils.h>

#define NVM_SIZE (0x4000000)
#define FLASH_BLOCK_SIZE (0x100)
#define FIP_FLAGS_END_OF_FILE   (0x8000)

STATUS fwu_load_fip_data(unsigned long fip_load_addr , unsigned long fip_nvm_addr, uint32_t *load_size)
{
	int ret;
	int fip_size;
	uintptr_t nvm_handle;
	size_t length_read;
	size_t length;
	unsigned long fip_offset;
	fip_toc_header_t *toc_header;
	fip_toc_entry_t *toc_entry;

	*load_size = 0;

	toc_header = (fip_toc_header_t *)fip_nvm_addr;

	/* Check if FIP exists. If it does not exist, it ends. */
	if (((toc_header->name != TOC_HEADER_NAME_FW_IPL) &&
		 (toc_header->name != TOC_HEADER_NAME_KEYRING) &&
		 (toc_header->name != TOC_HEADER_NAME_BOOT_FW) &&
		 (toc_header->name != TOC_HEADER_NAME_NS_BL2U)) ||
		(toc_header->serial_number == 0))
		return STATUS_SUCCESS;

	toc_entry = (fip_toc_entry_t *)(toc_header + 1);
	while ((toc_entry + 1) < (fip_toc_entry_t *)(FLASH_BASE + FLASH_SIZE))
	{
		if (is_uuid_null(&toc_entry->uuid))
		{
			/* get the total size of this FIP */
			fip_size = (int)toc_entry->offset_address;
			break;
		}
		toc_entry++;
	}

	if (0 == fip_size)
		return STATUS_FAIL;

	plat_get_nvm_handle(&nvm_handle);

	fip_offset = fip_nvm_addr - FLASH_BASE;

	ret = io_seek(nvm_handle, IO_SEEK_SET, fip_offset);
	if (ret != IO_SUCCESS)
		return STATUS_FAIL;
	
	ret = io_read(nvm_handle, (const uintptr_t)fip_load_addr,
		fip_size, &length_read);
	if ((ret != IO_SUCCESS) || (fip_size != length_read))
		return STATUS_FAIL;

	*load_size = fip_size;

	return STATUS_SUCCESS;
}

STATUS fwu_write_fip_data(unsigned long fip_load_addr, uint32_t load_size)
{
	int ret;
	uintptr_t nvm_handle;
	uintptr_t load_ram_addr;
	uintptr_t image_addr;
	size_t length_write;
    fip_toc_entry_t *toc_entry;

	plat_get_nvm_handle(&nvm_handle);

	load_ram_addr = fip_load_addr + load_size;
	toc_entry = (fip_toc_entry_t *)(fip_load_addr + sizeof(fip_toc_header_t));

	while ((toc_entry + 1) < (fip_toc_entry_t *)load_ram_addr)
	{
		if(is_uuid_null(&toc_entry->uuid))
			break;
		
		if (0 < toc_entry->size)
		{
			/* Seek to the given offset. */
			ret = io_seek(nvm_handle, IO_SEEK_SET, toc_entry->nvm_offset);
			if (ret != IO_SUCCESS)
				return STATUS_FAIL;

			image_addr = fip_load_addr + toc_entry->offset_address;
			/* Write to the given offset. */
			ret = io_write(nvm_handle, (const uintptr_t)image_addr,
				toc_entry->size, &length_write);
			if ((ret != IO_SUCCESS) || (toc_entry->size != length_write))
				return STATUS_FAIL;
		}
		toc_entry++;
	}

	return STATUS_SUCCESS;
}

STATUS fwu_erase_fip_data(unsigned long fip_nvm_addr)
{
	int ret;
	uintptr_t nvm_handle;
	size_t length_write;
	unsigned long fip_offset;
	fip_toc_header_t toc_header;

	plat_get_nvm_handle(&nvm_handle);

	/* Erase the FIP package Header infor. */
	memset(&toc_header, 0xFF, sizeof(fip_toc_header_t));

	fip_offset = fip_nvm_addr - FLASH_BASE;
	/* Seek to the given offset. */
	ret = io_seek(nvm_handle, IO_SEEK_SET, fip_offset);
	if (ret != IO_SUCCESS)
		return STATUS_FAIL;

	/* Write to the given offset. */
	ret = io_write(nvm_handle, (const uintptr_t)&toc_header,
		sizeof(fip_toc_header_t), &length_write);
	if ((ret != IO_SUCCESS) || (sizeof(fip_toc_header_t) != length_write))
		return STATUS_FAIL;

	return STATUS_SUCCESS;
}

STATUS fwu_update_fip(uintptr_t fip_addr)
{
	STATUS status;
	uint32_t load_size;
	uint64_t fip_flags;
	uintptr_t load_ram_addr;
	unsigned long fip_nvm_addr;
	fip_toc_header_t *toc_header;

	/* If the new FIP address is 0 it means no update. */
	if (fip_addr == 0)
		return STATUS_SUCCESS;

	fip_nvm_addr = fip_addr;
	load_ram_addr = FIP_IMAGE_TMP_DDR_ADDRESS;

	do
	{
		status = fwu_load_fip_data(load_ram_addr, fip_nvm_addr, &load_size);
		if(STATUS_SUCCESS != status)
			break;

		if(0 != load_size)
		{
			toc_header = (fip_toc_header_t *)load_ram_addr;
			fip_flags = toc_header->flags >> 32;

			status = fwu_write_fip_data(load_ram_addr, load_size);
			if(STATUS_SUCCESS != status)
				break;
			fip_nvm_addr += load_size;
		}
	} while ((0 != load_size) && (0 == (fip_flags & FIP_FLAGS_END_OF_FILE)));

	/* Erase the FIP package. */
	status = fwu_erase_fip_data(fip_addr);
	if(STATUS_SUCCESS != status)
		return status;

	INFO("FWU Image update success\n");

	return STATUS_SUCCESS;
}


