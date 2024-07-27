	.file	"file1.c"
	.text
	.globl	global_var1
	.data
	.align 4
	.type	global_var1, @object
	.size	global_var1, 4
global_var1:
	.long	10
	.globl	global_var2
	.bss
	.align 4
	.type	global_var2, @object
	.size	global_var2, 4
global_var2:
	.zero	4
	.section	.rodata
.LC0:
	.string	"Global variables: %d, %d\n"
	.text
	.globl	myFunction
	.type	myFunction, @function
myFunction:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	global_var2(%rip), %edx
	movl	global_var1(%rip), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	myFunction, .-myFunction
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
