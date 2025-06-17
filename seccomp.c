/*
 *
 * (c)2025 Ira Parsons
 * seccomp.c - interface to filter syscalls with seccomp (implementation)
 *
 */

#include <linux/seccomp.h>
#include <linux/filter.h>
#include <linux/audit.h>
#include <linux/signal.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <unistd.h>
#include "seccomp.h"
#include "syscalls.h"

void seccomp_restrict(void);

void seccomp_restrict(){

	return;

}
