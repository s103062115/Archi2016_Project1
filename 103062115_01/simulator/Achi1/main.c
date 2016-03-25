#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "instruction.h"
int end = 0;
unsigned int rvs(unsigned int buffer){
    unsigned int a = 0;
    int b1,b2,b3;
    b1 = (int)(pow(2,8)+0.01);
    b2 = (int)(pow(2,16)+0.01);
    b3 = (int)(pow(2,24)+0.01);
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
    ads = b%(int)(pow(2,26)+0.01);
    PC = PC+4;
    if(op == 0x03){
        registers[31] = PC;
    }
    PC = (PC)/(int)(pow(2,28)+0.01)*(int)(pow(2,28)+0.01) + ads*4;
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
    b1 = (int)(pow(2,11)+0.01);
    b2 = (int)(pow(2,16)+0.01);
    b3 = (int)(pow(2,21)+0.01);
    b4 = (int)(pow(2,26)+0.01);
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
            if(error_ao == 1)fprintf(edp, "In cycle %d: Address Overflow\n", cycle);
            if(error_dm == 1){
                fprintf(edp, "In cycle %d: Misalignment Error\n", cycle);
                end = 1;
            }
            if(error_no == 1){
                fprintf(edp, "In cycle %d: Number Overflow\n", cycle);
                end = 1;
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
