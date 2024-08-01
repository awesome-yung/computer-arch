
/// proc/c /// 
#include <stdio.h>
#include <malloc.h>

#include "parse.h"
#include "util.h"

/***************************************************************/
/* System (CPU and Memory) info.                               */
/***************************************************************/
struct MIPS32_proc_t g_processor;
// struct Pipe_pin pipe_pin = {0, 0, 0, 0, 0, 1};
struct Pipe_pin pipe_pin;



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

void pipe_update(){
    pipe_pin.WE = pipe_pin.MEM;
    pipe_pin.MEM = pipe_pin.EX;
    pipe_pin.EX = pipe_pin.ID;
    pipe_pin.ID = pipe_pin.IF;

    // printf("IF = %s, ID = %s, EX = %s, MEM = %s. WE = %s\n",
    //                                 dec_to_bin(pipe_pin.IF),
    //                                 dec_to_bin(pipe_pin.ID),
    //                                 dec_to_bin(pipe_pin.EX),
    //                                 dec_to_bin(pipe_pin.MEM),
    //                                 dec_to_bin(pipe_pin.WE));
    // if(pipe_pin.WE != 0){
    //     pipe_pin.inst_num++;
    // }
    pipe_pin.cycle_num++;
    g_processor.num_insts++;
    g_processor.ticks++;
}

void flush(){
    // pipe_update();
    pipe_pin.IF = 0;
    pipe_pin.ID = 0;
    pipe_pin.EX = 0;
    // pipe_pin.cycle_num++;
    g_processor.pc += BYTES_PER_WORD;
    pipe_pin.inst_num++;
    print_state();
}

void jump(){
    // pipe_update();
    pipe_pin.IF = 0;
    // pipe_pin.ID = 0;
    // pipe_pin.EX = 0;
    // pipe_pin.cycle_num++;
    // g_processor.pc += BYTES_PER_WORD;
    // pipe_pin.inst_num++;
    // print_state();
}
void stall(){

}

/***************************************************************/
/* Fetch an instruction indicated by PC                        */
/***************************************************************/
int fetch(uint32_t pc)
{  
    return mem_read_32(pc);
}
// void fetch(uint32_t pc)
// { 
//     pipe_pin.IF = mem_read_32(pc);
//     printf("mem read = %d\n",mem_read_32(pc));
//     printf("IF = %d\n",pipe_pin.IF);
// }

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
void jal_j(struct inst_t inst)
{
    int inst_opcode = inst.opcode;
    int rs, rt, rd, shamt, funct, imm, addr;
    // if(g_processor.pc >= MEM_TEXT_START + g_processor.input_insts*4)

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
            // case 0b100000: // add
            //     g_processor.regs[rd] = g_processor.regs[rs] + g_processor.regs[rt];
            //     break;

            // case 0b100010: // sub
            //     g_processor.regs[rd] = g_processor.regs[rs] - g_processor.regs[rt];
            //     break;

            // case 0b100001: // addu
            //     g_processor.regs[rd] = g_processor.regs[rs] + g_processor.regs[rt];
            //     // pipe_update();
            //     break;

            // case 0b100100: // and
            //     g_processor.regs[rd] = g_processor.regs[rs] & g_processor.regs[rt];
            //     break;

            case 0b001000: // jr
                g_processor.pc = g_processor.regs[rs];
                pipe_pin.is_jump = 1;
                break;
            
            // case 0b100111: // nor
            //     g_processor.regs[rd] = ~(g_processor.regs[rs] | g_processor.regs[rt]);
            //     break;

            // case 0b100101: // or
            //     g_processor.regs[rd] = g_processor.regs[rs] | g_processor.regs[rt];
            //     break;

            // case 0b101011: // sltu
            //     g_processor.regs[rd] = (g_processor.regs[rs] < g_processor.regs[rt]) ? 1 : 0;
            //     break;
            
            // case 0b000000: // sll
            //     g_processor.regs[rd] = g_processor.regs[rt] << shamt;
            //     break;
            
            // case 0b000010: // srl
            //     g_processor.regs[rd] = g_processor.regs[rt] >> shamt;
            //     break;

            // case 0b100011: // subu
            //     g_processor.regs[rd] = g_processor.regs[rs] - g_processor.regs[rt];
            //     break;
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
                pipe_pin.is_jump = 1;
                break;

            case 0b000011: // jal
                // g_processor.regs[31] = g_processor.pc + 4 ; // 다음 명령어 주소
                g_processor.regs[31] = g_processor.pc; // 다음 명령어 주소
                // printf("jal[ ] = %08x\n",g_processor.regs[31]);
                g_processor.pc = addr << 2;
                pipe_pin.is_jump = 1;
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
            // case 0b001001: // addiu
            //     g_processor.regs[rt] = g_processor.regs[rs] + imm;
            //     // pipe_update();
            //     break;

            // case 0b001100: // andi
            //     g_processor.regs[rt] = g_processor.regs[rs] & imm;
            //     break;

            // // case 0b000100: // beq
            // //     if(g_processor.regs[rt] == g_processor.regs[rs])
            // //         {
            // //         g_processor.pc = g_processor.pc + imm*4;
            // //         pipe_pin.is_flush = 1;
            // //         }
            // //     break;

            // // case 0b000101: // bne
            // //     if(g_processor.regs[rt] != g_processor.regs[rs])
            // //     {
            // //         g_processor.pc = pipe_pin.EX + imm*4;
            // //         pipe_pin.is_flush = 1;
            // //     }
            // //     break;

            // case 0b001111: // lui
            //     g_processor.regs[rt] = imm << 16;
            //     break;

            case 0b100011: // lw
                g_processor.regs[rt] = mem_read_32(g_processor.regs[rs] + imm);
                pipe_pin.is_stall = 1;
                break;
                
            // case 0b001101: // ori
            //     g_processor.regs[rt] = g_processor.regs[rs] | imm;
            //     break;

            // case 0b001011: // sltiu
            //     g_processor.regs[rt] = (g_processor.regs[rs] < imm) ? 1 : 0;
            //     // pipe_update();
            //     break;

            // case 0b101011: // sw
            //     mem_write_32(g_processor.regs[rt], g_processor.regs[rs]+imm);
            //     // [rs] + imm 주소의 값을 rt 레지스터가 가리키는 메모리에 저장
            //     break;
        }
    }
}


void execute(struct inst_t inst)
{
    int inst_opcode = inst.opcode;
    int rs, rt, rd, shamt, funct, imm, addr;
    // if(g_processor.pc >= MEM_TEXT_START + g_processor.input_insts*4)

    /* R type */ 
    // printf("inst_op = %02x\n",inst_opcode);
    if(inst_opcode == 0x0)
    {
        rs = inst.r_t.r_i.rs;
        rt = inst.r_t.r_i.rt;
        rd = inst.r_t.r_i.r_i.r.rd;
        shamt = inst.r_t.r_i.r_i.r.shamt;
        funct = inst.func_code;

        switch(funct)
        {
            // case 0b100000: // add
            //     g_processor.regs[rd] = g_processor.regs[rs] + g_processor.regs[rt];
            //     break;

            // case 0b100010: // sub
            //     g_processor.regs[rd] = g_processor.regs[rs] - g_processor.regs[rt];
            //     break;

            // case 0b100001: // addu
            //     g_processor.regs[rd] = g_processor.regs[rs] + g_processor.regs[rt];
            //     // pipe_update();
            //     break;

            // case 0b100100: // and
            //     g_processor.regs[rd] = g_processor.regs[rs] & g_processor.regs[rt];
            //     break;

            // case 0b001000: // jr
            //     g_processor.pc = g_processor.regs[rs];
            //     break;
            
            // case 0b100111: // nor
            //     g_processor.regs[rd] = ~(g_processor.regs[rs] | g_processor.regs[rt]);
            //     break;

            // case 0b100101: // or
            //     g_processor.regs[rd] = g_processor.regs[rs] | g_processor.regs[rt];
            //     break;

            // case 0b101011: // sltu
            //     g_processor.regs[rd] = (g_processor.regs[rs] < g_processor.regs[rt]) ? 1 : 0;
            //     break;
            
            // case 0b000000: // sll
            //     g_processor.regs[rd] = g_processor.regs[rt] << shamt;
            //     break;
            
            // case 0b000010: // srl
            //     g_processor.regs[rd] = g_processor.regs[rt] >> shamt;
            //     break;

            // case 0b100011: // subu
            //     g_processor.regs[rd] = g_processor.regs[rs] - g_processor.regs[rt];
            //     break;
        }
    }

    /* J type */
    else if(inst_opcode == 0x2 ||inst_opcode == 0x3)
    {
        addr = inst.r_t.target;
        switch(inst_opcode)
        {   
            // case 0b000010: // j
            //     g_processor.pc = addr << 2;
            //     break;

            // case 0b000011: // jal
            //     g_processor.regs[31] = g_processor.pc + 4 ; // 다음 명령어 주소
            //     g_processor.pc = addr << 2;
            //     pipe_pin.is_jump = 1;
            //     printf("jump\n");
            //     jump();

            //     break;
        }
    }

    /* I type */
    else
    {
        rs = inst.r_t.r_i.rs;
        rt = inst.r_t.r_i.rt;
        imm = inst.r_t.r_i.r_i.imm;
        // printf("op = %d , rs = 0x%05x, rt = 0x%05x, imm = 0x%016x\n",inst_opcode,rs,rt,imm);

        switch(inst_opcode)
        {
            // case 0b001001: // addiu
            //     g_processor.regs[rt] = g_processor.regs[rs] + imm;
            //     // pipe_update();
            //     break;

            // case 0b001100: // andi
            //     g_processor.regs[rt] = g_processor.regs[rs] & imm;
            //     break;

            case 0b000100: // beq
                if(g_processor.regs[rt] == g_processor.regs[rs])
                    {
                    g_processor.pc = g_processor.pc + imm*4;
                    pipe_pin.is_flush = 1;
                    }
                break;

            case 0b000101: // bne
                // printf("ex = %08x\n",pipe_pin.EX);
                // printf("rt = %d, rs = %d\n",g_processor.regs[rt] ,g_processor.regs[rs] );
                if(g_processor.regs[rt] != g_processor.regs[rs])
                {   
                    // printf("ex = %08x\n",pipe_pin.EX);
                    g_processor.pc = pipe_pin.EX + imm*4;
                    pipe_pin.is_flush = 1;
                }
                break;

            // case 0b001111: // lui
            //     g_processor.regs[rt] = imm << 16;
            //     break;

            // case 0b100011: // lw
            //     g_processor.regs[rt] = mem_read_32(g_processor.regs[rs] + imm);
            //     break;
                
            // case 0b001101: // ori
            //     g_processor.regs[rt] = g_processor.regs[rs] | imm;
            //     break;

            // case 0b001011: // sltiu
            //     g_processor.regs[rt] = (g_processor.regs[rs] < imm) ? 1 : 0;
            //     // pipe_update();
            //     break;

            // case 0b101011: // sw
            //     mem_write_32(g_processor.regs[rt], g_processor.regs[rs]+imm);
            //     // [rs] + imm 주소의 값을 rt 레지스터가 가리키는 메모리에 저장
            //     break;
        }
    }
}

void pre_mem(struct inst_t inst)
{
    int inst_opcode = inst.opcode;
    int rs, rt, rd, shamt, funct, imm, addr;
    // if(g_processor.pc >= MEM_TEXT_START + g_processor.input_insts*4)

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
            // case 0b100000: // add
            //     g_processor.regs[rd] = g_processor.regs[rs] + g_processor.regs[rt];
            //     break;

            // case 0b100010: // sub
            //     g_processor.regs[rd] = g_processor.regs[rs] - g_processor.regs[rt];
            //     break;

            // case 0b100001: // addu
            //     g_processor.regs[rd] = g_processor.regs[rs] + g_processor.regs[rt];
            //     // pipe_update();
            //     break;

            // case 0b100100: // and
            //     g_processor.regs[rd] = g_processor.regs[rs] & g_processor.regs[rt];
            //     break;

            // case 0b001000: // jr
            //     g_processor.pc = g_processor.regs[rs];
            //     break;
            
            // case 0b100111: // nor
            //     g_processor.regs[rd] = ~(g_processor.regs[rs] | g_processor.regs[rt]);
            //     break;

            // case 0b100101: // or
            //     g_processor.regs[rd] = g_processor.regs[rs] | g_processor.regs[rt];
            //     break;

            case 0b101011: // sltu
                g_processor.regs[rd] = (g_processor.regs[rs] < g_processor.regs[rt]) ? 1 : 0;
                break;
            
            // case 0b000000: // sll
            //     g_processor.regs[rd] = g_processor.regs[rt] << shamt;
            //     break;
            
            // case 0b000010: // srl
            //     g_processor.regs[rd] = g_processor.regs[rt] >> shamt;
            //     break;

            // case 0b100011: // subu
            //     g_processor.regs[rd] = g_processor.regs[rs] - g_processor.regs[rt];
            //     break;
        }
    }

    /* J type */
    else if(inst_opcode == 0x2 ||inst_opcode == 0x3)
    {
        addr = inst.r_t.target;
        switch(inst_opcode)
        {   
            // case 0b000010: // j
            //     g_processor.pc = addr << 2;
            //     break;

            // case 0b000011: // jal
            //     g_processor.regs[31] = g_processor.pc + 4 ; // 다음 명령어 주소
            //     g_processor.pc = addr << 2;
            //     break;
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
            // case 0b001001: // addiu
            //     g_processor.regs[rt] = g_processor.regs[rs] + imm;
            //     // pipe_update();
            //     break;

            // case 0b001100: // andi
            //     g_processor.regs[rt] = g_processor.regs[rs] & imm;
            //     break;

            // case 0b000100: // beq
            //     if(g_processor.regs[rt] == g_processor.regs[rs])
            //         {
            //         g_processor.pc = g_processor.pc + imm*4;
            //         pipe_pin.is_flush = 1;
            //         }
            //     break;

            // case 0b000101: // bne
            //     if(g_processor.regs[rt] != g_processor.regs[rs])
            //     {
            //         g_processor.pc = pipe_pin.EX + imm*4;
            //         pipe_pin.is_flush = 1;
            //     }
            //     break;

            // case 0b001111: // lui
            //     g_processor.regs[rt] = imm << 16;
            //     break;

            // case 0b100011: // lw
            //     g_processor.regs[rt] = mem_read_32(g_processor.regs[rs] + imm);
            //     break;
                
            // case 0b001101: // ori
            //     g_processor.regs[rt] = g_processor.regs[rs] | imm;
            //     break;

            case 0b001011: // sltiu
                g_processor.regs[rt] = (g_processor.regs[rs] < imm) ? 1 : 0;
                // pipe_update();
                break;

            // case 0b101011: // sw
            //     mem_write_32(g_processor.regs[rt], g_processor.regs[rs]+imm);
            //     // [rs] + imm 주소의 값을 rt 레지스터가 가리키는 메모리에 저장
            //     break;
        }
    }
}

void writeback(struct inst_t inst)
{
    int inst_opcode = inst.opcode;
    int rs, rt, rd, shamt, funct, imm, addr;
    // if(g_processor.pc >= MEM_TEXT_START + g_processor.input_insts*4)

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
                // pipe_update();
                break;

            case 0b100100: // and
                g_processor.regs[rd] = g_processor.regs[rs] & g_processor.regs[rt];
                break;

            // case 0b001000: // jr
            //     g_processor.pc = g_processor.regs[rs];
            //     break;
            
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
            // case 0b000010: // j
            //     g_processor.pc = addr << 2;
            //     break;

            // case 0b000011: // jal
            //     g_processor.regs[31] = g_processor.pc + 4 ; // 다음 명령어 주소
            //     g_processor.pc = addr << 2;
            //     break;
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
                // pipe_update();
                break;

            case 0b001100: // andi
                g_processor.regs[rt] = g_processor.regs[rs] & imm;
                break;

            // case 0b000100: // beq
            //     if(g_processor.regs[rt] == g_processor.regs[rs])
            //         {
            //         g_processor.pc = g_processor.pc + imm*4;
            //         pipe_pin.is_flush = 1;
            //         }
            //     break;

            // case 0b000101: // bne
            //     if(g_processor.regs[rt] != g_processor.regs[rs])
            //     {
            //         g_processor.pc = pipe_pin.EX + imm*4;
            //         pipe_pin.is_flush = 1;
            //     }
            //     break;

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
                // pipe_update();
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
    // printf("cy = %d, flu =%d\n",pipe_pin.cycle_num,pipe_pin.is_flush);
    
    // jump
    if(pipe_pin.is_jump)
    {
        jump();
        pipe_pin.is_jump = 0;
        // return;
    }    
    
    pipe_update();

    
    // early stop
    if((pipe_pin.IF == 0 & pipe_pin.ID==0 & pipe_pin.EX == 0 & pipe_pin.MEM == 0 & pipe_pin.WE==0 & pipe_pin.cycle_num != 1) | pipe_pin.inst_num>=100)
    {
        g_processor.running = FALSE; // PC가 test size롤 초과하면 cycle 종료
        pipe_pin.cycle_num--;
        // printf("stop  pc = %08x\n",g_processor.pc);
        return;
    }    
    
    if(pipe_pin.is_flush)
    {
        flush();
        pipe_pin.is_flush = 0;
        return;
    }
    
    if(pipe_pin.is_stall)
    {
        stall();
        pipe_pin.is_stall = 0;
        return;
    }

    // 1. fetch
    if(pipe_pin.is_flush != 1 & g_processor.pc < MEM_TEXT_START + g_processor.input_insts*4)
    {
        pipe_pin.IF = g_processor.pc;    // 주소값 보고 fetch 업데이트
        g_processor.pc += BYTES_PER_WORD;
    }
    else
    {
        pipe_pin.IF = 0;   
    }


    // 2. exacute
    // // 2. decode  pipe_pin.cycle_num--;

    // printf("-- %d wb = %08x\n",pipe_pin.inst_num,pipe_pin.WE);
    if(pipe_pin.WE != 0)
    {
        inst_reg = fetch(pipe_pin.WE);  // jal 나오면 조건 줘서 stall   
        inst = decode(inst_reg); // 조건 줘서 null이 아니면 실행
        writeback(inst);
        pipe_pin.inst_num++;
        // printf("inst_num = %d\n",pipe_pin.inst_num);
    }

    if(pipe_pin.MEM != 0)
    {
        // printf("WB\n");
        inst_reg = fetch(pipe_pin.MEM);  // jal 나오면 조건 줘서 stall   
        inst = decode(inst_reg); // 조건 줘서 null이 아니면 실행
        pre_mem(inst);
        // printf("2- %d wb = %08x\n",pipe_pin.inst_num,pipe_pin.WE);
    }

    if(pipe_pin.EX != 0)
    {
        // printf("EX = %08x\n",pipe_pin.EX);
        // printf("get EX = 0x%08x\n",pipe_pin.EX);
        inst_reg = fetch(pipe_pin.EX);  // jal 나오면 조건 줘서 stall   
        inst = decode(inst_reg); // 조건 줘서 null이 아니면 실행
        execute(inst);
    }

    if(pipe_pin.ID != 0)
    {
        // printf("EX = %08x\n",pipe_pin.EX);
        // printf("get EX = 0x%08x\n",pipe_pin.EX);
        inst_reg = fetch(pipe_pin.ID);  // jal 나오면 조건 줘서 stall   
        inst = decode(inst_reg); // 조건 줘서 null이 아니면 실행
        jal_j(inst);
    }
    // // 3. execute
    // execute(inst);

    // 4. update stats
    // printf("state = %d\n",g_processor.running);
    // printf("state WE = %08x, IF = %08x\n",pipe_pin.WE,pipe_pin.IF);
    

    print_state();
    // if(g_processor.running)
    // {
    //     print_state();
    // }
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



////////////////////////// mem.c ////////////////////////////
#include "parse.h"
#include "util.h"

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