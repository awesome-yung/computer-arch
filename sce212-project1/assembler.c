#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>


/*
 * For debug option. If you want to debug, set 1.
 * If not, set 0.
 */
#define DEBUG 0

#define MAX_SYMBOL_TABLE_SIZE   1024
#define MEM_TEXT_START          0x00400000
#define MEM_DATA_START          0x10000000
#define BYTES_PER_WORD          4
#define INST_LIST_LEN           22

char check_la[1024][1024];
int check_la_addr[1024];
int data_count = 0;

/******************************************************
 * Structure Declaration
 *******************************************************/

typedef struct inst_struct {
    char *name;
    char *op;
    char type;
    char *funct;
} inst_t;

typedef struct symbol_struct {
    char name[32];
    uint32_t address;
} symbol_t;

enum section {
    DATA = 0,
    TEXT,
    MAX_SIZE
};

/******************************************************
 * Global Variable Declaration
 *******************************************************/

inst_t inst_list[INST_LIST_LEN] = {       //  idx
    {"add",     "000000", 'R', "100000"}, //    0
    {"sub",     "000000", 'R', "100010"}, //    1
    {"addiu",   "001001", 'I', ""},       //    2
    {"addu",    "000000", 'R', "100001"}, //    3
    {"and",     "000000", 'R', "100100"}, //    4
    {"andi",    "001100", 'I', ""},       //    5
    {"beq",     "000100", 'I', ""},       //    6
    {"bne",     "000101", 'I', ""},       //    7
    {"j",       "000010", 'J', ""},       //    8
    {"jal",     "000011", 'J', ""},       //    9
    {"jr",      "000000", 'R', "001000"}, //   10
    {"lui",     "001111", 'I', ""},       //   11
    {"lw",      "100011", 'I', ""},       //   12
    {"nor",     "000000", 'R', "100111"}, //   13
    {"or",      "000000", 'R', "100101"}, //   14
    {"ori",     "001101", 'I', ""},       //   15
    {"sltiu",   "001011", 'I', ""},       //   16
    {"sltu",    "000000", 'R', "101011"}, //   17
    {"sll",     "000000", 'R', "000000"}, //   18
    {"srl",     "000000", 'R', "000010"}, //   19
    {"sw",      "101011", 'I', ""},       //   20
    {"subu",    "000000", 'R', "100011"}  //   21
};

symbol_t SYMBOL_TABLE[MAX_SYMBOL_TABLE_SIZE]; // Global Symbol Table

uint32_t symbol_table_cur_index = 0; // For indexing of symbol table

/* Temporary file stream pointers */
FILE *data_seg;
FILE *text_seg;

/* Size of each section */
uint32_t data_section_size = 0;
uint32_t text_section_size = 0;

int check_la_count_repeat = 0;


/******************************************************
 * Function Declaration
 *******************************************************/

/* Change file extension from ".s" to ".o" */
char* change_file_ext(char *str) {
    char *dot = strrchr(str, '.');

    if (!dot || dot == str || (strcmp(dot, ".s") != 0))
        return NULL;

    str[strlen(str) - 1] = 'o';
    return "";
}

/* Add symbol to global symbol table */
void symbol_table_add_entry(symbol_t symbol)
{
    SYMBOL_TABLE[symbol_table_cur_index++] = symbol;
#if DEBUG
    printf("symbol_table_add_entry DEBUG -> %s: 0x%08x\n", symbol.name, symbol.address);
#endif
}

/* Convert integer number to binary string */
char* num_to_bits(unsigned int num, int len)
{
    char* bits = (char *) malloc(len+1);
    int idx_ = len-1, i;
    while (num > 0 && idx_ >= 0) {
        if (num % 2 == 1) {
            bits[idx_--] = '1';
        } else {
            bits[idx_--] = '0';
        }
        num /= 2;
    }
    for (i = idx_; i >= 0; i--){
        bits[i] = '0';
    }
    bits[len] = '\0';
    return bits;
}

void add_data_to_la(const char* data)
{
    // printf(">>> %s\n",data);
    if(data==NULL)
    {
        sprintf(check_la[data_count], "%d", data_count);
    }
    else
    {
        strcpy(check_la[data_count], data);
    }
    check_la_addr[data_count] = data_count*4;
    data_count++;
}

int compare_last_pos(char *data)
{
    if(data == NULL)
    {
        return 1;
    }
    for (int i = 0; i < data_count; i++) {
        if (strcmp(check_la[i], data) == 0) {
            return 0;
        }
    }
    return 1;
}

void initialize_check_la() {
    memset(check_la, 0, sizeof(check_la));  // 배열을 0으로 초기화
    data_count = 0;  // 데이터 카운트 초기화
}


/* Record .text section to output file */
void record_text_section(FILE *output)
{
    uint32_t cur_addr = MEM_TEXT_START;
    char line[1024];
    int current_add = 0;
    char solution[1024];
    
    
    /* Point to text_seg stream */
    rewind(text_seg);

    /* Print .text section */
    strcpy(solution, num_to_bits(text_section_size<<2,32));
    fprintf(output, solution);
    fprintf(output, "\n");

    strcpy(solution, num_to_bits(data_section_size<<2,32));
    fprintf(output, solution);
    fprintf(output, "\n");

    while (fgets(line, 1024, text_seg) != NULL) {
        char inst[0x1000] = {0};
        char op[32] = {0};
        char label[32] = {0};
        char type = '0';
        int i=0, idx = 0;
        int rs, rt, rd, imm, shamt;
        int addr;
        int recall_point;
        char solution[1024];
        // char check_la;
        int check_la_count = 0;
        char upper_addr[1024];
        char lower_addr[1024];

        rs = rt = rd = imm = shamt = addr = 0;
        
        

#if DEBUG
        printf("\nrecord_text_section DEBUG -> 0x%08x: ", cur_addr);
#endif
        /* Find the instruction type that matches the line */
        /* blank */
        // char la[3] = "la";
        strcpy(inst, strtok(line, "\t"));

        while(idx<INST_LIST_LEN)
        {
            if(strcmp(inst, inst_list[idx].name) == 0 && strcmp(inst, "la") != 0)
            {
                type = inst_list[idx].type;
                break;
            }
            else if(strcmp(inst, "la") == 0)
            {
                idx = 11;
                int check_la_count = 1;
                type = inst_list[idx].type;
                break;
            }
            idx++;
        }

        char temp_word[1000]; // 임시로 명령어 뒷부분 저장
        
        switch (type) {
            case 'R':

                strcpy(temp_word, strtok(NULL, "\t"));  //  temp_word = $18, $17, 1
                strcpy(op, inst_list[idx].op);
                char funct[7];
                strcpy(funct, inst_list[idx].funct);
                
                if(strcmp(inst,"sll")==0 || strcmp(inst,"srl")==0)
                {
                    /* blank */
                    rd = atoi(strtok(temp_word, ", ")+1);
                    rt = atoi(strtok(NULL, ", ")+1);
                    shamt = atoi(strtok(NULL, ", "));

                    // rt = atoi(strtok(NULL, ", ")+1);
                    strcpy(solution, op);
                    strcpy(solution+6, num_to_bits(0,5));
                    strcpy(solution+11, num_to_bits(rt,5));
                    strcpy(solution+16, num_to_bits(rd,5));
                    strcpy(solution+21, num_to_bits(shamt,5));
                    strcpy(solution+26, funct);
                    fprintf(output, solution);
                }
                else if(strcmp(inst,"jr")==0)
                {
                    // printf("tem = %s\n",temp_word);
                    strcpy(temp_word, strtok(temp_word, "$")); //jr	$31에서 31
                    // printf("jr %s\n",temp_word);
                    
                    rd = 0;
                    rs = atoi(temp_word);
                    rt = 0;
                    shamt = 0;

                    // rt = atoi(strtok(NULL, ", ")+1);
                    strcpy(solution, op);
                    strcpy(solution+6, num_to_bits(rs,5));
                    strcpy(solution+11, num_to_bits(rt,5));
                    strcpy(solution+16, num_to_bits(rd,5));
                    strcpy(solution+21, num_to_bits(shamt,5));
                    strcpy(solution+26, funct);
                    fprintf(output, solution);

                }
                else
                {
                    rd = atoi(strtok(temp_word, ", ")+1);
                    rs = atoi(strtok(NULL, ", ")+1);
                    rt = atoi(strtok(NULL, ", ")+1);
                    strcpy(solution, op);
                    strcpy(solution+6, num_to_bits(rs,5));
                    strcpy(solution+11, num_to_bits(rt,5));
                    strcpy(solution+16, num_to_bits(rd,5));
                    strcpy(solution+21, num_to_bits(0,5));
                    strcpy(solution+26, funct);
                    fprintf(output, solution);
                }

#if DEBUG
                printf("record_text_section DEBUG R-> op:%s rs:$%d rt:$%d rd:$%d shamt:%d funct:%s\n",
                        op, rs, rt, rd, shamt, inst_list[idx].funct);
#endif
                break;

            case 'I':
                /* blank */

                // make lui
                strcpy(temp_word, strtok(NULL, "\t"));  // temp_word = $8, data1 
                // printf("temp>> %s\n",temp_word);
                strcpy(op, inst_list[idx].op);     //or //           = $17, $17, 0x1
                imm = MEM_DATA_START;
                // printf("b, imm = %d\n",imm);
                if(strcmp(inst,"la")==0)
                {
                    int not_first = 0;
                    rs = 0;
                    rt = atoi(strtok(temp_word, ", ")+1);  // rt = 8
                    strcpy(temp_word, strtok(NULL, ", "));  //temp_word = data1
                    for(int la_idx=0; la_idx < data_count; la_idx++)
                    {
                        // printf("%s == %s\n",temp_word, check_la[la_idx]);
                        if(strcmp(temp_word, check_la[la_idx])==0)
                        {
                            imm += check_la_addr[la_idx];
                            break;
                        }
                    }
                    int upper = (imm >> 16) & 0xFFFF;
                    int lower = imm & 0xFFFF;

                    // lui 저장
                    strcpy(solution,op);
                    strcpy(solution+6,num_to_bits(rs,5));
                    strcpy(solution+11,num_to_bits(rt,5));
                    strcpy(solution+16,num_to_bits(upper,16));

                    fprintf(output, solution);

                    if(lower != 0)
                    {
                        current_add++; // la 명령어로 current_add 추가
                        idx = 15;
                        rs = rt;
                        strcpy(op, inst_list[idx].op);

                        strcpy(solution,op);
                        strcpy(solution+6,num_to_bits(rs,5));
                        strcpy(solution+11,num_to_bits(rt,5));
                        strcpy(solution+16,num_to_bits(lower,16));
                        fprintf(output, "\n");
                        fprintf(output, solution);

                    }

                }

                else if(strcmp(inst,"bne")==0 || strcmp(inst,"beq")==0)
                {
                    int bne_count = 0;
                    strcpy(temp_word, strtok(temp_word, ", ")); // $11, $8, lab2
                    rs = atoi(temp_word + 1);
                    
                    strcpy(temp_word, strtok(NULL, ", ")); // $11, $8, lab2
                    rt = atoi(temp_word + 1);

                    strcpy(temp_word, strtok(NULL, ", ")); // temp_word = lab2
                    while(strcmp(temp_word,SYMBOL_TABLE[bne_count].name)!=0)
                    {
                        bne_count++;
                    }
                    imm = SYMBOL_TABLE[bne_count].address - current_add -1;
                    int constant = 0xffff;
                    int num;
                    num = constant & imm;   // 16비트로 정규화
                    
                    if(num<0)
                    {
                        num = (imm ^ constant) + 1;  // xor로 보수  
                        // num = (num ^ constant) + 1; 
                    }
                    strcpy(solution,op);
                    strcpy(solution+6,num_to_bits(rs,5));
                    strcpy(solution+11,num_to_bits(rt,5));  // la에서만 상위 16비트, 나머진 26비트
                    strcpy(solution+16,num_to_bits(num,16));
                    fprintf(output, solution);
                    
                    // printf("%d, %d \n",SYMBOL_TABLE[bne_count].address,current_add);
                    // printf("inst = %s, num = %d, imm = %d\n",inst,num,imm);
                    // printf("solution = %s\n",solution);
                  
                }

                else if(strcmp(inst,"lw")==0 || strcmp(inst,"sw")==0)
                {
                    strcpy(temp_word, strtok(temp_word, "($, )")); // $5, 0($3)
                    // printf("rt-> %s\n",temp_word);
                    rt = atoi(temp_word); // rt = 5
                    strcpy(temp_word, strtok(NULL, "($, )")); // $5, 0($3)에서 0
                    // printf("imm-> %s\n",temp_word);
                    imm = atoi(temp_word);
                    strcpy(temp_word, strtok(NULL, "($, )")); // $5, 0($3)에서 3

                    rs = atoi(temp_word);
                    
                    strcpy(solution,op);
                    strcpy(solution+6,num_to_bits(rs,5));
                    strcpy(solution+11,num_to_bits(rt,5));
                    // printf("rt, imm, rs-> %d,%d,%d\n",rt,imm,rs);  
                    strcpy(solution+16,num_to_bits(imm,16));
                    fprintf(output, solution);
                }

                else if(strcmp(inst,"lui")==0)
                {
                    strcpy(temp_word, strtok(temp_word, ", ")); // $17, 100
                    // printf("temp = %s\n",temp_word);
                    rs = 0;
                    rt = atoi(temp_word + 1);
                    strcpy(temp_word, strtok(NULL, ", ")); // 100
                    if(temp_word[0] == '0') // 16진수 시작
                    {
                        imm = strtol(temp_word,&solution,16);
                    }
                    else
                    {
                        imm = atoi(temp_word);    // 10진수 시작
                    }
                    
                    strcpy(upper_addr,num_to_bits(imm,16));
                    // printf("upper = %s, op=%s\n",upper_addr,op);
                    strcpy(solution,op);
                    // printf("sol = %s\n",solution);
                    strcpy(solution+6,num_to_bits(rs,5));
                    strcpy(solution+11,num_to_bits(rt,5));  
                    strcpy(solution+16,upper_addr);
                    fprintf(output, solution);

                }

                else // addiu, 
                {
                    strcpy(temp_word, strtok(temp_word, ", ")); // $17, $17, 0x1
                    rt = atoi(temp_word + 1);

                    strcpy(temp_word, strtok(NULL, ", "));
                    rs = atoi(temp_word + 1);

                    strcpy(temp_word, strtok(NULL, ", "));
                    if(temp_word[0] == '0') // 16진수 시작
                    {
                        imm = strtol(temp_word,&solution,16);
                    }
                    else
                    {
                        imm = atoi(temp_word);    // 10진수 시작
                    }
                    
                    
                    strcpy(upper_addr,num_to_bits(imm,16));
                    upper_addr[16] = '\0';
                    
                    strcpy(solution,op);
                    strcpy(solution+6,num_to_bits(rs,5));
                    strcpy(solution+11,num_to_bits(rt,5));  
                    strcpy(solution+16,upper_addr);
                    fprintf(output, solution);
                }

#if DEBUG
                printf("record_text_section DEBUG I-> op:%s rs:$%d rt:$%d imm:0x%x\n",
                        op, rs, rt, imm);
#endif
                break;

            case 'J':
                /* blank */
                strcpy(temp_word, strtok(NULL, "\t"));
                strcpy(op, inst_list[idx].op);

                // find destination command
                recall_point = 0;
                for(int i = 0; i<symbol_table_cur_index; i++)
                {
                    if(strcmp(SYMBOL_TABLE[i].name, temp_word)==0)
                    {
                        recall_point = SYMBOL_TABLE[i].address;
                        break;
                    } 
                }

                // convert destination to 26bit
                unsigned int shifted_add = MEM_TEXT_START >> 2; // (상단4개 없애고), 2개 shift = 32 -> 26
                shifted_add += recall_point; // + 상대위치
                // shifted_add++; // 자연수 보정
                char set_address[27];
                
                strcpy(set_address,num_to_bits(shifted_add, 26)); // address부분 선언

                strcpy(solution,op);
                strcpy(solution+6,set_address);
                fprintf(output, solution);


#if DEBUG
                printf("\nrecord_text_section DEBUG J-> op:%s addr:%i\n", op, addr);
#endif
                break;

            default:
                break;
        }
        fprintf(output, "\n");

        // cur_addr += BYTES_PER_WORD;
        current_add++;
    }
  
}

/* Record .data section to output file */
void record_data_section(FILE *output)
{
    uint32_t cur_addr = MEM_DATA_START;
    char line[1024];
    char temp_addr[100];
    char solution[1024];
    long num;
    char *endptr;
    int loop_count=0;

    /* Point to data segment stream */
    rewind(data_seg);

    /* Print .data section */
    while (fgets(line, 1024, data_seg) != NULL) {
        /* blank */
        char *token = strtok(line, "\n\t");
        char *last_token = NULL;

        while (token != NULL) {
            last_token = token;
            token = strtok(NULL, "\n\t");
            // printf("%s\n", last_token);
        }
        strcpy(temp_addr, last_token);

        
        if(temp_addr[0] == '0')  // 16진수 str -> long
        {
            num = strtol(temp_addr,&solution,16);
        }
        else // 10진수 str -> long
        {
            num = atoi(temp_addr);
        }
    
        strcpy(solution,num_to_bits(num,32));
        // printf("data = %d, %s\n",num, solution);
        fprintf(output, solution);
        fprintf(output, "\n");
        loop_count++;
#if DEBUG
        printf("\nrecord_data_section DEBUG -> 0x%08x: ", cur_addr);
        printf("%s", line);
#endif
        cur_addr += BYTES_PER_WORD;
    }
    
}

/* Fill the blanks */
void make_binary_file(FILE *output)
{
#if DEBUG
    char line[1024] = {0};
    rewind(text_seg);
    /* Print line of text segment */
    while (fgets(line, 1024, text_seg) != NULL) {
        printf("\nmake_binary_file DEBUG -> %s",line);
    }
    printf("\nmake_binary_file DEBUG -> text section size: %d, data section size: %d\n",
            text_section_size, data_section_size);
#endif

    /* Print text section size and data section size */
    /* blank */
    rewind(text_seg); 
    rewind(data_seg);  

    /* Print .text section */
    record_text_section(output);

    /* Print .data section */
    record_data_section(output);
}


/* Fill the blanks */
void make_symbol_table(FILE *input)
{
    char line[1024] = {0};
    uint32_t address = 0;
    enum section cur_section = MAX_SIZE;
    int check_la_count = 0;
    // char check_la[1024][1024];
    char check_add[1024];
    int loop_count=0;
    // address += BYTES_PER_WORD; // 시작 첫 칸
    /* Read each section and put the stream */
    
    while (fgets(line, 1024, input) != NULL) {
        char *temp;
        char _line[1024] = {0};
        strcpy(_line, line);
        temp = strtok(_line, "\n\t");

        /* Check section type */
        if (!strcmp(temp, ".data")) {
            /* blank */
            cur_section = DATA;
            data_seg = tmpfile();
            fprintf(data_seg, "");
            continue;
        }
        else if (!strcmp(temp, ".text")) {
            /* blank */
            cur_section = TEXT;
            text_seg = tmpfile();
            fprintf(text_seg, "");
            continue;
        }

        /* Put the line into each segment stream */
        
        if (cur_section == DATA) {
            char temp_inst[1024];
            /* blank */
            
            strcpy(temp_inst,temp); 
            if(temp_inst[strlen(temp_inst)-1] ==':')
            {
                temp_inst[strlen(temp_inst)-1] = '\0';
                strcpy(check_la[data_count], temp_inst);
                
            }
            else
            {
                sprintf(check_la[data_count], "data_%d", data_count); 
            }
            check_la_addr[data_count] = data_count*4;
            data_count++ ; 

            // printf("%s\n",temp_inst);
            while(temp != NULL)
            {
                fprintf(data_seg, temp);
                fprintf(data_seg, "\t");
                temp = strtok(NULL, "\n\t");
            }
            fprintf(data_seg, "\n");
            data_section_size ++;

        }

        else if (cur_section == TEXT) {
            /* blank */
            if(_line[0] != '\t')
            {
                // printf("if_1\n");
                symbol_t new_symbol;

                int len = strlen(temp); // 문자 마지막에 \0로 바꿔서 : 제거
                temp[len-1] = '\0';

                strcpy(new_symbol.name, temp);
                new_symbol.address = address/4;
                symbol_table_add_entry(new_symbol);
            }
            else
            {
                char temp_inst[100];
                char temp_data[100];
                char *temp_data_split;
                char load_data_from[100];

                strcpy(temp_inst, temp); // 명령어 임시 저장 = la
                
                // strcpy(check_la,temp);
                while(temp != NULL)
                {   
                    strcpy(check_add, temp);
                    fprintf(text_seg, temp);
                    fprintf(text_seg, "\t");
                    temp = strtok(NULL, "\n\t");
                }

                fprintf(text_seg, "\n");
                text_section_size ++;

                strcpy(temp_data,check_add); // $17, $17, $0 혹은 lab1 복사
                temp_data_split = strtok(temp_data,", ");
                // printf("> > : %s, %s\n", temp_data_split,temp_inst);
                temp_data_split = strtok(NULL,", "); // temp_data_split = &17 혹은 lab1
                // if(temp_data_split != NULL && temp_data_split[0]!='$')
                if(strcmp(temp_inst,"la")==0)
                {
                    strcpy(load_data_from,temp_data_split);
                    for(int la_idx=0; la_idx < data_count+1; la_idx++)
                    {
                        if(strcmp(load_data_from, check_la[la_idx])==0)
                        {
                            break;
                        }
                        else
                        {
                            add_data_to_la(load_data_from);
                            text_section_size++ ;
                            address += BYTES_PER_WORD; // la 명령어 있으면 주소 +4
                            break;
                        }
                    }
                }



                // if(compare_last_pos(load_data_from) && strcmp(temp_inst,"la")==0)
                // {
                //     if(check_la_count==1)
                //         {
                //             text_section_size ++;
                //         }
                //     add_data_to_la(load_data_from);
                // }
                // check_la에 마지막 명령어 저장
                address += BYTES_PER_WORD;
            }
        
        }

        
        
    }
    
}

/******************************************************
 * Function: main
 *
 * Parameters:
 *  int
 *      argc: the number of argument
 *  char*
 *      argv[]: array of a sting argument
 *
 * Return:
 *  return success exit value
 *
 * Info:
 *  The typical main function in C language.
 *  It reads system arguments from terminal (or commands)
 *  and parse an assembly file(*.s).
 *  Then, it converts a certain instruction into
 *  object code which is basically binary code.
 *
 *******************************************************/

int main(int argc, char* argv[])
{
    FILE *input, *output;
    char *filename;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <*.s>\n", argv[0]);
        fprintf(stderr, "Example: %s sample_input/example?.s\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Read the input file */
    input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    /* Create the output file (*.o) */
    filename = strdup(argv[1]); // strdup() is not a standard C library but fairy used a lot.
    if(change_file_ext(filename) == NULL) {
        fprintf(stderr, "'%s' file is not an assembly file.\n", filename);
        exit(EXIT_FAILURE);
    }

    output = fopen(filename, "w");
    if (output == NULL) {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    /******************************************************
     *  Let's complete the below functions!
     *
     *  make_symbol_table(FILE *input)
     *  make_binary_file(FILE *output)
     *  ├── record_text_section(FILE *output)
     *  └── record_data_section(FILE *output)
     *
     *******************************************************/
    make_symbol_table(input);
    make_binary_file(output);

    fclose(input);
    fclose(output);

    return 0;
}
