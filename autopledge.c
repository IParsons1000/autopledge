/*
 *
 * (c)2025 Ira Parsons
 * autopledge - automatically sandbox syscalls
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include "elf.h"
#include "syscalls.h"
#include "seccomp.h"
#include "glibc.h"

#define NAME "autopledge"
#define VERSION 0.1f

__attribute__ ((constructor)) void autopledege(void){

	/* detect executable filename */

	char *program = malloc(PATH_MAX);

	memset(program, 0, PATH_MAX);

	readlink("/proc/self/exe", program, PATH_MAX-1);

	/* sanity check provided program */

	if(access(program, F_OK | R_OK | X_OK) != 0){
		printf("Error: cannot access file %s\n", program);
		free(program);
		return;
	};

	/* load program */

	elf_t *bin;
	bin = elf_load(program);
	if(bin == NULL){
		free(program);
		return;
	};

	/* detect syscalls */

	elf_get_dyn_syscalls(bin);

	/* cleanup */

	elf_free(bin);

	/* filter syscalls */

	seccomp_restrict();

	/* cleanup pt. 2 */

	syscalls_free();

	return;

}
