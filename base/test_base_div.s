	.file	"test_base_div.c"
	.text
	.type	print_mxcsr, @function
print_mxcsr:
.LFB5014:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5014:
	.size	print_mxcsr, .-print_mxcsr
	.type	set_mxcsr, @function
set_mxcsr:
.LFB5015:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -4(%rbp)
#APP
# 73 "base/base.h" 1
	ldmxcsr -4(%rbp)
# 0 "" 2
#NO_APP
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5015:
	.size	set_mxcsr, .-set_mxcsr
	.type	get_mxcsr, @function
get_mxcsr:
.LFB5016:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
#APP
# 79 "base/base.h" 1
	stmxcsr -4(%rbp)
# 0 "" 2
#NO_APP
	movl	-4(%rbp), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5016:
	.size	get_mxcsr, .-get_mxcsr
	.section	.rodata
.LC0:
	.string	"Testing DIV (8-bit):"
.LC1:
	.string	"===================="
.LC2:
	.string	"0x%04X / 0x%02X:\n"
	.align 8
.LC3:
	.string	"  Quotient: 0x%02X, Remainder: 0x%02X\n"
	.text
	.type	test_div8, @function
test_div8:
.LFB5020:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movw	$4660, -2(%rbp)
	movb	$86, -3(%rbp)
#APP
# 18 "base/test_base_div.c" 1
	push $0
	popfq
	push $0
	popfq
# 0 "" 2
#NO_APP
	movzbl	-3(%rbp), %edx
	movzwl	-2(%rbp), %eax
#APP
# 19 "base/test_base_div.c" 1
	divb %dl
	
# 0 "" 2
#NO_APP
	movb	%al, -4(%rbp)
	movb	%dl, -5(%rbp)
	movzbl	-3(%rbp), %edx
	movzwl	-2(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movzbl	-5(%rbp), %edx
	movzbl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movw	$-21555, -2(%rbp)
	movb	$-17, -3(%rbp)
#APP
# 31 "base/test_base_div.c" 1
	push $0
	popfq
	push $0
	popfq
# 0 "" 2
#NO_APP
	movzbl	-3(%rbp), %edx
	movzwl	-2(%rbp), %eax
#APP
# 32 "base/test_base_div.c" 1
	divb %dl
	
# 0 "" 2
#NO_APP
	movb	%al, -4(%rbp)
	movb	%dl, -5(%rbp)
	movzbl	-3(%rbp), %edx
	movzwl	-2(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movzbl	-5(%rbp), %edx
	movzbl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5020:
	.size	test_div8, .-test_div8
	.section	.rodata
.LC4:
	.string	"\nTesting DIV (16-bit):"
.LC5:
	.string	"====================="
.LC6:
	.string	"0x%08X / 0x%04X:\n"
	.align 8
.LC7:
	.string	"  Quotient: 0x%04X, Remainder: 0x%04X\n"
	.text
	.type	test_div16, @function
test_div16:
.LFB5021:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	leaq	.LC4(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movl	$305419896, -4(%rbp)
	movw	$-25924, -6(%rbp)
#APP
# 55 "base/test_base_div.c" 1
	push $0
	popfq
	push $0
	popfq
# 0 "" 2
#NO_APP
	movzwl	-6(%rbp), %edx
	movl	-4(%rbp), %eax
#APP
# 56 "base/test_base_div.c" 1
	divw %dx
	
# 0 "" 2
#NO_APP
	movw	%ax, -8(%rbp)
	movw	%dx, -10(%rbp)
	movzwl	-6(%rbp), %edx
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movzwl	-10(%rbp), %edx
	movzwl	-8(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC7(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$-2023406815, -4(%rbp)
	movw	$4660, -6(%rbp)
#APP
# 68 "base/test_base_div.c" 1
	push $0
	popfq
	push $0
	popfq
# 0 "" 2
#NO_APP
	movzwl	-6(%rbp), %edx
	movl	-4(%rbp), %eax
#APP
# 69 "base/test_base_div.c" 1
	divw %dx
	
# 0 "" 2
#NO_APP
	movw	%ax, -8(%rbp)
	movw	%dx, -10(%rbp)
	movzwl	-6(%rbp), %edx
	movl	-4(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC6(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movzwl	-10(%rbp), %edx
	movzwl	-8(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC7(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5021:
	.size	test_div16, .-test_div16
	.section	.rodata
.LC8:
	.string	"\nTesting DIV (32-bit):"
.LC9:
	.string	"0x%016lX / 0x%08X:\n"
	.align 8
.LC10:
	.string	"  Quotient: 0x%08X, Remainder: 0x%08X\n"
	.text
	.type	test_div32, @function
test_div32:
.LFB5022:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	leaq	.LC8(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movabsq	$1311768467463790320, %rax
	movq	%rax, -8(%rbp)
	movl	$305419896, -12(%rbp)
#APP
# 92 "base/test_base_div.c" 1
	push $0
	popfq
	push $0
	popfq
# 0 "" 2
#NO_APP
	movl	-12(%rbp), %edx
	movq	-8(%rbp), %rax
#APP
# 93 "base/test_base_div.c" 1
	divl %edx
	
# 0 "" 2
#NO_APP
	movl	%eax, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	-12(%rbp), %edx
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC9(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	-20(%rbp), %edx
	movl	-16(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC10(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movabsq	$-81985529216486896, %rax
	movq	%rax, -8(%rbp)
	movl	$-1412567295, -12(%rbp)
#APP
# 105 "base/test_base_div.c" 1
	push $0
	popfq
	push $0
	popfq
# 0 "" 2
#NO_APP
	movl	-12(%rbp), %edx
	movq	-8(%rbp), %rax
#APP
# 106 "base/test_base_div.c" 1
	divl %edx
	
# 0 "" 2
#NO_APP
	movl	%eax, -16(%rbp)
	movl	%edx, -20(%rbp)
	movl	-12(%rbp), %edx
	movq	-8(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC9(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	-20(%rbp), %edx
	movl	-16(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC10(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5022:
	.size	test_div32, .-test_div32
	.section	.rodata
.LC11:
	.string	"\nTesting DIV (64-bit):"
.LC12:
	.string	"%llu / %llu:\n"
	.align 8
.LC13:
	.string	"  Quotient: %llu, Remainder: %llu\n"
	.text
	.type	test_div64, @function
test_div64:
.LFB5023:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	leaq	.LC11(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	leaq	.LC5(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movq	$100, -16(%rbp)
	movq	$0, -8(%rbp)
	movq	$5, -24(%rbp)
#APP
# 129 "base/test_base_div.c" 1
	push $0
	popfq
	push $0
	popfq
# 0 "" 2
#NO_APP
	movq	-16(%rbp), %rsi
	movq	-16(%rbp), %rax
	movq	-8(%rbp), %rdx
	movq	%rdx, %rax
	xorl	%edx, %edx
	movq	%rax, %rdx
	movq	-24(%rbp), %rcx
	movq	%rsi, %rax
#APP
# 130 "base/test_base_div.c" 1
	divq %rcx
	
# 0 "" 2
#NO_APP
	movq	%rax, -32(%rbp)
	movq	%rdx, -40(%rbp)
	movq	-16(%rbp), %rax
	movq	-24(%rbp), %rdx
	movq	%rax, %rsi
	leaq	.LC12(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	-40(%rbp), %rdx
	movq	-32(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC13(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	$100, -16(%rbp)
	movq	$0, -8(%rbp)
	movq	$7, -24(%rbp)
#APP
# 142 "base/test_base_div.c" 1
	push $0
	popfq
	push $0
	popfq
# 0 "" 2
#NO_APP
	movq	-16(%rbp), %rsi
	movq	-16(%rbp), %rax
	movq	-8(%rbp), %rdx
	movq	%rdx, %rax
	xorl	%edx, %edx
	movq	%rax, %rdx
	movq	-24(%rbp), %rcx
	movq	%rsi, %rax
#APP
# 143 "base/test_base_div.c" 1
	divq %rcx
	
# 0 "" 2
#NO_APP
	movq	%rax, -32(%rbp)
	movq	%rdx, -40(%rbp)
	movq	-16(%rbp), %rax
	movq	-24(%rbp), %rdx
	movq	%rax, %rsi
	leaq	.LC12(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movq	-40(%rbp), %rdx
	movq	-32(%rbp), %rax
	movq	%rax, %rsi
	leaq	.LC13(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5023:
	.size	test_div64, .-test_div64
	.data
	.type	global_divisor, @object
	.size	global_divisor, 1
global_divisor:
	.byte	10
	.section	.rodata
	.align 8
.LC14:
	.string	"\nTesting DIV with memory operands (simplified):"
	.align 8
.LC15:
	.string	"============================================="
	.align 8
.LC16:
	.string	"8-bit memory (0x%04X / 0x%02X):\n"
	.text
	.type	test_div_mem, @function
test_div_mem:
.LFB5024:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	leaq	.LC14(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	leaq	.LC15(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movw	$1000, -2(%rbp)
#APP
# 165 "base/test_base_div.c" 1
	push $0
	popfq
	push $0
	popfq
# 0 "" 2
#NO_APP
	movzwl	-2(%rbp), %eax
#APP
# 166 "base/test_base_div.c" 1
	divb %dl
	
# 0 "" 2
#NO_APP
	movb	%al, -3(%rbp)
	movb	%dl, -4(%rbp)
	movzbl	global_divisor(%rip), %eax
	movzbl	%al, %edx
	movzwl	-2(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC16(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movzbl	-4(%rbp), %edx
	movzbl	-3(%rbp), %eax
	movl	%eax, %esi
	leaq	.LC3(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5024:
	.size	test_div_mem, .-test_div_mem
	.section	.rodata
.LC17:
	.string	"\nDIV tests completed."
	.text
	.globl	main
	.type	main, @function
main:
.LFB5025:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$0, %eax
	call	test_div8
	movl	$0, %eax
	call	test_div16
	movl	$0, %eax
	call	test_div32
	movl	$0, %eax
	call	test_div64
	movl	$0, %eax
	call	test_div_mem
	leaq	.LC17(%rip), %rax
	movq	%rax, %rdi
	call	puts@PLT
	movl	$0, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5025:
	.size	main, .-main
	.ident	"GCC: (Debian 14.2.0-19) 14.2.0"
	.section	.note.GNU-stack,"",@progbits
