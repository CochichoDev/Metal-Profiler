CP=cp
LIB=libbsp.a

CPFLAGS = -c -O1 -nostdlib -nostartfiles $(AFLAGS)

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
	$(AR) -r $@ $^

$(RELEASEDIR)%.o: $(SRCDIR)%.S
	$(CC) $(CPFLAGS) $(INCLUDES) -o $@ $<

$(RELEASEDIR)%.o: $(SRCDIR)%.c
	$(CC) $(CPFLAGS) $(INCLUDES) -o $@ $<

clean:
	rm -rf $(RELEASEDIR)*
