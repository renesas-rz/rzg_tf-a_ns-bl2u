#
# Copyright (c) 2022, Renesas Electronics Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

SPI_FLASH		:= MT25QU512ABB

INCLUDE	+=	-Iinclude/plat/common										\
			-Iplat/renesas/rz/include									\
			-Iplat/renesas/rz/include/arch/aarch64						\
			-Iplat/renesas/rz/include									\
			-Iplat/renesas/rz/include/drivers/spi_multi					\
			-Iplat/renesas/rz/include/drivers/spi_multi/${SPI_FLASH}

SPI_MULTI_SOURCE := plat/renesas/rz/drivers/spi_multi/spi_multi.c                    \
                    plat/renesas/rz/drivers/spi_multi/${SPI_FLASH}/spi_multi_device.c

NS_BL2U_SOURCES	+=	drivers/renesas/io/io_spi.c						\
					drivers/renesas/io/io_spi_hw.c					\
					plat/common/aarch64/platform_up_stack.S			\
					${SPI_MULTI_SOURCE}								\
					plat/renesas/rz/drivers/scifa.S					\
					plat/renesas/rz/drivers/console/multi_console.c	\
					plat/renesas/rz/fwu_nvm_accessors.c				\
					plat/renesas/rz/rz_io_storage.c					\
					plat/renesas/rz/rz_setup.c						\
					plat/renesas/rz/lib/smc/aarch64/asm_smc.S		\
					plat/renesas/rz/lib/smc/aarch64/smc.c

