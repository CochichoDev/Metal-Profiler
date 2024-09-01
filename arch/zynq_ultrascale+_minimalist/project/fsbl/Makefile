CC := aarch64-none-elf-gcc
CC_FLAGS := -MMD -MP      -Wall -fmessage-length=0 -DARMA53_64 -Os -flto -ffat-lto-objects  
CFLAGS := 
BSP_FLAGS :=  -c
LN_FLAGS := -Wl,--start-group,-lxil,-lgcc,-lc,--end-group -Wl,--start-group,-lxilffs,-lxil,-lgcc,-lc,--end-group -Wl,--start-group,-lxilsecure,-lxil,-lgcc,-lc,--end-group -Wl,--start-group,-lxilpm,-lxil,-lgcc,-lc,--end-group                                                                                                                -n  -Wl,--gc-sections

SRCDIR=./src/
LIBDIR=./lib/
RELEASEDIR=./bin/
INCLUDEDIR=./include/ ./ ../bsp/include/

c_SOURCES := $(wildcard $(SRCDIR)*.c)
S_SOURCES := $(wildcard $(SRCDIR)*.S)
s_SOURCES := $(wildcard $(SRCDIR)*.s)
INCLUDES := $(foreach inc, $(INCLUDEDIR), $(wildcard $(inc)*.h))
OBJS := $(addprefix $(RELEASEDIR), $(notdir $(patsubst %.c, %.o, $(c_SOURCES))))
OBJS += $(addprefix $(RELEASEDIR), $(notdir $(patsubst %.S, %.o, $(S_SOURCES))))
OBJS += $(addprefix $(RELEASEDIR), $(notdir $(patsubst %.s, %.o, $(s_SOURCES))))
LSCRIPT := -Tlscript.ld

LIBA := $(wildcard $(LIBDIR)*.a)
LIBS := ../bsp/bin/libxil.a $(LIBA)
EXEC := fsbl_a53.elf

INCLUDEPATH := $(foreach inc, $(INCLUDEDIR), -I$(inc))
LIBPATH := $(foreach lib, $(LIBS), -L$(dir $(lib)))

all: $(EXEC)

$(EXEC): $(LIBS) $(OBJS) $(INCLUDES)
	$(CC) -o $@ $(sort $(OBJS)) $(CC_FLAGS) $(CFLAGS) $(LN_FLAGS) $(LIBPATH) $(LSCRIPT)

$(RELEASEDIR)%.o:$(SRCDIR)%.c
	$(CC) $(CFLAGS) $(BSP_FLAGS) $(CC_FLAGS) -c $< -o $@ $(INCLUDEPATH)

$(RELEASEDIR)%.o:$(SRCDIR)%.S
	$(CC) $(CFLAGS) $(BSP_FLAGS) $(CC_FLAGS) -c $< -o $@ $(INCLUDEPATH)

$(RELEASEDIR)%.o:$(SRCDIR)%.s
	$(CC) $(CFLAGS) $(BSP_FLAGS) $(CC_FLAGS) -c $< -o $@ $(INCLUDEPATH)

clean:
	rm -rf $(OBJS) $(EXEC) *.o

-include $(DEPFILES)
