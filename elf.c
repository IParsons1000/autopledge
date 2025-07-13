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

	int dyn = -1;
	int numrelo = 0;
	int *relo = NULL;
	int plt = -1;
	int got = -1;
	int text = -1;
	for(int i = 0; i < (elf->ehdr.e_shnum + 1); i++){
		if(elf->shdr[i].sh_type == SHT_DYNAMIC){
			dyn = i;
		};

		if(elf->shdr[i].sh_type == SHT_RELA){
			numrelo++;
			relo = realloc(relo, numrelo * sizeof(int));
			relo[numrelo-1] = i;
		};

		if(elf->shdr[i].sh_type == SHT_PROGBITS && !strcmp(&(elf->secs[elf->ehdr.e_shstrndx][elf->shdr[i].sh_name]), ".plt")){
			plt = i;
		};

		if(elf->shdr[i].sh_type == SHT_PROGBITS && !strcmp(&(elf->secs[elf->ehdr.e_shstrndx][elf->shdr[i].sh_name]), ".got.plt")){
			got = i;
		};

		if(elf->shdr[i].sh_type == SHT_PROGBITS && !strcmp(&(elf->secs[elf->ehdr.e_shstrndx][elf->shdr[i].sh_name]), ".text")){
			text = i;
		};

		if(i == elf->ehdr.e_shnum){
			break;
		};
	};


	if(!numrelo || relo == NULL){
		log_note("no relocation sections found in file %s", elf->name);
	};

	if(plt == -1){
		log_note("no procedure linkage table found in file %s", elf->name);
	};

	if(got == -1){
		log_note("no global offset table found in file %s", elf->name);
	};

	if(text == -1){
		log_note("no .text section found in file %s", elf->name);
	};

	elf->dynamic = dyn;
	elf->numrelo = numrelo;
	elf->relo = relo;
	elf->plt = plt;
	elf->got = got;
	elf->text = text;

	if(elf->dynamic == -1){
		log_note("no .dynamic section found in file %s", elf->name);
	}
	else{
		/* locate .dynsym and .dynstr section */

		int dsymi = -1;
		int dstri = -1;
		Elf64_Dyn *dynamic = (Elf64_Dyn *)elf->secs[elf->dynamic];

		for(int k = 0; k < (int)(elf->shdr[elf->dynamic].sh_size / elf->shdr[elf->dynamic].sh_entsize); k++){
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

	elf_t *elf;

	elf = elf_load(lib);
	if(elf == NULL){
		return dynsyms;
	};

	if(elf->dynsym == -1 || elf->dynstr == -1 || !elf->numrelo || (elf->plt == -1 && elf->got == -1) || elf->text == -1 || !elf->numneeded){
		return dynsyms;
	};

	Elf64_Sym *dynsymsec = (Elf64_Sym *)elf->secs[elf->dynsym];
	char *dynstrsec = (char *)elf->secs[elf->dynstr];

	int numdynsyms = 0;
	for(int i = 0; dynsyms[i] != NULL; i++){
		numdynsyms++;
	};

	/* reconstruct table mapping call address to symbol.
	 *  in most cases this will be call in .text -> plt entry -> got entry -> symbol,
	 *  but if the library was compiled with -fno-plt then it will be .text -> got entry -> symbol
	 */
	char **libdynsyms = NULL; /* list of dynamic symbols (native and external) */
	long *libdynaddr = NULL;  /* list of call addrs for each symbol */
	int *libdynidx = NULL;    /* list of .dynsym indices for each symbol */
	int *libdynused = NULL;   /* which of these are called in our target functions */
	int libdynnum = 0;

	long curr_addr = elf->shdr[elf->plt].sh_addr;
	int pltorgot = (elf->plt == -1) ? elf->got : elf->plt;
	long *working = NULL;

	for(int i = 0; i < (int)(elf->shdr[pltorgot].sh_size / elf->shdr[pltorgot].sh_entsize); i++){

		libdynaddr = realloc(libdynaddr, ++libdynnum * sizeof(long));
		if(libdynaddr == NULL){
			log_error("realloc() failed (%s)", strerror(errno));
			elf_free(elf);
			return dynsyms;
		};

		libdynaddr[libdynnum-1] = curr_addr;

		libdynused = realloc(libdynused, libdynnum * sizeof(int));
		if(libdynused == NULL){
			log_error("realloc() failed (%s)", strerror(errno));
			free(libdynaddr);
			elf_free(elf);
			return dynsyms;
		};

		if(pltorgot == elf->plt){
			/* jmp a d d r <@plt>   <--- significant one
			 * push v a l ue
			 * jmp a d d r <@.text>
			 */
			uint8_t *plt = (uint8_t *)&(elf->secs[elf->plt][(int)curr_addr-elf->shdr[elf->plt].sh_addr]);
			working = realloc(working, libdynnum * sizeof(long));
			if(working == NULL){
				log_error("realloc() failed (%s)", strerror(errno));
				free(libdynaddr);
				free(libdynused);
				elf_free(elf);
				return dynsyms;
			};
			if(((uint16_t *)plt)[0] == 0x25ff){ /* jmp = 0x25ff */
				working[libdynnum-1] = (int)curr_addr + *((uint32_t *)&(plt[2])) + 6; /* 6 is sizeof jmp instruction */
			}
			else{
				working[libdynnum-1] = -1;
			};
		};

		curr_addr += elf->shdr[pltorgot].sh_entsize;
	};

	if(working == NULL){
		working = libdynaddr;
	};

	libdynsyms = malloc(libdynnum * sizeof(char *));
	if(libdynsyms == NULL){
		log_error("malloc() failed (%s)", strerror(errno));
		free(libdynaddr);
		free(libdynused);
		if(working != NULL){
			free(working);
		};
		elf_free(elf);
		return dynsyms;
	};

	libdynidx = malloc(libdynnum * sizeof(int));
	if(libdynidx == NULL){
		log_error("malloc() failed (%s)", strerror(errno));
		free(libdynaddr);
		free(libdynused);
		free(libdynsyms);
		if(working != NULL){
			free(working);
		};
		elf_free(elf);
		return dynsyms;
	};

	memset(libdynused, 0, libdynnum * sizeof(int));

	for(int i = 0; i < elf->numrelo; i++){
		Elf64_Rela *relos = (Elf64_Rela *)elf->secs[elf->relo[i]];
		for(int j = 0; j < (int)(elf->shdr[elf->relo[i]].sh_size / elf->shdr[elf->relo[i]].sh_entsize); j++){
			for(int k = 0; k < libdynnum; k++){
				if(relos[j].r_offset == (unsigned)working[k]){
					libdynsyms[k] = &dynstrsec[dynsymsec[ELF64_R_SYM(relos[j].r_info)].st_name];
					libdynidx[k] = ELF64_R_SYM(relos[j].r_info);
					break;
				};
			};
		};
	};

	/* detect calls used in target functions from searching for them in the function's code */
	if(elf->text != -1){
		for(int i = 0; i < numdynsyms; i++){
			long start = elf->shdr[elf->text].sh_addr;
			long end = start;

			for(int x = 0; x < (int)(elf->shdr[elf->dynsym].sh_size / elf->shdr[elf->dynsym].sh_entsize); x++){
				if(!strcmp(dynsyms[i], &dynstrsec[dynsymsec[x].st_name])){
					start = dynsymsec[x].st_value;
					end = start + dynsymsec[x].st_size;
					break;
				};
			};

			/* only analyze if function is local and hasn't already been analyzed */
			if(!start || start == end){
				continue;
			};

			for(long x = start; x < end; x++){
				int rel_addr = x-elf->shdr[elf->text].sh_addr;
				if(elf->secs[elf->text][rel_addr] == (char)0xe8){
					/* 0xe8 = call */
					uint32_t maybe = (uint32_t)(x + *((int32_t *)&(elf->secs[elf->text][++rel_addr])) + 5); /* 5 is sizeof call instruction */
					if(maybe >= libdynaddr[0] && maybe <= libdynaddr[libdynnum-1] && !(maybe % 16) && !libdynused[(maybe-libdynaddr[0])/16]){
						if(dynsymsec[libdynidx[(maybe-libdynaddr[0])/16]].st_value){
							char found = 0;
							char *name = NULL;
							for(int z = 0; z < numdynsyms; z++){
								name = &(dynstrsec[dynsymsec[libdynidx[(maybe-libdynaddr[0])/16]].st_name]);
								if(!strcmp(name, dynsyms[i])){
									found = 1;
									break;
								};
							};

							if(!found){
								dynsyms = realloc(dynsyms, (++numdynsyms + 1) * sizeof(char *));
								if(dynsyms == NULL){
									log_error("malloc() failed (%s)", strerror(errno));
									free(libdynaddr);
									free(libdynidx);
									free(libdynused);
									free(libdynsyms);
									if(working != NULL){
										free(working);
									};
									elf_free(elf);
									return dynsyms;
								};
								dynsyms[numdynsyms-1] = name;
								dynsyms[numdynsyms] = NULL;
							};
						}
						else{
							libdynused[(maybe-libdynaddr[0])/16] = 1;
						};
						x += 4;
					};
				};
			};
		};
	};

	if(working != libdynaddr){
		free(working);
	};

	/* allow syscalls for used functions */
	if(libdynnum && libdynsyms != NULL && libdynaddr != NULL && libdynused != NULL){
		char **temp = NULL;
		int numtemp = 0;
		
		for(int i = 0; i < libdynnum; i++){
			if(libdynused[i]){
				temp = realloc(temp, ++numtemp * sizeof(char *));
				if(temp == NULL){
					log_error("realloc() failed (%s)", strerror(errno));
					free(libdynsyms);
					free(libdynidx);
					free(libdynused);
					free(libdynaddr);
					elf_free(elf);
					return dynsyms;
				};
				temp[numtemp-1] = libdynsyms[i];
			};
		};

		temp = realloc(temp, (numtemp + 1) * sizeof(char *));
		if(temp == NULL){
			log_error("realloc() failed (%s)", strerror(errno));
			free(libdynsyms);
			free(libdynidx);
			free(libdynused);
			free(libdynaddr);
			elf_free(elf);
			return dynsyms;
		};
		temp[numtemp] = NULL;

		free(libdynsyms);
		free(libdynidx);
		free(libdynused);
		free(libdynaddr);

		libdynsyms = temp;
		libdynnum = numtemp;

		if(libdynsyms != NULL){

			for(int i = 0; i < elf->numneeded; i++){
				char done = 0;

				for(int j = 0; j < DYN_NUM_HANDLERS; j++){
					if(!strcmp(elf->needed[i], dyn_handler[j].obj)){
						done++;
						libdynsyms = (*dyn_handler[j].handler)(libdynsyms);
						break;
					};
				};

				if(!done){
					libdynsyms = elf_get_lib_dyn_syscalls(elf->needed[i], libdynsyms);
				};
			};

		};
	};

	elf_free(elf);

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

		if(elf->relo != NULL){
			free(elf->relo);
		};

		free(elf);
	};

	return;

}
