/*
 *
 * (c)2025 Ira Parsons
 * glibc.c - glibc syscall-by-function info (implementation)
 *
 */

#include <stdio.h>
#include <string.h>
#include "glibc.h"

const syscall_by_fun_t glibc_syscalls_by_fun[GLIBC_NUM_FUNS] = {
	{ "printf", (int *)&(int []){ 0, -1 } },
	{ "scanf", (int *)&(int []){ 0, 1, -1 } }
};

char **glibc_get_syscalls(char **funs);

char **glibc_get_syscalls(char **funs){

	if(funs == NULL){
		return NULL;
	};

	for(int i = 0; funs[i] != NULL; i++){
		for(int j = 0; j < GLIBC_NUM_FUNS; j++){
			if(!strcmp(funs[i], glibc_syscalls_by_fun[j].fun)){
				syscalls_add(glibc_syscalls_by_fun[j].syscalls);
				break;
			};
		};
	};

	return funs;

}
