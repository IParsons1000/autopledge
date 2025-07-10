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
	int dynamic;       /* shdr index of .dynamic section */
	int dynsym;        /* shdr index of .dynsym section */
	int dynstr;        /* shdr index of .dynstr section */
	char **needed;     /* array of names of needed so's */
	int numneeded;     /* number of needed so's */
	int *relo;         /* array of shdr indices of relocation sections */
	int numrelo;       /* number of relocation sections */
	int plt;           /* shdr index of .plt section */
	int got;           /* shdr index of .got.plt section */
	int text;          /* shdr index of .text section */
} elf_t;

typedef struct dyn_handler_s {
	char *obj;
	char **(*handler)(char **);
} dyn_handler_t;

#define DYN_NUM_HANDLERS 1

elf_t *elf_load(char *file);
char **elf_get_dynsym(elf_t *elf);
void elf_get_dyn_syscalls(elf_t *elf);
void elf_free(elf_t *elf);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUTOPLEDGE_ELF_H */
