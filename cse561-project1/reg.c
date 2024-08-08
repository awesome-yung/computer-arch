#include <stdio.h>
#include <stdlib.h>
#include "reg.h"

#define NUM_INSTRUCTIONS 100

struct inst inst_c[NUM_INSTRUCTIONS];
struct pipeline pipe_state;

void print_result(){
    for(int i=0; i<NUM_INSTRUCTIONS; i++)
    {
        printf("%d fu{%d} src{%d,%d} dst{%d} FE{%d,%d} DE{%d,%d} RN{%d,%d} DI{%d,%d} IS{%d,%d} RR{%d,%d} EX{%d,%d} WB{%d,%d} CM{%d,%d}",
        i,inst_c[i].fu, inst_c[i].src_1, inst_c[i].src_2, inst_c[i].dst,
        inst_c[i].FE.start, inst_c[i].FE.duration,
        inst_c[i].DE.start, inst_c[i].DE.duration,
        inst_c[i].RN.start, inst_c[i].RN.duration,
        inst_c[i].DI.start, inst_c[i].DI.duration,
        inst_c[i].IS.start, inst_c[i].IS.duration,
        inst_c[i].RR.start, inst_c[i].RR.duration,
        inst_c[i].EX.start, inst_c[i].EX.duration,
        inst_c[i].WB.start, inst_c[i].WB.duration,
        inst_c[i].CM.start, inst_c[i].CM.duration);
        printf("\n");
    }
}