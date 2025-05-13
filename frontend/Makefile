CC := zig c++
CCFLAGS := -std=c++23 -Wall -lasan -fsanitize=leak -fsanitize=address -fsanitize=undefined

SRC := $(shell find src/ -name '*.cpp')
EXE := ./build/exec

run: cmp
	$(EXE) input.txt

clean:
	rm $(EXE)

build/:
	@mkdir build/

cmp: build/
	@$(CC) $(CCFLAGS) $(SRC) -o $(EXE)
