# MODIFIED BY DIOGO COCHICHO
###############################################################################
# Copyright (c) 2014 - 2020 Xilinx, Inc.  All rights reserved.
# SPDX-License-Identifier: MIT
###############################################################################
DRIVER_LIB_VERSION = v1.0

include config.make
COMPILER=aarch64-none-elf-gcc
ARCHIVER=aarch64-none-elf-ar
CC=$(COMPILER)
AR=$(ARCHIVER)
CP=cp
COMPILER_FLAGS= -c
EXTRA_COMPILER_FLAGS?=
LIB=libxil.a
CC_FLAGS = $(subst -pg, -DPROFILING, $(COMPILER_FLAGS))
ECC_FLAGS = $(subst -pg, -DPROFILING, $(EXTRA_COMPILER_FLAGS))

ECC_FLAGS	+= -nostartfiles -march=armv8-a
ECC_FLAGS_NO_FLTO1 = $(subst -flto,,$(ECC_FLAGS))
ECC_FLAGS_NO_FLTO = $(subst -ffat-lto-objects,,$(ECC_FLAGS_NO_FLTO1))

COPYDIR0=../../../../../../../export/Meta/sw/Meta/standalone_domain/bsplib/lib/
COPYDIR1=../../../../../../../export/Meta/sw/Meta/standalone_psu_cortexa53_1/bsplib/lib/
COPYDIR2=../../../../../../../export/Meta/sw/Meta/standalone_psu_cortexa53_2/bsplib/lib/
COPYDIR3=../../../../../../../export/Meta/sw/Meta/standalone_psu_cortexa53_3/bsplib/lib/
COPYDIRINCLUDE=../../../../../../../export/Meta/sw/Meta/standalone_domain/bsplib/lib/
RELEASEDIR=../../../lib/
INCLUDEDIR=../../../include/
INCLUDES=-I./. -I${INCLUDEDIR}

SRCFILES:=$(wildcard *.c)

ASSEMBLY_SRCFILES:=$(wildcard *.S)

OBJECTS =	$(addprefix $(RELEASEDIR),$(addsuffix .o, $(basename $(filter-out _exit.c, $(wildcard *.c)))))

ASSEMBLY_OBJECTS  = $(addprefix $(RELEASEDIR),$(addsuffix .o, $(basename $(wildcard *.S))))

EXIT_OBJ = $(addprefix $(RELEASEDIR),$(addsuffix .o,$(basename _exit.c)))


INCLUDEFILES=*.h
INCLUDEFILES+=includes_ps/*.h
#libs: $(LIBS)

$(LIB): $(OBJECTS) $(ASSEMBLY_OBJECTS) $(EXIT_OBJ)
	$(ARCHIVER) -r $(RELEASEDIR)$@ $^
	$(CP) $(RELEASEDIR)$@ $(COPYDIR0)
	$(CP) $(RELEASEDIR)$@ $(COPYDIR1)
	$(CP) $(RELEASEDIR)$@ $(COPYDIR2)
	$(CP) $(RELEASEDIR)$@ $(COPYDIR3)

#standalone_libs: $(OBJECTS) $(ASSEMBLY_OBJECTS) $(EXIT_OBJ)

DEPFILES := $(SRCFILES:%.c=$(RELEASEDIR)%.d)

include $(wildcard $(DEPFILES))

ASSEMBLY_DEPFILES := $(ASSEMBLY_SRCFILES:%.S=$(RELEASEDIR)%.d)

include $(wildcard $(ASSEMBLY_DEPFILES))

include $(wildcard ../../../../dep.mk)

$(RELEASEDIR)_exit.o: _exit.c
	$(CC) $(CC_FLAGS) $(ECC_FLAGS_NO_FLTO) $(INCLUDES)  $(DEPENDENCY_FLAGS) -o $@ $<
	$(CP) $@ $(COPYDIR)

$(RELEASEDIR)%.o: %.S
	echo $(ECC_FLAGS)
	$(CC) $(CC_FLAGS) $(ECC_FLAGS) $(INCLUDES)  $(DEPENDENCY_FLAGS) -o $@ $<
	$(CP) $@ $(COPYDIR)

$(RELEASEDIR)%.o: %.c
	$(CC) $(CC_FLAGS) $(ECC_FLAGS) $(INCLUDES) $(DEPENDENCY_FLAGS) -o $@ $<
	$(CP) $@ $(COPYDIR)

.PHONY: include
include: $(addprefix $(INCLUDEDIR),$(wildcard *.h)) $(addprefix $(INCLUDEDIR), $(subst includes_ps/,,$(wildcard includes_ps/*.h)))



$(INCLUDEDIR)%.h: includes_ps/%.h
	$(CP) $< $@



$(INCLUDEDIR)%.h: %.h
	$(CP) $< $@

clean:
	rm -rf ${OBJECTS}
	rm -rf ${ASSEMBLY_OBJECTS}
	rm -rf $(DEPFILES)
	rm -rf $(ASSEMBLY_DEPFILES)
	rm -rf $(EXIT_OBJ)
