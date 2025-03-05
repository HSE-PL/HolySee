CC := zig c++ 
CCFLAGS := -std=c++23 -Wall -lasan -fsanitize=leak -fsanitize=address

SRC := $(shell find . -name '*.cpp')
EXE := ./build/exec

run: cmp
	@cat ./examples/tdce/double-pass.json | $(EXE)

test: cmp
	@cat ./examples/tdce/simple.json | $(EXE) | brili -p

jrun:
	@cat ./examples/tdce/simple.json | $(EXE)

clean:
	rm $(EXE)


build/:
	@mkdir build/

cmp: build/
	@$(CC) $(CCFLAGS) $(SRC) -o $(EXE)
