 #Developed by Diogo Cochicho

CC:=gcc

SRC:=src
BIN:=bin
OBJ:=bin
INCLUDE:=include

CFLAGS:=
CPPFLAGS:=-I$(INCLUDE)

TARGET:=$(BIN)/autometalbench
SRC_FILES:=$(wildcard $(SRC)/*.c)
OBJ_FILES:=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRC_FILES))

.PHONY: clean all 

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) -Wall $^ -o $@

-include $(patsubst $(OBJ)/%.o, $(OBJ)/%.d, $(OBJ_FILES))
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c -Wall -g -O3 -MMD $(CPPFLAGS) $< -o $@

clean:
	@rm -v $(BIN)/*
