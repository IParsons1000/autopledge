/*
 *
 * (c)2025 Ira Parsons
 * autopledge test suite - test-recursive util
 *
 */

#include <stdio.h>
#include "b.h"

/* Return values:
 * 	6 - success
 *
 */

int main(void){

	int a = b("a.c");

	printf("%d\n", a);

	return a;

};
