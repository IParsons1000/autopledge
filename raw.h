/*
 *
 * (c)2025 Ira Parsons
 * raw.h - detect raw assembly syscalls (specification)
 *
 */

#ifndef _AUTOPLEDGE_RAW_H
#define _AUTOPLEDGE_RAW_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "elf.h"

void raw_get_syscalls(elf_t *elf);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUTOPLEDGE_RAW_H */
