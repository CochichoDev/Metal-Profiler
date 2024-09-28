RM := rm -rf

AFLAGS ?=

CC=aarch64-none-elf-gcc

SRCDIR=./src/
INCLUDEDIR=./include/ ./ ../bsp/include/ /usr/aarch64-none-elf/include/
LIBDIR=../bsp/bin/
RELEASEDIR=./bin/

INCLUDEPATH=$(foreach inc, $(INCLUDEDIR), -I$(inc))
LIBPATH=$(foreach lib, $(LIBDIR), -L$(lib))

LIBS=-Wl,--start-group,$(patsubst lib%,-l%,$(basename $(notdir $(foreach libdir,$(LIBDIR),$(wildcard $(libdir)*.a)))),)-lm,-lgcc,--end-group


SRCS=$(foreach dir,$(SRCDIR),$(wildcard $(dir)*.c) $(wildcard $(dir)*.S))
OBJS=$(addsuffix .o, $(addprefix $(RELEASEDIR), $(basename $(notdir $(SRCS))))) ../LOADER/bin/main.o

TARGET=Core1.elf

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

