CC := clang++
CCFLAGS := -std=c++23 -Wall -fsanitize=leak -fsanitize=address

SRC := main.cpp json2ir.cpp ir.cpp

EXE := ./build/exec

run: cmp
	cat ./examples/ack.json | $(EXE)
	rm $(EXE)

build/:
	@mkdir build/

cmp: build/
	$(CC) $(CCFLAGS) $(SRC) -o $(EXE)
