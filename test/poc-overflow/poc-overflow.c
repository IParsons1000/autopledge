/*
 *
 * (c)2025 Ira Parsons
 * autopledge test suite - poc-overflow target vulnerable program
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){

	char buf[16];

	gets(buf);

	return 0;

}
