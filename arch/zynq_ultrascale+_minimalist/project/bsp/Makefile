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

SRCDIR=./src/
RELEASEDIR=./bin/
INCLUDEDIR=./include/ /usr/aarch64-none-elf/include/
INCLUDES=-I./. $(foreach inc, $(INCLUDEDIR), -I$(inc))

SRCFILES:=$(wildcard $(SRCDIR)*.c)

ASSEMBLY_SRCFILES:=$(wildcard $(SRCDIR)*.S)

OBJECTS =	$(addprefix $(RELEASEDIR),$(addsuffix .o, $(basename $(notdir $(wildcard $(SRCDIR)*.c)))))

ASSEMBLY_OBJECTS  = $(addprefix $(RELEASEDIR),$(addsuffix .o, $(basename $(notdir $(wildcard $(SRCDIR)*.S)))))

INCLUDEFILES=$(foreach inc, $(INCLUDEDIR), $(wildcard $(inc)*.h))

.PHONY: all clean

all: $(RELEASEDIR)$(LIB)
$(RELEASEDIR)$(LIB): $(OBJECTS) $(ASSEMBLY_OBJECTS)
	$(ARCHIVER) -r $@ $^

$(RELEASEDIR)%.o: $(SRCDIR)%.S
	echo $(ECC_FLAGS)
	$(CC) $(CFLAGS) $(CC_FLAGS) $(ECC_FLAGS) $(INCLUDES)  $(DEPENDENCY_FLAGS) -o $@ $<

$(RELEASEDIR)%.o: $(SRCDIR)%.c
	$(CC) $(CFLAGS) $(CC_FLAGS) $(ECC_FLAGS) $(INCLUDES) $(DEPENDENCY_FLAGS) -o $@ $<


clean:
	rm -rf ${OBJECTS}
	rm -rf ${ASSEMBLY_OBJECTS}
	rm -rf $(EXIT_OBJ)
	rm -rf $(RELEASEDIR)$(LIB)
