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

    mov rdi, DynamicPrimitiveArray4
    _call __halloc

    mov rdi, DynamicArray1024
    _call __halloc

    mov rdi, DynamicArray1024
    _call __halloc
    push rax

    push 0x7c00
    push 0x55AA
    push 0xDEAD

    .L1:
    mov rax, [spd]
    test rax, [rax]
    jmp .L1
    ret

section .data
spd: dq 0

section .rodata
LinkedList:
dq 16
dq linked_list
dd 1
dd 0

Heap:
dq 24
dq heap
dd 1
dd 0

StaticArray600:
dq 600 * 8
dq static_array_600
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

dynamic_array_1024: db "dynamic array 1024", 0
dynamic_primitive_array_4: db "dynamic primitive array 2", 0
static_array_600: db "static array 600", 0
heap: db "Heap", 0
linked_list: db "LinkedList", 0
