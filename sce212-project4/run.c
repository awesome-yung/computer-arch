#include <stdio.h>
#include <malloc.h>
#include "parse.h"
#include "util.h"
#include "run.h"

struct Pipe_pin pipe_pin;

/***************************************************************/
/* Advance a cycle                                             */
/***************************************************************/
void cycle()
{
    int inst_reg;
    
    struct inst_t inst;    
    
    // early stop ( 모든 cycle이 비었다 & 첫번째 cycle 아니다 ) | inst 100 이하다 
    if((pipe_pin.IF == 0 & pipe_pin.ID==0 & pipe_pin.EX == 0 & pipe_pin.MEM == 0 & pipe_pin.WE==0 & pipe_pin.cycle_num != 1) | pipe_pin.inst_num>=100)
    {
        g_processor.running = FALSE;
        pipe_pin.cycle_num--;
        return;
    }    
    
    // 1. fetch 추가
    if(pipe_pin.is_flush != 1 & g_processor.pc < MEM_TEXT_START + g_processor.input_insts*4)
    {   
        pipe_pin.IF = g_processor.pc;
    }

    else
    {
        pipe_pin.IF = 0;
        g_processor.pc -= 4;
    }

    // 2. cycle 시작
    if(pipe_pin.WE != 0)
    {
        inst_reg = fetch(pipe_pin.WE);    
        inst = decode(inst_reg);
        WE_cycle(inst);
        pipe_pin.inst_num++;
    }
    if(pipe_pin.MEM != 0)
    {
        inst_reg = fetch(pipe_pin.MEM); // bne 조건 확인을 위해 mem에 미리 저장
        inst = decode(inst_reg); 
        MEM_cycle(inst);
    }  
    if(pipe_pin.EX != 0)
    {
        inst_reg = fetch(pipe_pin.EX); // bne, beq에서 flush 판단
        inst = decode(inst_reg); 
        EX_cycle(inst);
    }  
    if(pipe_pin.ID != 0)
    {
        inst_reg = fetch(pipe_pin.ID);  // jal 나오면 stall   
        inst = decode(inst_reg); 
        ID_cycle(inst);
    }
    if(pipe_pin.IF != 0)
    {
        inst_reg = fetch(pipe_pin.IF);  // lw stall을 위해 hazard 점검
        inst = decode(inst_reg); 
        IF_cycle(inst);
    }

    print_state();
  
    // 3. jump, stall, flush 실행
    if(pipe_pin.is_jump)
        {
            jump();
            pipe_pin.is_jump = 0;
            return;
        }    
    else if(pipe_pin.is_stall)
        {
            stall();
            pipe_pin.is_stall = 0;
        }
    else if(pipe_pin.is_flush)
    {
        flush();
        pipe_pin.is_flush = 0;
        return;
    }
    else
        {   
            pipe_update();
        }

}

void run(int num_cycles) {
    int i;

    printf("Simulating for %d instructions...\n\n", 100);
    for (i = 0; i < num_cycles; i++) {
        if (g_processor.running == FALSE) {
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