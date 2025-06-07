/*
 *
 * (c)2025 Ira Parsons
 * speed test for autopledge
 *
 */

#include <stdio.h>
#include <math.h>

int main(int argc, char *argv[]){

	double e = 1.0 + 0x1p-50;

	for(int i = 0; i < 50; i++){
		e *= e;
	};

	printf("%f\n", e);

	return 0;

};
