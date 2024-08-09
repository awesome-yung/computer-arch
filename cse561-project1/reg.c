#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "reg.h"

#define NUM_INSTRUCTIONS 100
int DI_pass = 0;
int bias=0;

struct Inst inst_c[NUM_INSTRUCTIONS];
struct Pipeline pipe_state[NUM_INSTRUCTIONS];
struct Dst_list dst_list;

// 배열에서 숫자가 존재하는지 확인
int find_dst(int scr1, int src2) {
    int result1 = 0; 
    int result2 = 0;
    for (int i = 0; i < dst_list.size; i++) {
        if (dst_list.list[i].dst == scr1) 
        {
            // printf("= fi d=%d, t=%d\n",dst_list.list[i].dst, dst_list.list[i].tim);
            result1 = dst_list.list[i].tim;
            break;
        }
    }
    for (int i = 0; i < dst_list.size; i++) {
        if (dst_list.list[i].dst == src2) 
        {
            // printf("= fi d=%d, t=%d\n",dst_list.list[i].dst, dst_list.list[i].tim);
            result2 = dst_list.list[i].tim;
            break;
        }
    }
    return (result1 > result2) ? result1 : result2;
}

// 배열에 숫자를 추가
void append_dst(int dst, int tim) {

    if( dst != -1)
    {
        // printf("wb = %d %d \n",dst,tim);
        if(dst_list.size != 0) 
        {
            for(int i=0;i<dst_list.size;i++)
            {
                if (dst_list.list[i].dst == dst) 
                {
                    dst_list.list[i].tim = tim; // 최근 tim으로 갱신
                    return;
                }
            }       
            dst_list.list[dst_list.size].dst = dst; // 없으면 dst 추가
            dst_list.list[dst_list.size].tim = tim;
            dst_list.size++;
        }
        else // dst_list에 처음 추가하는 경우
        {
            dst_list.list[0].dst = dst;
            dst_list.list[0].tim = tim;
            dst_list.size++;
            // printf("call 1\n");
        }
    }
    

    // // list 확인
    // // printf("-> ");
    // for(int i=0;i<dst_list.size;i++)
    // {
    //     printf("d=%d,t=%d  ",dst_list.list[i].dst,dst_list.list[i].tim);
    // }
    // printf("\n");
}


//commit

// void CM(int num){
//     int start = inst_c[num].WB.start + inst_c[num].WB.duration;
//     int duration;
//     int bundle_count = num%3;
//     int last_inst_pos = inst_c[num-1].CM.start + inst_c[num-1].CM.duration -1;
    
//     last_inst_pos = (last_inst_pos < 0) ? 0 : last_inst_pos; // max(0, last)

//     // last_inst_pos - start를 하면 duration보다 1 적게 나옴

//     if(last_inst_pos == 0)  // 처음 진입
//     {
//         duration = 1;
//         printf("%d case_(0) %d\n",num,duration);
//         // printf("case_1, du=%d. la=%d, st=%d\n",duration,last_inst_pos,start);
//     }
//     else if(bundle_count != 0 & last_inst_pos != 0) // bundle 안에 있을 때
//     {
//         if(start>last_inst_pos)
//         {
//             duration = 1;    
//             printf("%d case_(1-1) %d %d %d\n",num, duration, start, last_inst_pos);
//         }
//         else
//         {
//             duration = last_inst_pos - start + 1;
//             printf("%d case_(1-2) %d %d %d\n",num,duration,start,last_inst_pos);
//         }
//         // duration = last_inst_pos - start;
//         // printf("case_2, du=%d. la=%d, st=%d\n",duration,last_inst_pos,start);
//     }
//     else  // bundle 벗어날 때
//     {
//         if(start>last_inst_pos)
//         {
//             duration = 1;    
//             printf("%d case_(2-1) %d %d %d\n",num,duration,start,last_inst_pos);
//         }
//         else
//         {
//             duration = last_inst_pos - start + 2;
//             printf("%d case_(2-2) %d %d %d\n",num,duration,start,last_inst_pos);
//         }
//         // printf("case_3, du=%d. la=%d, st=%d\n",duration,last_inst_pos-1,start);
//     }
//     // printf("du = %d\n",duration);
//     inst_c[num].CM.start = start;
//     inst_c[num].CM.duration = duration;

//     pipe_state[start].CM.width++;

//     // printf("CM s = %d, CM = %d\n",start,duration);
// }

// commit
void CM(int num){
    int start = inst_c[num].WB.start + inst_c[num].WB.duration;
    int duration = 1;
    int last_inst_pos = inst_c[num-1].CM.start + inst_c[num-1].CM.duration -1;
    last_inst_pos = (last_inst_pos < 0) ? 0 : last_inst_pos; // max(0, last)
    int st = start;

    for(int i=0;i<30;i++)
    {
        pipe_state[st].CM.width++;
        if(st>=last_inst_pos & pipe_state[st].CM.width<=3)
        {
            break;
        }
        // printf("n=%d, la=%d, st=%d, wi=%d t/f=%d\n",num,last_inst_pos,st,pipe_state[st].CM.width,st>=last_inst_pos & pipe_state[st].CM.width<3);
        st++;
        duration++;
    }
    
    inst_c[num].CM.start=start;
    inst_c[num].CM.duration=duration;
    
    // printf("%d, %d, %d\n",num,start,duration);
}

// writeback
void WB(int num){
    int start = inst_c[num].EX.start + inst_c[num].EX.duration;
    int duration = 1;

    inst_c[num].WB.start = start;
    inst_c[num].WB.duration = duration;
    append_dst(inst_c[num].dst, start);
    // printf("wb-> dst = %d\n",start);
    
    pipe_state[start].WB.width++;
    // printf("WB s = %d, WB = %d\n",start,duration);
}

// execute
void EX(int num){
    int start = inst_c[num].RR.start + inst_c[num].RR.duration;
    inst_c[num].EX.start = start;
    int duration;
    
    int type = inst_c[num].fu;
    switch(type)
    {
        case 0:
            duration = 1;
            break;
        case 1:
            duration = 2;
            break;
        case 2:
            duration = 5;
            break;
    }
    inst_c[num].EX.duration = duration;

    int st = start;
    for(int i=0;i<duration;i++)
    {
        pipe_state[st].EX.width++;
        st++;
    }
    // printf("EX s = %d, EX = %d\n",start,duration);
}

// regRead
void RR(int num){
    int start = inst_c[num].IS.start + inst_c[num].IS.duration;
    int duration = 1;

    inst_c[num].RR.start = start;
    inst_c[num].RR.duration = duration;
    pipe_state[start].RR.width++;
    
    // printf("RR s = %d, RR = %d\n",start,duration);
}

// issue
void IS(int num){
    int start = inst_c[num].DI.start + inst_c[num].DI.duration;
    int src1 = inst_c[num].src_1;
    int src2 = inst_c[num].src_2;
    int dst = inst_c[num].dst;
    int duration;
    int tim = find_dst(src1, src2);
    int next_point;

    if(num==32|num==33)
    {
        // printf("%d t=%d, s=%d\n",num,tim,start);
    }
    inst_c[num].IS.start = start;
    if(tim != 0)  // dependacy가 있을 경우
    {
        duration = tim - start;
        duration = (duration<=0) ? 1 : duration;
        next_point = start + duration;
        for(int i=0;i<10;i++)
        {
            if(pipe_state[next_point].RR.width<3) 
            {
                break;
            }
            duration++;
            next_point++;
        }
        
    }
    else
    {
        duration = 1;
        next_point = start + duration;
        for(int i=0;i<10;i++)
        {
            if(pipe_state[next_point].RR.width<3) 
            {
                break;
            }
            duration++;
            next_point++;
        }
    }
    
    inst_c[num].IS.duration = duration;

    int st = start;
    // printf("n=%d, d=%d t=%d, s=%d\n",num,duration,tim,st);
    for(int i=0; i<duration;i++)
        {
            pipe_state[st].IS.width++;
            pipe_state[st].IS.iq++;
            // printf("IS->(%d, %d) is=%d,iq=%d, RR=%d, dif=%d\n",num, st,pipe_state[st].IS.width,pipe_state[st].IS.iq,pipe_state[st].RR.width,tim - start);
            st++;
        }

    // 마지막 st의 iq는 제외한다.
    st--;
    pipe_state[st].IS.iq--;

    // // 검증
    // st = start;
    // for(int j=0;j<duration;j++)
    // {
    //     printf("IS->(%d, %d) is=%d,iq=%d, RR=%d, dif=%d\n",num, st,pipe_state[st].IS.width, pipe_state[st].IS.iq, pipe_state[st].RR.width, tim - start);
    //     st++;
    // }

}
// dispatch
void DI(int num){
    int start = inst_c[num].RN.start + inst_c[num].RN.duration;
    int duration = 1;
    int st = start;
    int rest_inst = 100 - (num);

    for(int i=0;i<10;i++)
    {
        // if(pipe_state[st].DI.width<=3 & (pipe_state[st].IS.iq <= 12 | pipe_state[st].IS.iq + rest_inst<=15))
        if((pipe_state[st].DI.width<=3) & pipe_state[st].IS.iq <= 12)
        {
            pipe_state[st].DI.width++;
            break;
        }
        else if(pipe_state[st].IS.iq + rest_inst <= 15)
        {
            pipe_state[st].DI.width++;
            break;
        }
        st++;
        duration++;
    }

    inst_c[num].DI.start=start;
    inst_c[num].DI.duration=duration;

}


// rename
void RN(int num){

    int start = inst_c[num].DE.start + inst_c[num].DE.duration;
    int duration = 1;
    int st=start;
    int rest_inst = 100 - (num);

    for(int i=0;i<10;i++)
    {
        // if(pipe_state[st].DI.width<=3 & (pipe_state[st].IS.iq <= 12 | pipe_state[st].IS.iq + rest_inst<=15))
        if(pipe_state[st].DI.width<=3 & pipe_state[st].IS.iq <= 12)
        {
            pipe_state[st].RN.width++;
            break;
        }
        st++;
        duration++;
    }


    inst_c[num].RN.start = start;
    inst_c[num].RN.duration = duration;
    // printf("RN s = %d, RN = %d\n",start,duration);
}

//decode
void DE(int num){

    int start = inst_c[num].FE.start + inst_c[num].FE.duration;
    int duration = 1;
    int rest_inst = 100 - (num);
    
    int st=start;
    for(int i=0;i<10;i++)
    {
        // if(pipe_state[st].DI.width<=3 & (pipe_state[st].IS.iq <= 12 | pipe_state[st].IS.iq + rest_inst<=15))
        if(pipe_state[st].DI.width<=3 & pipe_state[st].IS.iq <= 12)
        {
            pipe_state[st].DE.width++;
            break;
        }
        st++;
        duration++;
    }
    inst_c[num].DE.start = start;
    inst_c[num].DE.duration = duration;

    // printf("DE s = %d, DE = %d\n",start,duration);

}

//Fetch
void FE(int num){

    int start = num/3 + bias;
    int duration = 1;
    int rest_inst = 100 - (num);
    int st = start;

    for(int i=0;i<10;i++)
    {
        // if(pipe_state[start].DI.width<=3 & (pipe_state[start].IS.iq <= 12 | pipe_state[start].IS.iq + rest_inst<=15))
        if(pipe_state[st].DI.width<=3 & pipe_state[st].IS.iq <= 12)
        {
            pipe_state[st].FE.width++;
            break;
        }
        st++;
        bias++;
    }
    inst_c[num].FE.start = st;
    inst_c[num].FE.duration = duration;

    pipe_state[start].FE.width++;
    // printf("FE s = %d, FE = %d\n",start,duration);

}



void run(char* file_name){
    FILE *prog;
    char buffer[30];
    prog = fopen(file_name, "r");
    for(int num=0; num<NUM_INSTRUCTIONS; num++)
    // for(int num=0; num<30; num++)
    {
        if (fgets(buffer, sizeof(buffer), prog) != NULL)
        {
            // printf("%s\n",buffer);
            strtok(buffer, " ");
            inst_c[num].fu = atoi(strtok(NULL, " "));
            inst_c[num].dst = atoi(strtok(NULL, " "));
            inst_c[num].src_1 = atoi(strtok(NULL, " "));
            inst_c[num].src_2 = atoi(strtok(NULL, " "));
        }
        FE(num);
        DE(num);
        RN(num);
        DI(num);
        IS(num);
        RR(num);
        EX(num);
        WB(num);
        CM(num);

    }
    
}



void print_result(char* file_name){
    
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
    int cycle = inst_c[99].CM.start+inst_c[99].CM.duration-1;
    float ipc = (float)NUM_INSTRUCTIONS / cycle;
    printf("# === Simulator Command =========\n");
    printf("# ./cse561sim 60 15 3 %s \n",file_name);
    printf("# === Processor Configuration ===\n");
    printf("# ROB_SIZE = %d\n",60);
    printf("# IQ_SIZE  = %d\n",15);
    printf("# WIDTH    = %d\n",3);
    printf("# === Simulation Results ========\n");
    printf("# Dynamic Instruction Count = %d\n",NUM_INSTRUCTIONS);
    printf("# Cycles                    = %d\n",cycle);
    printf("# Instructions Per Cycle    = %0.2f\n",ipc);

}