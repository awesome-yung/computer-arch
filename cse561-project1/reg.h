#ifndef __REG_H__

#include <stdint.h>

struct Dst_pos{
    int dst;
    int tim;
};

struct Dst_list{
    int size;
    struct Dst_pos list[100];
};

struct pipe_width{
    uint8_t width;
};

struct Pipeline{
    struct pipe_width FE;
    struct pipe_width DE;
    struct pipe_width RN;
    struct pipe_width DI;
    struct pipe_width IS;
    struct pipe_width RR;
    struct pipe_width EX;
    struct pipe_width WB;
    struct pipe_width CM;
};

struct stage {
    uint8_t start;
    uint8_t duration;
};

struct Inst{
    int8_t src_1;
    int8_t src_2;
    int8_t fu;
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
int     find_dst(int scr1, int src2);
void    append_dst(int dst, int tim);

void CM(int num);
void WB(int num);
void EX(int num);
void RR(int num);
void IS(int num);
void DI(int num);
void RN(int num);
void DE(int num);
void FE(int num);
void run(char* file_name);



// void fetch(int num);
// void run(char* file_name);

// void decode(int num);
// void rename(int num);
// void dispatch(int num);


#endif