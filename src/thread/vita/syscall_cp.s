.syntax unified
.global __cp_begin
.hidden __cp_begin
.global __cp_end
.hidden __cp_end
.global __cp_cancel
.hidden __cp_cancel
.hidden __cancel
.global __syscall_cp_asm
.hidden __syscall_cp_asm
.type __syscall_cp_asm,%function
__syscall_cp_asm:
	mov ip,sp
        stmfd sp!,{r4,r5,r6,r7,lr}
__cp_begin:
        ldr r0,[r0]
        cmp r0,#0
        blne __cp_cancel
        mov r0,r1
        mov r1,r2
        mov r2,r3
        ldmfd ip,{r3,r4,r5,r6,r7}
        stmfd sp!,{r4,r5,r6,r7}
        bl __syscall
        ldmfd sp!,{r4,r5,r6,r7}
__cp_end:
	ldmfd sp!,{r4,r5,r6,r7,lr}
	bx lr
__cp_cancel:
	ldmfd sp!,{r4,r5,r6,r7,lr}
	b __cancel
