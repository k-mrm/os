.text

.align 8
.globl TrapHandler
TrapHandler:
	// popq	%rdi
	call	Trap
	iret
