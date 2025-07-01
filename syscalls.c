/*
 *
 * (c)2025 Ira Parsons
 * syscalls.c - program syscall list and manipulators thereof (implementation)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syscalls.h"

int *syscalls = NULL;
int numsyscalls = 0;

void _syscalls_add(int syscall);
void syscalls_add(int *calls);
int syscalls_free(void);

void _syscalls_add(int syscall){

	if(syscalls == NULL){
		syscalls = malloc(++numsyscalls * sizeof(int));
		if(syscalls == NULL){
			printf("Error: malloc(++numsyscalls * sizeof(int)) failed\n");
			return;
		};
		syscalls[0] = syscall;
	}
	else{
		/* add in order, and only add if not already present */

		int i, a = 0;
		for(i = 0; i < numsyscalls; i++){
			if(syscalls[i] == syscall){
				a = 1;
				break;
			}
			else if(syscalls[i] > syscall){
				break;
			};
		};

		if(!a){
			syscalls = realloc(syscalls, ++numsyscalls * sizeof(int));
			if(syscalls == NULL){
				printf("Error: realloc(syscalls, ++numsyscalls * sizeof(int)) failed\n");
				return;
			};

			if(i != numsyscalls-1){
				memmove(&syscalls[i+1], &syscalls[i], (numsyscalls-i-1) * sizeof(int));
			};
			syscalls[i] = syscall;
		};
	};

#ifdef DEBUG
	fprintf(stderr, "%d\n", syscall);
#endif /* DEBUG */

	return;

}

void syscalls_add(int *calls){

	/* add a -1 terminated list of syscalls to total */

	if(calls != NULL){
		for(int i = 0; calls[i] != -1; i++){
			_syscalls_add(calls[i]);
		};
	};

	return;

}

int syscalls_free(){

	free(syscalls);

	return 1;

}
