RM := rm
CC := aarch64-none-elf-gcc
LD := aarch64-none-elf-ld
OBJCOPY := aarch64-none-elf-objcopy

AFLAGS := -c -Wall -MMD
CPFLAGS := -c -MMD -O0 -Wall -nostdlib

SRCDIR=src/
INCLUDEDIR=./include/ ./ ../bsp/include/ /usr/aarch64-none-elf/include/
LIBDIR=../bsp/bin/
RELEASEDIR=./bin/

INCLUDEPATH=$(foreach inc, $(INCLUDEDIR), -I$(inc))
LIBPATH=$(foreach lib, $(LIBDIR), -L$(lib))

LIBS=-Wl,--start-group,$(patsubst lib%,-l%,$(basename $(notdir $(foreach libdir,$(LIBDIR),$(wildcard $(libdir)*.a)))),)--end-group

OBJS=$(addsuffix .o, $(addprefix $(RELEASEDIR), $(basename $(notdir $(wildcard $(SRCDIR)*.c) $(wildcard $(SRCDIR)*.S)))))

TARGET=kernel8.img

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) lscript.ld $(TRANSLATION_TABLE)
	@echo 'Building target: $@'
	$(LD) -T lscript.ld -o $(RELEASEDIR)$(@:.img=.elf) $(OBJS) $(LIBPATH) -lbsp
	@$(OBJCOPY) $(RELEASEDIR)$(@:.img=.elf) -O binary $(RELEASEDIR)$@
	@echo 'Finished building target: $@'

$(RELEASEDIR)%.o: $(SRCDIR)%.c
	@echo 'Building file: $<'
	@$(CC) -Wall -O0 -c $(CPFLAGS) $(INCLUDEPATH) -o $@ $<
	@echo 'Finished building: $<'

$(RELEASEDIR)%.o: $(SRCDIR)%.S
	@echo 'Building file: $<'
	@$(CC) -Wall -O0 -c $(AFLAGS) $(INCLUDEPATH) -o $@ $<
	@echo 'Finished building: $<'


clean:
	$(RM) $(RELEASEDIR)*

