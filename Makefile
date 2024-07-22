 #Developed by Diogo Cochicho

CC:=gcc
AR:=ar

SRC:=src
BIN:=bin
OBJ:=bin
INCLUDE:=include

CFLAGS:=
CPPFLAGS:=-I$(INCLUDE)

TARGET:=$(BIN)/autometalbench
TARGET_API:=$(BIN)/libapi.a
SRC_FILES:=$(wildcard $(SRC)/*.c)
API_SRC_FILES:=$(wildcard $(SRC)/api/*.c)
OBJ_FILES:=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRC_FILES))
API_OBJ_FILES:=$(patsubst $(SRC)/api/%.c, $(OBJ)/%.o, $(API_SRC_FILES))

.PHONY: clean all 

all: $(TARGET) $(TARGET_API)

$(TARGET): $(OBJ_FILES)
	$(CC) -Wall $^ -o $@

$(TARGET_API): $(API_OBJ_FILES)
	$(AR) rcs $@ $^  

-include $(patsubst $(OBJ)/%.o, $(OBJ)/%.d, $(OBJ_FILES))
#-include $(patsubst $(OBJ)/%.o, $(OBJ)/%.d, $(API_OBJ_FILES))
$(OBJ)/%.o: $(SRC)/%.c 
	$(CC) -c -Wall -g -O3 -MMD $(CPPFLAGS) $< -o $@

$(OBJ)/%.o: $(SRC)/api/%.c 
	$(CC) -c -Wall -fPIC -O3 -MMD $(CPPFLAGS) $< -o $@

clean:
	@rm -v $(BIN)/*
