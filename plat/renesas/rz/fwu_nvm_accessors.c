/*
 * Copyright (c) 2022, Renesas Electronics Corporation. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <debug.h>
#include <fwu_nvm.h>
#include <io_storage.h>
#include <platform.h>
#include <platform_def.h>
#include <status.h>
#include <string.h>
#include <uuid_utils.h>
#include <firmware_image_package.h>

#define UUID_TRUSTED_BOOT_FIRMWARE_BL2 \
	{{0x5f, 0xf9, 0xec, 0x0b}, {0x4d, 0x22}, {0x3e, 0x4d}, 0xa5, 0x44, {0xc3, 0x9d, 0x81, 0xc7, 0x3f, 0x0a} }

STATUS fwu_write_data (uintptr_t src_addr, ssize_t dest_offset, uint32_t write_size)
{
	int ret;
	uintptr_t nvm_handle;
	size_t length_write;

	/* Check param */
	if ((NULL == (void *)src_addr) || (0 == write_size))
		return STATUS_INVALID_PARAMETER;

	/* Get handle */
	plat_get_nvm_handle(&nvm_handle);

	/* Seek */
	ret = io_seek(nvm_handle, IO_SEEK_SET, dest_offset);
	if (ret != IO_SUCCESS)
		return STATUS_FAIL;

	/* Write */
	ret = io_write(nvm_handle, (const uintptr_t)src_addr, write_size, &length_write);
	if ((ret != IO_SUCCESS) || (0 == length_write))
		return STATUS_FAIL;

	return STATUS_SUCCESS;
}

STATUS fwu_erase_fip_data(ssize_t fip_nvm_offset)
{
	int ret;
	uintptr_t nvm_handle;
	size_t length_write;
	fip_toc_header_t toc_header;

	/* Get handle */
	plat_get_nvm_handle(&nvm_handle);

	/* Erase the FIP package Header info. */
	memset(&toc_header, 0xFF, sizeof(fip_toc_header_t));

	if (0 == fip_nvm_offset)
		return STATUS_INVALID_PARAMETER;

	/* Seek to the given offset. */
	ret = io_seek(nvm_handle, IO_SEEK_SET, fip_nvm_offset);
	if (ret != IO_SUCCESS)
		return STATUS_FAIL;

	/* Write to the given offset. */
	ret = io_write(nvm_handle, (const uintptr_t)&toc_header,
		sizeof(fip_toc_header_t), &length_write);
	if ((ret != IO_SUCCESS) || (sizeof(fip_toc_header_t) != length_write))
		return STATUS_FAIL;

	return STATUS_SUCCESS;
}

STATUS fwu_load_bin_data (ssize_t fip_offset, uuid_t uuid, ssize_t dest_offset, uint32_t* load_size)
{
	int ret;
	uintptr_t nvm_handle;

	size_t length_read;
	ssize_t read_offset;
	fip_toc_header_t toc_header;
	fip_toc_entry_t toc_entry;

	uint64_t bin_offset = 0;
	uint64_t bin_size = 0;

	STATUS status;

	/* Get handle */
	plat_get_nvm_handle(&nvm_handle);

	/* Check FIP header */
	ret = io_seek(nvm_handle, IO_SEEK_SET, fip_offset);
	if (ret != IO_SUCCESS)
		return STATUS_FAIL;

	ret = io_read(nvm_handle, (uintptr_t)(&toc_header), sizeof(toc_header), &length_read);
	if (ret != IO_SUCCESS)
		return STATUS_FAIL;

	if ((TOC_HEADER_NAME != toc_header.name) || (0 == toc_header.serial_number))
	{
		return STATUS_UNSUPPORTED;
	}

	/* Read FIP entries */
	read_offset = fip_offset + sizeof(toc_header);
	do
	{
		ret = io_seek(nvm_handle, IO_SEEK_SET, read_offset);
	    if (ret != IO_SUCCESS)
			return STATUS_FAIL;

		ret = io_read(nvm_handle, (uintptr_t)&toc_entry, sizeof(toc_entry), &length_read);
		if (ret != IO_SUCCESS)
			return STATUS_FAIL;

		if(0 == memcmp(&uuid, &toc_entry.uuid, sizeof(uuid_t)))
		{
			bin_offset = toc_entry.offset_address;
			bin_size = toc_entry.size;
			break;
		}
		read_offset += sizeof(toc_entry);
	}
	while (0 == is_uuid_null(&toc_entry.uuid));

	if (bin_size <= 0)
	{
		return STATUS_NOT_FOUND;
	}
	else
	{
		bin_offset += fip_offset;
	}

	/* Clear buffer */
	memset((void *)FIP_IMAGE_TMP_DDR_ADDRESS, 0x00, bin_size);

	/* Read and store buffer */
	ret = io_seek(nvm_handle, IO_SEEK_SET, bin_offset);
	if (ret != IO_SUCCESS)
		return STATUS_LOAD_ERROR;

	ret = io_read(nvm_handle, FIP_IMAGE_TMP_DDR_ADDRESS, bin_size, &length_read);
	if ((ret != IO_SUCCESS) || (0 == length_read))
		return STATUS_LOAD_ERROR;

	/* Write */
	status = fwu_write_data((uintptr_t)FIP_IMAGE_TMP_DDR_ADDRESS, dest_offset, (uint32_t)bin_size);

	return status;
}


STATUS fwu_load_fip_data(ssize_t fip_offset, uuid_t exclude_uuid, ssize_t dest_offset, uint32_t* load_size)
{
	int i;
	int ret;
	uintptr_t nvm_handle;

	uintptr_t buffer_ptr;
	ssize_t read_offset;
	size_t length_read;

	fip_toc_entry_t* toc_entry_ptr;

	uint32_t entry_counter = 0;

	ssize_t buffer_offset;
	uint64_t bin_offset = 0;
	uint64_t bin_size = 0;

	STATUS status;

	buffer_ptr = FIP_IMAGE_TMP_DDR_ADDRESS;
	read_offset = fip_offset;

	/* Get handle */
	plat_get_nvm_handle(&nvm_handle);

	/* Check FIP header */
	ret = io_seek(nvm_handle, IO_SEEK_SET, read_offset);
	if (ret != IO_SUCCESS)
		return STATUS_FAIL;

	ret = io_read(nvm_handle, buffer_ptr, sizeof(fip_toc_header_t), &length_read);
	if (ret != IO_SUCCESS)
		return STATUS_FAIL;

	if ((TOC_HEADER_NAME != ((fip_toc_header_t*)buffer_ptr)->name) ||
		(0 == ((fip_toc_header_t*)buffer_ptr)->serial_number))
	{
		return STATUS_UNSUPPORTED;
	}

	/* Read FIP entries and store to buffer */
	read_offset += sizeof(fip_toc_header_t);
	toc_entry_ptr = (fip_toc_entry_t *)(buffer_ptr + sizeof(fip_toc_header_t));

	while (1)
	{
		ret = io_seek(nvm_handle, IO_SEEK_SET, read_offset);
		if (ret != IO_SUCCESS)
			return STATUS_FAIL;

		ret = io_read(nvm_handle, (uintptr_t)toc_entry_ptr, sizeof(fip_toc_entry_t), &length_read);
		if (ret != IO_SUCCESS)
			return STATUS_FAIL;

		read_offset += sizeof(fip_toc_entry_t);
		if(0 != is_uuid_null(&toc_entry_ptr->uuid))
		{
			break;
		}
		// memo: if uuid is null, is_uuid_null return 1

		if(0 != memcmp(&exclude_uuid, &toc_entry_ptr->uuid, sizeof(uuid_t)) )
		{
			toc_entry_ptr++;
			entry_counter++;
		}
	}

	if(entry_counter < 1) // entry
	{
		return STATUS_NOT_FOUND;
	}

	/* Load fip data to buffer */
	toc_entry_ptr = (fip_toc_entry_t *)(buffer_ptr + sizeof(fip_toc_header_t));
	buffer_offset = sizeof(fip_toc_header_t) + (sizeof(fip_toc_entry_t) * (entry_counter + 1));
	buffer_ptr = FIP_IMAGE_TMP_DDR_ADDRESS + buffer_offset;

	while (0 == is_uuid_null(&toc_entry_ptr->uuid))
	{
		// source offset
		read_offset = fip_offset + toc_entry_ptr->offset_address;

		ret = io_seek(nvm_handle, IO_SEEK_SET, read_offset);
		if (ret != IO_SUCCESS)
			return STATUS_FAIL;

		ret = io_read(nvm_handle, buffer_ptr, toc_entry_ptr->size, &length_read);
		if (ret != IO_SUCCESS)
			return STATUS_FAIL;

		toc_entry_ptr->offset_address = buffer_offset;

		// next data offset
		buffer_offset += toc_entry_ptr->size;
		// next data ptr
		buffer_ptr += toc_entry_ptr->size;

		// next entry
		toc_entry_ptr++;
	}

	/* Write */
	status = fwu_write_data((uintptr_t)FIP_IMAGE_TMP_DDR_ADDRESS, dest_offset, (uint32_t)buffer_offset);

	*load_size = buffer_offset;
	return status;
}

STATUS fwu_update_fip(uintptr_t fip_addr)
{
	STATUS status;
	uint32_t load_size;
	ssize_t fip_offset;
	ssize_t dest_offset;
	uuid_t bl2_uuid = UUID_TRUSTED_BOOT_FIRMWARE_BL2;

	/* If the new FIP address is NULL it means no update. */
	if (NULL == (void *)fip_addr)
		return STATUS_SUCCESS;

	/* Convert to offset(support spi multi flash only) */
	fip_offset = fip_addr - FLASH_BASE;

	/* Load BL2 */
	dest_offset = BP_BL2_OFFSET;
	status = fwu_load_bin_data(fip_offset, bl2_uuid, dest_offset, &load_size);

	/* Laod FIP without BL2 */
	if(STATUS_SUCCESS == status)
	{
		dest_offset = FW_FIP_OFFSET;
		status = fwu_load_fip_data (fip_offset, bl2_uuid, dest_offset, &load_size);
	}

	/* Erase the FIP package. */
	fwu_erase_fip_data(fip_offset);
	INFO("FWU Image update %s\n", (STATUS_SUCCESS == status) ? "success" : "failed");

	return status;
}
