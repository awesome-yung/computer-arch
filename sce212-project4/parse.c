
/// proc/c /// 
#include <stdio.h>
#include <malloc.h>

#include "parse.h"
#include "util.h"

/***************************************************************/
/* System (CPU and Memory) info.                               */
/***************************************************************/
struct MIPS32_proc_t g_processor;
extern struct Pipe_pin pipe_pin;

int loader_queue[2]; // inst_queue


/***************************************************************/
/* Fetch an instruction indicated by PC                        */
/***************************************************************/
int fetch(uint32_t pc)
{  
    return mem_read_32(pc);
}


/***************************************************************/
/* TODO: Decode the given encoded 32bit data (word)            */
/***************************************************************/
struct inst_t decode(int word)
{
    struct inst_t inst;
    char bin_data[32+2];
    int detec_op = (word >> 26) & 0x3F; //op : 000000=R, 000010,000011=J, 나머지=I

    if(detec_op==0) // type = 'R'
    {
        inst.opcode = (word >> 26) & 0x3F;
        inst.r_t.r_i.rs = (word >> 21) & 0x1F;
        inst.r_t.r_i.rt = (word >> 16) & 0x1F; 
        inst.r_t.r_i.r_i.r.rd = (word >> 11) & 0x1F; 
        inst.r_t.r_i.r_i.r.shamt = (word >> 6) & 0x1F;
        inst.func_code = (word >> 0) & 0x3F;
    }
    else if(detec_op==2 || detec_op==3) // type = 'J'
    {
        inst.opcode = (word >> 26) & 0x3F;
        inst.r_t.target = (word >> 0) & 0x3ffffff;
    }
    else // type = 'I'
    {
        inst.opcode = (word >> 26) & 0x3F;
        inst.r_t.r_i.rs = (word >> 21) & 0x1F;
        inst.r_t.r_i.rt = (word >> 16) & 0x1F;
        inst.r_t.r_i.r_i.imm = (word >> 0) & 0xffff;
    }    
    return inst;
}

/***************************************************************/
/* TODO: Execute the decoded instruction                       */
/***************************************************************/
void IF_cycle(struct inst_t inst)
{
    int inst_opcode = inst.opcode;
    int rs, rt, rd, shamt, funct, imm, addr;

    /* R type */ 
    if(inst_opcode == 0x0)
    {
        rs = inst.r_t.r_i.rs;
        rt = inst.r_t.r_i.rt;
        rd = inst.r_t.r_i.r_i.r.rd;
        shamt = inst.r_t.r_i.r_i.r.shamt;
        funct = inst.func_code;
        loader_queue[0] = rs;  // queue 저장
        loader_queue[1] = rt;

        switch(funct)
        {
  
        }
    }

    /* J type */
    else if(inst_opcode == 0x2 ||inst_opcode == 0x3)
    {
        addr = inst.r_t.target;
        switch(inst_opcode)
        {   
 
        }
    }

    /* I type */
    else
    {
        rs = inst.r_t.r_i.rs;
        rt = inst.r_t.r_i.rt;
        imm = inst.r_t.r_i.r_i.imm;
        loader_queue[0] = rs;  // queue 저장
        loader_queue[1] = 0;


        switch(inst_opcode)
        {
 
        }
    }
}

void ID_cycle(struct inst_t inst)
{
    int inst_opcode = inst.opcode;
    int rs, rt, rd, shamt, funct, imm, addr;

    /* R type */ 
    if(inst_opcode == 0x0)
    {
        rs = inst.r_t.r_i.rs;
        rt = inst.r_t.r_i.rt;
        rd = inst.r_t.r_i.r_i.r.rd;
        shamt = inst.r_t.r_i.r_i.r.shamt;
        funct = inst.func_code;

        switch(funct)
        {

            case 0b001000: // jr
                g_processor.pc = g_processor.regs[rs] -4;
                pipe_pin.is_jump = 1;
                break;
   
        }
    }

    /* J type */
    else if(inst_opcode == 0x2 ||inst_opcode == 0x3)
    {
        addr = inst.r_t.target;
        switch(inst_opcode)
        {   
            case 0b000010: // j
                if(pipe_pin.is_flush != 1) 
                {
                    g_processor.pc = (addr << 2) -4;
                    pipe_pin.is_jump = 1;
                }
                break;

            case 0b000011: // jal
                if(pipe_pin.is_flush != 1)
                {
                    g_processor.regs[31] = g_processor.pc + 4 ; 
                    g_processor.pc = (addr << 2) -4;
                    pipe_pin.is_jump = 1;
                }
                
                break;
        }
    }

    /* I type */
    else
    {
        rs = inst.r_t.r_i.rs;
        rt = inst.r_t.r_i.rt;
        imm = inst.r_t.r_i.r_i.imm;

        switch(inst_opcode)
        {
  
        }
    }
}


void EX_cycle(struct inst_t inst)
{
    int inst_opcode = inst.opcode;
    int rs, rt, rd, shamt, funct, imm, addr;

    /* R type */ 
    if(inst_opcode == 0x0)
    {
        rs = inst.r_t.r_i.rs;
        rt = inst.r_t.r_i.rt;
        rd = inst.r_t.r_i.r_i.r.rd;
        shamt = inst.r_t.r_i.r_i.r.shamt;
        funct = inst.func_code;

        switch(funct)
        {
   
        }
    }

    /* J type */
    else if(inst_opcode == 0x2 ||inst_opcode == 0x3)
    {
        addr = inst.r_t.target;
        switch(inst_opcode)
        {   
   
        }
    }

    /* I type */
    else
    {
        rs = inst.r_t.r_i.rs;
        rt = inst.r_t.r_i.rt;
        imm = inst.r_t.r_i.r_i.imm;

        switch(inst_opcode)
        {
            case 0b000100: // beq
                if(g_processor.regs[rt] == g_processor.regs[rs])
                    {
                        g_processor.pc = g_processor.pc + imm*4 -8;
                        pipe_pin.is_flush = 1;
                    }
                break;

            case 0b000101: // bne
                if(g_processor.regs[rt] != g_processor.regs[rs])
                    {   
                        g_processor.pc = g_processor.pc + imm*4 -8;
                        pipe_pin.is_flush = 1;
                    }
                break;

            case 0b100011: // lw
                if(rt == loader_queue[0] | rt == loader_queue[1]) // EX에서 Hazard 판단만 함
                    {
                        pipe_pin.is_stall = 1;
                    }
                break;
        }
    }
}

void MEM_cycle(struct inst_t inst)
{
    int inst_opcode = inst.opcode;
    int rs, rt, rd, shamt, funct, imm, addr;

    /* R type */ 
    if(inst_opcode == 0x0)
    {
        rs = inst.r_t.r_i.rs;
        rt = inst.r_t.r_i.rt;
        rd = inst.r_t.r_i.r_i.r.rd;
        shamt = inst.r_t.r_i.r_i.r.shamt;
        funct = inst.func_code;

        switch(funct)
        {
            case 0b101011: // sltu
                g_processor.regs[rd] = (g_processor.regs[rs] < g_processor.regs[rt]) ? 1 : 0;
                break;
        }
    }

    /* J type */
    else if(inst_opcode == 0x2 ||inst_opcode == 0x3)
    {
        addr = inst.r_t.target;
        switch(inst_opcode)
        {   
         
        }
    }

    /* I type */
    else
    {
        rs = inst.r_t.r_i.rs;
        rt = inst.r_t.r_i.rt;
        imm = inst.r_t.r_i.r_i.imm;

        switch(inst_opcode)
        {
          
            case 0b001111: // lui
                g_processor.regs[rt] = imm << 16;
                break;

            case 0b100011: // lw
                g_processor.regs[rt] = mem_read_32(g_processor.regs[rs] + imm);
                break;
                
            case 0b001101: // ori
                g_processor.regs[rt] = g_processor.regs[rs] | imm;
                break;

            case 0b001011: // sltiu
                g_processor.regs[rt] = (g_processor.regs[rs] < imm) ? 1 : 0;
                break;

        }
    }
}

void WE_cycle(struct inst_t inst)
{
    int inst_opcode = inst.opcode;
    int rs, rt, rd, shamt, funct, imm, addr;

    /* R type */ 
    if(inst_opcode == 0x0)
    {
        rs = inst.r_t.r_i.rs;
        rt = inst.r_t.r_i.rt;
        rd = inst.r_t.r_i.r_i.r.rd;
        shamt = inst.r_t.r_i.r_i.r.shamt;
        funct = inst.func_code;

        switch(funct)
        {
            case 0b100000: // add
                g_processor.regs[rd] = g_processor.regs[rs] + g_processor.regs[rt];
                break;

            case 0b100010: // sub
                g_processor.regs[rd] = g_processor.regs[rs] - g_processor.regs[rt];
                break;

            case 0b100001: // addu
                g_processor.regs[rd] = g_processor.regs[rs] + g_processor.regs[rt];
                break;

            case 0b100100: // and
                g_processor.regs[rd] = g_processor.regs[rs] & g_processor.regs[rt];
                break;
            
            case 0b100111: // nor
                g_processor.regs[rd] = ~(g_processor.regs[rs] | g_processor.regs[rt]);
                break;

            case 0b100101: // or
                g_processor.regs[rd] = g_processor.regs[rs] | g_processor.regs[rt];
                break;
            
            case 0b000000: // sll
                g_processor.regs[rd] = g_processor.regs[rt] << shamt;
                break;
            
            case 0b000010: // srl
                g_processor.regs[rd] = g_processor.regs[rt] >> shamt;
                break;

            case 0b100011: // subu
                g_processor.regs[rd] = g_processor.regs[rs] - g_processor.regs[rt];
                break;
        }
    }

    /* J type */
    else if(inst_opcode == 0x2 ||inst_opcode == 0x3)
    {
        addr = inst.r_t.target;
        switch(inst_opcode)
        {   
 
        }
    }

    /* I type */
    else
    {
        rs = inst.r_t.r_i.rs;
        rt = inst.r_t.r_i.rt;
        imm = inst.r_t.r_i.r_i.imm;

        switch(inst_opcode)
        {
            case 0b001001: // addiu
                g_processor.regs[rt] = g_processor.regs[rs] + imm;
                break;

            case 0b001100: // andi
                g_processor.regs[rt] = g_processor.regs[rs] & imm;
                break;

            case 0b101011: // sw
                mem_write_32(g_processor.regs[rt], g_processor.regs[rs]+imm);
                // [rs] + imm 주소의 값을 rt 레지스터가 가리키는 메모리에 저장
                break;
        }
    }
}


/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump() {
    int k;

    printf("Current register values :\n");
    printf("-------------------------------------\n");
    printf("PC: 0x%08x\n", g_processor.pc);
    printf("Registers:\n");
    for (k = 0; k < MIPS_REGS; k++)
        printf("R%d: 0x%08x\n", k, g_processor.regs[k]);
    printf("\n");
}




