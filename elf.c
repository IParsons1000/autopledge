/*
 *
 * (c)2025 Ira Parsons
 * elf.c - elf parsing implementation
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "elf.h"

elf_t *elf_load(char *file);

elf_t *elf_load(char *file){

	/* open binary for reading */

	int fd;
	fd = open(file, O_RDONLY);
	if(fd == -1){
		printf("Error: could not open %s\n", file);
		return NULL;
	};

	/* read elf header into memory */

	elf_t *elf = malloc(sizeof(elf_t));
	if(elf == NULL){
		printf("Error: malloc(sizeof(elf_t)) failed\n");
		return NULL;
	};

	if(read(fd, &elf->ehdr, sizeof(Elf64_Ehdr)) != sizeof(Elf64_Ehdr)){
		printf("Error: unable to load elf header of file %s\n", file);
		free(elf);
		return NULL;
	};

	/* sanity check elf header */

	if(!( elf->ehdr.e_ident[0] == ELFMAG0 && elf->ehdr.e_ident[1] == ELFMAG1 && elf->ehdr.e_ident[2] == ELFMAG2 && elf->ehdr.e_ident[3] == ELFMAG3 )){
		printf("Error: file %s is not in elf format\n", file);
		free(elf);
		return NULL;
	};

	return elf;

};
