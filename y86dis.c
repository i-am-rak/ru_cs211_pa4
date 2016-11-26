#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

void bigToLittleEndian(char * dest, char * original){
    int i = strlen(original)-1;
    int k = 0;
    while(k < strlen(original)){
        dest[k] = original[i-1];
        dest[k+1] = original[i];
        k = k+2;
        i = i-2;
    }
}

void doNoOp(char input){
    if(input == '0'){
        //IP = IP + 1;
        printf("nop\n");
    }
    else
        printf("ERROR <wonky1>\n");
}

void doHalt(char input){
    if(input == '0'){
        printf("halt\n");
    }
    else
        printf("ERROR <wonky2>");
}

void dorrmovl(char input, char * bitval){
    if(input == '0'){
        printf("rrmovl r%c,r%c\n",bitval[0],bitval[1]);
    }
    else
        printf("ERROR <wonky3>\n");
}

void doirmovl(char input,char * bitval){
    char * tmp = malloc(strlen(bitval) - 1);
    bigToLittleEndian(tmp,bitval+2);
    tmp[strlen(bitval)-2] = '\0';

    if(input == '0' && bitval[0] == 'f'){
        printf("irmovl 0x%s,r%c\n",tmp,bitval[1]);
        free(tmp);
    }
    else
        printf("ERROR <wonky4>\n");

}

void dormmovl(char input,char * bitval){
    char * tmp = malloc(strlen(bitval) - 1);
    bigToLittleEndian(tmp,bitval+2);
    tmp[strlen(bitval)-2] = '\0';
   
    if(input == '0'){
        printf("rmmovl r%c,0x%s(r%c)\n",bitval[0],tmp,bitval[1]);
        free(tmp);
    }
    else
        printf("ERROR <wonky5>\n");
}

void domrmovl(char input,char * bitval){
    char * tmp = malloc(strlen(bitval) - 1);
    bigToLittleEndian(tmp,bitval+2);
    tmp[strlen(bitval)-2] = '\0';

    if(input == '0'){
        printf("mrmovl 0x%s(r%c),r%c\n",tmp, bitval[1],bitval[0]);
    }
    else
        printf("ERROR <wonky6>\n");
}

void doOper(char input, char * bitval){
    switch(input){
        case '0':
            printf("addl r%c,r%c\n", bitval[0], bitval[1]);
            break;
        case '1':
            printf("subl r%c,r%c\n", bitval[0], bitval[1]);
            break;
        case '2':
            printf("andl r%c,r%c\n", bitval[0], bitval[1]);
            break;
        case '3':
            printf("xorl r%c,r%c\n", bitval[0], bitval[1]);
            break;
        case '4':
            printf("mull r%c,r%c\n", bitval[0], bitval[1]);
            break;
        case '5':
            printf("cmpl r%c,r%c\n", bitval[0], bitval[1]);
            break;  
        default:  
            printf("ERROR <wonky7>\n"); 
            break;
    }
}


void doJmp(char input, char * bitval){
    
    char * dest = malloc(9);
    strncpy(dest, bitval,8);
    dest[8] = '\0';

    char * tmp = malloc(strlen(dest) + 1);
    bigToLittleEndian(tmp,dest);
    tmp[strlen(dest)] = '\0';
    free(dest);
    switch(input){
        case '0':
            printf("jmp 0x%s\n", tmp);
            free(tmp);
            break;
        case '1':
            printf("jle 0x%s\n", tmp);
            free(tmp);
            break;
        case '2':
            printf("jl 0x%s\n", tmp);
            free(tmp);
            break;
        case '3':
            printf("je 0x%s\n", tmp);
            free(tmp);
            break;
        case '4':
            printf("jne 0x%s\n", tmp);
            free(tmp);
            break;
        case '5':
            printf("jge 0x%s\n", tmp);
            free(tmp);
            break;
        case '6':
            printf("jg 0x%s\n", tmp);
            free(tmp);
            break;
        default:
            printf("ERROR <wonky8>\n"); 
            break;
    }
}

void doCall(char input, char * bitval){
    char * dest = malloc(9);
    strncpy(dest, bitval,8);
    dest[8] = '\0';

    char * tmp = malloc(strlen(dest) + 1);
    bigToLittleEndian(tmp,dest);
    tmp[strlen(dest)] = '\0';

    if(input == '0'){
        printf("call 0x%s\n", tmp);
    }
    else
        printf("ERROR <wonky9>\n"); 
}

void doRet(char input){
    if(input == '0')
        printf("ret\n");
    else
        printf("ERROR <wonky10>\n"); 
}

void doPushl(char input, char * bitval){
    if(input == '0'&& bitval[1] == 'f'){
        printf("pushl r%c\n",bitval[0]);
    }
    else
        printf("ERROR <wonky11>\n");
}

void doPopl(char input, char * bitval){
    if(input == '0'&& bitval[1] == 'f'){
        printf("popl r%c\n",bitval[0]);
    }
    else
        printf("ERROR <wonky12>\n");
}

void doReadX(char input, char * bitval){
    char * tmp = malloc(strlen(bitval) - 1);
    bigToLittleEndian(tmp,bitval+2);
    tmp[strlen(bitval)-2] = '\0';

    if(input == '0' && bitval[1] == 'f'){
        printf("readb 0x%s(r%c)\n",tmp,bitval[0]);
        free(tmp);
    }
    else if(input == '1' && bitval[1] == 'f'){
        printf("readl 0x%s(r%c)\n",tmp,bitval[0]);
        free(tmp);
    }
     else
        printf("ERROR <wonky13>\n");
}

void doWriteX(char input, char * bitval){
    char * tmp = malloc(strlen(bitval) - 1);
    bigToLittleEndian(tmp,bitval+2);
    tmp[strlen(bitval)-2] = '\0';

    
    if(input == '0' && bitval[1] == 'f'){
        printf("writeb 0x%s(r%c)\n",tmp,bitval[0]);
        free(tmp);
    }
    else if(input == '1' && bitval[1] == 'f'){
        printf("writel 0x%s(r%c)\n",tmp,bitval[0]);
        free(tmp);
    }
    else
        printf("ERROR <wonky14>\n");
}

void doMovsbl(char input, char * bitval){
    char * tmp = malloc(strlen(bitval) - 1);
    bigToLittleEndian(tmp,bitval+2);
    tmp[strlen(bitval)-2] = '\0';


    if(input == '0'){
        printf("movsbl 0x%s(r%c),r%c\n",tmp,bitval[1], bitval[0]);
        free(tmp);
    }
    else
        printf("ERROR <wonky15>\n");
}
int main(int argc, char ** argv){
    if(argv[1][0] == '-' && argv[1][1] == 'h'){
        printf("y86dis [-h] <y86 input file> \nInput is a *.y86 file\n");
        return 0;
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
    printf("\nMachine code : %s\n", buff+i);
    printf("Assembly code  : \n");
    //int x = strlen(buff);
    int size = strlen(buff);
    //printf("%d\n", size);
    //printf("%c ,%d,  Machine code %s",buff[i],x,buff + i);
    fclose(fp);
    int line = 0;
    int notHalted = 1;
    do{
        line++;
        char opcode = buff[i];
        if(isspace(opcode)){
            return 0;
        }
        printf("%d: ", line);
        char fncode = buff[i+1];
        //char argcode = 0;
        //char arg2code = 0;
        char * bit = malloc(13);
        int j = 0;
        for(j =0; j< 13; j++){
            bit[j] = '\0';
        }
        //printf("%d",(size - i)); 
        if((size - i) > 2){
            bit[0] = buff[i+2];
            bit[1] = buff[i+3];
        }
        if((size - i) > 4){
            bit[2] = buff[i+4];
            bit[3] = buff[i+5];

            if((size - i) > 6){
                bit[4] = buff[i+6];
                bit[5] = buff[i+7];
            
                if((size - i) > 8){
                    bit[6] = buff[i+8];
                    bit[7] = buff[i+9];
            
                    if((size - i) > 10){    
                        bit[8] = buff[i+10];
                        bit[9] = buff[i+11];
                    }
                }
            }
        } 
        
        switch(opcode){
            case '0':
                doNoOp(fncode);
                i = i+2;
                break;
            case '1':
                doHalt(fncode);
                i = i+2;
                //notHalted = 0;
                break;
            case '2':
                dorrmovl(fncode,bit);
                i = i+4;
                break;
            case '3':
                doirmovl(fncode,bit);
                i = i+12;
                break;
            case '4':
                dormmovl(fncode,bit);
                i = i+12;
                break;
            case '5':
                domrmovl(fncode,bit);
                i = i+12;
                break;
            case '6':
                doOper(fncode, bit);
                i = i+4;
                break;
            case '7':
                doJmp(fncode, bit);
                i = i+10;
                break;
            case '8':
                doCall(fncode, bit);
                i = i+10;
                break;
            case '9':
                doRet(fncode);
                i = i+2;
                break;
            case 'a':
                doPushl(fncode, bit);
                i = i+4;
                break;
            case 'b':
                doPopl(fncode, bit);
                i = i+4;
                break;
            case 'c':
                doReadX(fncode, bit);
                i = i+12;
                break;
            case 'd':
                doWriteX(fncode, bit);
                i = i+12;
                break;
            case 'e':
                doMovsbl(fncode, bit);
                i = i+12;
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
