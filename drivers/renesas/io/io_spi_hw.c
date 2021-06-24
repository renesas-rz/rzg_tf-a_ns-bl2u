/*
 * Copyright (c) 2021, Renesas Electronics Corporation. All rights reserved.
 * Copyright (c) 2018-2019, Arm Limited. All rights reserved.
 * 
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <spi.h>
#include <platform_def.h>
#include <io_storage.h>
#include "io_spi_hw.h"

int flash_block_write(file_state_t *fp, uint32_t offset,
		const uintptr_t buffer, size_t *written)
{
	int ret;
	uintptr_t buffer_ptr = buffer;
	uint32_t remaining = fp->block_spec->block_size;
	uint32_t block_offset = offset;

	/* address passed should be block aligned */
	assert(!(offset % fp->block_spec->block_size));

    assert(fp->block_spec->block_size >= QSPI_BLOCK_SIZE);
    assert(!(fp->block_spec->block_size % QSPI_BLOCK_SIZE));

    while (remaining > 0)
    {
        if(rzg_spi_write_buff(block_offset, buffer_ptr, QSPI_BLOCK_SIZE))
            return IO_FAIL;

        block_offset += QSPI_BLOCK_SIZE;
        buffer_ptr += QSPI_BLOCK_SIZE;
        remaining -= QSPI_BLOCK_SIZE;
    }
      
	*written = fp->block_spec->block_size;

    return IO_SUCCESS;
}

static char block_buffer[SPI_FLASH_BLOCK_SIZE] __aligned(sizeof(uint32_t));

int flash_partial_write(file_state_t *fp, uint32_t offset,
		const uintptr_t buffer, size_t length, size_t *written)
{
	uintptr_t block_start;
	uint32_t block_size;
	uint32_t block_offset;
	int ret;

	assert((fp != NULL) && (fp->block_spec != NULL));
	assert(written != NULL);

	block_size = fp->block_spec->block_size;
	/* Start address of the block to write */
	block_start = (offset / block_size) * block_size;

	/* Ensure 'block_buffer' is big enough to contain a copy of the block.
	 * 'block_buffer' is reserved at build time - so it might not match  */
	assert(sizeof(block_buffer) >= block_size);

	/*
	 * Check the buffer fits inside a single block.
	 * It must not span several blocks
	 */
	assert((offset / block_size) ==
		  ((offset + length - 1) / block_size));

	/* Make a copy of the block from flash to a temporary buffer */
	memcpy(block_buffer, (void *)(fp->block_spec->region_address +
						block_start), block_size);

	/* Calculate the offset of the buffer into the block */
	block_offset = offset % block_size;

	/* update the content of the block buffer */
	memcpy(block_buffer + block_offset, (void *)buffer, length);

	/* Write the block buffer back */
	ret = flash_block_write(fp, block_start,
					(uintptr_t)block_buffer, written);
	if (ret == IO_SUCCESS)
		*written = length;

	return ret;
}
