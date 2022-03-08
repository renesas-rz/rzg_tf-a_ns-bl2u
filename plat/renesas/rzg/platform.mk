#
# Copyright (c) 2021, Renesas Electronics Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#


INCLUDE	+=	-Iinclude/plat/common/				\
			-Iinclude/plat/renesas/rzg/			\
			-Iplat/renesas/rzg/drivers/spi/		\
			-Iplat/renesas/rzg/include

NS_BL2U_SOURCES	+=	drivers/renesas/console/rzg_console.S		\
					drivers/renesas/io/io_spi.c					\
					drivers/renesas/io/io_spi_hw.c				\
					plat/common/aarch64/platform_up_stack.S		\
					plat/renesas/rzg/drivers/spi/spi.c			\
					plat/renesas/rzg/drivers/spi/rpc_accessor.c	\
					plat/renesas/rzg/fwu_nvm_accessors.c		\
					plat/renesas/rzg/rzg_io_storage.c			\
					plat/renesas/rzg/rzg_setup.c				\
					plat/renesas/rzg/lib/smc/aarch64/asm_smc.S	\
					plat/renesas/rzg/lib/smc/aarch64/smc.c

