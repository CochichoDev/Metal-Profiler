RM := rm
CC ?= aarch64-none-elf-gcc
LD ?= aarch64-none-elf-ld
CP := cp
OBJCOPY ?= aarch64-none-elf-objcopy

FIRMWARE_DIR := firmware
SRC_DIR := src
STUB_DIR := armstub
BIN_DIR := bin
INCLUDE_DIR := include

DEPLOY_DIR := /home/cochicho/mnt

CPPFLAGS := -I$(INCLUDE_DIR) -D__arch64__
CPFLAGS := -c -MMD -O0 -Wall -nostdlib
ASFLAGS := -c -Wall -MMD
LFLAGS := 

TARGET := kernel8.img
STUBBIN := armstub8.bin

ELF := $(TARGET:.img=.elf)
STUBELF := $(STUBBIN:.bin=.elf)

SRC := $(wildcard $(SRC_DIR)/*.c)
SRC += $(wildcard $(SRC_DIR)/*.S)
OBJ := $(foreach src,$(basename $(SRC)),$(addprefix $(BIN_DIR)/,$(notdir $(src))))
OBJ := $(foreach src,$(OBJ),$(addsuffix .o,$(src)))
DEP := $(OBJ:.o=.d)

STUBSRC := $(wildcard $(STUB_DIR)/*.S)
STUBOBJ := $(STUBSRC:.S=.o)
STUBOBJ := $(foreach obj,$(STUBOBJ),$(addprefix $(BIN_DIR)/,$(notdir $(obj))))

LSCRIPT := lscript.ld

.PHONY: clean deploy all

#all: $(TARGET) $(STUBBIN)
all: $(TARGET)


$(STUBBIN): $(STUBOBJ)
	$(LD) --section-start=.text=0 -o $(BIN_DIR)/armstub8.elf $<
	$(OBJCOPY) $(BIN_DIR)/armstub8.elf -O binary $(BIN_DIR)/$@
	@echo "STUB Constructed"

$(BIN_DIR)/%.o: $(STUB_DIR)/%.S
	$(CC) $(CPFLAGS) -MMD -c $< -o $@
	@echo "STUB Compiled"

$(TARGET): $(BIN_DIR)/$(ELF)
	@$(OBJCOPY) -O binary $< $(BIN_DIR)/$@
	@echo "$(TARGET) was generated"

$(BIN_DIR)/$(ELF): $(OBJ) $(LSCRIPT)
	@$(LD) $(LFLAGS) -T $(LSCRIPT) $(filter $(OBJ),$^) -o $@
	@echo "$@ was linked"

-include $(DEP)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.S
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CPPFLAGS) $(ASFLAGS) $< -o $@
	@echo "$< was compiled"

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CPPFLAGS) $(CPFLAGS) $< -o $@
	@echo "$< was compiled"

deploy: $(BIN_DIR)/$(TARGET) $(FIRMWARE_DIR)/config.txt
	$(RM) -f $(DEPLOY_DIR)/*
	$(CP) $^ $(DEPLOY_DIR)/
	$(CP) $(FIRMWARE_DIR)/start4.elf $(DEPLOY_DIR)/
	$(CP) $(FIRMWARE_DIR)/fixup4.dat $(DEPLOY_DIR)/
	sync
	@echo "Kernel deployed"

clean:
	@$(RM) -f $(BIN_DIR)/*
	@echo "All cleaned"
