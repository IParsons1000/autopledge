/*
 *
 * (c)2025 Ira Parsons
 * glibcxx.c - c++ stdlib syscall-by-function info (implementation)
 *
 */

#include <string.h>
#include "glibcxx.h"

#ifndef GLIBCXX_SYSCALL_BY_FUN_FILE
#define GLIBCXX_SYSCALL_BY_FUN_FILE "tools/glibcxx-syscalls-per-function"
#endif /* GLIBCXX_SYSCALL_BY_FUN_FILE */

const syscall_by_fun_t glibcxx_syscalls_by_fun[] = {
	#include GLIBCXX_SYSCALL_BY_FUN_FILE
	{ NULL, NULL }
};

char **glibcxx_get_syscalls(char **funs);

char **glibcxx_get_syscalls(char **funs){

	if(funs == NULL){
		return funs;
	};

	for(int i = 0; funs[i] != NULL; i++){
		for(int j = 0; glibcxx_syscalls_by_fun[j].fun != NULL; j++){
			if(!strcmp(funs[i], glibcxx_syscalls_by_fun[j].fun)){
				syscalls_add(glibcxx_syscalls_by_fun[j].syscalls);
				break;
			};
		};
	};

	return funs;

}
