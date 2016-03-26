#include <stdio.h>
#include <stdlib.h>
#define v0 2
#define v1 3
#define a0 4
#define a1 5
#define a2 6
#define a3 7
#define t0 8
#define t1 9
#define t2 10
#define t3 11
#define t4 12
#define t5 13
#define t6 14
#define t7 15
#define s0 16
#define s1 17
#define s2 18
#define s3 19
#define s4 20
#define s5 21
#define s6 22
#define s7 23
#define t8 24
#define t9 25
#define gp 28
#define sp 29
#define fp 30
#define ra 31

unsigned int Pow(int a,int p){
    int i;
    unsigned int tmp = 1;
    for(i = 0;i < p;i++){
        tmp = tmp*a;
    }
    return tmp;
}
unsigned int Dm[256] = {0};
int Dnum;
unsigned int Im[256] = {0};
int Inum;

unsigned int registers[32] = {0};

int error_w0 = 0;
int error_no = 0;
int error_ao = 0;
int error_dm = 0;
int error(){
    return error_w0 || error_no || error_ao || error_dm;
}
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
    return a/(int)(Pow(2,31)+0.01);
}
int Add(int rs,int rt,int rd){
    if(registers[rs] + registers[rt] < registers[rs] || registers[rs] + registers[rt] < registers[rt]){
        registers[rd] = registers[rs] + registers[rt];
        return -1;
    }
    if((int)registers[rs] < 0 && (int)(registers[rs] + registers[rt]) >= (int)registers[rt]){
        registers[rd] = registers[rs] + registers[rt];
        return -1;
    }
    if((int)registers[rt] < 0 && (int)(registers[rs] + registers[rt]) >= (int)registers[rs]){
        registers[rd] = registers[rs] + registers[rt];
        return -1;
    }
    if(sign(registers[rs]) == sign(registers[rt]) && sign(registers[rs] + registers[rt]) != sign(registers[rs])){
        registers[rd] = registers[rs] + registers[rt];
        return -1;
    }
    registers[rd] = registers[rs] + registers[rt];
    return 1;
}
int Sub(int rs,int rt,int rd){
    if(registers[rs] - registers[rt] > registers[rs]){
        registers[rd] = registers[rs] - registers[rt];
        return -1;
    }
    if((int)registers[rt] > 0 && (int)(registers[rs] - registers[rt]) >= (int)registers[rs]){
        registers[rd] = registers[rs] - registers[rt];
        return -1;
    }
    if((sign(registers[rs] - registers[rt])>0 && registers[rs] < registers[rt]) || (sign(registers[rs] - registers[rt])<0 && registers[rs] > registers[rt])){
        registers[rd] = registers[rs] - registers[rt];
        return -1;
    }
    registers[rd] = registers[rs] - registers[rt];
    return 1;
}
void And(int rs,int rt,int rd){
    unsigned int A1 = registers[rt];
    unsigned int A2 = registers[rs];
    int i;
    registers[rd] = 0;
    for(i = 0;i < 32;i++){
        registers[rd] = registers[rd] + ((A1%2)&&(A2%2))*(int)(Pow(2,i)+0.01);
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
        registers[rd] = registers[rd] + ((A1%2)||(A2%2))*(int)(Pow(2,i)+0.01);
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
        registers[rd] = registers[rd] + ((A1+A2)%2)*(int)(Pow(2,i)+0.01);
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
        registers[rd] = registers[rd] + (!((A1%2)||(A2%2)))*(int)(Pow(2,i)+0.01);
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
        registers[rd] = registers[rd] + (!((A1%2)&&(A2%2)))*(int)(Pow(2,i)+0.01);
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
        registers[rt] = registers[rt] + ((A1%2)&&(A2%2))*(int)(Pow(2,i)+0.01);
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
        registers[rt] = registers[rt] + ((A1%2)||(A2%2))*(int)(Pow(2,i)+0.01);
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
        registers[rt] = registers[rt] + (!((A1%2)||(A2%2)))*(int)(Pow(2,i)+0.01);
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
            registers[rt] = Dm[((int)registers[rs] + C)/4]%(int)(Pow(2,16));

        }else{
            registers[rt] = Dm[((int)registers[rs] + C)/4]/(int)(Pow(2,16));
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
            registers[rt] = Dm[((int)registers[rs] + A1)/4]%(int)(Pow(2,16));
        }else{
            registers[rt] = Dm[((int)registers[rs] + A1)/4]/(int)(Pow(2,16));
        }
        if(registers[rt]>=(int)(Pow(2,15))){
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
            registers[rt] = Dm[((int)registers[rs] + A1)/4]%(int)(Pow(2,8));
        }else if(((int)registers[rs] + C)%4 == 2){
            registers[rt] = Dm[((int)registers[rs] + A1)/4]/(int)(Pow(2,8))%(int)(Pow(2,8));
        }else if(((int)registers[rs] + C)%4 == 1){
            registers[rt] = Dm[((int)registers[rs] + A1)/4]/(int)(Pow(2,16))%(int)(Pow(2,8));
        }else{
            registers[rt] = Dm[((int)registers[rs] + A1)/4]/(int)(Pow(2,24));
        }
        if(registers[rt]>=(int)(Pow(2,7))){
            registers[rt] = registers[rt]+0xffffff00;
        }
    }
}
void Lbu(int rs,int rt,int C){
    if((int)registers[rs] + C > 1023){
        error_ao = 1;
    }else{
        if(((int)registers[rs] + C)%4 == 3){
            registers[rt] = Dm[((int)registers[rs] + C)/4]%(int)(Pow(2,8));
        }else if(((int)registers[rs] + C)%4 == 2){
            registers[rt] = Dm[((int)registers[rs] + C)/4]/(int)(Pow(2,8))%(int)(Pow(2,8));
        }else if(((int)registers[rs] + C)%4 == 1){
            registers[rt] = Dm[((int)registers[rs] + C)/4]/(int)(Pow(2,16))%(int)(Pow(2,8));
        }else{
            registers[rt] = Dm[((int)registers[rs] + C)/4]/(int)(Pow(2,24));
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
int end = 0;
unsigned int rvs(unsigned int buffer){
    unsigned int a = 0;
    int b1,b2,b3;
    b1 = (int)(Pow(2,8)+0.01);
    b2 = (int)(Pow(2,16)+0.01);
    b3 = (int)(Pow(2,24)+0.01);
    a = a+buffer/b3;
    a = a+(buffer%b3)/b2*b1;
    a = a+(buffer%b2)/b1*b2;
    a = a+(buffer%b1)*b3;
    return a;
}
void Rtype(unsigned int b){

    if(func == add){
        if(Add(rs,rt,rd) < 0){
            error_no = 1;
        }
    }else if(func == addu){
        int a = Add(rs,rt,rd);
    }else if(func == sub){
        if(Sub(rs,rt,rd) < 0){
            error_no = 1;
        }
    }else if(func == and){
        And(rs,rt,rd);
    }else if(func == or){
        Or(rs,rt,rd);
    }else if(func == xor){
        Xor(rs,rt,rd);
    }else if(func == nor){
        Nor(rs,rt,rd);
    }else if(func == nand){
        Nand(rs,rt,rd);
    }else if(func == slt){
        Slt(rs,rt,rd);
    }else if(func == sll){
        Sll(rt,rd,shamt);
    }else if(func == srl){
        Srl(rt,rd,shamt);
    }else if(func == sra){
        Sra(rt,rd,shamt);
    }
    PC = PC+4;
    if(func == jr){
        PC = registers[rs];
    }
}
void Jtype(unsigned int b){
    ads = b%(int)(Pow(2,26)+0.01);
    PC = PC+4;
    if(op == 0x03){
        registers[31] = PC;
    }
    PC = (PC)/(int)(Pow(2,28)+0.01)*(int)(Pow(2,28)+0.01) + ads*4;
}
void Itype(unsigned int b){
    if(op == 0x08){
        if(Addi(rs,rt,imm)<0){
            error_no = 1;
        }
    }else if(op == 0x08){
        int a = Addi(rt,rd,imm);
    }else if(op == 0x23){
        Lw(rs,rt,imm);
    }else if(op == 0x21){
        Lh(rs,rt,imm);
    }else if(op == 0x25){
        Lhu(rs,rt,imm);
    }else if(op == 0x20){
        Lb(rs,rt,imm);
    }else if(op == 0x24){
        Lbu(rs,rt,imm);
    }else if(op == 0x2B){
        Sw(rs,rt,imm);
    }else if(op == 0x29){
        Sh(rs,rt,imm);
    }else if(op == 0x28){
        Sb(rs,rt,imm);
    }else if(op == 0x0f){
        Lui(rt,imm);
    }else if(op == 0x0c){
        Andi(rs,rt,imm);
    }else if(op == 0x0d){
        Ori(rs,rt,imm);
    }else if(op == 0x0e){
        Nori(rs,rt,imm);
    }else if(op == 0x0a){
        Slti(rs,rt,imm);
    }
    PC = PC+4;
    if(op == 0x04){
        if(Cpr(rs,rt) == 0){
            PC = PC + imm*4;
        }
    }else if(op == 0x05){
        if(Cpr(rs,rt) != 0){
            PC = PC + imm*4;
        }
    }else if(op == 0x07){
        if(Cpr(rs,0) > 0){
            PC = PC + imm*4;
        }
    }
}
void decode(unsigned int b){

    func = b%64;
    int b1,b2,b3,b4;
    b1 = (int)(Pow(2,11)+0.01);
    b2 = (int)(Pow(2,16)+0.01);
    b3 = (int)(Pow(2,21)+0.01);
    b4 = (int)(Pow(2,26)+0.01);
    op = b/b4;
    imm = b%b2;
    shamt = (b%b1)/64;
    rs = (b%b4)/b3;
    rt = (b%b3)/b2;
    rd = (b%b2)/b1;
    if(op == 0){
        Rtype(b);
    }else if(op == 0x3F){
        end = 1;
    }else if(op == 0x02 || op == 0x03){
        Jtype(b);
    }else{
        Itype(b);
    }
    if(registers[0] != 0){
        error_w0 = 1;
        registers[0] = 0;
    }
}
int main () {
    FILE* iImage;
    FILE* dImage;
    FILE* snp;
    FILE* edp;
    unsigned int buffer;
    iImage = fopen ("iimage.bin","rb");
    dImage = fopen("dimage.bin","rb");
    snp = fopen("snapshot.rpt","w");
    edp = fopen("error_dump.rpt","w");
    int i=0;
    int j;

    while(fread(&buffer, 4, 1, dImage)){
        i++;
        if(i == 1){
            registers[sp] = rvs(buffer);
        }
        if(i == 2)Dnum = rvs(buffer);
        if(i > 2)Dm[i-3] = rvs(buffer);
    }
    i = 0;
    while(fread(&buffer, 4, 1, iImage)){
        i++;
        if(i > 256){
            error_ao = 1;
            break;
        }
        unsigned int db = rvs(buffer);
        if(i == 1){
            inPC = db;
            PC = db;
        }else if(i == 2){
            Inum = db;
        }else if(i > Inum+2){
            break;
        }else{
            Im[i-3] = db;
        }

    }
    int cycle = 0;
    fprintf(snp,"cycle %d\n",cycle);
    for(j = 0;j < 32;j++)fprintf(snp,"$%02d: 0x%08X\n",j,registers[j]);
    fprintf(snp,"PC: 0x%08X\n\n\n",PC);
    cycle++;
    for(i = 0;i < Inum;){
        decode(Im[i]);
        if(error() != 0){
            if(error_w0 == 1)fprintf(edp, "In cycle %d: Write $0 Error\n", cycle);
            if(error_ao == 1){
                fprintf(edp, "In cycle %d: Address Overflow\n", cycle);
                end = 1;
            }
            if(error_dm == 1){
                fprintf(edp, "In cycle %d: Misalignment Error\n", cycle);
                end = 1;
            }
            if(error_no == 1){
                fprintf(edp, "In cycle %d: Number Overflow\n", cycle);
            }
            error_w0 = 0;
            error_ao = 0;
            error_no = 0;
            error_dm = 0;
        }

        if(end)break;
        fprintf(snp,"cycle %d\n",cycle);
        for(j = 0;j < 32;j++){

            fprintf(snp,"$%02d: 0x%08X\n",j,registers[j]);
        }
        fprintf(snp,"PC: 0x%08X\n\n\n",PC);
        if(PC%4 != 0)error_dm = 1;
        i = (PC-inPC)/4;
        cycle++;
    }
    return 0;
}
