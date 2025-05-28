section .data

section .text
%include "lol.nasm"
global _start

_start:
  call start 
  mov rax, 0x3c
  syscall
