[BITS 64]
default rel
extern __rt_init
extern __halloc
extern __GC
extern __go
extern __sleep
global start
global main

%macro _call 1
    and rsp, -16
    call %1
%endmacro

%macro _sp 0
    mov rax, [spd]
    test rax, [rax]
%endmacro

section .text

start:
    lea rdi, [main]
    lea rsi, [spd]
    mov rdx, rsp
    lea rcx, [LinkedList]
    lea r8, [Heap]
    mov r9, 0x300000000 ; 12 gb
;    mov r9, 0x100000000 ; 4  gb
;    mov r9, 0x10000000  ; 256mb
;    mov r9, 0x1000000   ; 16 mb
;    mov r9, 0x200000    ; 2  mb
;    mov r9, 0x100000    ; 1  mb
;    mov r9, 0x40000     ; 256kb
;    mov r9, 0x10000     ; 64 kb
    _call __rt_init

    mov rax, 60
    mov rdi, 0
    syscall

main:
    push 0x23126
    push 0x42
    push 0x42
    push 0xBEEF

    mov rdi, StaticArray500
    _call __halloc
    mov r12, rax

    mov rdi, StaticArray600
    _call __halloc

    mov rdi, DynamicArray256
    _call __halloc
    mov [rax], r12
    mov r12, rax

    mov rdi, DynamicArray256
    _call __halloc
    mov [rax], r12
    push rax

    mov r13, 8
.loop:
    lea rdi, [ebashim_musor]
    _call __go
    dec r13
    jnz .loop
;jmp .L1
;jmp $
;    mov rdi, DynamicPrimitiveArray4
;    _call __halloc

    mov rdi, StaticArray500
    _call __halloc
    push rax

    push 0x7c00
    push 0x55AA
    push 0xDEAD

    _call ebashim_musor
.L1:
    _sp
    jmp .L1
    ret



thread2:
    push 0xCAFE
    push 0xBABE
    push 0x1234
    push 0x6666
    push 0x2222
    push 0x7575
    .L1:
    mov rdi, StaticArray300
    _call __halloc

    _sp
    jmp .L1
    ret

ebashim_musor_lite:
    push 0x1234
    push 0x3451
    push 0x8989
.L1:
    _sp
    mov rdi, StaticArray500
    _call __halloc
;    push rax
;    push 0x8989

    mov r13, 6
.L2:
    _sp
;    mov rdi, LinkedList
;    _call __halloc
    mov rdi, StaticArray500
    _call __halloc
;    mov rdi, LinkedList
;    _call __halloc
;    mov rdi, Heap
;    _call __halloc
    mov rdi, StaticArray2000
    _call __halloc
;    mov rdi, Heap
;    _call __halloc
    dec r13
    jnz .L2

;    pop rax
;    pop rax

    mov r13, 10
.L3:
    _sp
    mov rdi, StaticArray600
    _call __halloc
    dec r13
    jnz .L3

    jmp .L1

ebashim_musor:
_sp
    push 0x1234
    push 0x3451
    push 0x126
    mov rdi, DynamicArray256
    call __halloc
    mov r12, rax
    push r12
    push 0
.L1:
_sp

    mov rdi, DynamicArray1024
    call __halloc
;    mov [r12], rax
;    mov r12, rax
;    jmp .L1

    mov rdi, StaticArray500
    call __halloc
    push rax
    push 0
    mov rdi, StaticArray600
    call __halloc
    push rax
    push 0

    mov r13, 100
.L2:
_sp
    mov rdi, DynamicArray1024
    call __halloc
    mov rdi, StaticArray500
    call __halloc
    mov rdi, DynamicArray4096
    call __halloc
    mov rdi, StaticArray500
    call __halloc
    mov rdi, StaticArray500
    call __halloc
    mov rdi, DynamicArray1024
    call __halloc
    dec r13
    jnz .L2

    pop rax
    pop rax
    pop rax
    pop rax
    mov r13, 30
.L3:
    _sp
    mov rdi, DynamicArray4096
    call __halloc
    dec r13
    jnz .L3

    _sp
    jmp .L1

section .data
spd: dq 0

section .rodata
LinkedList:
dq 1600
dq linked_list
dq 1

StaticArray2000:
dq 2000*8
dq static_array_2000
dq 1

StaticArray800:
dq 800 * 8
dq static_array_600
dq 1

StaticArray600:
dq 600 * 8
dq static_array_600
dq 1

StaticArray500:
dq 500 * 8
dq static_array_500
dq 1

StaticArray300:
dq 300 * 8
dq static_array_500
dq 1

DynamicPrimitiveArray4:
dq 3200
dq dynamic_primitive_array_4
dq 0

DynamicArray256:
dq 2048
dq dynamic_array_256
dq 1

DynamicArray1024:
dq 8192
dq dynamic_array_1024
dq 1

DynamicArray4096:
dq 32768
dq dynamic_array_1024
dq 1

Heap:
dq 2400
dq heap
dq 1

dynamic_array_256: db "dynamic array 256", 0
dynamic_array_1024: db "dynamic array 1024", 0
dynamic_array_4096: db "dynamic array 4096", 0
dynamic_primitive_array_4: db "dynamic primitive array 2", 0
static_array_2000: db "static array 2000", 0
static_array_600: db "static array 600", 0
static_array_500: db "static array 500", 0
static_array_300: db "static array 300", 0
heap: db "Heap", 0
linked_list: db "LinkedList", 0
