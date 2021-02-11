	.file	"Lab1_p1.c"
	.text
	.globl	power_usage
	.type	power_usage, @function
power_usage:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%edi, -20(%rbp)
	movss	%xmm0, -24(%rbp)
	pxor	%xmm0, %xmm0
	movss	%xmm0, -4(%rbp)
	cvtsi2ssl	-20(%rbp), %xmm0
	movss	-24(%rbp), %xmm1
	mulss	%xmm1, %xmm0
	movss	%xmm0, -4(%rbp)
	movss	-4(%rbp), %xmm0
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	power_usage, .-power_usage
	.section	.rodata
	.align 8
.LC3:
	.string	"Total Power cost for machines in factory = %f\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movl	%edi, -52(%rbp)
	movq	%rsi, -64(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$15, -32(%rbp)
	movss	.LC1(%rip), %xmm0
	movss	%xmm0, -28(%rbp)
	movl	$9, -24(%rbp)
	movss	.LC2(%rip), %xmm0
	movss	%xmm0, -20(%rbp)
	movl	$0, -36(%rbp)
	jmp	.L4
.L7:
	cmpl	$0, -36(%rbp)
	jne	.L5
	movl	-28(%rbp), %edx
	movl	-32(%rbp), %eax
	movd	%edx, %xmm0
	movl	%eax, %edi
	call	power_usage
	movd	%xmm0, %eax
	movl	%eax, -16(%rbp)
	jmp	.L6
.L5:
	cmpl	$1, -36(%rbp)
	jne	.L6
	movl	-20(%rbp), %edx
	movl	-24(%rbp), %eax
	movd	%edx, %xmm0
	movl	%eax, %edi
	call	power_usage
	movd	%xmm0, %eax
	movl	%eax, -12(%rbp)
.L6:
	addl	$1, -36(%rbp)
.L4:
	cmpl	$1, -36(%rbp)
	jle	.L7
	movss	-16(%rbp), %xmm1
	movss	-12(%rbp), %xmm0
	addss	%xmm1, %xmm0
	cvtss2sd	%xmm0, %xmm0
	leaq	.LC3(%rip), %rdi
	movl	$1, %eax
	call	printf@PLT
	movl	$0, %eax
	movq	-8(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L9
	call	__stack_chk_fail@PLT
.L9:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.section	.rodata
	.align 4
.LC1:
	.long	1143930880
	.align 4
.LC2:
	.long	1147207680
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
