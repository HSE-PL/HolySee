[BITS 64]
default rel
extern __rt_init
extern __halloc
extern __GC
global start
global main

%macro _call 1
    and rsp, -16
    call %1
%endmacro

section .text
start:
  lea rdi, [main]
  lea rsi, [spd]
  call __rt_init

main:
  mov rdi, 3000
  _call __halloc
  jmp $
  _call __GC
  mov rax, [spd]
  test rax, [rax]

  mov rax, 60
  mov rdi, 42
  syscall

section .data
spd:
  dq 0
