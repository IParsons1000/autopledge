/*
 *
 * (c)2025 Ira Parsons
 * elf.c - elf parsing implementation
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "elf.h"

elf_t *elf_load(char *file);
char **elf_get_dynsym(elf_t *elf);
void elf_free(elf_t *elf);

elf_t *elf_load(char *file){

	/* open binary for reading */

	int fd;
	fd = open(file, O_RDONLY);
	if(fd == -1){
		printf("Error: could not open %s\n", file);
		return NULL;
	};

	/* setup new elf structure */

	elf_t *elf = malloc(sizeof(elf_t));
	if(elf == NULL){
		printf("Error: malloc(sizeof(elf_t)) failed\n");
		return NULL;
	};

	elf->name = malloc(strlen(file));
	if(elf->name == NULL){
		printf("Error: malloc(strlen(file)) failed\n");
	};

	elf->name = strncpy(elf->name, file, strlen(file));
	if(elf->name == NULL){
		printf("Error: strncpy(elf->name, file, strlen(file)) failed\n");
	};

	/* read elf header into memory */

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

char **elf_get_dynsym(elf_t *elf){

	if(elf == NULL || elf->shdr == NULL || elf->secs == NULL){
		return NULL;
	};

	/* locate .dynamic section */

	int i;
	for(i = 0; i < (elf->ehdr.e_shnum + 1); i++){
		if(elf->shdr[i].sh_type == SHT_DYNAMIC){
			break;
		};

		if(i == elf->ehdr.e_shnum){
			i = -1;
			break;
		};
	};

	if(i == -1){
		printf("Note: no .dynamic section found in file %s", elf->name);
		return NULL;
	};

	/* load .dynamic section */

	Elf64_Dyn *dynamic = (Elf64_Dyn *)elf->secs[i];

	if(dynamic == NULL){
		return NULL;
	};

	/* locate .dynsym and .dynstr section */

	int dsymi = -1;
	int dstri = -1;

	for(int k = 0; k < (int)(elf->shdr[i].sh_size / elf->shdr[i].sh_entsize); k++){
		if(dynamic[k].d_tag == DT_SYMTAB){
			for(int a = 0; a < elf->ehdr.e_shnum; a++){
				if(elf->shdr[a].sh_addr == dynamic[k].d_un.d_ptr){
					dsymi = a;
					break;
				};
			};
		};

		if(dynamic[k].d_tag == DT_STRTAB){
			for(int a = 0; a < elf->ehdr.e_shnum; a++){
				if(elf->shdr[a].sh_addr == dynamic[k].d_un.d_ptr){
					dstri = a;
					break;
				};
			};
		};
	};

	if(dsymi == -1){
		printf("Error: no .dynsym section in file %s\n", elf->name);
		return NULL;
	};

	if(dstri == -1){
		printf("Error: no .dynstr section in file %s\n", elf->name);
		return NULL;
	};

	/* load .dynsym section as a symbol table and .dynstr as a string table */

	Elf64_Sym *dynsym = (Elf64_Sym *)elf->secs[dsymi];
	char *dynstr = elf->secs[dstri];

	if(elf->shdr[dsymi].sh_entsize != sizeof(Elf64_Sym)){
		printf("Error: .dynsym entry size does not match expected value in file %s\n", elf->name);
		return NULL;
	};

	/* return dynamically linked functions */

	char **syms = malloc(sizeof(char *));
	int numsyms = 0;

	if(syms == NULL){
		printf("Error: malloc(0) failed\n");
		return NULL;
	};

	for(int j = 0; j < (int)(elf->shdr[dsymi].sh_size / elf->shdr[dsymi].sh_entsize); j++){
		if(dynsym[j].st_info & STT_FUNC){
			numsyms++;
			syms = realloc(syms, numsyms * sizeof(char *));
			if(syms == NULL){
				printf("Error: realloc(syms, numsyms * sizeof(char *)) failed\n");
				return NULL;
			};
			syms[numsyms-1] = &dynstr[dynsym[j].st_name];
		};
	};

	/* add null terminating entry */

	syms = realloc(syms, (numsyms + 1) * sizeof(char *));
	if(syms == NULL){
		printf("Error: realloc(syms, (numsyms + 1) * sizeof(char *)) failed\n");
		return NULL;
	};
	syms[numsyms] = NULL;

	if(!numsyms){
		free(syms);
		return NULL;
	}
	else{
		return syms;
	};

}

void elf_free(elf_t *elf){

	if(elf != NULL){
		if(elf->name != NULL){
			free(elf->name);
		};

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
