#
# Copyright (c) 2017-2018, ARM Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

XLAT_TABLES_LIB_SRCS	:=	$(addprefix lib/xlat_tables_v2/,	\
				aarch64/enable_mmu.S			\
				aarch64/xlat_tables_arch.c		\
				xlat_tables_context.c			\
				xlat_tables_core.c			\
				xlat_tables_utils.c)

XLAT_TABLES_LIB_V2	:=	1
$(eval $(call add_define,XLAT_TABLES_LIB_V2))
