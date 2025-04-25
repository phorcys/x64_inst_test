.global ficomp_test
ficomp_test:
    push %rbp
    mov %rsp, %rbp
    
    fldt 16(%rbp)     # Load long double from stack
    ftst              # Check if ST(0) is NAN
    fnstsw %ax
    test $0x4000, %ax
    jnz .handle_nan
    
    ficompl 32(%rbp)  # Compare with 32-bit integer and pop
    jmp .done
    
.handle_nan:
    fstp %st(0)       # Pop NAN value
    fldz              # Push 0 to maintain stack balance
    ficompl 32(%rbp)  # Compare 0 with integer (will set invalid op flag)
    
.done:
    # Save status word before returning
    fnstsw %ax
    mov %ax, -2(%rbp)
    
    mov %rbp, %rsp
    pop %rbp
    ret
