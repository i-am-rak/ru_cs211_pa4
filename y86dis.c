#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

void doNoOp(char input){
    if(input = '0'){
        printf("nop\n");
    }
    else
        printf("ERROR <wonky>\n");
}

void doHalt(char input){
    if(input = '0'){
        printf("halt\n");
    }
    else
        printf("ERROR <wonky>");
}

void dorrmovl(char input, char arg1, char arg2){
    if(input == '0'){
       printf("rrmovl r%c,r%c\n",arg1,arg2);
    }
    else
        printf("ERROR <wonky>\n");
}

void doirmovl(char input, char arg1, char arg2){
    if(input == '0' && arg1 == 'f'){
        printf("irmovl f,r%c\n",arg2);
    }
    else
        printf("ERROR <wonky>\n");

}

void dormmovl(char input, char arg1, char arg2){
    if(input == '0'){
       printf("rmmovl r%c,D(r%c)\n",arg1,arg2);
    }
    else
        printf("ERROR <wonky>\n");
}

void domrmovl(char input, char arg1, char arg2){
    if(input == '0'){
       printf("mrmovl D(r%c),r%c\n",arg1,arg2);
    }
    else
        printf("ERROR <wonky>\n");
}



int main(int argc, char ** argv){
    if(argv[1][0] == '-' && argv[1][1] == 'h'){
        printf("y86dis [-h] <y86 input file> \n Input is a *.y86 file");
    }

    FILE *fp;
    //FILE *op;

    char buff[10000];
    //char buff2[1000];

    fp = fopen(argv[1],"r");
    
    if(fp == NULL){
        printf("ERROR: <File not found>\n");
        return 0;
    }

    int textFound = 0;
    int i = 5;

    while(fgets(buff,10000, (FILE*)fp) != NULL){
        if(buff[0] == '.' && buff[1] == 't'  && buff[2] == 'e' && buff[3] == 'x' && buff[4] == 't'){
            //buff2 = buff;
            textFound = 1;
            //printf("%s", buff+5);
            break;
        }
    }

    if(textFound == 0){
        printf("ERROR: <.text not found>\n");
        return 0;
    }

    while(isspace(buff[i])){
        i++;
    }
    while(!(isspace(buff[i]))){
        i++;
    }

    while(isspace(buff[i])){
        i++;
    }

    //int x = strlen(buff);
    int size = strlen(buff);
    //printf("%d\n", size);
    //printf("%c ,%d,  Machine code %s",buff[i],x,buff + i);
    fclose(fp);

    int notHalted = 1;
    do{
        char opcode = buff[i];
        if(isspace(opcode)){
            return 0;
        }
        char fncode = buff[i+1];
        char argcode = 0;
        char arg2code = 0;
        //printf("%d",(size - i)); 
        if((size - i) > 2){
            argcode = buff[i+2];
            arg2code = buff[i+3];
        }

        switch(opcode){
            case '0':
                doNoOp(fncode);
                i = i+2;
                break;
            case '1':
                doHalt(fncode);
                i = i+2;
                notHalted = 0;
                break;
            case '2':
                dorrmovl(fncode,argcode,arg2code);
                i = i+4;
                break;
            case '3':
                doirmovl(fncode, argcode, arg2code);
                i = i+4;
                break;
            case '4':
                dormmovl(fncode, argcode, arg2code);
                i = i+4;
                break;
            case '5':
                domrmovl(fncode, argcode, arg2code);
                i = i+4;
                break;
            default:
                printf("ERROR: <Unrecognized opcode %c>\n", opcode);
                return 0;
                //doNoOp(fncode
        }
    
    }
    while(notHalted && ((i-1) < size));


    return 0;
}
