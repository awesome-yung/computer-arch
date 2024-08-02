/// loader.c /// 
#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "util.h"

struct Pipe_pin pipe_pin = {0, 0, 0, 0, 0, 1, 0, 0, 0, 0};

void pipe_update(){
    // printf("update\n");
    pipe_pin.WE = pipe_pin.MEM;
    pipe_pin.MEM = pipe_pin.EX;
    pipe_pin.EX = pipe_pin.ID;
    pipe_pin.ID = pipe_pin.IF;


    pipe_pin.cycle_num++;
    g_processor.num_insts++;
    g_processor.ticks++;
    g_processor.pc += BYTES_PER_WORD;
}

void flush(){
    pipe_pin.WE = pipe_pin.MEM;
    pipe_pin.MEM = pipe_pin.EX;
    pipe_pin.IF = 0;
    pipe_pin.ID = 0;
    pipe_pin.EX = 0;

    pipe_pin.inst_num++;
    pipe_pin.cycle_num++;

    print_state();
    pipe_update();
}

void jump(){
    pipe_pin.IF = 0;
    pipe_update();
}

void stall(){
    pipe_pin.WE = pipe_pin.MEM;
    pipe_pin.MEM = pipe_pin.EX;
    pipe_pin.EX = 0;
    pipe_pin.cycle_num++;
}

void print_state(){
    printf("Current pipeline PC state :\n");
    printf("-------------------------------------\n");
    printf("CYCLE %d:", pipe_pin.cycle_num);

    if (pipe_pin.IF != 0)
        printf("0x%08x", pipe_pin.IF);
    else
        printf("          ");

    printf("|");

    if (pipe_pin.ID != 0)
        printf("0x%08x", pipe_pin.ID);
    else
        printf("          ");

    printf("|");

    if (pipe_pin.EX != 0)
        printf("0x%08x", pipe_pin.EX);
    else
        printf("          ");

    printf("|");

    if (pipe_pin.MEM != 0)
        printf("0x%08x", pipe_pin.MEM);
    else
        printf("          ");

    printf("|");

    if (pipe_pin.WE != 0)
        printf("0x%08x", pipe_pin.WE);
    else
        printf("          ");

    printf("\n\n");
}

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

struct mem_region_t g_mem_regions[] = {
    { MEM_TEXT_START, MEM_TEXT_SIZE, NULL },
    { MEM_DATA_START, MEM_DATA_SIZE, NULL },
};

#define MEM_NREGIONS (sizeof(g_mem_regions)/sizeof(struct mem_region_t))

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Allocate and zero memory                        */
/*                                                             */
/***************************************************************/
void init_memory() {
    int i;
    int mem_nr_regions = (sizeof(g_mem_regions)/sizeof(struct mem_region_t));

    for (i = 0; i < mem_nr_regions; i++) {
        g_mem_regions[i].mem = malloc(g_mem_regions[i].size);
        memset(g_mem_regions[i].mem, 0, g_mem_regions[i].size);
    }
    // printf(" mem_nr_regions = %d\n",mem_nr_regions);
}
/***************************************************************/
/*                                                             */
/* Procedure: mem_read_32                                      */
/*                                                             */
/* Purpose: Read a 32-bit word from memory                     */
/*                                                             */
/***************************************************************/
uint32_t mem_read_32(uint32_t address)
{
    int i;
    int mem_nr_regions = (sizeof(g_mem_regions)/sizeof(struct mem_region_t));

    for (i = 0; i < mem_nr_regions; i++) {
        if (address >= g_mem_regions[i].start &&
            address < (g_mem_regions[i].start + g_mem_regions[i].size)) {
            uint32_t offset = address - g_mem_regions[i].start;

            return
                (g_mem_regions[i].mem[offset+3] << 24) |
                (g_mem_regions[i].mem[offset+2] << 16) |
                (g_mem_regions[i].mem[offset+1] <<  8) |
                (g_mem_regions[i].mem[offset+0] <<  0);
        }
    }

    return 0;
}

/***************************************************************/
/*                                                             */
/* Procedure: mem_write_32                                     */
/*                                                             */
/* Purpose: Write a 32-bit word to memory                      */
/*                                                             */
/***************************************************************/
void mem_write_32(uint32_t address, uint32_t value)
{
    int i;
    int mem_nr_regions = (sizeof(g_mem_regions)/sizeof(struct mem_region_t));

    for (i = 0; i < mem_nr_regions; i++) {
        if (address >= g_mem_regions[i].start &&
            address < (g_mem_regions[i].start + g_mem_regions[i].size)) {
            uint32_t offset = address - g_mem_regions[i].start;

            g_mem_regions[i].mem[offset+3] = (value >> 24) & 0xFF;
            g_mem_regions[i].mem[offset+2] = (value >> 16) & 0xFF;
            g_mem_regions[i].mem[offset+1] = (value >>  8) & 0xFF;
            g_mem_regions[i].mem[offset+0] = (value >>  0) & 0xFF;
            return;
        }
    }
}