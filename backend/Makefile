CC := zig c++ 
CCFLAGS := -std=c++23 -Wall -lasan -fsanitize=leak -fsanitize=address

SRC := $(shell find . -name '*.cpp')
EXE := ./build/exec

run: cmp
	cat ./examples/ack.json | $(EXE)

jrun:
	cat ./examples/ack.json | $(EXE)

clean:
	rm $(EXE)


build/:
	@mkdir build/

cmp: build/
	$(CC) $(CCFLAGS) $(SRC) -o $(EXE)
