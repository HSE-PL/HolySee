src := ./hello.nasm
obj := ./build/exec.o
exe := ./build/exe

run: link
	$(exe)

link: objComp
	ld $(obj) -o $(exe)

objComp: 
	nasm -felf64 -g -F dwarf $(src) -o $(obj)

exedir/:
	@mkdir build/

