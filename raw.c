/*
 *
 * (c)2025 Ira Parsons
 * raw.c - detect raw assembly syscalls (implementation)
 *
 */

#include <stdio.h>
#include "elf.h"
#include "syscalls.h"
#include "raw.h"

void raw_get_syscalls(elf_t *elf);

void raw_get_syscalls(elf_t *elf){

	if(!elf->numexec || elf->exec == NULL){
		printf("Note: no executable sections found in file %s\n", elf->name);
		return;
	};

	return;

}
