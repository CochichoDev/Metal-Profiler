RM := rm -rf

AFLAGS ?=

CC=aarch64-none-elf-gcc

SRCDIR=./src/
INCLUDEDIR=./include/ ./ ../bsp/include/ /usr/aarch64-none-elf/include/
LIBDIR=../bsp/bin/
RELEASEDIR=./bin/

INCLUDEPATH=$(foreach inc, $(INCLUDEDIR), -I$(inc))
LIBPATH=$(foreach lib, $(LIBDIR), -L$(lib))

LIBS=-Wl,--start-group,$(patsubst lib%,-l%,$(basename $(notdir $(foreach libdir,$(LIBDIR),$(wildcard $(libdir)*.a)))),)--end-group

OBJS=$(addsuffix .o, $(addprefix $(RELEASEDIR), $(basename $(notdir $(wildcard $(SRCDIR)*.c) $(wildcard $(SRCDIR)*.S)))))
TRANSLATION_TABLE=$(LIBDIR)translation_table.o

TARGET=LOADER.elf

.PHONY: all clean

all:$(RELEASEDIR)$(TARGET)
$(RELEASEDIR)$(TARGET): $(OBJS) lscript.ld $(TRANSLATION_TABLE)
	@echo 'Building target: $@'
	$(CC) -Wl,-T -Wl,lscript.ld -Wl,--build-id=none $(LIBPATH) -nostdlib -o $@ $(OBJS) $(TRANSLATION_TABLE)
	@echo 'Finished building target: $@'

$(RELEASEDIR)%.o: $(SRCDIR)%.c
	@echo 'Building file: $<'
	$(CC) -Wall -O0 -c $(AFLAGS) $(INCLUDEPATH) -o $@ $<
	@echo 'Finished building: $<'

$(RELEASEDIR)%.o: $(SRCDIR)%.S
	@echo 'Building file: $<'
	$(CC) -Wall -O0 -c $(AFLAGS) $(INCLUDEPATH) -o $@ $<
	@echo 'Finished building: $<'


clean:
	$(RM) $(RELEASEDIR)*

