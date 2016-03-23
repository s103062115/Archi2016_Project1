#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "instruction.h"
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
    func = b%64;
    int b1,b2,b3,b4;
    b1 = (int)(pow(2,11)+0.01);
    b2 = (int)(pow(2,16)+0.01);
    b3 = (int)(pow(2,21)+0.01);
    b4 = (int)(pow(2,26)+0.01);
    shamt = (b%b1)/64;
    rs = (b%b4)/b3;
    rt = (b%b3)/b2;
    rd = (b%b2)/b1;
    if(func == add){
        if(Add(rs,rt,rd) < 0){
            /**error**/
        }
    }else if(func == addu){
        int a = Add(rs,rt,rd);
    }else if(func == sub){
        if(Sub(rs,rt,rd) < 0){
            /**error**/
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
}
void decode(unsigned int b){
    if(b/(int)(pow(2,26)+0.01) == 0){
        Rtype(b);
    }
}
int main () {
    FILE* iImage;
    FILE* dImage;
    unsigned int PC,buffer;
    int b_buff[32] = {0};
    iImage = fopen ( "iimage.bin" , "rb" );
    dImage = fopen("dimage.bin","rb");
    int i=0;
    int j;
    int Inum;
    while(fread(&buffer, 4, 1, dImage)){
        i++;
        if(i == 1){
            registers[sp] = rvs(buffer);
        }
    }
    i = 0;
    while(fread(&buffer, 4, 1, iImage)){
        i++;
        unsigned int db = rvs(buffer);
        if(i == 1){
            PC = db;
        }else if(i == 2){
            Inum = db;
        }else if(i > Inum+2){
            break;
        }else{
            decode(buffer);
        }
        for(j = 0;j < 32;j++)printf("%d %x\n",j,registers[j]);
    }

    return 0;
}
