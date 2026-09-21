.text
.globl encode_huffman_x86_64
.type encode_huffman_x86_64, @function

encode_huffman_x86_64:
    pushq %rbx
    pushq %r12
    pushq %r13

    xorl %r9d, %r9d
    xorl %ebx, %ebx
    xorl %r13d, %r13d

.loop_input:
    testq %rsi, %rsi
    je .done

    movzbl (%rdi), %eax
    incq %rdi
    decq %rsi

    leaq (%rdx,%rax,8), %r12
    movl 0(%r12), %r11d
    movl 4(%r12), %r10d
    testl %r10d, %r10d
    je .loop_input

.loop_bit:
    movl %r10d, %ecx
    decl %ecx
    movl %r11d, %eax
    shrl %cl, %eax
    andl $1, %eax

    shll $1, %ebx
    orl %eax, %ebx
    incl %r13d
    decl %r10d

    cmpl $8, %r13d
    jne .next_bit
    movb %bl, (%rcx,%r9,1)
    incq %r9
    xorl %ebx, %ebx
    xorl %r13d, %r13d

.next_bit:
    testl %r10d, %r10d
    jne .loop_bit
    jmp .loop_input

.done:
    movq %r9, 0(%r8)
    movb %bl, 8(%r8)
    movb %r13b, 9(%r8)
    movq %r9, %rax

    popq %r13
    popq %r12
    popq %rbx
    ret

.size encode_huffman_x86_64, .-encode_huffman_x86_64
