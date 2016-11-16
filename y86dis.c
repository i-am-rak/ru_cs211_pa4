#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

int main(int argc, char ** argv){
    if(argv[1][0] == '-' && argv[1][1] == 'h'){
        printf("y86dis [-h] <y86 input file> \n Input is a *.y86 file");
    }

    FILE *fp;
    //FILE *op;

    char buff[1000];
    char buff2[1000];
    
    fp = fopen(argv[1],"r");
    
    if(fp == NULL){
        printf("ERROR: <File not found>\n");
        return 0;
    }

    int i = 0;
    while(fgets(buff,1000, (FILE*)fp) != NULL){
        if(buff[0] == '.' && buff[1] == 't'  && buff[2] == 'e' && buff[3] == 'x' && buff[4] == 't'){
            buff2 = buff+5;
            //printf("%s", buff+5);
            break;
        }
    }

    fclose(fp);
    return 0;
}
