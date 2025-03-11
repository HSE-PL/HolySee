[BITS 64]
extern __rt_init
extern __halloc
extern __GC
global start

section .text
start:
  lea rdi, [main]
  lea rsi, [spd]
  call __rt_init

main:
  mov rdi, 3000
  call __halloc ;<-------
  jmp $
  push rax
;  jmp $
  push 42
  push 228
  ret
;  call __GC
  mov rax, [spd]
  test rax, [rax]

  mov rax, 60
  mov rdi, 42
  syscall

section .data
spd:
  dq 0
