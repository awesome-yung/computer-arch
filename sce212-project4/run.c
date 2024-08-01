#include <stdio.h>
#include <malloc.h>
#include "parse.h"
#include "util.h"
#include "run.h"

struct Pipe_pin pipe_pin;


void run(int num_cycles) {
    int i;
    // pipe_pin.cycle_num = 1;

    if (g_processor.running == FALSE) {
        printf("[ERROR] Can't simulate, Simulator is halted\n\n");
        return;
    }

    // printf("[INFO] Simulating for %d cycles...\n\n", num_cycles);
    printf("Simulating for %d instructions...\n\n", 100);
    for (i = 0; i < num_cycles; i++) {
        if (g_processor.running == FALSE) {
            // printf("[INFO] Simulator halted\n");
            break;
        }
        cycle();
    }
    rdump();
    if (g_processor.running == FALSE) 
    {
        printf("Simulator halted after %d cycles\n",pipe_pin.cycle_num);
    }
    printf("\n");
}