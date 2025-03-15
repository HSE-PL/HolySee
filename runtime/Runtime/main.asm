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
  mov rdx, rsp
  lea rcx, [table]
  _call __rt_init
  mov rax, 60
  mov rdi, 7
  syscall

main:
push 0x12345678
  push 0x42
  push 0x42
  push 0x42
  push 0x42
  push 0x42
  mov rdi, 3000
  push 0x228
  _call __halloc
  push 0x337
;  jmp $
  _call __GC
  push 0x24126
  push 0x23126
  push 0x22126
  mov rax, [spd]
  test rax, [rax]

  mov rax, 60
  mov rdi, 42
  syscall
  jmp $

section .data
spd:
  dq 0
section .rodata
table:
dq 2 ; size
dq 0x10 ; sizeof linkedlist (128)
dq 0x18 ; sizeof heap (129)

