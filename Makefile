 #Developed by Diogo Cochicho

CC:=gcc
AR:=ar

CP:=cp

SRC:=src
BIN:=bin
OBJ:=obj
LIB:=lib
INCLUDE:=include

CFLAGS:=
CPPFLAGS:=-I$(INCLUDE)
LFLAGS:=-L$(BIN)

TARGET:=$(BIN)/autometalbench
TARGET_API:=$(BIN)/libAMBapi.so
SRC_FILES:=$(wildcard $(SRC)/*.c)
API_SRC_FILES:=$(wildcard $(SRC)/api/*.c)
OBJ_FILES:=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRC_FILES))
API_OBJ_FILES:=$(patsubst $(SRC)/api/%.c, $(OBJ)/%.o, $(API_SRC_FILES))

.PHONY: clean all 

all: $(TARGET)

$(TARGET): $(OBJ_FILES) $(TARGET_API)
	$(CC) $(LFLAGS) -lAMBapi -Wall $(filter %.o, $^) -o $@

$(TARGET_API): $(API_OBJ_FILES)
	$(CC) -Wall -shared $^ -o $(TARGET_API)
	$(CP) $@ $(LIB)/$(notdir $@)


#$(TARGET_API): $(API_OBJ_FILES)
#	$(AR) rcs $@ $^  

-include $(patsubst $(OBJ)/%.o, $(OBJ)/%.d, $(OBJ_FILES))
-include $(patsubst $(OBJ)/%.o, $(OBJ)/%.d, $(API_OBJ_FILES))
$(OBJ)/%.o: $(SRC)/%.c 
	$(CC) -c -Wall -g -O3 -MMD $(CPPFLAGS) $< -o $@

$(OBJ)/%.o: $(SRC)/api/%.c 
	$(CC) -c -Wall -fPIC -O3 -MMD $(CPPFLAGS) $< -o $@

clean:
	@rm -v $(BIN)/*
	@rm -v $(OBJ)/*
