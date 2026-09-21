.text
.globl encode_huffman_x86_64
.type encode_huffman_x86_64, @function

# system v arguments
# rdi input rsi input size rdx codes rcx output r8 state
encode_huffman_x86_64:
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14

    # keep the output pointer across shift operations
    movq %rcx, %r14

    # clear output size bit buffer and bit count
    xorl %r9d, %r9d
    xorl %ebx, %ebx
    xorl %r13d, %r13d

.loop_input:
    # stop after all input bytes are encoded
    testq %rsi, %rsi
    je .done

    # load the next input symbol
    movzbl (%rdi), %eax
    incq %rdi
    decq %rsi

    # load the symbol code and code length
    leaq (%rdx,%rax,8), %r12
    movl 0(%r12), %r11d
    movl 4(%r12), %r10d
    testl %r10d, %r10d
    je .loop_input

.loop_bit:
    # read the next code bit from most significant to least significant
    movl %r10d, %ecx
    decl %ecx
    movl %r11d, %eax
    shrl %cl, %eax
    andl $1, %eax

    # append the bit to the output buffer
    shll $1, %ebx
    orl %eax, %ebx
    incl %r13d
    decl %r10d

    # write a complete output byte
    cmpl $8, %r13d
    jne .next_bit
    movb %bl, (%r14,%r9,1)
    incq %r9
    xorl %ebx, %ebx
    xorl %r13d, %r13d

.next_bit:
    # continue with the current code or load another symbol
    testl %r10d, %r10d
    jne .loop_bit
    jmp .loop_input

.done:
    # store the pending output state
    movq %r9, 0(%r8)
    movb %bl, 8(%r8)
    movb %r13b, 9(%r8)
    movq %r9, %rax

    # restore callee-saved registers
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    ret

.size encode_huffman_x86_64, .-encode_huffman_x86_64
