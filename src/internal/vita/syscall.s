.syntax unified
.global __syscall
.hidden __syscall
.type __syscall,%function
__syscall:
        b __vita_syscall_interp
