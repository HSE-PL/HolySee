extern __rt_init
extern __halloc
global start
extern printf
global main
section .text
start:
.start:
  mov rdi, main
  mov rsi, spd
  mov rdx, rsp
  mov rcx, Struct
  mov r8, Struct2
  mov r9, 262144
  call __rt_init

  mov rax, 60
  mov rdi, 0
  syscall
hehe:
  push rbp
  mov rbp, rsp
  sub rsp, 32
  mov qword [rbp - 8], rdi
.hehe:
  mov qword [rbp - 16], 1
  mov r15, qword [rbp - 8]
  mov r14, qword [rbp - 16]
  add r15, r14
  mov qword [rbp - 24], r15
  mov rax, qword [rbp - 24]
  mov rsp, rbp
  pop rbp
  ret
fib:
  push rbp
  mov rbp, rsp
  sub rsp, 176
  mov qword [rbp - 8], rdi
.fib:
  mov qword [rbp - 16], 1
  mov r14, qword [rbp - 8]
  mov r15, qword [rbp - 16]
  xor r13, r13
  cmp r14, r15
  setle r13b
  mov qword [rbp - 24], r13
  mov r15, qword [rbp - 24]
  test r15, r15
  je .if.end1
  jne .if.true1
.if.true1:
  mov qword [rbp - 32], 1
  mov rax, qword [rbp - 32]
  mov rsp, rbp
  pop rbp
  ret
  mov rax, [spd]
  test rax, [rax]
  jmp .if.end1
.if.end1:
  mov rdi, Struct2
  call __halloc
  mov qword [rbp - 40], rdi
  mov rdi, Struct2
  call __halloc
  mov qword [rbp - 48], rdi
  mov rdi, Struct2
  call __halloc
  mov qword [rbp - 56], rdi
  mov rdi, Struct2
  call __halloc
  mov qword [rbp - 64], rdi
  mov rdi, Struct2
  call __halloc
  mov qword [rbp - 72], rdi
  mov rdi, Struct2
  call __halloc
  mov qword [rbp - 80], rdi
  mov rdi, Struct2
  call __halloc
  mov qword [rbp - 88], rdi
  mov rdi, Struct2
  call __halloc
  mov qword [rbp - 96], rdi
  mov rdi, Struct2
  call __halloc
  mov qword [rbp - 104], rdi
  mov qword [rbp - 112], 2
  mov r15, qword [rbp - 8]
  mov r14, qword [rbp - 112]
  sub r15, r14
  mov qword [rbp - 120], r15
  mov rdi, qword [rbp - 120]
  call fib
  mov qword [rbp - 128], rax
  mov rax, [spd]
  test rax, [rax]
  mov qword [rbp - 136], 1
  mov r14, qword [rbp - 8]
  mov r15, qword [rbp - 136]
  sub r14, r15
  mov qword [rbp - 144], r14
  mov rdi, qword [rbp - 144]
  call fib
  mov qword [rbp - 152], rax
  mov rax, [spd]
  test rax, [rax]
  mov r15, qword [rbp - 152]
  mov r14, qword [rbp - 128]
  add r15, r14
  mov qword [rbp - 160], r15
  mov rax, qword [rbp - 160]
  mov rsp, rbp
  pop rbp
  ret
fact:
  push rbp
  mov rbp, rsp
  sub rsp, 80
  mov qword [rbp - 8], rdi
.fact:
  mov qword [rbp - 16], 1
  mov r14, qword [rbp - 8]
  mov r15, qword [rbp - 16]
  xor r13, r13
  cmp r14, r15
  setle r13b
  mov qword [rbp - 24], r13
  mov r15, qword [rbp - 24]
  test r15, r15
  je .if.end0
  jne .if.true0
.if.true0:
  mov qword [rbp - 32], 1
  mov rax, qword [rbp - 32]
  mov rsp, rbp
  pop rbp
  ret
  mov rax, [spd]
  test rax, [rax]
  jmp .if.end0
.if.end0:
  mov qword [rbp - 40], 1
  mov r15, qword [rbp - 8]
  mov r14, qword [rbp - 40]
  sub r15, r14
  mov qword [rbp - 48], r15
  mov rdi, qword [rbp - 48]
  call fact
  mov qword [rbp - 56], rax
  mov rax, [spd]
  test rax, [rax]
  mov r14, qword [rbp - 56]
  mov r15, qword [rbp - 8]
  imul r14, r15
  mov qword [rbp - 64], r14
  mov rax, qword [rbp - 64]
  mov rsp, rbp
  pop rbp
  ret
main:
  push rbp
  mov rbp, rsp
  sub rsp, 144
.main:
  mov qword [rbp - 8], 0
  mov r15, qword [rbp - 8]
  mov qword [rbp - 16], r15
  mov qword [rbp - 24], 0
  mov r15, qword [rbp - 24]
  mov qword [rbp - 32], r15
  mov qword [rbp - 40], 13
  mov rdi, qword [rbp - 40]
  call hehe
  mov qword [rbp - 48], rax
  mov rax, [spd]
  test rax, [rax]
  mov qword [rbp - 56], 20
  mov rdi, qword [rbp - 56]
  call fib
  mov qword [rbp - 64], rax
  mov rax, [spd]
  test rax, [rax]
  mov rdi, str.0
  mov rsi, qword [rbp - 64]
  call printf
  mov qword [rbp - 72], 10
  mov r15, qword [rbp - 72]
  mov qword [rbp - 16], r15
  mov qword [rbp - 80], 0
  mov r15, qword [rbp - 80]
  mov qword [rbp - 32], r15
  mov qword [rbp - 88], 48
  mov r15, qword [rbp - 16]
  mov r14, qword [rbp - 88]
  add r15, r14
  mov qword [rbp - 96], r15
  mov rdi, str.1
  mov rsi, qword [rbp - 96]
  call printf
  mov rax, [spd]
  test rax, [rax]
  jmp .while.cond0
.while.cond0:
  mov qword [rbp - 104], 20
  mov r14, qword [rbp - 16]
  mov r15, qword [rbp - 104]
  xor r13, r13
  cmp r14, r15
  setl r13b
  mov qword [rbp - 112], r13
  mov r15, qword [rbp - 112]
  test r15, r15
  je .while.end0
  jne .while.body0
.while.body0:
  mov rdi, str.2
  mov rsi, qword [rbp - 16]
  call printf
  mov qword [rbp - 120], 1
  mov r15, qword [rbp - 16]
  mov r14, qword [rbp - 120]
  add r15, r14
  mov qword [rbp - 128], r15
  mov r14, qword [rbp - 128]
  mov qword [rbp - 16], r14
  mov rax, [spd]
  test rax, [rax]
  jmp .while.cond0
.while.end0:
  mov rax, qword [rbp - 16]
  mov rsp, rbp
  pop rbp
  ret


section .data
Struct:
dq 16
dq "Struct"
dq 0

Struct2:
dq 152
dq "Struct2"
dq 1

spd: dq 0
StructTODOMANGLING:
dq "Struct"
Struct2TODOMANGLING:
dq "Struct2"
str.0:
db " %zu", 10, 0
str.1:
db " %zu", 10, 0
str.2:
db " %zu", 10, 0

