/*
 *
 * (c)2025 Ira Parsons
 * autopledge test suite - test-recursive helper (c.so)
 *
 */

#include <unistd.h>
#include <net/if.h>
#include "c.h"

int c(char *f){

	f = d(f);

	usleep(42);

	return 6;

};

char *d(char *f){

	char *e;

	e = if_indextoname(14, f);

	return e;

};
