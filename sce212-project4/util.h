/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   util.h                                                    */
/*                                                             */
/***************************************************************/

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*          DO NOT MODIFY THIS FILE!                            */
/*          You should only proc.c file!                        */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#ifndef _UTIL_H_
#define _UTIL_H_

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define FALSE 0
#define TRUE  1

/* Functions */
char**		str_split(char *a_str, const char a_delim);
int		    str_to_int(const char *s);
char*       dec_to_bin(int n);

// /loader.h ///
#include "parse.h"

extern struct MIPS32_proc_t g_processor;


void        load_inst_to_mem(const char *buffer, const int index);
void        load_data_to_mem(const char *buffer, const int index);
void        load_program(char *program_filename);
void        print_state();
void        pipe_update();
void        flush();
void        jump();
void        stall();
void        init_memory();
uint32_t    mem_read_32(uint32_t address);
void        mem_write_32(uint32_t address, uint32_t value);


#endif



// #ifndef __LOADER_H__
// #define __LOADER_H__

// #include "proc.h"

// extern struct MIPS32_proc_t g_processor;


// void        load_inst_to_mem(const char *buffer, const int index);
// void        load_data_to_mem(const char *buffer, const int index);
// void        load_program(char *program_filename);

// #endif