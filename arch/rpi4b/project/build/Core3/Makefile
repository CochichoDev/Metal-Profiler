RM := rm -rf

AFLAGS ?=

SRCDIR=./src/
INCLUDEDIR=./include/ ./ ../bsp/include/ /usr/aarch64-none-elf/include/
LIBDIR=../bsp/bin/
RELEASEDIR=./bin/

INCLUDEPATH=$(foreach inc, $(INCLUDEDIR), -I$(inc))
LIBPATH=$(foreach lib, $(LIBDIR), -L$(lib))

LIBS=-Wl,--start-group,$(patsubst lib%,-l%,$(basename $(notdir $(foreach libdir,$(LIBDIR),$(wildcard $(libdir)*.a)))),)-lgcc,--end-group

OBJS=$(addsuffix .o, $(addprefix $(RELEASEDIR), $(basename $(notdir $(wildcard $(SRCDIR)*.c))))) 

TARGET=Core3.elf

.PHONY: all clean

all:$(RELEASEDIR)$(TARGET)
$(RELEASEDIR)$(TARGET): $(OBJS) lscript.ld 
	@echo 'Building target: $@'
	$(CC) -Wl,-T -Wl,lscript.ld -Wl,--nmagic,--build-id=none $(LIBPATH) -static -nostdlib -o $@ $(OBJS) $(LIBS) 
	@echo 'Finished building target: $@'

$(RELEASEDIR)%.o: $(SRCDIR)%.c
	@mkdir -p ./bin
	@echo 'Building file: $<'
	$(CC) -Wall -O1 -c $(AFLAGS) $(INCLUDEPATH) -o $@ $<
	@echo 'Finished building: $<'



clean:
	$(RM) $(RELEASEDIR)*

