/*
 *
 * (c)2025 Ira Parsons
 * raw.c - detect raw assembly syscalls (implementation)
 *
 */

#include <stdio.h>
#include <stddef.h>
#include "elf.h"
#include "syscalls.h"
#include "raw.h"

#define OP_SYSCALL (uint16_t)0x0f05 /* syscall */

void raw_get_syscalls(elf_t *elf);

void raw_get_syscalls(elf_t *elf){

	if(!elf->numexec || elf->exec == NULL){
		return;
	};

	for(int i = 0; i < elf->numexec; i++){
		for(long int j = 0; j < (long int)elf->shdr[elf->exec[i]].sh_size; j+=2){
			uint16_t *opc = (uint16_t *)(&(elf->secs[elf->exec[i]][j]));
			if((*opc) == OP_SYSCALL){
printf("Found syscall : %ld\n", j);
			};
		};
	};

	return;

}
