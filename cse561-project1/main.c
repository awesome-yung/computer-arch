#include <stdio.h>
#include <stdlib.h>
#include "reg.h"

int ROB_SIZE;
int IQ_SIZE;
int WIDTH;

int main(int argc, char *argv[]) {

    char opt;
    // while ((opt=getopt(argc, argv, "n:")) != -1) {
    //     // printf("opt = %c\n",opt);
    //     switch (opt) 
    //     {
    //         case 'r':
    //             ROB_SIZE = 1;
    //             break;
    //         case 'i':
    //             IQ_SIZE = 1;
    //             break;
    //         case 'w':
    //             WIDTH = 1;
    //             break;
    //     }

    // }

    // printf("r=%s, i=%s, w=%s\n",ROB_SIZE, IQ_SIZE,WIDTH);
    char* file_name = argv[argc-1];
    
    run(file_name);

    print_result(file_name);

    // printf("tititi\n\n\n\n");
    // printf("tg34gae\n\n\n\n");

    exit(EXIT_SUCCESS);
}