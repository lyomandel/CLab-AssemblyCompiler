/*
 ============================================================================
 	 	 	 	 	 	 	 MAMAN 14
 	 	 	 	 	 	 	 MAIN SOURCE FILE
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "compiler.h"
int main(int argc, char** argv) {
	int i;
	if (argc < 2) {
		printf("No files found to compile, please try again \n");
	} else if (argc >= 2) {
		for (i = 1; i < argc; i++) {
			printf("\n");
			compile(argv[i]);
			printf("\n");
		}
	}
	return 1;
}
