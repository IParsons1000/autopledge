/*
 *
 * (c)2025 Ira Parsons
 * glibcxx.h - c++ stdlib syscall-by-function info (specification)
 *
 */

#ifndef _AUTOPLEDGE_GLIBCXX_H
#define _AUTOPLEDGE_GLIBCXX_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "syscalls.h"

char **glibcxx_get_syscalls(char **funs);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUTOPLEDGE_GLIBCXX_H */
