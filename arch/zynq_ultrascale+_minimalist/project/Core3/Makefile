RM := rm -rf

AFLAGS ?=

CC=aarch64-none-elf-gcc

SRCDIR=./src/
INCLUDEDIR=./include/ ./ ../bsp/include/
LIBDIR=../bsp/bin/
RELEASEDIR=./bin/

INCLUDEPATH=$(foreach inc, $(INCLUDEDIR), -I$(inc))
LIBPATH=$(foreach lib, $(LIBDIR), -L$(lib))

LIBS=-Wl,--start-group,$(patsubst lib%,-l%,$(basename $(notdir $(foreach libdir,$(LIBDIR),$(wildcard $(libdir)*.a)))),)-lgcc,--end-group

OBJS=$(addsuffix .o, $(addprefix $(RELEASEDIR), $(basename $(notdir $(wildcard $(SRCDIR)*.c))))) ../LOADER/bin/main.o

TARGET=Core3.elf

.PHONY: all clean

all:$(RELEASEDIR)$(TARGET)
$(RELEASEDIR)$(TARGET): $(OBJS) lscript.ld
	@echo 'Building target: $@'
	$(CC) -Wl,-T -Wl,lscript.ld -Wl,--build-id=none $(LIBPATH) -nostdlib -o $@ $(OBJS) $(LIBS) 
	@echo 'Finished building target: $@'

$(RELEASEDIR)%.o: $(SRCDIR)%.c
	@echo 'Building file: $<'
	$(CC) -Wall -O1 -c $(AFLAGS) $(INCLUDEPATH) -o $@ $<
	@echo 'Finished building: $<'



clean:
	$(RM) $(RELEASEDIR)*

