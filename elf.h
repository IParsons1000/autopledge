/*
 *
 * (c)2025 Ira Parsons
 * elf.h - elf parsing specification
 *
 */

#ifndef _AUTOPLEDGE_ELF_H
#define _AUTOPLEDGE_ELF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <elf.h>

typedef struct elf_s {
	char *name;        /* file name (for diagnostic messages) */
	Elf64_Ehdr ehdr;   /* elf header */
	Elf64_Phdr *phdr;  /* array ofprogram headers */
	Elf64_Shdr *shdr;  /* array of section headers */
	char **segs;       /* array of raw contents of segments */
	char **secs;       /* array of raw contents of sections */
} elf_t;

elf_t *elf_load(char *file);
char **elf_get_dynsym(elf_t *elf);
void elf_free(elf_t *elf);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUTOPLEDGE_ELF_H */
