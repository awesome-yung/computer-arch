#include <stdio.h>
#include <stdlib.h>
#include "reg.h"


int main(int argc, char *argv[]) {
	FILE *prog;
    char buffer[32 + 2];

    printf(argv[argc-1]);
    printf("\n");
    char* file_name = argv[argc-1];

    prog = fopen(file_name, "r");


    // 1st: read the size of text section
    if (fgets(buffer, 32 + 1, prog) != NULL) {
        printf("buffer =  %s\n",buffer);
    }
    print_result();
    exit(EXIT_SUCCESS);
}