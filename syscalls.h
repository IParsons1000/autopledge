/*
 *
 * (c)2025 Ira Parsons
 * syscalls.h - program syscall list and manipulators thereof (specification)
 *
 */

#ifndef _AUTOPLEDGE_SYSCALLS_H
#define _AUTOPLEDGE_SYSCALLS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct syscall_by_fun_s {
	char *fun;
	int *syscalls;
} syscall_by_fun_t;

extern int *syscalls; /* dynamic array of syscalls used by program */
extern int numsyscalls; /* number of members of above array */

void syscalls_add(int *calls);
int syscalls_free(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUTOPLEDGE_SYSCALLS_H */
