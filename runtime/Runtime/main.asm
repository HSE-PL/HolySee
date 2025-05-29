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
  mov rdi, qword [rbp - 8]
  mov r11, qword [rbp - 16]
  add rdi, r11
  mov qword [rbp - 24], rdi
  mov rax, qword [rbp - 24]
  mov rsp, rbp
  pop rbp
  ret
main:
  push rbp
  mov rbp, rsp
  sub rsp, 112
.main:
  mov qword [rbp - 8], 13
  mov rdi, qword [rbp - 8]
  call hehe
  mov qword [rbp - 16], rax
  mov rax, [spd]
  test rax, [rax]
  mov qword [rbp - 24], 10
  mov r11, qword [rbp - 24]
  mov qword [rbp - 32], r11
  mov qword [rbp - 40], 0
  mov r11, qword [rbp - 40]
  mov qword [rbp - 48], r11
  mov qword [rbp - 56], 48
  mov r11, qword [rbp - 32]
  mov rdi, qword [rbp - 56]
  add r11, rdi
  mov qword [rbp - 64], r11
  mov rdi, str.0
  mov rsi, qword [rbp - 64]
  call printf
  mov rax, [spd]
  test rax, [rax]
  jmp .while.cond0
.while.cond0:
  mov qword [rbp - 72], 20
  mov rdi, qword [rbp - 32]
  mov r11, qword [rbp - 72]
  xor r10, r10
  cmp rdi, r11
  setl r10b
  mov qword [rbp - 80], r10
  mov r11, qword [rbp - 80]
  test r11, r11
  je .while.end0
  jne .while.body0
.while.body0:
  mov rdi, str.1
  mov rsi, qword [rbp - 32]
  call printf
  mov qword [rbp - 88], 1
  mov r11, qword [rbp - 32]
  mov rdi, qword [rbp - 88]
  add r11, rdi
  mov qword [rbp - 96], r11
  mov rdi, qword [rbp - 96]
  mov qword [rbp - 32], rdi
  mov rax, [spd]
  test rax, [rax]
  jmp .while.cond0
.while.end0:
  mov rax, qword [rbp - 32]
  mov rsp, rbp
  pop rbp
  ret


section .data
Struct:
dq 16
dq "Struct"
dq 0

Struct2:
dq 16
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

