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
  push 0x228

  mov rdi, 130
  _call __halloc
  push rax
  push 0x0
  mov rdi, 131
  _call __halloc
  push rax

  push 0x232
  mov rdi, 132
  _call __halloc
  mov rdi, 133
  _call __halloc
  mov r12, rax
  mov rdi, 128
  _call __halloc
  mov rdi, 42
  mov rsi, rax
  shl rsi, 16
  shr rsi, 16
  mov qword [rsi], rdi
;  jmp $
  mov qword [rsi + 8], r12
  push rax
  push 0x337
;  jmp $
;  _call __GC
  push 0x24126
  push 0x23126
  push 0x22126
  push 0x7c00
;  jmp $
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
dq 6 ; size
dq 0x10 ; sizeof linkedlist (128)
dq 0x18 ; sizeof heap (129)
dq 0x2f00 ; 130 (82)
dq 0x1e00 ; 131 (83)
dq 0x1d00 ; 132 (84)
dq 0x0c00 ; 133 (85)
