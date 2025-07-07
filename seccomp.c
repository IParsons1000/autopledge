/*
 *
 * (c)2025 Ira Parsons
 * seccomp.c - interface to filter syscalls with seccomp (implementation)
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <unistd.h>
#include <linux/seccomp.h>
#include <linux/filter.h>
#include <linux/audit.h>
#include <sys/ptrace.h>
#include <sys/prctl.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include "seccomp.h"
#include "syscalls.h"
#include "syslog.h"

int seccomp_restrict(void);

int seccomp_restrict(){

	/* check if seccomp is allowed */
	if(prctl(PR_GET_SECCOMP) != 0){
		log_error("seccomp is already enabled");
		return 1;
	};

	/* construct bpf's for seccomp */
	struct sock_filter *bpf_filters = malloc((2 * (numsyscalls + 1)) * sizeof(struct sock_filter));
	if(bpf_filters == NULL){
		log_error("malloc() failed (%s)", strerror(errno));
		return 1;
	};

	/* load syscall number */
	bpf_filters[0] = (struct sock_filter) BPF_STMT(BPF_LD | BPF_W | BPF_ABS, (offsetof(struct seccomp_data, nr)));

	for(int i = 0; i < 2*numsyscalls; i++){
		bpf_filters[i+1] = (struct sock_filter) BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, syscalls[i/2], 0, 1);
		bpf_filters[(++i)+1] = (struct sock_filter) BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW);
	};

	/* if the syscall hasn't triggered any other filters, reject it */
#ifdef LENIENT
	bpf_filters[(2*numsyscalls)+1] = (struct sock_filter) BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_TRAP);
#else /* !LENIENT */
	bpf_filters[(2*numsyscalls)+1] = (struct sock_filter) BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS);
#endif /* LENIENT */

	struct sock_fprog bpf_program = { .len = (2*numsyscalls) + 2, .filter = bpf_filters };

	/* set no_new_privs in case it's not already set so seccomp doesn't fail */
	if(prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) == -1){
		log_error("prctl(PR_SET_NO_NEW_PRIVS, 1) failed (%s)", strerror(errno));
		return 1;
	};

	/* do necessary cleanup before potentially restricting close(2) */
	log_close();

	/* make the call */
	if(syscall(SYS_seccomp, SECCOMP_SET_MODE_FILTER, 0, &bpf_program) != 0){
		log_init();
		log_error("seccomp syscall failed (%s)", strerror(errno));
		return 1;
	};

	return 0;

}
