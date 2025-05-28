section .data
hello: db 'hi', 10
helen: equ $-hello

section .text
%include "lol.nasm"
global _start

%macro debug 0
  mov rax, 1
  mov rdi, 1
  mov rsi, hello
  mov rdx, helen
  syscall
%endmacro

_start:
  call start 
  mov rax, 0x3c
  syscall
