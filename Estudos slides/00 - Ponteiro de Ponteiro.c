#include <stdio.h>
#include <stdint.h>

int main(int argc, char* argv[]){ 

char** args = argv;

char*** pargs = &args; 

printf("main(%i, %s)\n", argc, argv[0]);

printf("main(%i, %s)\n", argc, (*pargs)[0]);

}