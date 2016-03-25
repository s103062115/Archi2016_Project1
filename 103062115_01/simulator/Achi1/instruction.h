#include<stdio.h>
#include"register.h"
#include"Dmemory.h"
#include"Imemory.h"
#include"error.h"
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
int op,rs,rt,rd,shamt,func,imm,ads;
unsigned int inPC,PC;
unsigned int S(int C){
    unsigned int A1;
    if(C>=0x8000){
        A1 = C+0xffff0000;
    }else{
        A1 = C;
    }
    return A1;
}

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
int Addi(int rs,int rt,int C){
    unsigned int A1 = S(C);
    registers[rt] = registers[rs] + A1;
    if((C>=0x8000) == sign(registers[rs]) && sign(registers[rt]) != sign(registers[rs])){
        return -1;
    }
    return 1;
}
void Lui(int rt,int C){

    registers[rt] = C << 16;
}
void Andi(int rs,int rt,int C){
    unsigned int A1 = S(C);
    unsigned int A2 = registers[rs];
    int i;
    registers[rt] = 0;
    for(i = 0;i < 32;i++){
        registers[rt] = registers[rt] + ((A1%2)&&(A2%2))*(int)(pow(2,i)+0.01);
        A1 = A1/2;
        A2 = A2/2;
    }
}
void Ori(int rs,int rt,int C){
    unsigned int A1 = S(C);
    unsigned int A2 = registers[rs];
    int i;
    registers[rt] = 0;
    for(i = 0;i < 32;i++){
        registers[rt] = registers[rt] + ((A1%2)||(A2%2))*(int)(pow(2,i)+0.01);
        A1 = A1/2;
        A2 = A2/2;
    }
}
void Nori(int rs,int rt,int C){
    unsigned int A1 = S(C);
    unsigned int A2 = registers[rs];
    int i;
    registers[rt] = 0;
    for(i = 0;i < 32;i++){
        registers[rt] = registers[rt] + (!((A1%2)||(A2%2)))*(int)(pow(2,i)+0.01);
        A1 = A1/2;
        A2 = A2/2;
    }
}
void Slti(int rs,int rt,int C){
    int A2 = (int)registers[rs];
    int A1 = S(C);
    registers[rt] = (int)A2 < (int)A1;
}
int Cpr(int rs,int rt){
    if(((int)registers[rs] > (int)registers[rt] && (int)(registers[rs] - registers[rt])<0) || ((int)registers[rs] < (int)registers[rt] && (int)(registers[rs] - registers[rt])>0))error_no = 1;
    return registers[rs] - registers[rt];
}
void Lw(int rs,int rt,int C){
    int A1 = S(C);
    if(((int)registers[rs] + A1)%4!=0){
        error_dm = 1;
    }
    if((int)registers[rs] + A1 > 1023 || (int)registers[rs] + A1 < 0){
        error_ao = 1;
    }

    if(error() == 0){
        registers[rt] = Dm[((int)registers[rs] + C)/4];
    }
}
void Lhu(int rs,int rt,int C){
    if(((int)registers[rs] + C)%2!=0){
        error_dm = 1;
    }
    if((int)registers[rs] + C > 1023 || (int)registers[rs] + C < 0){
        error_ao = 1;
    }
    if(error() == 0){
        if(((int)registers[rs] + C)%4 == 2){
            registers[rt] = Dm[((int)registers[rs] + C)/4]%(int)(pow(2,16));

        }else{
            registers[rt] = Dm[((int)registers[rs] + C)/4]/(int)(pow(2,16));
        }
    }
}
void Lh(int rs,int rt,int C){
    int A1 = S(C);
    if(((int)registers[rs] + A1)%2!=0){
        error_dm = 1;
    }else if((int)registers[rs] + A1 > 1023 || (int)registers[rs] + A1 < 0){
        error_ao = 1;
    }
    if(error() == 0){
        if(((int)registers[rs] + A1)%4 == 2){
            registers[rt] = Dm[((int)registers[rs] + A1)/4]%(int)(pow(2,16));
        }else{
            registers[rt] = Dm[((int)registers[rs] + A1)/4]/(int)(pow(2,16));
        }
        if(registers[rt]>=(int)(pow(2,15))){
            registers[rt] = registers[rt]+0xffff0000;
        }
    }
}
void Lb(int rs,int rt,int C){
    int A1 = S(C);
    if((int)registers[rs] + A1 > 1023 || (int)registers[rs] + A1 < 0){
        error_ao = 1;
    }else{
        if(((int)registers[rs] + A1)%4 == 3){
            registers[rt] = Dm[((int)registers[rs] + A1)/4]%(int)(pow(2,8));
        }else if(((int)registers[rs] + C)%4 == 2){
            registers[rt] = Dm[((int)registers[rs] + A1)/4]/(int)(pow(2,8))%(int)(pow(2,8));
        }else if(((int)registers[rs] + C)%4 == 1){
            registers[rt] = Dm[((int)registers[rs] + A1)/4]/(int)(pow(2,16))%(int)(pow(2,8));
        }else{
            registers[rt] = Dm[((int)registers[rs] + A1)/4]/(int)(pow(2,24));
        }
        if(registers[rt]>=(int)(pow(2,7))){
            registers[rt] = registers[rt]+0xffffff00;
        }
    }
}
void Lbu(int rs,int rt,int C){
    if((int)registers[rs] + C > 1023){
        error_ao = 1;
    }else{
        if(((int)registers[rs] + C)%4 == 3){
            registers[rt] = Dm[((int)registers[rs] + C)/4]%(int)(pow(2,8));
        }else if(((int)registers[rs] + C)%4 == 2){
            registers[rt] = Dm[((int)registers[rs] + C)/4]/(int)(pow(2,8))%(int)(pow(2,8));
        }else if(((int)registers[rs] + C)%4 == 1){
            registers[rt] = Dm[((int)registers[rs] + C)/4]/(int)(pow(2,16))%(int)(pow(2,8));
        }else{
            registers[rt] = Dm[((int)registers[rs] + C)/4]/(int)(pow(2,24));
        }
    }
}
void Sw(int rs,int rt,int C){
    int A1 = S(C);
    if(((int)registers[rs] + A1)%4!=0){
        error_dm = 1;
    }
    if((int)registers[rs] + A1 > 1023){
        error_ao = 1;
    }
    if(error() == 0){
        Dm[((int)registers[rs] + A1)/4] = registers[rt];
    }
}
void Sh(int rs,int rt,int C){
    int A1 = S(C);
    if(((int)registers[rs] + A1)%2!=0){
        error_dm = 1;
    }else if((int)registers[rs] + A1 > 1023){
        error_ao = 1;
    }
    if(error() == 0){
        if(((int)registers[rs] + A1)%4 == 2){
            Dm[((int)registers[rs] + A1)/4] = Dm[((int)registers[rs] + A1)/4]/0x00010000*0x00010000;
            Dm[((int)registers[rs] + A1)/4] = Dm[((int)registers[rs] + A1)/4] + registers[rt]%0x00010000;

        }else{
            Dm[((int)registers[rs] + A1)/4] = Dm[((int)registers[rs] + A1)/4]%0x00010000;
            Dm[((int)registers[rs] + A1)/4] = Dm[((int)registers[rs] + A1)/4] + registers[rt]%0x00010000*0x00010000;
        }
    }
}
void Sb(int rs,int rt,int C){
    int A1 = S(C);
    if((int)registers[rs] + A1 > 1023){
        error_ao = 1;
    }else{
        if(((int)registers[rs] + A1)%4 == 3){
            Dm[((int)registers[rs] + A1)/4] = Dm[((int)registers[rs] + A1)/4]/0x00000100*0x00000100;
            Dm[((int)registers[rs] + A1)/4] = Dm[((int)registers[rs] + A1)/4] + registers[rt]%0x00000100;
        }else if(((int)registers[rs] + A1)%4 == 2){
            Dm[((int)registers[rs] + A1)/4] = Dm[((int)registers[rs] + A1)/4]/0x00010000*0x00010000 + Dm[((int)registers[rs] + A1)/4]%0x00000100;
            Dm[((int)registers[rs] + A1)/4] = Dm[((int)registers[rs] + A1)/4] + registers[rt]%0x00000100*0x00000100;
        }else if(((int)registers[rs] + A1)%4 == 1){
            Dm[((int)registers[rs] + A1)/4] = Dm[((int)registers[rs] + A1)/4]/0x01000000*0x01000000 + Dm[((int)registers[rs] + A1)/4]%0x00010000;
            Dm[((int)registers[rs] + A1)/4] = Dm[((int)registers[rs] + A1)/4] + registers[rt]%0x00000100*0x00010000;
        }else{
            Dm[((int)registers[rs] + A1)/4] = Dm[((int)registers[rs] + A1)/4]%0x01000000;
            Dm[((int)registers[rs] + A1)/4] = Dm[((int)registers[rs] + A1)/4] + registers[rt]%0x00000100*0x01000000;
        }
    }
}
