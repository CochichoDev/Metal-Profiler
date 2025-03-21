 #Developed by Diogo Cochicho

CC:=gcc
AR:=ar

CP:=cp

SRC:=src
BIN:=bin
OBJ:=obj
LIB:=lib
INCLUDE:=include include/api
ARCH:=arch

LIBS:=dl crypto pthread m TUI
CFLAGS:=
CPPFLAGS:=$(foreach inc, $(INCLUDE), -I$(inc)) -fno-stack-protector 
LFLAGS:=-L$(LIB) $(foreach lib,$(LIBS),-l$(lib)) -Wl,--no-as-needed -lAMBapi -lcjson

TARGET:=$(BIN)/autometalbench
TARGET_API:=$(BIN)/libAMBapi.so
SRC_FILES:=$(wildcard $(SRC)/*.c)
API_SRC_FILES:=$(wildcard $(SRC)/api/*.c)
OBJ_FILES:=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRC_FILES))
API_OBJ_FILES:=$(patsubst $(SRC)/api/%.c, $(OBJ)/%.o, $(API_SRC_FILES))

ARCH_LIBS:=$(foreach module, $(sort $(dir $(wildcard $(ARCH)/*/module/))), $(module)lib)

.PHONY: clean all 

all: $(TARGET)

$(TARGET): $(OBJ_FILES) $(TARGET_API)
	$(CC) -Wall $(filter %.o, $^) -o $@ $(LFLAGS)

$(TARGET_API): $(API_OBJ_FILES)
	$(CC) -Wall -shared $^ -o $(TARGET_API)
	$(CP) $@ $(LIB)/$(notdir $@)
	tee $(foreach dir, $(ARCH_LIBS), $(dir)/$(notdir $@)) < $@ > /dev/null

-include $(patsubst $(OBJ)/%.o, $(OBJ)/%.d, $(OBJ_FILES))
-include $(patsubst $(OBJ)/%.o, $(OBJ)/%.d, $(API_OBJ_FILES))
$(OBJ)/%.o: $(SRC)/%.c 
	$(CC) -c -Wall -g -O0 -g -MMD $(CPPFLAGS) $< -o $@

$(OBJ)/%.o: $(SRC)/api/%.c 
	$(CC) -c -Wall -fPIC -O0 -g -MMD $(CPPFLAGS) $< -o $@

clean:
	@rm -v $(BIN)/*
	@rm -v $(OBJ)/*
