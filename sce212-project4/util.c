/// loader.c /// 
#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "util.h"


void load_inst_to_mem(const char *buffer, const int index)
{
    int word;

    word = str_to_int(buffer);
    mem_write_32(MEM_TEXT_START + index, word);
}

void load_data_to_mem(const char *buffer, const int index)
{
    uint32_t word;
    word = str_to_int(buffer);
    mem_write_32(MEM_DATA_START + index, word);
}

/**************************************************************/
/*                                                            */
/* TODO: Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {
    FILE *prog;
    int i = 0;
    char buffer[32 + 2]; // 32 + newline + null

    //to notifying data & text segment size
    int text_size = 0;
    int data_size = 0;

    /* Open program file. */
    prog = fopen(program_filename, "r");

    if (prog == NULL) {
        printf("Error: Can't open program file %s\n", program_filename);
        exit(EXIT_FAILURE);
    }


    /* Read in the program. */

    // 1st: read the size of text section
    if (fgets(buffer, 32 + 1, prog) != NULL) {
        //check text segment size
        text_size = str_to_int(buffer);
        g_processor.input_insts = text_size/4;

    } else {
        printf("Error occured while loading %s\n", program_filename);
        exit(EXIT_FAILURE);
    }


    // 2nd: read the size of data section
    // TODO
    if (fgets(buffer, 32 + 1, prog) != NULL) {
        //check text segment size
        data_size = str_to_int(buffer);
        // data_size =  data_size;
    }


    // 3rd: load the text and data binary to memory
    // TODO
    for (i = 0; i < text_size; i+=4) {
        if (fgets(buffer, 32 + 1, prog) != NULL) {
            load_inst_to_mem(buffer, i);
        } else {
            printf("Error occured while loading text segment\n");
            exit(EXIT_FAILURE);
        }
    }

    for (i = 0; i < data_size; i+=4) {
        if (fgets(buffer, 32 + 1, prog) != NULL) {
            load_data_to_mem(buffer, i);
        } else {
            printf("Error occured while loading data segment\n");
            exit(EXIT_FAILURE);
        }
    }

    fclose(prog);

    g_processor.pc = MEM_TEXT_START;
    g_processor.running = TRUE;
}


/// util.c ///
/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   util.c                                                    */
/*                                                             */
/***************************************************************/

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*          DO NOT MODIFY THIS FILE!                            */
/*          You should only the proc.c file!                    */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

// #include "proc.h"
#include "util.h"

/***************************************************************/
/*                                                             */
/* Procedure: str_split                                        */
/*                                                             */
/* Purpose: To parse main function argument                    */
/*                                                             */
/***************************************************************/
char** str_split(char *a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp) {
        if (a_delim == *tmp) {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
     *        knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result) {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token) {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }

    return result;
}

/***************************************************************/
/* Procedure: convert binary string to integer                 */
/***************************************************************/
int str_to_int(const char *s)
{
    return (int) strtol(s, NULL, 2);
}


/***************************************************************/
/* Procedure: convert decimal to binary string                 */
/***************************************************************/

char* dec_to_bin(int n)
{
    int c, d, t;
    char *p;

    t = 0;
    p = (char*)malloc(32+1);

    if (p == NULL)
        exit(EXIT_FAILURE);

    for (c = 31 ; c >= 0 ; c--) {
        d = n >> c;

        if (d & 1)
            *(p+t) = 1 + '0';
        else
            *(p+t) = 0 + '0';

        t++;
    }
    *(p+t) = '\0';

    return  p;
}

