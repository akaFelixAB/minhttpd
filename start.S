.intel_syntax noprefix

.text

/* functions: rdi, rsi, rdx, rcx, r8, r9 */
/*  syscalls: rdi, rsi, rdx, r10, r8, r9 */
/*                           ^^^         */

.global _syscall
_syscall:
    mov r10, rcx
    syscall
    ret

#define make_call(call_name, call_number) \
.global call_name; \
.type call_name, @function; \
call_name:; \
    mov eax, call_number; \
    jmp _syscall

make_call(read, 0)
make_call(write, 1)
make_call(open, 2)
make_call(close, 3)
make_call(socket, 41)
make_call(accept, 43)
make_call(shutdown, 48)
make_call(bind, 49)
make_call(listen, 50)
make_call(setsockopt, 54)
make_call(fork, 57)
make_call(exit, 60)

.global _start
_start:
    xor rbp, rbp
    pop rdi             // argc 
    mov rsi, rsp        // argv

    and rsp, -16        // align stack
    sub rsp, 8          // shadow space

    call main
    mov rdi, rax        // exit code
    call exit

    hlt                 // just in case
.size _start, .-_start  // size of _start