# computer-arch  
  
2024 7월  
안정섭교수님 연구실에서 진행한  
컴퓨터 구조 과제입니다.  
  
  
## 🌈 project 0 - Warming Up <구조체 생성>  
- input으로 주어진 directory의 구조를 생성  
  
  
## 🌈 project 1 - MIPS 명령어를 2진수 명령어로 변환  
- lui, ori가 16bit로 32bit 주소를 load하는 방식  
- jal, j, beq, bne 분기점 이동 시 PC 위치 파악  
- lw, sw의 동작 방식 파악  
등, 명령어의 동작 방식을 이해해야함,  
  
  
## 🌈 project 2 - 2진수 명령어의 register 조작  
- loader.c의 2진수 명령어를 load하는 명령어 완성  
- R, I, J type 명령어의 op, rs, rt, rd, imm, shamt의 각 의미를 파악  
  
  
## 🌈 project 3 - 8-way-set-associative cache의 동작  
- read hit, read miss, write hit, write miss, write back을 count  
- build_cache()를 완성하기 위해 각 set의 line의 구조를 이해한다  
- 8-way의 동작방식을 먼저 이해한다  
- Replacement Policy, Write Back을 위한 dirty를 이해한다.  
  
  
## 🌈 project 4 - Pipeline 제작  
- 2진수 명령어의 register 조작 및 IF-ID-EW-MEM-WE 각 단계 별 명령어 수행  
- flush, stall(by jump,by hazard)를 통제하기 위한 각 단계의 clue를 고민한다.  
