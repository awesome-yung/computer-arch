/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   sce212sim.c                                               */
/*                                                             */
/***************************************************************/

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*          DO NOT MODIFY THIS FILE!                            */
/*          You should only the proc.c file!                    */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <getopt.h>

#include "util.h"
#include "run.h"


/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename) {
    init_memory();
    load_program(program_filename);
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
    int debug_set = 0;
    char** tokens;
    int i = 200;		//for loop

    int num_inst_set = 0;
    int num_inst = 0;

    int mem_dump_set = 0;
    int start_addr = 0;
    int end_addr = 0;

    int opt;
    extern int optind;
    char *trace_name;
    FILE *fp;
    char line[33];

    initialize(argv[argc-1]);

    run(i);


    exit(EXIT_SUCCESS);
}
