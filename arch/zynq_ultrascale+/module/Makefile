 #Developed by Diogo Cochicho

CC:=gcc

SRC:=src
BIN:=bin
OBJ:=bin
INCLUDE:=include ../../../include/api

CFLAGS:=
CPPFLAGS:=$(foreach incdir,$(INCLUDE),-I$(incdir))

TARGET:=$(BIN)/dmodule.so
SRC_FILES:=$(wildcard $(SRC)/*.c)
OBJ_FILES:=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRC_FILES))

.PHONY: clean all 

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) -Wall -shared $^ -o $@

-include $(patsubst $(OBJ)/%.o, $(OBJ)/%.d, $(OBJ_FILES))
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c -Wall -g -O3 -fpic -MMD $(CPPFLAGS) $< -o $@

clean:
	@rm -v $(BIN)/*
