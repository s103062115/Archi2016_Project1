#include<stdio.h>
#include"register.h"
#define add 0x20
#define addu 0x21
#define sub 0x22
#define and 0x24
#define or 0x25
#define xor 0x26
#define nor 0x27
#define nand 0x28
#define slt 0x2A
#define sll 0x00
#define srl 0x02
#define sra 0x03
#define jr 0x08
int op,rs,rt,rd,shamt,func;
int sign(unsigned int a){
    return a/(int)(pow(2,31)+0.01);
}
int Add(int rs,int rt,int rd){
    registers[rd] = registers[rs] + registers[rt];
    if(sign(registers[rs]) == sign(registers[rt]) && sign(registers[rd]) != sign(registers[rs])){
        return -1;
    }
    return 1;
}
int Sub(int rs,int rt,int rd){
    registers[rd] = registers[rs] - registers[rt];
    if((sign(registers[rd])>0 && registers[rs] < registers[rt]) || (sign(registers[rd]<0 && registers[rs] > registers[rt]))){
        return -1;
    }
    return 1;
}
void And(int rs,int rt,int rd){
    unsigned int A1 = registers[rt];
    unsigned int A2 = registers[rs];
    int i;
    registers[rd] = 0;
    for(i = 0;i < 32;i++){
        registers[rd] = registers[rd] + ((A1%2)&&(A2%2))*(int)(pow(2,i)+0.01);
        A1 = A1/2;
        A2 = A2/2;
    }
}
void Or(int rs,int rt,int rd){
    unsigned int A1 = registers[rt];
    unsigned int A2 = registers[rs];
    int i;
    registers[rd] = 0;
    for(i = 0;i < 32;i++){
        registers[rd] = registers[rd] + ((A1%2)||(A2%2))*(int)(pow(2,i)+0.01);
        A1 = A1/2;
        A2 = A2/2;
    }
}
void Xor(int rs,int rt,int rd){
    unsigned int A1 = registers[rt];
    unsigned int A2 = registers[rs];
    int i;
    registers[rd] = 0;
    for(i = 0;i < 32;i++){
        registers[rd] = registers[rd] + ((A1+A2)%2)*(int)(pow(2,i)+0.01);
        A1 = A1/2;
        A2 = A2/2;
    }
}
void Nor(int rs,int rt,int rd){
    unsigned int A1 = registers[rt];
    unsigned int A2 = registers[rs];
    int i;
    registers[rd] = 0;
    for(i = 0;i < 32;i++){
        registers[rd] = registers[rd] + (!((A1%2)||(A2%2)))*(int)(pow(2,i)+0.01);
        A1 = A1/2;
        A2 = A2/2;
    }
}
void Nand(int rs,int rt,int rd){
    unsigned int A1 = registers[rt];
    unsigned int A2 = registers[rs];
    int i;
    registers[rd] = 0;
    for(i = 0;i < 32;i++){
        registers[rd] = registers[rd] + (!((A1%2)&&(A2%2)))*(int)(pow(2,i)+0.01);
        A1 = A1/2;
        A2 = A2/2;
    }
}
void Slt(int rs,int rt,int rd){
    int A1 = registers[rt];
    int A2 = registers[rs];
    registers[rd] = A1>A2;
}
void Sll(int rt,int rd,int C){
    int A1 = registers[rt];
    registers[rd] = A1 >> C;
}
void Srl(int rt,int rd,int C){
    int A1 = registers[rt];
    registers[rd] = A1 << C;
}
void Sra(int rt,int rd,int C){
    registers[rd] = registers[rt] >> C;
}
