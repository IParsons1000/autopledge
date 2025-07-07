/*
 *
 * (c)2025 Ira Parsons
 * autopledge - automatically sandbox syscalls
 *
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include "elf.h"
#include "syscalls.h"
#include "seccomp.h"
#include "glibc.h"
#include "syslog.h"

#define NAME "autopledge"
#define VERSION 0.1f

__attribute__ ((constructor)) void autopledege(void){

	/* initialize logging */

	log_init();

	/* detect executable filename */

	char *program = malloc(PATH_MAX);
	if(program == NULL){
		log_error("malloc() failed (%s)", strerror(errno));
		log_close();
		return;
	};

	memset(program, 0, PATH_MAX);

	if(readlink("/proc/self/exe", program, PATH_MAX-1) == -1){
		log_error("readlink(\"/proc/self/exe\") failed (%s)", strerror(errno));
		free(program);
		log_close();
		return;
	};

	/* sanity check provided program */

	if(access(program, F_OK | R_OK | X_OK) != 0){
		log_error("cannot access file %s (%s)", program, strerror(errno));
		free(program);
		log_close();
		return;
	};

	/* load program */

	elf_t *bin;
	bin = elf_load(program);
	if(bin == NULL){
		free(program);
		log_close();
		return;
	};

	/* detect syscalls */

	elf_get_dyn_syscalls(bin);

	/* cleanup */

	elf_free(bin);

	/* filter syscalls */

	if(seccomp_restrict()){
		log_close(); /* seccomp_restrict should clean up logging, unless
                                 it fails before it gets there */
	};

	/* cleanup pt. 2 */

	syscalls_free();

	return;

}
