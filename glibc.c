/*
 *
 * (c)2025 Ira Parsons
 * glibc.c - glibc syscall-by-function info (implementation)
 *
 */

#include <string.h>
#include "glibc.h"

#ifndef GLIBC_SYSCALL_BY_FUN_FILE
#define GLIBC_SYSCALL_BY_FUN_FILE "tools/glibc-syscalls-per-function"
#endif /* GLIBC_SYSCALL_BY_FUN_FILE */

const syscall_by_fun_t glibc_syscalls_by_fun[] = {
	#include GLIBC_SYSCALL_BY_FUN_FILE
	{ "elf_load", (int *)&(int []){ SYS_open, SYS_openat, SYS_read, SYS_lseek, SYS_getpid, SYS_fstat, SYS_close, SYS_exit, SYS_exit_group, SYS_write, SYS_socket, -1 } },
	{ NULL, NULL }
};

char **glibc_get_syscalls(char **funs);

char **glibc_get_syscalls(char **funs){

	if(funs == NULL){
		return NULL;
	};

	for(int i = 0; funs[i] != NULL; i++){
		for(int j = 0; glibc_syscalls_by_fun[j].fun != NULL; j++){
			if(!strcmp(funs[i], glibc_syscalls_by_fun[j].fun)){
				syscalls_add(glibc_syscalls_by_fun[j].syscalls);
				break;
			};
		};
	};

	return funs;

}
