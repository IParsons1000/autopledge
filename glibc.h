/*
 *
 * (c)2025 Ira Parsons
 * glibc.h - glibc syscall-by-function info (specification)
 *
 */

#ifndef _AUTOPLEDGE_GLIBC_H
#define _AUTOPLEDGE_GLIBC_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

char **glibc_get_syscalls(char **funs);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUTOPLEDGE_GLIBC_H */
