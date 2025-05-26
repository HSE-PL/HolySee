main:
  push rbp
  mov rbp, rsp
  sub rsp, 80
.main:
  mov qword [rbp - 8], 10
  mov r11, qword [rbp - 8]
  mov qword [rbp - 16], r11
  jmp .while.cond0
.while.cond0:
  mov qword [rbp - 24], 0
  mov r11, qword [rbp - 16]
  mov r10, qword [rbp - 24]
  cmp r11, r10
  xor r11, r11
  setge r11b
  mov qword [rbp - 32], r11
  mov r10, qword [rbp - 32]
  test r10, r10
  je .while.body0
  jne .while.end0
.while.body0:
  mov qword [rbp - 40], 3
  mov r10, qword [rbp - 40]
  mov qword [rbp - 48], r10
  mov qword [rbp - 56], 1
  mov r10, qword [rbp - 16]
  mov r11, qword [rbp - 56]
  sub r10, r11
  mov qword [rbp - 64], r10
  mov r11, qword [rbp - 64]
  mov qword [rbp - 72], r11
  jmp .while.cond0
.while.end0:
  mov rax, qword [rbp - 16]
  mov rsp, rbp
  pop rbp
  ret
