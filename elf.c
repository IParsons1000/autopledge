/*
 *
 * (c)2025 Ira Parsons
 * elf.c - elf parsing implementation
 *
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include "elf.h"
#include "glibc.h"
#include "log.h"

dyn_handler_t dyn_handler[DYN_NUM_HANDLERS] = {
	{ "libc.so.6", &glibc_get_syscalls }
};

elf_t *elf_load(char *file);
char **elf_get_dynsym(elf_t *elf);
void elf_get_dyn_syscalls(elf_t *elf);
char **elf_get_lib_dyn_syscalls(char *lib, char **dynsym);
void elf_free(elf_t *elf);

elf_t *elf_load(char *file){

	/* open binary for reading */

	int fd;
	fd = open(file, O_RDONLY);
	if(fd == -1){
		log_error("cannot open file %s (%s)", file, strerror(errno));
		return NULL;
	};

	/* setup new elf structure */

	elf_t *elf = malloc(sizeof(elf_t));
	if(elf == NULL){
		log_error("malloc() failed (%s)", strerror(errno));
		return NULL;
	};

	elf->name = malloc(strlen(file));
	if(elf->name == NULL){
		log_error("malloc() failed (%s)", strerror(errno));
		return NULL;
	};

	elf->name = strncpy(elf->name, file, strlen(file));
	if(elf->name == NULL){
		log_error("strncpy() failed (%s)", strerror(errno));
		return NULL;
	};

	/* read elf header into memory */

	if(read(fd, &elf->ehdr, sizeof(Elf64_Ehdr)) != sizeof(Elf64_Ehdr)){
		log_error("unable to load elf header of file %s", file);
		elf_free(elf);
		return NULL;
	};

	/* sanity check elf header */

	if(!( elf->ehdr.e_ident[0] == ELFMAG0 && elf->ehdr.e_ident[1] == ELFMAG1 && elf->ehdr.e_ident[2] == ELFMAG2 && elf->ehdr.e_ident[3] == ELFMAG3 )){
		log_error("file %s is not in elf format", file);
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
			log_error("malloc() failed (%s)", strerror(errno));
			elf_free(elf);
			return NULL;
		};

		if(lseek(fd, elf->ehdr.e_phoff, SEEK_SET) == -1){
			log_error("unable to load program header(s) of file %s", file);
			elf_free(elf);
			return NULL;
		};

		for(int i = 0; i < elf->ehdr.e_phnum; i++){
			if(read(fd, &elf->phdr[i], sizeof(Elf64_Phdr)) != sizeof(Elf64_Phdr)){
				log_error("unable to read program header(s) of file %s", file);
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
			log_error("malloc() failed (%s)", strerror(errno));
			elf_free(elf);
			return NULL;
		};

		for(int i = 0; i < elf->ehdr.e_phnum; i++){
			elf->segs[i] = malloc(elf->phdr[i].p_filesz + 1);
			if(elf->segs[i] == NULL){
				log_error("malloc() failed (%s)", strerror(errno));
				elf_free(elf);
				return NULL;
			};

			if(lseek(fd, elf->phdr[i].p_offset, SEEK_SET) == -1 || read(fd, elf->segs[i], elf->phdr[i].p_filesz) != (ssize_t)elf->phdr[i].p_filesz){
				log_error("unable to load program segment %d of file %s", i, file);
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
			log_error("malloc() failed (%s)", strerror(errno));
			elf_free(elf);
			return NULL;
		};

		if(lseek(fd, elf->ehdr.e_shoff, SEEK_SET) == -1){
			log_error("unable to load section header(s) of file %s", file);
			elf_free(elf);
			return NULL;
		};

		for(int i = 0; i < elf->ehdr.e_shnum; i++){
			if(read(fd, &elf->shdr[i], sizeof(Elf64_Shdr)) != sizeof(Elf64_Shdr)){
				log_error("unable to read section header(s) of file %s", file);
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
			log_error("malloc() failed (%s)", strerror(errno));
			elf_free(elf);
			return NULL;
		};

		for(int i = 0; i < elf->ehdr.e_shnum; i++){
			elf->secs[i] = malloc(elf->shdr[i].sh_size + 1);
			if(elf->secs[i] == NULL){
				log_error("malloc() failed (%s)", strerror(errno));
				elf_free(elf);
				return NULL;
			};

			if(lseek(fd, elf->shdr[i].sh_offset, SEEK_SET) == -1 || read(fd, elf->secs[i], elf->shdr[i].sh_size) != (ssize_t)elf->shdr[i].sh_size){
				log_error("unable to load section %s of file %s", (char *)&elf->shdr[i].sh_name, file);
				elf_free(elf);
				return NULL;
			};

			elf->secs[i][elf->shdr[i].sh_size] = 0; /* Add null termination, because these are raw data not strings */
		};
	};

	/* locate .dynamic and relocation sections */

	int i;
	int numrelo = 0;
	int *relo = NULL;
	for(i = 0; i < (elf->ehdr.e_shnum + 1); i++){
		if(elf->shdr[i].sh_type == SHT_DYNAMIC){
			break;
		};

		if(elf->shdr[i].sh_type == SHT_RELA){
			numrelo++;
			relo = realloc(relo, numrelo * sizeof(int));
			relo[numrelo-1] = i;
		};

		if(i == elf->ehdr.e_shnum){
			i = -1;
			break;
		};
	};


	if(!numrelo || relo == NULL){
		log_note("no relocation sections found in file %s", elf->name);
	};

	elf->numrelo = numrelo;
	elf->relo = relo;

	if(i == -1){
		log_note("no .dynamic section found in file %s", elf->name);
		elf->dynamic = -1;
	}
	else{
		elf->dynamic = i;

		/* locate .dynsym and .dynstr section */

		int dsymi = -1;
		int dstri = -1;
		Elf64_Dyn *dynamic = (Elf64_Dyn *)elf->secs[elf->dynamic];

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
			log_note("no .dynsym section in file %s", elf->name);
			return NULL;
		};

		if(dstri == -1){
			log_note("no .dynstr section in file %s", elf->name);
			return NULL;
		};

		elf->dynsym = dsymi;
		elf->dynstr = dstri;

		/* enumerate needed shared objects */

		if(!elf->shdr[elf->dynamic].sh_size || !elf->shdr[elf->dynamic].sh_entsize){
			return NULL;
		};

		int numneeded = 0;
		char **needed = NULL;

		for(int k = 0; k < (int)(elf->shdr[elf->dynamic].sh_size / elf->shdr[elf->dynamic].sh_entsize); k++){
			if(((Elf64_Dyn *)(elf->secs[elf->dynamic]))[k].d_tag == DT_NEEDED){
				needed = realloc(needed, ++numneeded * sizeof(char *));
				if(needed == NULL){
					log_error("realloc() failed (%s)", strerror(errno));
					return NULL;
				};
				needed[numneeded-1] = &elf->secs[elf->dynstr][(int)((Elf64_Dyn *)(elf->secs[elf->dynamic]))[k].d_un.d_val];
			};
		};

		elf->numneeded = numneeded;
		elf->needed = needed;

	};

	return elf;

}

char **elf_get_dynsym(elf_t *elf){

	if(elf == NULL || elf->shdr == NULL || elf->secs == NULL){
		return NULL;
	};

	/* load .dynamic section */

	if(elf->dynamic == -1 || elf->dynsym == -1 || elf->dynstr == -1){
		return NULL;
	};

	Elf64_Dyn *dynamic = (Elf64_Dyn *)elf->secs[elf->dynamic];

	if(dynamic == NULL){
		return NULL;
	};

	/* load .dynsym section as a symbol table and .dynstr as a string table */

	Elf64_Sym *dynsym = (Elf64_Sym *)elf->secs[elf->dynsym];
	char *dynstr = elf->secs[elf->dynstr];

	if(elf->shdr[elf->dynsym].sh_entsize != sizeof(Elf64_Sym)){
		log_error(".dynsym entry size does not match expected value in file %s", elf->name);
		return NULL;
	};

	/* return dynamically linked functions */

	char **syms = NULL;
	int numsyms = 0;

	for(int j = 0; j < (int)(elf->shdr[elf->dynsym].sh_size / elf->shdr[elf->dynsym].sh_entsize); j++){
		if(dynsym[j].st_info & STT_FUNC){
			numsyms++;
			syms = realloc(syms, numsyms * sizeof(char *));
			if(syms == NULL){
				log_error("realloc() failed (%s)", strerror(errno));
				return NULL;
			};
			syms[numsyms-1] = &dynstr[dynsym[j].st_name];
		};
	};

	/* add null terminating entry */

	syms = realloc(syms, (numsyms + 1) * sizeof(char *));
	if(syms == NULL){
		log_error("realloc() failed (%s)", strerror(errno));
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

void elf_get_dyn_syscalls(elf_t *elf){

	if(!elf->numneeded || elf->needed == NULL){
		return;
	};

	char **dynsyms = elf_get_dynsym(elf);

	for(int i = 0; i < elf->numneeded; i++){
		char done = 0;
		for(int j = 0; j < DYN_NUM_HANDLERS; j++){
			if(!strcmp(elf->needed[i], dyn_handler[j].obj)){
				done++;
				dynsyms = (*dyn_handler[j].handler)(dynsyms);
				break;
			};
		};

		if(!done){

			dynsyms = elf_get_lib_dyn_syscalls(elf->needed[i], dynsyms);
		};
	};

	return;

};

char **elf_get_lib_dyn_syscalls(char *lib, char **dynsyms){

	elf_t *bin;

	bin = elf_load(lib);
	if(bin == NULL){
		return dynsyms;
	};

	Elf64_Sym *dynsymsec = (Elf64_Sym *)bin->secs[bin->dynsym];
	char *dynstrsec = (char *)bin->secs[bin->dynstr];

	/* assume that the number of symbols in a library will be greater than in the
	 *  executable */
	for(int i = 0; i < (int)(bin->shdr[bin->dynsym].sh_size / bin->shdr[bin->dynsym].sh_entsize); i++){
		if( (dynsymsec[i].st_info & STT_FUNC) && ((dynsymsec[i].st_info >> 4) & STB_GLOBAL)){
			for(int j = 0; dynsyms[j] != NULL; j++){
				if(!strcmp(dynsyms[j], &(dynstrsec[dynsymsec[i].st_name]))){
					// found the symbol: dynsymsec[i]
					int start = 0, end = 0;
					int found = 0;

					/* locate symbol in relocation table */
					for(int a = 0; a < bin->numrelo; a++){
						Elf64_Rela *relos = (Elf64_Rela *)bin->secs[bin->relo[a]];
						for(int b = 0; b < (int)(bin->shdr[bin->relo[a]].sh_size / bin->shdr[bin->relo[a]].sh_entsize); b++){
							if((int)ELF64_R_SYM(relos[b].r_info) == i){
								found = 1;

								int a = relos[b].r_addend;
								//int b = bin->phdr[0].p_vaddr;
								//int p = relos[b].r_offset;
								int s = dynsymsec[i].st_value;
								//int z = dynsymsec[i].st_size;

								switch(ELF64_R_TYPE(relos[b].r_info)){
									case R_X86_64_GLOB_DAT:
									case R_X86_64_JUMP_SLOT:
										start = s;
										break;
									case R_X86_64_64:
									case R_X86_64_32:
									case R_X86_64_32S:
									case R_X86_64_16:
									case R_X86_64_8:
										start = s + a;
										break;
									default:
										found = 0;
										break;
								};

								end = (found) ? (start + dynsymsec[i].st_size) : 0;
								break;
							};
						};

						if(found){
							break;
						};
					};

					if(found){
						//search for calls
						end=end;
					};
				};
			};
		};
	};

	elf_get_dyn_syscalls(bin);

	elf_free(bin);

	return dynsyms;

};

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

		if(elf->needed != NULL){
			free(elf->needed);
		};

		free(elf);
	};

	return;

}
