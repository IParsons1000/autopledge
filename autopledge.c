/*
 *
 * (c)2025 Ira Parsons
 * autopledge - automatically sandbox syscalls
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "elf.h"
#include "syscalls.h"
#include "seccomp.h"
#include "glibc.h"

#define NAME "autopledge"
#define VERSION 0.1f
#define USAGE "Usage:\vautopledge [ -vh ] <file>\n"

int main(int argc, char *argv[]){

	/* process command line arguments */

	extern int optind, optopt;
	extern char *optarg;

	int opt;
	while((opt = getopt(argc, argv, "vh")) != -1){
		switch(opt){
			case 'v':
				printf("%s %.1f\n", NAME, VERSION);
				return 0;
				break;
			case 'h': /* FALLTHRU */
			default:
				printf("%s", USAGE);
				return 0;
				break;
		};
	};

	if(optind >= argc){
		printf("%s", USAGE);
		return 0;
	};

	/* sanity check provided program */

	if(access(argv[optind], F_OK | R_OK | X_OK) != 0){
		printf("Error: cannot access file %s\n", argv[optind]);
		return 1;
	};

	/* load program */

	elf_t *bin;
	bin = elf_load(argv[optind]);
	if(bin == NULL){
		return 1;
	};

	/* detect syscalls */

	elf_get_dyn_syscalls(bin);

	/* filter syscalls */

	seccomp_restrict();

	/* cleanup */

	elf_free(bin);

	return 0;

}
