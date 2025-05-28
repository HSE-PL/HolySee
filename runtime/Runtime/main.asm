extern __rt_init
extern __halloc
global start
global main
section .text
start:
.start:
  mov rdi, main
  mov rsi, spd
  mov rdx, rsp
  mov rcx, Struct
  mov r8, Struct2
  mov r9, 0x40000 
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
  sub rsp, 80
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
  mov rax, [spd]
  test rax, [rax]
  jmp .while.cond0
.while.cond0:
  mov qword [rbp - 40], 0
  mov r11, qword [rbp - 32]
  mov rdi, qword [rbp - 40]
  cmp r11, rdi
  xor r11, r11
  setg r11b
  mov qword [rbp - 48], r11
  mov rdi, qword [rbp - 48]
  test rdi, rdi
  je .while.body0
  jne .while.end0
.while.body0:
  mov rdi, Struct
  call __halloc
  mov qword [rbp - 56], rdi
  mov qword [rbp - 64], 3
  mov rdi, qword [rbp - 64]
  mov qword [rbp - 72], rdi
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
dq 4600
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

