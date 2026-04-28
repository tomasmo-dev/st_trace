format elf64

section '.text'
public get_rbp

get_rbp:
    mov rax, rbp ; rax is the return value, which is the value of the base pointer
    ret