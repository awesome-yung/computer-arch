#ifndef __REG_H__

#include <stdint.h>

struct pipeline{
    uint8_t FE,DE,RN,DI,IS,RR,EX,WB,CM;
};

struct stage {
    uint8_t start;
    uint8_t duration;
};

struct inst{
    uint8_t src_1;
    uint8_t src_2;
    uint8_t fu;
    int8_t dst;
    struct stage FE;
    struct stage DE;
    struct stage RN;
    struct stage DI;
    struct stage IS;
    struct stage RR;
    struct stage EX;
    struct stage WB;
    struct stage CM;
};

void    print_result();

#endif