/*
 *
 * (c)2025 Ira Parsons
 * autopledge test suite - test-recursive helper (b.so)
 *
 */

#include "b.h"
#include "c.h"

int b(char *f){

	return c(f);

};
