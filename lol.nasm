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
  mov r8, Struct
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
  sub rsp, 104
.main:
  mov rax, Struct
  call __halloc
  mov qword [rbp - 8], rax
  mov qword [rbp - 16], 13
  mov rdi, qword [rbp - 16]
  call hehe
  mov qword [rbp - 24], rax
  mov rax, [spd]
  test rax, [rax]
  mov qword [rbp - 32], 10
  mov r11, qword [rbp - 32]
  mov qword [rbp - 40], r11
  jmp .while.cond0
.while.cond0:
  mov qword [rbp - 48], 0
  mov r11, qword [rbp - 40]
  mov rdi, qword [rbp - 48]
  cmp r11, rdi
  xor r11, r11
  setge r11b
  mov qword [rbp - 56], r11
  mov rdi, qword [rbp - 56]
  test rdi, rdi
  je .while.body0
  jne .while.end0
.while.body0:
  mov qword [rbp - 64], 3
  mov rdi, qword [rbp - 64]
  mov qword [rbp - 72], rdi
  mov qword [rbp - 80], 1
  mov rdi, qword [rbp - 40]
  mov r11, qword [rbp - 80]
  sub rdi, r11
  mov qword [rbp - 88], rdi
  mov r11, qword [rbp - 88]
  mov qword [rbp - 96], r11
  jmp .while.cond0
.while.end0:
  mov rax, qword [rbp - 40]
  mov rsp, rbp
  pop rbp
  ret


section .data
Struct:
dq 16
dq StructTODOMANGLING
dq 0

spd: dq 0
StructTODOMANGLING:
dq StructTODOMANGLING, 10

