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
    mov r9, 0x20000
    _call __rt_init

    mov rax, 60
    mov rdi, 0
    syscall

main:
    push 0x23126
    push 0x42
    push 0x42
    push 0xBEEF

    mov rdi, StaticArray600
    _call __halloc
    mov r12, rax

    mov rdi, LinkedList
    _call __halloc
    mov [rax], r12
    mov r12, rax

    mov rdi, Heap
    _call __halloc
    mov [rax], r12
    push rax

    lea rdi, [thread2]
;    _call __go

;jmp $
    mov rdi, DynamicPrimitiveArray4
    _call __halloc

    mov rdi, DynamicArray1024
    _call __halloc

    mov rdi, DynamicArray1024
    _call __halloc
    push rax

    mov rdi, StaticArray500
    _call __halloc

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
;    mov rdi, 5
;    _call __sleep
    .L1:
    mov rdi, DynamicPrimitiveArray4
    _call __halloc
    jmp .L1
;    push rax
    push 0x2222
    push 0x7575

.L2:
    _sp
    jmp .L2
    ret

ebashim_musor:
    push 0x1234
    push 0x3451
    push 0x8989
.L1:
    mov rdi, LinkedList
    _call __halloc
    mov rdi, Heap
    _call __halloc
    mov rdi, LinkedList
    _call __halloc
    mov rdi, Heap
    _call __halloc
    mov rdi, StaticArray500
    _call __halloc
    _sp
    jmp .L1

section .data
spd: dq 0

section .rodata
LinkedList:
dq 16
dq linked_list
dd 1
dd 0


StaticArray600:
dq 600 * 8
dq static_array_600
dd 1
dd 0

StaticArray500:
dq 500 * 8
dq static_array_500
dd 1
dd 0

DynamicPrimitiveArray4:
dq 32
dq dynamic_primitive_array_4
dd 0
dd 0

DynamicArray1024:
dq 8192
dq dynamic_array_1024
dd 1
dd 0

Heap:
dq 24
dq heap
dd 1
dd 0

dynamic_array_1024: db "dynamic array 1024", 0
dynamic_primitive_array_4: db "dynamic primitive array 2", 0
static_array_600: db "static array 600", 0
static_array_500: db "static array 500", 0
heap: db "Heap", 0
linked_list: db "LinkedList", 0
