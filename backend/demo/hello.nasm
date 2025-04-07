section .text

global _start
_start:
  push rax
  mov rdi, 6
  call fib
  mov rdi, rax
  mov rax, 0x3c
  syscall

fib:
  push rbp
  mov rbp, rsp
  sub rsp, 168
  mov qword [rbp - 8], rdi
.fib:
  mov rdi, qword [rbp - 8]
  mov qword [rbp - 16], rdi
  mov qword [rbp - 24], 0
  mov rdi, qword [rbp - 16]
  mov r11, qword [rbp - 24]
  cmp rdi, r11
  je .then.0
  jne .else.0
.then.0:
  mov qword [rbp - 32], 0
  mov rax, qword [rbp - 32]
  mov rsp, rbp
  pop rbp
  ret
.else.0:
  mov r11, qword [rbp - 8]
  mov qword [rbp - 40], r11
  mov qword [rbp - 48], 1
  mov r11, qword [rbp - 40]
  mov rdi, qword [rbp - 48]
  cmp r11, rdi
  je .then.5
  jne .else.5
.then.5:
  mov qword [rbp - 56], 1
  mov rax, qword [rbp - 56]
  mov rsp, rbp
  pop rbp
  ret
.else.5:
  mov rdi, qword [rbp - 8]
  mov qword [rbp - 64], rdi
  mov qword [rbp - 72], 1
  mov rdi, qword [rbp - 64]
  mov r11, qword [rbp - 72]
  sub rdi, r11
  mov qword [rbp - 80], rdi
  mov rdi, qword [rbp - 80]
  call fib
  mov qword [rbp - 88], rax
  mov r11, qword [rbp - 88]
  mov qword [rbp - 96], r11
  mov r11, qword [rbp - 8]
  mov qword [rbp - 104], r11
  mov qword [rbp - 112], 2
  mov r11, qword [rbp - 104]
  mov rdi, qword [rbp - 112]
  sub r11, rdi
  mov qword [rbp - 120], r11
  mov rdi, qword [rbp - 120]
  call fib
  mov qword [rbp - 128], rax
  mov rdi, qword [rbp - 128]
  mov qword [rbp - 136], rdi
  mov rdi, qword [rbp - 88]
  mov qword [rbp - 144], rdi
  mov rdi, qword [rbp - 128]
  mov qword [rbp - 152], rdi
  mov rdi, qword [rbp - 144]
  mov r11, qword [rbp - 152]
  add rdi, r11
  mov qword [rbp - 160], rdi
  mov rax, qword [rbp - 160]
  mov rsp, rbp
  pop rbp
  ret
main:
  push rbp
  mov rbp, rsp
  sub rsp, 32
  mov qword [rbp - 8], rdi
.main:
  mov rdi, qword [rbp - 8]
  mov qword [rbp - 16], rdi
  mov rdi, qword [rbp - 16]
  call fib
  mov qword [rbp - 24], rax
  mov rsp, rbp
  pop rbp
  ret

