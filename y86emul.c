#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

unsigned char * memory;
int IP;
int OF = 0;
int ZF = 0;
int SF = 0;

typedef union{
    struct {
        unsigned int b0 : 1;
        unsigned int b1 : 1;
        unsigned int b2 : 1;
        unsigned int b3 : 1;
        unsigned int b4 : 1;
        unsigned int b5 : 1;
        unsigned int b6 : 1;
        unsigned int b7 : 1;
    }codepart;

    unsigned char byte;
}regbyte;

regbyte registers[8][4];

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
    //dest[strlen(dest) + 1] = '\0';
}

void seperateToToge(char * dest, char * input){
    int i = 0;
    int k;
    for(k = 0; k< 8; k=k+2){
       char * tmp3 = malloc(4);
        strncpy(tmp3,input+k,2);
        tmp3[2] = '\0'; 
        dest[i] = hexToDecimal(tmp3);
    //printf("%d,%s\n",memory[i],tmp3);
        i++;
        free(tmp3);
    }
}

void getStrFromReg(char * dest, int i){
    int x = 0;
    char * tmpa = malloc(3);
    int count = 0;
    for(x = 0; x < 4; x++){
        sprintf(tmpa, "%02x", registers[i][x].byte);
        dest[count] = tmpa[0];
        dest[count+1] = tmpa[1];
        count = count+2;
    }
    dest[8] = '\0';
    free(tmpa);
}

void storeInReg(int regi, char * val){
    char * dest = malloc(strlen(val));
    seperateToToge(dest,val);

    int i = 0;
    for(i = 0; i < 4; i++){
        registers[regi][i].byte = dest[i];
    }
    getStrFromReg(dest,regi);
    fprintf(stdout, "\n%s\n", dest);
    free(dest);
}

void rrMovl(int a, int b){
    int i = 0;
    for(i = 0; i < 4; i++){
        registers[b][i] = registers[a][i];
    }
}

void rmMovl(int a, int b, char * val){
    //char dest[9] = getStrFromVal(b);
    char * orig = malloc(10);
    getStrFromReg(orig,b);
    char * rev = malloc(10);
    bigToLittleEndian(rev,orig);
    
    int addr  = hexToDecimal(rev);
    int displ = hexToDecimal(val);
    int i = 0;
    for(i = 0; i < 4; i++){
        memory[addr + displ + i] = registers[a][i].byte;
    }
    free(orig);
    free(rev);

}

void mrMovl(int a, int b, char * val){

    char * orig = malloc(10);
    getStrFromReg(orig, b);
    char * rev = malloc(10);
    bigToLittleEndian(rev,orig);

    int addr = hexToDecimal(rev);
    int displ = hexToDecimal(val);
    int i = 0;
    for(i = 0; i < 4; i++){
        registers[a][i].byte = memory[addr + displ];
    }
    free(orig);
    free(rev);
}

void movSBL(int a, int b, char * val){
    char * orig = malloc(10);
    getStrFromReg(orig, b);
    char * rev = malloc(10);
    bigToLittleEndian(rev,orig);

    int addr = hexToDecimal(rev);
    int displ = hexToDecimal(val);

    char bit = memory[addr+displ];
    char * tmp = malloc(4);
    sprintf(tmp, "%02x",bit);
    tmp[2] = '\0';
    char sign = hexToDecimal("ff");
    if(tmp[0] < '8'){
        sign = hexToDecimal("00");
    }
    registers[a][0].byte = sign;
    registers[a][1].byte = sign;
    registers[a][2].byte = sign;
    registers[a][3].byte = bit;
    free(tmp);
    free(rev);
    free(orig);
}

void doCmpl(int a, int b){
    char * rev = malloc(10);
    getStrFromReg(rev, a);
    char * val1str = malloc(10);
    bigToLittleEndian(val1str,rev);
    int val1 = hexToDecimal(val1str);
    char * val2str = malloc(10);
    getStrFromReg(rev, b);
    bigToLittleEndian(val2str,rev);
    int val2 = hexToDecimal(val2str);
    int tmpres = val1 - val2;

    //fprintf(stdout, "\n %d - %d = %d \n", val1, val2, tmpres);
    /*
    if(val1 < 0 && val2 > 0 && tmpres > 0){
        OF = 1;
    }
    else if(val1 > 0 && val2 < 0 && tmpres < 0){
        OF = 1;
    }
    else{
        OF = 0;
    }
*/
    OF = 0;
    if(tmpres == 0){
        ZF = 1;
    }
    else{
        ZF = 0;
    }

    if(tmpres < 0){
        SF = 1;
    }
    else{
        SF = 0;
    }

    free(val2str);
    free(val1str);
    free(rev);
}

void doJump(char * dest){
    fprintf(stdout, "\njmpcompleted\n");
    IP = hexToDecimal(dest);
}

void doJe(char * dest){
    if(ZF == 1){
        IP = hexToDecimal(dest);
    }
    else
        IP = IP + 5;
}

void doNoOp(char input){
    if(input = '0'){
        IP = IP + 1;
        fprintf(stdout,"nop\n");
    }
    else
        fprintf(stderr,"ERROR <wonky1>\n");
}

void doHalt(char input){
    if(input = '0'){
        fprintf(stdout,"halt\n");
    }
    else
        fprintf(stderr,"ERROR <wonky2>\n");
}

void dorrmovl(char input, char * bitval){

    int a = bitval[0] - '0';
    int b = bitval[1] - '0';
    if(input == '0'){
        rrMovl(a ,b);
        fprintf(stdout,"rrmovl r%c,r%c\n",bitval[0],bitval[1]);
    }
    else
        fprintf(stdout,"ERROR <wonky3>\n");
}

void doirmovl(char input,char * bitval){
    char * tmp = malloc(strlen(bitval) - 1);
    bigToLittleEndian(tmp,bitval+2);
    tmp[strlen(bitval)-2] = '\0'; 
   
    if(input == '0' && bitval[0] == 'f'){
        int regi = bitval[1] - '0';
        storeInReg(regi,bitval+2);
        fprintf(stdout,"irmovl 0x%s,r%c\n",tmp,bitval[1]);
        free(tmp);
    }
    else
        fprintf(stderr,"ERROR <wonky4>\n");

}

void dormmovl(char input,char * bitval){
    char * tmp = malloc(strlen(bitval) - 1);
    bigToLittleEndian(tmp,bitval+2);
    tmp[strlen(bitval)-2] = '\0';
    int a = bitval[0] - '0';
    int b = bitval[1] - '0';
    if(input == '0'){
        rmMovl(a,b,tmp);
        fprintf(stdout,"rmmovl r%c,0x%s(r%c)\n",bitval[0],tmp,bitval[1]);
        free(tmp);
    }
    else
        fprintf(stderr,"ERROR <wonky5>\n");
}

void domrmovl(char input,char * bitval){
    char * tmp = malloc(strlen(bitval) - 1);
    bigToLittleEndian(tmp,bitval+2);
    tmp[strlen(bitval)-2] = '\0';
    int a = bitval[0] - '0';
    int b = bitval[1] - '0';

    if(input == '0'){
        mrMovl(a,b,tmp);
        fprintf(stdout,"mrmovl 0x%s(r%c),r%c\n",tmp, bitval[0],bitval[1]);
    }
    else
        fprintf(stderr,"ERROR <wonky6>\n");
}

void doAdd(int a, int b){
    char * rev = malloc(10);
    getStrFromReg(rev, a);
    printf("%s\n",rev);
    
    char * val1str = malloc(10);
    bigToLittleEndian(val1str,rev);
    
    int val1 = hexToDecimal(val1str);
    char * val2str = malloc(10);
    
    getStrFromReg(rev, b);
    printf("%s\n",rev);
    bigToLittleEndian(val2str,rev);
    
    int val2 = hexToDecimal(val2str);
    int tmpres = val1 + val2;
    //printf("%d + %d = %d\n", val1, val2, tmpres);
    if(val1 > 0 && val2 > 0 && tmpres < 0){
        OF = 1;
    }
    else if(val1 < 0 && val2 < 0 && tmpres < 0){
        OF = 1;
    }
    else
        OF = 0;

    if(tmpres < 0)
        SF = 1;
    else
        SF = 0;

    if(tmpres == 0)
        ZF = 1;
    else
        ZF = 0;

    char * tmp = malloc(9);

    sprintf(tmp,"%08x",tmpres);
    
    bigToLittleEndian(rev,tmp);
    storeInReg(b,rev);

    free(val2str);
    free(val1str);
    free(rev);

}

void doOper(char input, char * bitval){
    int a = bitval[0] - '0';
    int b = bitval[1] - '0';

    switch(input){
        case '0':
            fprintf(stdout,"addl r%c,r%c\n", bitval[0], bitval[1]);
            doAdd(a,b);
            break;
        case '1':
            fprintf(stdout,"subl r%c,r%c\n", bitval[0], bitval[1]);
            break;
        case '2':
            fprintf(stdout,"andl r%c,r%c\n", bitval[0], bitval[1]);
            break;
        case '3':
            fprintf(stdout,"xorl r%c,r%c\n", bitval[0], bitval[1]);
            break;
        case '4':
            fprintf(stdout,"mull r%c,r%c\n", bitval[0], bitval[1]);
            break;
        case '5':
            fprintf(stdout,"cmpl r%c,r%c\n", bitval[0], bitval[1]);
            doCmpl(a,b);
            break;  
        default:  
            fprintf(stderr,"ERROR <wonky7>\n"); 
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
    switch(input){
        case '0':
            fprintf(stdout,"jmp 0x%s\n", tmp);
            doJump(tmp);
            //IP = IP + 5;
            free(tmp);
            break;
        case '1':
            fprintf(stdout,"jle 0x%s\n", tmp);
            IP = IP+5;
            free(tmp);
            break;
        case '2':
            fprintf(stdout,"jl 0x%s\n", tmp);
            IP = IP+5;          
            free(tmp);
            break;
        case '3':
            fprintf(stdout,"je 0x%s\n", tmp);
            doJe(tmp);
            //IP = IP + 5;
            free(tmp);
            break;
        case '4':
            fprintf(stdout,"jne 0x%s\n", tmp);
            IP = IP+5;
            free(tmp);
            break;
        case '5':
            fprintf(stdout,"jge 0x%s\n", tmp);
            IP = IP+5;
            free(tmp);
            break;
        case '6':
            fprintf(stdout,"jg 0x%s\n", tmp);
            IP = IP+5;
            free(tmp);
            break;
        default:
            fprintf(stderr,"ERROR <wonky8>\n"); 
            break;
    }
    free(dest);
}

void doCall(char input, char * bitval){
    char * dest = malloc(9);
    strncpy(dest, bitval,8);
    dest[8] = '\0';

    char * tmp = malloc(strlen(dest) + 1);
    bigToLittleEndian(tmp,dest);
    tmp[strlen(dest)] = '\0';

    if(input == '0'){
        fprintf(stdout,"call 0x%s\n", tmp);
    }
    else
        fprintf(stderr,"ERROR <wonky9>\n"); 
}

void doRet(char input){
    if(input == '0')
        fprintf(stdout,"ret\n");
    else
        fprintf(stderr,"ERROR <wonky10>\n"); 
}

void doPushl(char input, char * bitval){
    if(input == '0'&& bitval[1] == 'f'){
        fprintf(stdout,"pushl r%c\n",bitval[0]);
    }
    else
        fprintf(stderr,"ERROR <wonky11>\n");
}

void doPopl(char input, char * bitval){
    if(input == '0'&& bitval[1] == 'f'){
        fprintf(stdout,"popl r%c\n",bitval[0]);
    }
    else
        fprintf(stderr,"ERROR <wonky12>\n");
}

void doReadX(char input, char * bitval){
    char * tmp = malloc(strlen(bitval) - 1);
    bigToLittleEndian(tmp,bitval+2);
    tmp[strlen(bitval)-2] = '\0';

    if(input == '0' && bitval[1] == 'f'){
        fprintf(stdout,"readb 0x%s(r%c)\n",tmp,bitval[0]);
        free(tmp);
    }
    else if(input == '1' && bitval[1] == 'f'){
        fprintf(stdout,"readl 0x%s(r%c)\n",tmp,bitval[0]);
        free(tmp);
    }
     else
        fprintf(stderr,"ERROR <wonky13>\n");
}

void writeStuff(int a, char * val){
    char * orig = malloc(10);
    getStrFromReg(orig, a);
    char * rev = malloc(10);
    bigToLittleEndian(rev,orig);

    int addr = hexToDecimal(rev);
    int displ = hexToDecimal(val);

    fprintf(stderr,"%c",memory[addr + displ]);
    free(rev);
    free(orig);
}

void doWriteX(char input, char * bitval){
    char * tmp = malloc(strlen(bitval) - 1);
    bigToLittleEndian(tmp,bitval+2);
    tmp[strlen(bitval)-2] = '\0';

    int a = bitval[0] - '0';
    
    if(input == '0' && bitval[1] == 'f'){
        writeStuff(a,bitval+2);
        fprintf(stdout,"writeb 0x%s(r%c)\n",tmp,bitval[0]);
        free(tmp);
    }
    else if(input == '1' && bitval[1] == 'f'){
        fprintf(stdout,"writel 0x%s(r%c)\n",tmp,bitval[0]);
        free(tmp);
    }
    else
        fprintf(stderr,"ERROR <wonky14>\n");
}

void doMovsbl(char input, char * bitval){
    char * tmp = malloc(strlen(bitval) - 1);
    bigToLittleEndian(tmp,bitval+2);
    tmp[strlen(bitval)-2] = '\0';
    int a = bitval[0] - '0';
    int b = bitval[1] - '0';
    if(input == '0'){
        movSBL(a,b,bitval+2);
        fprintf(stdout,"movsbl 0x%s(r%c),r%c\n",tmp,bitval[1], bitval[0]);
        free(tmp);
    }
    else
        fprintf(stderr,"ERROR <wonky15>\n");
}

int main(int argc, char ** argv){

    FILE* fp;
    int bufferSize = 10000;
    char * buff = malloc(10000+1);

    fp = fopen(argv[1],"r");
    
    if(fp == NULL){
        fprintf(stderr,"ERROR: <File not found>\n");
        return 0;
    }

    int sizeFound = 0;
    int size;
    int opSize;
    int i = 5;
    while(fgets(buff,bufferSize, (FILE*)fp) != NULL){
        if(buff[0] == '.' && buff[1] == 's'  && buff[2] == 'i' && buff[3] == 'z' && buff[4] == 'e'){
            if(sizeFound == 1){
                fprintf(stderr,"ERROR: <more than one .size>");
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
            size = hexToDecimal(tmp);
            bufferSize = size;
            buff = realloc(buff,bufferSize);
            //printf("%d\n", size);
            memory = malloc(size);
            //int l = 0;
            //for(l = 0; l < size; l++){
            //    memory[l] = 0;
            //}

            fprintf(stdout,"Memory created, intialized to 0\n");

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

            char * tmp2 = malloc(10);;
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
            //free(tmp3);
            free(tmp2);

            fprintf(stdout,"Long Address 0x%08x (dec index): %d, Value : %d%d%d%d , %02x%02x%02x%02x\n", address, address, memory[address], memory[address+1], memory[address+2], memory[address+3], memory[address], memory[address+1], memory[address+2], memory[address+3]);
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

            char * tmp2 = malloc(4);
            strncpy(tmp2,buff+i,2);
            tmp2[2] = '\0';

            //printf("Long Address 0x%s (dec index): %d , Value : %s\n",tmp,address,tmp2);
            memory[address] = hexToDecimal(tmp2);

            fprintf(stdout,"Byte address 0x%08x (dec index): %d, Value : %d , %02x\n",address, address, memory[address], memory[address]);
            free(tmp2);
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

            fprintf(stdout,"\nText code 0x%08x (dec index): %d,-> Value : ", address, address);
             
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
            free(tmp2);
            free(tmp);
 
        }

    }
    free(buff);
    int line = 0;
    int notHalted = 1;
    //printf("%d%c\n",IP,memory[IP]);
    int insSize = (opSize/2) + IP;
    do{
        line++;
        char * tmp9 = malloc(3);
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
                notHalted = 0;
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
                fprintf(stdout,"ERROR: <Unrecognized opcode %c>\n", opcode);
                return 0;
                //doNoOp(fncode
        }
    //fprintf(stdout, "\nIP : %d , insSize : %d\n",IP,insSize);
    //free(tmp9);
    //free(bit);
    }
    while(notHalted && (IP < insSize));
    printf("\n");

    fprintf(stdout, "OF:%d SF:%d ZF:%d\n",OF,SF,ZF);

    return 0;

}
