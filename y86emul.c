#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

unsigned char * memory;
int IP;
int OF;
int ZF;
int SF;

char registers[8];

unsigned int hexToDecimal(char * hexAddress){
    return strtol(hexAddress, NULL, 16);
}

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
    if(input = '0'){
        IP = IP + 1;
        printf("nop\n");
    }
    else
        printf("ERROR <wonky1>\n");
}

void doHalt(char input){
    if(input = '0'){
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
        printf("mrmovl 0x%s(r%c),r%c\n",tmp, bitval[0],bitval[1]);
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

    FILE* fp;
    char buff[10000];

    fp = fopen(argv[1],"r");
    
    if(fp == NULL){
        printf("ERROR: <File not found>\n");
        return 0;
    }

    int sizeFound = 0;
    int size;
    int opSize;
    int i = 5;
    while(fgets(buff,10000, (FILE*)fp) != NULL){
        if(buff[0] == '.' && buff[1] == 's'  && buff[2] == 'i' && buff[3] == 'z' && buff[4] == 'e'){
            if(sizeFound == 1){
                printf("ERROR: <more than one .size>");
                return 0;
            }
            i = 5;
            while(isspace(buff[i])){
                i++;
            }
            int x = i;
            while(!isspace(buff[i])){
                i++;
            }
            char * tmp = malloc(i-x + 2);
            strncpy(tmp,buff+x,i-x);
            tmp[i-x + 1] = '\0';
            size = atoi(tmp);
            //printf("%d\n", size);
            memory = malloc(size);
            //int l = 0;
            //for(l = 0; l < size; l++){
            //    memory[l] = 0;
            //}

            printf("Memory created, intialized to 0\n");

            free(tmp);
            sizeFound = 1;
            
        }
        
        else if(buff[0] == '.' && buff[1] == 'l'  && buff[2] == 'o' && buff[3] == 'n' && buff[4] == 'g'){
            i = 5;
            int strlent = strlen(buff);
            while(isspace(buff[i])){
                i++;
            }
            int x = i;
            while(!isspace(buff[i])){
                i++;
            }

            char * tmp = malloc((i-x) + 2);
            strncpy(tmp,buff+x,i-x);
            tmp[i-x + 1] = '\0';
            int address = hexToDecimal(tmp);
            
            while(isspace(buff[i])){
                i++;
            }
            x = i;
            while(!isspace(buff[i])){
                i++;
            }

            char tmp2[9];
            int k = 0;
            for(k = 0; k < 8; k++){
                tmp2[k] = '0';
            }
            strncpy(tmp2+7-i+x,buff+x-1,i-x+1);
            tmp2[8] = '\0';
            
            //int longValue = hexToDecimal(tmp);

            //printf("Long Address 0x%s (dec index): %d , Value : %s\n",tmp,address,tmp2);
            i = address;
            for(k = address; k< address+ 8; k=k+2){
                char * tmp3 = malloc(3);
                strncpy(tmp3,tmp2-address+k,2); 
                tmp3[2] = '\0';
                memory[i] = hexToDecimal(tmp3);
                //printf("%d,%s\n",memory[i],tmp3);
                i++;
                //free(tmp3);
            }

            printf("Long Address 0x%08x (dec index): %d, Value : %d%d%d%d , %02x%02x%02x%02x\n", address, address, memory[address], memory[address+1], memory[address+2], memory[address+3], memory[address], memory[address+1], memory[address+2], memory[address+3]);
            free(tmp);
        }

        else if(buff[0] == '.' && buff[1] == 'b'  && buff[2] == 'y' && buff[3] == 't' && buff[4] == 'e'){
            i = 5;
            int strlent = strlen(buff);
            while(isspace(buff[i])){
                i++;
            }
            int x = i;
            while(!isspace(buff[i])){
                i++;
            }

            char * tmp = malloc((i-x) + 2);
            strncpy(tmp,buff+x,i-x);
            tmp[i-x + 1] = '\0';
            int address = hexToDecimal(tmp);
            
            while(isspace(buff[i])){
                i++;
            }

            char tmp2[3];
            strncpy(tmp2,buff+i,2);
            tmp2[2] = '\0';

            //printf("Long Address 0x%s (dec index): %d , Value : %s\n",tmp,address,tmp2);
            memory[address] = hexToDecimal(tmp2);

            printf("Byte address 0x%08x (dec index): %d, Value : %d , %02x\n",address, address, memory[address], memory[address]);
            free(tmp);
        }

        else if(buff[0] == '.' && buff[1] == 't'  && buff[2] == 'e' && buff[3] == 'x' && buff[4] == 't'){
            i = 5;
            
            int strlent = strlen(buff);
            while(isspace(buff[i])){
                i++;
            }
            
            int x = i;
            while(!isspace(buff[i])){
                i++;
            }

            char * tmp = malloc((i-x) + 2);
            strncpy(tmp,buff+x,i-x);
            tmp[i-x + 1] = '\0';
            int address = hexToDecimal(tmp);
            
            while(isspace(buff[i])){
                i++;
            }
            x = i;
            while(!isspace(buff[i])){
                i++;
            }
            //printf("\n%s\n", buff+x);

            char * tmp2 = malloc((i-x) + 2);
            strncpy(tmp2, buff+x, i-x);
            tmp2[i-x + 1] = '\0';
            //printf("\n%s\n",tmp2);
            opSize = i-x;

            //printf("\n%d\n",opSize);
            i = address;
            int k;
            for(k = address; k< address+ opSize; k=k+2){
                char * tmp3 = malloc(3);
                strncpy(tmp3,tmp2-address+k,2);
                tmp3[2] = '\0'; 
                memory[i] = hexToDecimal(tmp3);
                //printf("%d,%s\n",memory[i],tmp3);
                i++;
                //free(tmp3);
            }

            printf("\nText code 0x%08x (dec index): %d,-> Value : ", address, address);
             
            /*for(k = address; k < address + (opSize/2); k++){
                printf("%d",memory[k]);
            }*/
            printf("\n");
            //printf("-%02x-\n",memory[address]);
            for(k = address; k < address + (opSize/2); ++k){
                printf("%02x",memory[k]);
            }
            printf("\n\n");
            //printf("\n-%02x-\n",memory[address]);
            IP = address;
            //printf("\n-%c-\n",memory[address]);
 
        }

    }

    int line = 0;
    int notHalted = 1;
    //printf("%d%c\n",IP,memory[IP]);
    int insSize = (opSize/2) + IP;
    do{
        line++;
        char tmp9[2];
        sprintf(tmp9,"%02x",memory[IP]);
        char opcode = tmp9[0];
        //printf("*%c*\n",tmp9[0]);
        if(isspace(opcode)){
            return 0;
        }
        printf("%d: ", line);
        char fncode = tmp9[1];
        //printf("*%c*\n",tmp9[1]);

        //char argcode = 0;
        //char arg2code = 0;
        char * bit = malloc(13);
        int j = 0;
        for(j =0; j< 13; j++){
            bit[j] = '\0';
        }
        //printf("%d",(size - i)); 
        if((insSize - IP) > 1){
            sprintf(tmp9,"%02x",memory[IP+1]);
            //printf("*%c*\n",tmp9[0]);
            //printf("*%c*\n",tmp9[1]);
            bit[0] = tmp9[0];
            bit[1] = tmp9[1];
        }
        if((insSize - IP) > 2){
            sprintf(tmp9,"%02x",memory[IP+2]);
            //printf("*%c*\n",tmp9[0]);
            //printf("*%c*\n",tmp9[1]);
            bit[2] = tmp9[0];
            bit[3] = tmp9[1];

            if((insSize - IP) > 3){
                sprintf(tmp9,"%02x",memory[IP+3]);
                //printf("*%c*\n",tmp9[0]);
                //printf("*%c*\n",tmp9[1]);
                bit[4] = tmp9[0];
                bit[5] = tmp9[1];
            
                if((insSize - IP) > 4){
                    sprintf(tmp9,"%02x",memory[IP+4]);
                    //printf("*%c*\n",tmp9[0]);
                    //printf("*%c*\n",tmp9[1]);
                    bit[6] = tmp9[0];
                    bit[7] = tmp9[1];
            
                    if((insSize - IP) > 5){
                        sprintf(tmp9,"%02x",memory[IP+5]);
//                        printf("*%c*\n",tmp9[0]);
//                        printf("*%c*\n",tmp9[1]);
                        bit[8] = tmp9[0];
                        bit[9] = tmp9[1];
                    }
                }
            }
        } 
        
        switch(opcode){
            case '0':
                doNoOp(fncode);
                break;
            case '1':
                doHalt(fncode);
                IP = IP+1;
                //notHalted = 0;
                break;
            case '2':
                dorrmovl(fncode,bit);
                IP = IP+2;
                break;
            case '3':
                doirmovl(fncode,bit);
                IP = IP+6;
                break;
            case '4':
                dormmovl(fncode,bit);
                IP = IP+6;
                break;
            case '5':
                domrmovl(fncode,bit);
                IP = IP+6;
                break;
            case '6':
                doOper(fncode, bit);
                IP = IP+2;
                break;
            case '7':
                doJmp(fncode, bit);
                IP = IP+5;
                break;
            case '8':
                doCall(fncode, bit);
                IP = IP+5;
                break;
            case '9':
                doRet(fncode);
                IP = IP+1;
                break;
            case 'a':
                doPushl(fncode, bit);
                IP = IP+2;
                break;
            case 'b':
                doPopl(fncode, bit);
                IP = IP+2;
                break;
            case 'c':
                doReadX(fncode, bit);
                IP = IP+6;
                break;
            case 'd':
                doWriteX(fncode, bit);
                IP = IP+6;
                break;
            case 'e':
                doMovsbl(fncode, bit);
                IP = IP+6;
                break;
            default:
                printf("ERROR: <Unrecognized opcode %c>\n", opcode);
                return 0;
                //doNoOp(fncode
        }
    
    }
    while(notHalted && (IP < insSize));

    

    return 0;
}
