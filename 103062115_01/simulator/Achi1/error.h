#include<stdio.h>
int error_w0 = 0;
int error_no = 0;
int error_ao = 0;
int error_dm = 0;
int error(){
    return error_w0 || error_no || error_ao || error_dm;
}
