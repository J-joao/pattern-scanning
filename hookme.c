#include <stdio.h>

/*
    compile this file and open it using a disassembler, get the body of 
    both mysum functions and run the pattern scanner inside the executable's 
    directory
*/
int mysum(int a, int b)  {
    return a+b;
}

int mysum2(int a, int b) {
    return a+b;
}

void main(void) {
    mysum(1, 2);
    mysum2(1, 2);
}
