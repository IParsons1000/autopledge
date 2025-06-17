/*
 *
 * (c)2025 Ira Parsons
 * seccomp.h - interface to filter syscalls with seccomp (specification)
 *
 */

#ifndef _AUTOPLEDGE_SECCOMP_H
#define _AUTOPLEDGE_SECCOMP_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


void seccomp_restrict(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUTOPLEDGE_SECCOMP_H */
