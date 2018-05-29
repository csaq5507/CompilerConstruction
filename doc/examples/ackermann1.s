	.file	"ackermann1.c"
	.text
	.globl	ackermann
	.type	ackermann, @function
ackermann:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$8, %esp
	cmpl	$0, 8(%ebp)
	jne	.L2
	cmpl	$1000, 12(%ebp)
	je	.L2
	movl	12(%ebp), %eax
	addl	$1, %eax
	jmp	.L3
.L2:
	cmpl	$0, 12(%ebp)
	jne	.L4
	movl	8(%ebp), %eax
	subl	$1, %eax
	subl	$8, %esp
	pushl	$1
	pushl	%eax
	call	ackermann
	addl	$16, %esp
	jmp	.L3
.L4:
	movl	12(%ebp), %eax
	subl	$1, %eax
	subl	$8, %esp
	pushl	%eax
	pushl	8(%ebp)
	call	ackermann
	addl	$16, %esp
	movl	%eax, %edx
	movl	8(%ebp), %eax
	subl	$1, %eax
	subl	$8, %esp
	pushl	%edx
	pushl	%eax
	call	ackermann
	addl	$16, %esp
.L3:
	leave
	ret
	.size	ackermann, .-ackermann
	.section	.rodata
	.align 4
.LC0:
	.string	"number of float multiplications: "
.LC1:
	.string	"%d"
.LC3:
	.string	"\nm: "
.LC4:
	.string	"\nn: "
.LC5:
	.string	"\n%d\n"
	.text
	.globl	main
	.type	main, @function
main:
	leal	4(%esp), %ecx
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ecx
	subl	$36, %esp
	subl	$12, %esp
	pushl	$.LC0
	call	puts
	addl	$16, %esp
	subl	$8, %esp
	leal	-20(%ebp), %eax
	pushl	%eax
	pushl	$.LC1
	call	__isoc99_scanf
	addl	$16, %esp
	flds	.LC2
	fstps	-16(%ebp)
	movl	$0, -12(%ebp)
	jmp	.L6
.L7:
	flds	-16(%ebp)
	fmuls	-16(%ebp)
	fstps	-16(%ebp)
	addl	$1, -12(%ebp)
.L6:
	movl	-20(%ebp), %eax
	cmpl	%eax, -12(%ebp)
	jl	.L7
	subl	$12, %esp
	pushl	$.LC3
	call	printf
	addl	$16, %esp
	subl	$8, %esp
	leal	-24(%ebp), %eax
	pushl	%eax
	pushl	$.LC1
	call	__isoc99_scanf
	addl	$16, %esp
	subl	$12, %esp
	pushl	$.LC4
	call	printf
	addl	$16, %esp
	subl	$8, %esp
	leal	-28(%ebp), %eax
	pushl	%eax
	pushl	$.LC1
	call	__isoc99_scanf
	addl	$16, %esp
	movl	-28(%ebp), %edx
	movl	-24(%ebp), %eax
	subl	$8, %esp
	pushl	%edx
	pushl	%eax
	call	ackermann
	addl	$16, %esp
	subl	$8, %esp
	pushl	%eax
	pushl	$.LC5
	call	printf
	addl	$16, %esp
	movl	$0, %eax
	movl	-4(%ebp), %ecx
	leave
	leal	-4(%ecx), %esp
	ret
	.size	main, .-main
	.section	.rodata
	.align 4
.LC2:
	.long	1070386381
	.ident	"GCC: (Ubuntu 5.5.0-12ubuntu1~16.04) 5.5.0 20171010"
	.section	.note.GNU-stack,"",@progbits
