#
# Copyright (c) 2021, Renesas Electronics Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

NS_BL2U_NAME	= ns_bl2u

VERSION_MAJOR		:= 1
VERSION_MINOR		:= 0

PLAT := rz

ifneq (${DEBUG}, 0)
	BUILD_TYPE	:=	debug
	# Use LOG_LEVEL_INFO by default for debug builds
	LOG_LEVEL       :=      40
else
	BUILD_TYPE	:=	release
	# Use LOG_LEVEL_ERROR by default for release builds
	LOG_LEVEL       :=      20
endif

# Default build string (git branch and commit)
ifeq (${BUILD_STRING},)
		BUILD_STRING    :=      $(shell git log -n 1 --pretty=format:"%h")
endif

################################################################################
# Toolchain
################################################################################
CC          :=  ${CROSS_COMPILE}gcc
CPP         :=  ${CROSS_COMPILE}cpp
AS          :=  ${CROSS_COMPILE}gcc
AR          :=  ${CROSS_COMPILE}ar
LD          :=  ${CROSS_COMPILE}ld
OC          :=  ${CROSS_COMPILE}objcopy
OD          :=  ${CROSS_COMPILE}objdump
NM          :=  ${CROSS_COMPILE}nm
PP          :=  ${CROSS_COMPILE}gcc -E

# directory definition
NS_BL2U_TOP		= $(shell 'pwd')
NS_BL2U_SRCDIR	= $(NS_BL2U_TOP)
NS_BL2U_OUTDIR	= $(NS_BL2U_TOP)/out
NS_BL2U_OBJDIR	= $(NS_BL2U_TOP)/obj

#output file name
NS_BL2U_OUTPUT	= ${NS_BL2U_OUTDIR}/${NS_BL2U_NAME}.bin
NS_BL2U_MOT		= ${NS_BL2U_OUTDIR}/${NS_BL2U_NAME}.mot
NS_BL2U_ELF		= ${NS_BL2U_OUTDIR}/${NS_BL2U_NAME}.elf
NS_BL2U_MAPFILE	= ${NS_BL2U_OUTDIR}/${NS_BL2U_NAME}.map
NS_BL2U_DUMP	= ${NS_BL2U_OUTDIR}/${NS_BL2U_NAME}.dump

VERSION_STRING		:= 	v${VERSION_MAJOR}.${VERSION_MINOR}(${PLAT},${BUILD_TYPE}):${BUILD_STRING}

################################################################################
# NS_BL2U Common definitions
################################################################################
# Include libc
include lib/libc/libc.mk
include lib/xlat_tables_v2/xlat_tables.mk

#input file name
INCLUDE	+=	-I./						\
			-Ilib/aarch64				\
			-Iinclude					\
			-Iinclude/plat				\
			-Iinclude/common			\
			-Iinclude/common/aarch64	\
			-Iinclude/lib				\
			-Iinclude/lib/libc			\
			-Iinclude/lib/libc/aarch64	\
			-Iinclude/lib/aarch64		\
			-Iinclude/lib/delay			\
			-Iinclude/lib/utils			\
			-Iinclude/lib/xlat_tables	\
			-Iinclude/drivers/io		\
			-Iinclude/drivers/spi

NS_BL2U_SOURCES	+=	${LIBC_SRCS}								\
					${XLAT_TABLES_LIB_SRCS}						\
					fwu/ns_bl2u/ns_bl2u_main.c					\
					fwu/ns_bl2u/aarch64/ns_bl2u_entrypoint.S	\
					drivers/io/io_storage.c						\
					lib/locks/aarch64/spinlock.S				\
					lib/delay/delay.c							\
					lib/debug/debug.c							\
					lib/utils/mp_printf.c						\
					lib/utils/uuid.c							\
					lib/aarch64/exception_stubs.S				\
					lib/aarch64/cache_helpers.S					\
					lib/aarch64/misc_helpers.S

################################################################################
# NS_BL2U Platform definitions
################################################################################
define rwildcard
$(strip $(foreach d,$(wildcard ${1}*),$(call rwildcard,${d}/,${2}) $(filter $(subst *,%,%${2}),${d})))
endef

PLATFORM_MAKEFILE		:= platform.mk
PLATFORM_ROOT			?= plat/

ALL_PLATFORM_MK_FILES       := $(call rwildcard,${PLATFORM_ROOT},${PLATFORM_MAKEFILE})
PLAT_MAKEFILE_FULL          := $(filter %/${PLAT}/${PLATFORM_MAKEFILE},${ALL_PLATFORM_MK_FILES})
include ${PLAT_MAKEFILE_FULL}

# Sort NS_BL2U_SOURCES source files to remove duplicates
NS_BL2U_SOURCES := $(sort ${NS_BL2U_SOURCES})

#object file name
NS_BL2U_OBJECTS := $(addprefix $(NS_BL2U_OBJDIR)/,$(notdir $(patsubst %.c,%.o,$(filter %.c,$(NS_BL2U_SOURCES)))))
NS_BL2U_OBJECTS += $(addprefix $(NS_BL2U_OBJDIR)/,$(notdir $(patsubst %.S,%.o,$(filter %.S,$(NS_BL2U_SOURCES)))))

#linker file name
NS_BL2U_LINKERFILE = ${NS_BL2U_TOP}/fwu/ns_bl2u/ns_bl2u.ld.S
NS_BL2U_SCRIPTFILE = ${NS_BL2U_OBJDIR}/ns_bl2u.ld
NS_BL2U_DEP_FILE	= $(NS_BL2U_SCRIPTFILE:.ld=.d)

# C source option
ifneq (${DEBUG}, 0)
	NS_BL2U_CFLAGS += -g
	NS_BL2U_CPPFLAGS += -g
	NS_BL2U_ASFLAGS += -g
endif

NS_BL2U_CFLAGS += -mgeneral-regs-only -mstrict-align														\
				-nostdinc -march=armv8-a -ffunction-sections -fdata-sections -ffreestanding -fno-builtin	\
				-fno-common -Os -std=gnu99 -fno-omit-frame-pointer -fno-stack-protector						\
				-DLOG_LEVEL=${LOG_LEVEL}
NS_BL2U_CPPFLAGS += -mgeneral-regs-only -mstrict-align														\
				-nostdinc -march=armv8-a -ffunction-sections -fdata-sections -ffreestanding -fno-builtin	\
				-fno-common -Os -std=gnu99 -fno-omit-frame-pointer -fno-stack-protector
NS_BL2U_ASFLAGS += -nostdinc -ffreestanding -Wa,--fatal-warnings	\
				-Werror -Wmissing-include-dirs						\
				-D__ASSEMBLY__ -mgeneral-regs-only -march=armv8-a

# Definition of # of suffix rule applies to extension
.SUFFIXES : .c .o .S

# linker flags
LDFLAGS := --fatal-warnings -O1 --gc-sections --build-id=none

MAKE_DEP = -Wp,-MD,$(DEP) -MT $$@
define MAKE_S
$(eval OBJ := $(NS_BL2U_OBJDIR)/$(patsubst %.S,%.o,$(notdir $(1))))
$(eval DEP := $(patsubst %.o,%.d,$(OBJ)))

$(OBJ):$(1)
	@if [ ! -e `dirname $$@` ]; then mkdir -p `dirname $$@`; fi
	$(CC) ${NS_BL2U_ASFLAGS} ${INCLUDE} $(MAKE_DEP) -c $$< -o $$@
-include $(DEP)
endef

define MAKE_C
$(eval OBJ := $(NS_BL2U_OBJDIR)/$(patsubst %.c,%.o,$(notdir $(1))))
$(eval DEP := $(patsubst %.o,%.d,$(OBJ)))
$(OBJ):$(1)
	@if [ ! -e `dirname $$@` ]; then mkdir -p `dirname $$@`; fi
	$(CC) $(NS_BL2U_CFLAGS) ${INCLUDE} $(MAKE_DEP) -c $$< -o $$@
-include $(DEP)
endef

define MAKE_LD

$(eval DEP := $(1).d)

$(1) : $(2)
	@if [ ! -e `dirname $$@` ]; then mkdir -p `dirname $$@`; fi
	$(CC) ${NS_BL2U_ASFLAGS} ${INCLUDE} -P -E $(MAKE_DEP) -o $$@ $$<

-include $(DEP)
endef

C_OBJS := $(filter %.c,$(NS_BL2U_SOURCES))
S_OBJS := $(filter %.S,$(NS_BL2U_SOURCES))



###################################################
# Command
.PHONY : all

all: $(NS_BL2U_OBJDIR) $(NS_BL2U_OUTDIR) $(NS_BL2U_OUTPUT)

# Make Directory
$(NS_BL2U_OBJDIR):
	mkdir -p $@

$(NS_BL2U_OUTDIR):
	mkdir -p $@

# Compile
$(eval $(foreach obj,$(C_OBJS),$(call MAKE_C,$(obj))))
$(eval $(foreach obj,$(S_OBJS),$(call MAKE_S,$(obj))))
$(eval $(call MAKE_LD,$(NS_BL2U_SCRIPTFILE),$(NS_BL2U_LINKERFILE)))

# Linker
$(NS_BL2U_OUTPUT): $(NS_BL2U_ELF) $(NS_BL2U_DUMP)
	$(OC) -O binary $(NS_BL2U_ELF) $(NS_BL2U_OUTPUT)

$(NS_BL2U_DUMP): $(NS_BL2U_ELF)
	@echo "  OD      $$@"
	${OD} -dx $< > $@

$(NS_BL2U_ELF):$(NS_BL2U_SCRIPTFILE) $(NS_BL2U_OBJECTS) 
	@echo "  LD      $$@"
	@echo  'const char build_message[] = "Built : "__TIME__", "__DATE__; \
			const char version_string[] = "${VERSION_STRING}";' | \
	$(CC) $(NS_BL2U_CFLAGS) -xc -c - -o $(NS_BL2U_OBJDIR)/build_message.o
	$(LD) -o $@ $(LDFLAGS) -Map=${NS_BL2U_MAPFILE} \
	--script $(NS_BL2U_SCRIPTFILE) $(NS_BL2U_OBJECTS) $(NS_BL2U_OBJDIR)/build_message.o

###################################################
# Clean
.PHONY: clean
clean:
	rm -rf $(NS_BL2U_OBJDIR) $(NS_BL2U_OUTDIR)


