/*
 *
 * (c)2025 Ira Parsons
 * seccomp.c - interface to filter syscalls with seccomp (implementation)
 *
 */

#include <linux/seccomp.h>
#include <linux/filter.h>
#include <linux/audit.h>
#include <linux/signal.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <unistd.h>
#include "seccomp.h"
#include "syscalls.h"

void seccomp_restrict(void);

void seccomp_restrict(){

	/* construct bpf's for seccomp */
	struct sock_filter *bpf_filters = malloc(numsyscalls * 2 * sizeof(struct sock_filter));

	for(int i = 0; i < numsyscalls; i++){
		bpf_filters[i] = BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, syscalls[i/2], 0, 4);
		bpf_filters[i++] = BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW);
	};

	struct sock_fprog bpf_program = { numsyscalls, bpf_filters };

	/* set no_new_privs in case it's not already set so seccomp doesn't fail */
	prctl(PR_SET_NO_NEW_PRIVS, 1);

	/* make the call */
	syscall(SYS_seccomp, SECCOMP_SET_MODE_FILTER, 0, &bpf_program);

	return;

}
