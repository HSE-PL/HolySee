CC := zig c++
# CC := clang++ 
# CCFLAGS := -std=c++23 -Wall -fsanitize=leak -fsanitize=address -fsanitize=undefined
CCFLAGS := -std=c++23 -Wall -lasan -fsanitize=leak -fsanitize=address -fsanitize=undefined

SRC := $(shell find . -name '*.cpp')
EXE := ./build/exec

run: cmp
	@cat ./examples/fib.json | $(EXE)

test: cmp
	# @cat ./examples/tdce/double-pass.json | brili -p 2 2
	@cat ./examples/tdce/double-pass.json | $(EXE) | brili -p 2 2

jrun:
	@cat ./examples/tdce/simple.json | $(EXE)

clean:
	rm $(EXE)


build/:
	@mkdir build/

cmp: build/
	@$(CC) $(CCFLAGS) $(SRC) -o $(EXE)
