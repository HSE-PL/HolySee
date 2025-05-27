CC := zig c++
CCFLAGS := -std=c++23 -Wall -lasan -fsanitize=leak -fsanitize=address -fsanitize=undefined

SRC := $(shell find src/ -name '*.cpp')
EXE := ./build/exec

ASMSRC := main.nasm
OBJ := main.o
BIN := bin

run: cmp
	$(EXE) input.txt --dump-ir

asm:
	nasm -felf64 -g -F dwarf $(ASMSRC) -o $(OBJ) 
	ld $(OBJ) -o $(BIN)
	./$(BIN)

asmclean:
	rm $(BIN)
	rm $(OBJ)

clean:
	rm $(EXE)

build/:
	@mkdir build/

cmp: build/
	@$(CC) $(CCFLAGS) $(SRC) -o $(EXE)
