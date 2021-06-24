#
# Copyright (c) 2016-2019, ARM Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

LIBC_SRCS	:=	$(addprefix lib/libc/,		\
			abort.c				\
			assert.c			\
			exit.c				\
			memchr.c			\
			memcmp.c			\
			memcpy.c			\
			memmove.c			\
			memset.c			\
			printf.c			\
			putchar.c			\
			puts.c				\
			rand.c				\
			snprintf.c			\
			strchr.c			\
			strcmp.c			\
			strlcpy.c			\
			strlen.c			\
			strncmp.c			\
			strncpy.c			\
			strnlen.c			\
			strrchr.c)


LIBC_SRCS	+=	$(addprefix lib/libc/aarch64/,	\
			setjmp.S)

INCLUDES	+=	-Iinclude/lib/libc		\
			-Iinclude/lib/libc/$(ARCH)	\
