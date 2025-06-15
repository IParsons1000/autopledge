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
void elf_free(elf_t *elf);

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
		elf_free(elf);
		return NULL;
	};

	/* sanity check elf header */

	if(!( elf->ehdr.e_ident[0] == ELFMAG0 && elf->ehdr.e_ident[1] == ELFMAG1 && elf->ehdr.e_ident[2] == ELFMAG2 && elf->ehdr.e_ident[3] == ELFMAG3 )){
		printf("Error: file %s is not in elf format\n", file);
		elf_free(elf);
		return NULL;
	};

	/* load program header(s) */

	if(!elf->ehdr.e_phnum){
		elf->phdr = NULL;
	}
	else{
		elf->phdr = malloc(elf->ehdr.e_phnum * elf->ehdr.e_phentsize);
		if(elf->phdr == NULL){
			printf("Error: malloc(elf->ehdr.e_phnum * elf->ehdr.e_phentsize) failed\n");
			elf_free(elf);
			return NULL;
		};

		if(lseek(fd, elf->ehdr.e_phoff, SEEK_SET) == -1){
			printf("Error: unable to load program header(s) of file %s\n", file);
			elf_free(elf);
			return NULL;
		};

		for(int i = 0; i < elf->ehdr.e_phnum; i++){
			if(read(fd, &elf->phdr[i], sizeof(Elf64_Phdr)) != sizeof(Elf64_Phdr)){
				printf("Error: unable to read program header(s) of file %s\n", file);
				elf_free(elf);
				return NULL;
			};
		};
	};

	/* load program segments */

	if(elf->phdr == NULL){
		elf->segs = NULL;
	}
	else{
		elf->segs = malloc(elf->ehdr.e_phnum * sizeof(char *));
		if(elf->segs == NULL){
			printf("Error: malloc(elf->ehdr.e_phnum * sizeof(char *)) failed\n");
			elf_free(elf);
			return NULL;
		};

		for(int i = 0; i < elf->ehdr.e_phnum; i++){
			elf->segs[i] = malloc(elf->phdr[i].p_filesz + 1);
			if(elf->segs[i] == NULL){
				printf("Error: malloc(elf->phdr[i].p_filesz + 1) failed\n");
				elf_free(elf);
				return NULL;
			};

			if(lseek(fd, elf->phdr[i].p_offset, SEEK_SET) == -1 || read(fd, elf->segs[i], elf->phdr[i].p_filesz) != (ssize_t)elf->phdr[i].p_filesz){
				printf("Error: unable to load program segment %d of file %s\n", i, file);
				elf_free(elf);
				return NULL;
			};

			elf->segs[i][elf->phdr[i].p_filesz] = 0; /* Add null termination, because these are raw data not strings */
		};
	};

	/* load section header(s) */

	if(!elf->ehdr.e_shnum){
		elf->shdr = NULL;
	}
	else{
		elf->shdr = malloc(elf->ehdr.e_shnum * elf->ehdr.e_shentsize);
		if(elf->shdr == NULL){
			printf("Error: malloc(elf->ehdr.e_shnum * elf->ehdr.e_shentsize) failed\n");
			elf_free(elf);
			return NULL;
		};

		if(lseek(fd, elf->ehdr.e_shoff, SEEK_SET) == -1){
			printf("Error: unable to load section header(s) of file %s\n", file);
			elf_free(elf);
			return NULL;
		};

		for(int i = 0; i < elf->ehdr.e_shnum; i++){
			if(read(fd, &elf->shdr[i], sizeof(Elf64_Shdr)) != sizeof(Elf64_Shdr)){
				printf("Error: unable to read section header(s) of file %s\n", file);
				elf_free(elf);
				return NULL;
			};
		};
	};

	/* load sections */

	if(elf->shdr == NULL){
		elf->secs = NULL;
	}
	else{
		elf->secs = malloc(elf->ehdr.e_shnum * sizeof(char *));
		if(elf->secs == NULL){
			printf("Error: malloc(elf->ehdr.e_shnum * sizeof(char *)) failed\n");
			elf_free(elf);
			return NULL;
		};

		for(int i = 0; i < elf->ehdr.e_shnum; i++){
			elf->secs[i] = malloc(elf->shdr[i].sh_size + 1);
			if(elf->secs[i] == NULL){
				printf("Error: malloc(elf->shdr[i].sh_size + 1) failed\n");
				elf_free(elf);
				return NULL;
			};

			if(lseek(fd, elf->shdr[i].sh_offset, SEEK_SET) == -1 || read(fd, elf->secs[i], elf->shdr[i].sh_size) != (ssize_t)elf->shdr[i].sh_size){
				printf("Error: unable to load section %s of file %s\n", (char *)&elf->shdr[i].sh_name, file);
				elf_free(elf);
				return NULL;
			};

			elf->secs[i][elf->shdr[i].sh_size] = 0; /* Add null termination, because these are raw data not strings */
		};
	};

	return elf;

}

void elf_free(elf_t *elf){

	if(elf != NULL){
		if(elf->phdr != NULL){
			if(elf->segs != NULL){
				for(int i = 0; i < elf->ehdr.e_phnum; i++){
					if(elf->segs[i] != NULL){
						free(elf->segs[i]);
					};
				};
				free(elf->segs);
			};

			free(elf->phdr);
		};

		if(elf->shdr != NULL){
			if(elf->secs != NULL){
				for(int i = 0; i < elf->ehdr.e_shnum; i++){
					if(elf->secs[i] != NULL){
						free(elf->secs[i]);
					};
				};
				free(elf->secs);
			};

			free(elf->shdr);
		};

		free(elf);
	};

	return;

}
