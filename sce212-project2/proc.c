/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   proc.c                                                    */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <malloc.h>

#include "proc.h"
#include "mem.h"
#include "util.h"

/***************************************************************/
/* System (CPU and Memory) info.                               */
/***************************************************************/
struct MIPS32_proc_t g_processor;

/***************************************************************/
/* Command list                                                */
/***************************************************************/

// inst_t inst_list[INST_LIST_LEN] = {       //  idx
//     {"add",     "000000", 'R', "100000"}, //    0
//     {"sub",     "000000", 'R', "100010"}, //    1
//     {"addiu",   "001001", 'I', ""},       //    2
//     {"addu",    "000000", 'R', "100001"}, //    3
//     {"and",     "000000", 'R', "100100"}, //    4
//     {"andi",    "001100", 'I', ""},       //    5
//     {"beq",     "000100", 'I', ""},       //    6
//     {"bne",     "000101", 'I', ""},       //    7
//     {"j",       "000010", 'J', ""},       //    8
//     {"jal",     "000011", 'J', ""},       //    9
//     {"jr",      "000000", 'R', "001000"}, //   10
//     {"lui",     "001111", 'I', ""},       //   11
//     {"lw",      "100011", 'I', ""},       //   12
//     {"nor",     "000000", 'R', "100111"}, //   13
//     {"or",      "000000", 'R', "100101"}, //   14
//     {"ori",     "001101", 'I', ""},       //   15
//     {"sltiu",   "001011", 'I', ""},       //   16
//     {"sltu",    "000000", 'R', "101011"}, //   17
//     {"sll",     "000000", 'R', "000000"}, //   18
//     {"srl",     "000000", 'R', "000010"}, //   19
//     {"sw",      "101011", 'I', ""},       //   20
//     {"subu",    "000000", 'R', "100011"}  //   21
// };


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
void execute(struct inst_t inst)
{
    int inst_opcode = inst.opcode;
    int rs, rt, rd, shamt, funct, imm, addr;
    if(g_processor.pc >= MEM_TEXT_START + g_processor.input_insts*4)
    {
        g_processor.running = FALSE; // PC가 test size롤 초과하면 cycle 종료
    }

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

            case 0b001000: // jr
                g_processor.pc = g_processor.regs[rs];
                break;
            
            case 0b100111: // nor
                g_processor.regs[rd] = ~(g_processor.regs[rs] | g_processor.regs[rt]);
                break;

            case 0b100101: // or
                g_processor.regs[rd] = g_processor.regs[rs] | g_processor.regs[rt];
                break;

            case 0b101011: // sltu
                g_processor.regs[rd] = (g_processor.regs[rs] < g_processor.regs[rt]) ? 1 : 0;
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
            case 0b000010: // j
                g_processor.pc = addr << 2;
                break;

            case 0b000011: // jal
                g_processor.regs[31] = g_processor.pc + 4 ; // 다음 명령어 주소
                g_processor.pc = addr << 2;
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
            case 0b001001: // addiu
                g_processor.regs[rt] = g_processor.regs[rs] + imm;
                break;

            case 0b001100: // andi
                g_processor.regs[rt] = g_processor.regs[rs] & imm;
                break;

            case 0b000100: // beq
                if(g_processor.regs[rt] == g_processor.regs[rs])
                    {
                    g_processor.pc = g_processor.pc + imm*4;
                    }
                break;

            case 0b000101: // bne
                if(g_processor.regs[rt] != g_processor.regs[rs])
                    g_processor.pc = g_processor.pc + imm*4;
                break;

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

            case 0b101011: // sw
                mem_write_32(g_processor.regs[rt], g_processor.regs[rs]+imm);
                // [rs] + imm 주소의 값을 rt 레지스터가 가리키는 메모리에 저장
                break;
        }
    }
}

/***************************************************************/
/* Advance a cycle                                             */
/***************************************************************/
void cycle()
{
    int inst_reg;
    struct inst_t inst;
 
    // 1. fetch
    inst_reg = fetch(g_processor.pc);
    g_processor.pc += BYTES_PER_WORD;

    // 2. decode
    inst = decode(inst_reg);

    // 3. execute
    execute(inst);

    // 4. update stats
    g_processor.num_insts++;
    g_processor.ticks++;
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

    printf("\n[INFO] Current register values :\n");
    printf("-------------------------------------\n");
    printf("PC: 0x%08x\n", g_processor.pc);
    printf("Registers:\n");
    for (k = 0; k < MIPS_REGS; k++)
        printf("R%d: 0x%08x\n", k, g_processor.regs[k]);
}


/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate MIPS for n cycles                      */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {
    int i;

    if (g_processor.running == FALSE) {
        printf("[ERROR] Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("[INFO] Simulating for %d cycles...\n", num_cycles);
    for (i = 0; i < num_cycles; i++) {
        if (g_processor.running == FALSE) {
            printf("[INFO] Simulator halted\n");
            break;
        }
        cycle();
    }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate MIPS until HALTed                      */
/*                                                             */
/***************************************************************/
void go() {
    if (g_processor.running == FALSE) {
        printf("[ERROR] Can't simulate, Simulator is halted\n\n");
        return;
    }

    printf("[INFO] Simulating...\n");
    while (g_processor.running)
        cycle();
    printf("[INFO] Simulator halted\n");
}
