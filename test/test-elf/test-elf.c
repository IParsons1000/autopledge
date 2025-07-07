/*
 *
 * (c)2025 Ira Parsons
 * autopledge test suite - test-elf util
 *
 */

#include "../../elf.h"

/* Return values:
 * 	0 - success
 * 	1 - elf_load failed
 * 	2 - elf_get_dynsym failed
 *
 */

int main(int argc, char *argv[]){

	elf_t *elf = elf_load(argv[1]);
	if(elf == NULL){
		return 1;
	};

	char **dynsym = elf_get_dynsym(elf);
	if(elf == NULL){
		return 2;
	};

	elf_free(elf);

	return 0;

}
