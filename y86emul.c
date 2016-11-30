#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>

static volatile int keepRunning = 1;

unsigned char * memory;
unsigned int IP;

int AOK = 0;
int size = 0;
struct {
    unsigned int OF : 1;
    unsigned int ZF : 1;
    unsigned int SF : 1;
} flags;

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

int hexToDecimal(char * hexAddress){
    //return strtol(hexAddress, NULL, 16);
    int x;
    sscanf(hexAddress,"%x", &x);
    return x;
}

void intHandler(int dummy) {
    if(AOK == 1)
        fprintf(stdout,"\nSignal interupt detected : Status Code AOK\n");
    else
        fprintf(stdout,"\nSignal interupt detected : Code not executed\n");
    keepRunning = 0;
    exit(1);
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
    char * tmp3 = calloc(1,4);
    for(k = 0; k< 8; k=k+2){
        strncpy(tmp3,input+k,2);
        tmp3[2] = '\0'; 
        dest[i] = hexToDecimal(tmp3);
    //printf("%d,%s\n",memory[i],tmp3);
        i++;
    }
    free(tmp3);
}

void getStrFromReg(char * dest, int i){
    
    int x = 0;
    char * tmpa = calloc(1,5);
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
    char * dest = calloc(1,strlen(val) + 1);
    seperateToToge(dest,val);

    int i = 0;
    for(i = 0; i < 4; i++){
        registers[regi][i].byte = dest[i];
    }
    //getStrFromReg(dest,regi);
    //fprintf(stdout, "\n%s\n", dest);
    free(dest);
}

void rrMovl(int a, int b){
    int i = 0;
    for(i = 0; i < 4; i++){
        registers[b][i].byte = registers[a][i].byte;
    }
}

void rmMovl(int a, int b, char * val){
    //char dest[9] = getStrFromVal(b);
    char * orig = calloc(1,10);
    getStrFromReg(orig,b);
    char * rev = calloc(1,10);
    bigToLittleEndian(rev,orig);
    
    int addr  = hexToDecimal(rev);
    int displ = hexToDecimal(val);
    //fprintf(stderr,"%d\n", displ);
    int i = 0;
    for(i = 0; i < 4; i++){
        if((addr + displ + i) > size || (addr + displ + i) < 0){
            printf("Status code: ADR. Program exiting.\n");
            exit(1);
        }
        memory[addr + displ + i] = registers[a][i].byte;
        //fprintf(stdout,"%c\n%c\n", memory[addr + displ + i] , registers[a][i].byte);
    }
    free(orig);
    free(rev);

}

void mrMovl(int a, int b, char * val){

    char * orig = calloc(1,10);
    getStrFromReg(orig, b);
    char * rev = calloc(1,10);
    bigToLittleEndian(rev,orig);

    int addr = hexToDecimal(rev);
    int displ = hexToDecimal(val);
    int i = 0;
    for(i = 0; i < 4; i++){
        if((addr + displ + i) > size || (addr + displ + i) < 0){
            fprintf(stderr,"Status code : ADR. Program exiting.\n");
            exit(1);
        }
        registers[a][i].byte = memory[addr + displ + i];
    }
    getStrFromReg(orig,4);
    //fprintf(stdout,"%s\n",orig);
    free(orig);
    free(rev);
}

void movSBL(int a, int b, char * val){
    char * orig = calloc(1,10);
    getStrFromReg(orig, b);
    char * rev = calloc(1,10);
    bigToLittleEndian(rev,orig);

    int addr = hexToDecimal(rev);
    int displ = hexToDecimal(val);
    if((addr + displ) > size || (addr + displ) < 0){
        fprintf(stderr, "Status code: ADR. Program exiting.\n");
        exit(1);
    }
    char bit = memory[addr+displ];
    char * tmp = calloc(1,4);
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

void doNoOp(char input){
    if(input == '0'){
        IP = IP + 1;
        //fprintf(stdout,"nop\n");
    }
    else{
        fprintf(stderr,"Status code : INS. Program exiting.\n");
        exit(1);
    }
}

void doHalt(char input){
    if(input == '0'){
        //fprintf(stdout,"halt\n");
    }
    else{
        fprintf(stderr,"Status code : INS. Program exiting.\n");
        exit(1);
    }
}

void dorrmovl(char input, char * bitval){

    int a = bitval[0] - '0';
    int b = bitval[1] - '0';
    if(input == '0'){
        rrMovl(a ,b);
        //fprintf(stdout,"rrmovl r%c,r%c\n",bitval[0],bitval[1]);
    }
    else{
        fprintf(stdout,"Status code : INS. Program exiting.\n");
        exit(1);   
    }
}
void doirmovl(char input,char * bitval){
    char * tmp = calloc(1,strlen(bitval) + 1);
    bigToLittleEndian(tmp,bitval+2);
    tmp[strlen(bitval)-2] = '\0'; 
   
    if(input == '0' && bitval[0] == 'f'){
        int regi = bitval[1] - '0';
        storeInReg(regi,bitval+2);
        //fprintf(stdout,"irmovl 0x%s,r%c\n",tmp,bitval[1]);
        free(tmp);
    }
    else{
        fprintf(stderr,"Status code : INS. Program exiting.\n");
        exit(1);
    }
}

void dormmovl(char input,char * bitval){
    char * tmp = calloc(1,strlen(bitval) + 1);
    bigToLittleEndian(tmp,bitval+2);
    tmp[strlen(bitval)-2] = '\0';
    int a = bitval[0] - '0';
    int b = bitval[1] - '0';
    
    if(input == '0'){
        rmMovl(a,b,tmp);
        //fprintf(stdout,"rmmovl r%c,0x%s(r%c)\n",bitval[0],tmp,bitval[1]);
        free(tmp);
    }
    else{
        fprintf(stderr,"Status code : INS. Program exiting.\n");
        exit(1);
    }
}
void domrmovl(char input,char * bitval){
    char * tmp = calloc(1,strlen(bitval) + 1);
    bigToLittleEndian(tmp,bitval+2);
    tmp[strlen(bitval)-2] = '\0';
    int a = bitval[0] - '0';
    int b = bitval[1] - '0';

    if(input == '0'){
        mrMovl(a,b,tmp);
        //fprintf(stdout,"mrmovl 0x%s(r%c),r%c\n",tmp, bitval[1],bitval[0]);
    }
    else{
        fprintf(stderr,"Status code : INS. Program exiting.\n");
        exit(1);
    }
}
void doAdd(int a, int b){
    
    char * rev = calloc(1,10);
    getStrFromReg(rev, a);
    //printf("%s\n",rev);
    
    char * val1str = calloc(1,10);
    bigToLittleEndian(val1str,rev);
    
    int val1 = hexToDecimal(val1str);
    char * val2str = calloc(1,10);
    
    getStrFromReg(rev, b);
    //printf("%s\n",rev);
    bigToLittleEndian(val2str,rev);
    
    int val2 = hexToDecimal(val2str);
    int tmpres = val1 + val2;
    //printf("%d + %d = %d\n", val1, val2, tmpres);
    if(val1 > 0 && val2 > 0 && tmpres < 0){
        flags.OF = 1;
    }
    else if(val1 < 0 && val2 < 0 && tmpres > 0){
        flags.OF = 1;
    }
    else
        flags.OF = 0;

    if(tmpres < 0)
        flags.SF = 1;
    else
        flags.SF = 0;

    if(tmpres == 0)
        flags.ZF = 1;
    else
        flags.ZF = 0;
    
    //fprintf(stdout, "OF:%d SF:%d ZF:%d\n",flags.OF,flags.SF,flags.ZF);


    char * tmp = calloc(1,9);

    sprintf(tmp,"%08x",tmpres);
    
    bigToLittleEndian(rev,tmp);
    storeInReg(b,rev);
    free(tmp);
    free(val2str);
    free(val1str);
    free(rev);

}

void doSubl(int a, int b){
    char * rev = calloc(1,10);
    getStrFromReg(rev, a);
    //printf("%s\n",rev);
    
    char * val1str = calloc(1,10);
    bigToLittleEndian(val1str,rev);
    
    int val1 = hexToDecimal(val1str);
    char * val2str = calloc(1,10);
    
    getStrFromReg(rev, b);
    //printf("%s\n",rev);
    bigToLittleEndian(val2str,rev);
    
    int val2 = hexToDecimal(val2str);
    int tmpres = val2 - val1;
    //printf("%d + %d = %d\n", val1, val2, tmpres); 
    
    if(val1 < 0 && val2 > 0 && tmpres > 0){
        flags.OF = 1;
    }
    else if(val1 > 0 && val2 < 0 && tmpres < 0){
        flags.OF = 1;
    }
    else{
        flags.OF = 0;
    }
    if(tmpres < 0)
        flags.SF = 1;
    else
        flags.SF = 0;

    if(tmpres == 0)
        flags.ZF = 1;
    else
        flags.ZF = 0;

    //fprintf(stdout, "OF:%d SF:%d ZF:%d\n",flags.OF,flags.SF,flags.ZF);


    char * tmp = calloc(1,9);

    sprintf(tmp,"%08x",tmpres);
    tmp[8] = '\0';
    bigToLittleEndian(rev,tmp);
    storeInReg(b,rev);

    free(tmp);
    free(val2str);
    free(val1str);
    free(rev);
}

void doAndl(int a, int b){

    int i = 0;
    for(i = 0; i < 4; i++){
        //printf("orig : %08x, %08x \n", registers[a][i].byte , registers[b][i].byte);
        registers[b][i].byte = registers[a][i].byte & registers[b][i].byte;
        //printf("val : %08x\n", registers[b][i].byte);
    }
    
    char * res = calloc(1,10);
    getStrFromReg(res, b);
     
    char * val1str = calloc(1,10);
    bigToLittleEndian(val1str,res);
    
    int val1 = hexToDecimal(val1str);

    flags.OF = 0;
    if(val1 < 0)
        flags.SF = 1;
    else
        flags.SF = 0;
    
    if(val1 == 0)
        flags.ZF = 1;
    else
        flags.ZF = 0; 
    //printf("%s\n",res);
    //fprintf(stdout, "OF:%d SF:%d ZF:%d\n",flags.OF,flags.SF,flags.ZF);

    free(val1str);
    free(res);

}

void doXorl(int a, int b){

    int i = 0;
    for(i = 0; i < 4; i++){
        registers[b][i].byte = registers[a][i].byte ^ registers[b][i].byte;
    }
    
    char * res = calloc(1,10);
    getStrFromReg(res, b);
     
    char * val1str = calloc(1,10);
    bigToLittleEndian(val1str,res);
    
    int val1 = hexToDecimal(val1str);

    flags.OF = 0;
    if(val1 < 0)
        flags.SF = 1;
    else
        flags.SF = 0;
    
    if(val1 == 0)
        flags.ZF = 1;
    else
        flags.ZF = 0; 
    //printf("%s\n",res);
    //fprintf(stdout, "OF:%d SF:%d ZF:%d\n",flags.OF,flags.SF,flags.ZF);
    
    free(val1str);
    free(res);

}

void doMull(int a, int b){
    char * rev = calloc(1,10);
    getStrFromReg(rev, a);
    //printf("%s\n",rev);
    
    char * val1str = calloc(1,10);
    bigToLittleEndian(val1str,rev);
    
    int val1 = hexToDecimal(val1str);
    char * val2str = calloc(1,10);
    
    getStrFromReg(rev, b);
    //printf("%s\n",rev);
    bigToLittleEndian(val2str,rev);
    
    int val2 = hexToDecimal(val2str);
    int tmpres = val1 * val2;
    //fprintf(stdout,"%d * %d = %d\n", val1, val2, tmpres);
    if(val1 > 0 && val2 > 0 && tmpres < 0){
        flags.OF = 1;
    }
    else if(val1 < 0 && val2 < 0 && tmpres < 0){
        flags.OF = 1;
    }
    else if(((val1 < 0 && val2 > 0)||(val1 > 0 && val2 < 0)) && tmpres > 0){
        flags.OF = 1;
    }
    else
        flags.OF = 0;

    if(tmpres < 0)
        flags.SF = 1;
    else
        flags.SF = 0;

    if(tmpres == 0)
        flags.ZF = 1;
    else
        flags.ZF = 0;

    //fprintf(stdout, "OF:%d SF:%d ZF:%d\n",flags.OF,flags.SF,flags.ZF);

    char * tmp = calloc(1,9);

    sprintf(tmp,"%08x",tmpres);
    tmp[8] = '\0';
    bigToLittleEndian(rev,tmp);
    storeInReg(b,rev);

    free(tmp);
    free(val2str);
    free(val1str);
    free(rev);

}

void doCmpl(int a, int b){
    char * rev = calloc(1,10);
    getStrFromReg(rev, a);
    char * val1str = calloc(1,10);
    bigToLittleEndian(val1str,rev);
    int val1 = hexToDecimal(val1str);
    char * val2str = calloc(1,10);
    getStrFromReg(rev, b);
    bigToLittleEndian(val2str,rev);
    int val2 = hexToDecimal(val2str);
    int tmpres = val1 - val2;

    flags.OF = 0;
    if(tmpres == 0){
        flags.ZF = 1;
    }
    else{
        flags.ZF = 0;
    }

    if(tmpres < 0){
        flags.SF = 1;
    }
    else{
        flags.SF = 0;
    }

    //fprintf(stdout, "OF:%d SF:%d ZF:%d\n",flags.OF,flags.SF,flags.ZF);

    free(val2str);
    free(val1str);
    free(rev);
}


void doOper(char input, char * bitval){
    int a = bitval[0] - '0';
    int b = bitval[1] - '0';

    switch(input){
        case '0':
            //fprintf(stdout,"addl r%c,r%c\n", bitval[0], bitval[1]);
            doAdd(a,b);
            break;
        case '1':
            //fprintf(stdout,"subl r%c,r%c\n", bitval[0], bitval[1]);
            doSubl(a,b);
            break;
        case '2':
            //fprintf(stdout,"andl r%c,r%c\n", bitval[0], bitval[1]);
            doAndl(a,b);
            break;
        case '3':
            //fprintf(stdout,"xorl r%c,r%c\n", bitval[0], bitval[1]);
            doXorl(a,b);
            break;
        case '4':
            //fprintf(stdout,"mull r%c,r%c\n", bitval[0], bitval[1]);
            doMull(a,b);
            break;
        case '5':
            //fprintf(stdout,"cmpl r%c,r%c\n", bitval[0], bitval[1]);
            doCmpl(a,b);
            break;  
        default:  
            fprintf(stderr,"Status code : INS. Program exiting.\n"); 
            exit(1);
    }
}

void doJump(char * dest){
    IP = hexToDecimal(dest);
}

void doJle(char * dest){
    //fprintf(stdout, "OF:%d SF:%d ZF:%d\n",flags.OF,flags.SF,flags.ZF);


    if((flags.SF ^ flags.OF) || flags.ZF){
        IP = hexToDecimal(dest);
    }
    else
        IP = IP + 5;
}

void doJl(char * dest){
    //fprintf(stdout, "OF:%d SF:%d ZF:%d\n",flags.OF,flags.SF,flags.ZF);


    if(flags.SF ^ flags.OF){
        IP = hexToDecimal(dest);
    }
    else
        IP = IP + 5;
}

void doJe(char * dest){
    //fprintf(stdout, "OF:%d SF:%d ZF:%d\n",flags.OF,flags.SF,flags.ZF);


    if(flags.ZF == 1){
        IP = hexToDecimal(dest);
    }
    else
        IP = IP + 5;
}

void doJne(char * dest){
    //fprintf(stdout, "OF:%d SF:%d ZF:%d\n",flags.OF,flags.SF,flags.ZF);


    if(flags.ZF != 1){
        IP = hexToDecimal(dest);
    }
    else
        IP = IP + 5;
}

void doJge(char * dest){
   //fprintf(stdout, "OF:%d SF:%d ZF:%d\n",flags.OF,flags.SF,flags.ZF);

    if(!(flags.SF ^ flags.OF)){
        IP = hexToDecimal(dest);
    }
    else
        IP = IP + 5;
}

void doJg(char * dest){
    //fprintf(stdout, "OF:%d SF:%d ZF:%d\n",flags.OF,flags.SF,flags.ZF);


    if(!(flags.SF^flags.OF) && !flags.ZF){
        IP = hexToDecimal(dest);
    }
    else
        IP = IP + 5;
}

void doJmp(char input, char * bitval){
    
    char * dest = calloc(1,9);
    strncpy(dest, bitval,8);
    dest[8] = '\0';

    char * tmp = calloc(1,strlen(dest) + 1);
    bigToLittleEndian(tmp,dest);
    tmp[strlen(dest)] = '\0';
    switch(input){
        case '0':
            //fprintf(stdout,"jmp 0x%s\n", tmp);
            doJump(tmp);
            //IP = IP + 5;
            free(tmp);
            break;
        case '1':
            //fprintf(stdout,"jle 0x%s\n", tmp);
            doJle(tmp);
            free(tmp);
            break;
        case '2':
            //fprintf(stdout,"jl 0x%s\n", tmp);
            doJl(tmp);         
            free(tmp);
            break;
        case '3':
            //fprintf(stdout,"je 0x%s\n", tmp);
            doJe(tmp);
            //IP = IP + 5;
            free(tmp);
            break;
        case '4':
            //fprintf(stdout,"jne 0x%s\n", tmp);
            doJne(tmp);
            free(tmp);
            break;
        case '5':
            //fprintf(stdout,"jge 0x%s\n", tmp);
            doJge(tmp);
            free(tmp);
            break;
        case '6':
            //fprintf(stdout,"jg 0x%s\n", tmp);
            doJg(tmp);
            free(tmp);
            break;
        default:
            fprintf(stderr,"Status Code : INS. Program exiting.\n");
            exit(1); 
    }
    free(dest);
}

void call(char * val){
    char * rev = calloc(1,10);
    char * toge = calloc(1,10);
      
    getStrFromReg(rev, 4);
    char * val2str = calloc(1,10);
    bigToLittleEndian(val2str,rev);
    int espa = hexToDecimal(val2str);
    espa = espa - 4;
 
    char * tmp = calloc(1,10);
    int pf = IP+5;
    sprintf(tmp,"%08x",pf);
    bigToLittleEndian(rev,tmp);
    tmp[8] = '\0';
    seperateToToge(toge,rev);

    int i = 0;
    for(i = 0; i < 4; i++){
        if((espa + i) > size || (espa + i) < 0){
            fprintf(stderr, "Status code : ADR. Program exiting.\n");
            exit(1);
        }
        memory[espa + i] = toge[i];
    }
    
    //bigToLittleEndian(val2str,val);
    //fprintf(stdout,"%d\n",hexToDecimal(val));
    IP = hexToDecimal(val);

    char * tmp2 = calloc(1,10);
    sprintf(tmp2,"%08x",espa);
    tmp2[8] = '\0';
    bigToLittleEndian(rev,tmp2);
    storeInReg(4,rev);
    free(tmp2);
    free(tmp);
    free(val2str);
    free(toge);
    free(rev);
 
}

void doCall(char input, char * bitval){
    char * dest = calloc(1,9);
    strncpy(dest, bitval,8);
    dest[8] = '\0';

    char * tmp = calloc(1,strlen(dest) + 1);
    bigToLittleEndian(tmp,dest);
    tmp[strlen(dest)] = '\0';

    if(input == '0'){
        call(tmp);
        //fprintf(stdout,"call 0x%s\n", tmp);
    }
    else{
        fprintf(stderr,"Status code: INS. Program exiting.\n");
        exit(1);
    }
    free(tmp);
    free(dest);
}

void ret(){
    
    char * rev = calloc(1,10);
    
    getStrFromReg(rev, 4);
    rev[8] = '\0';
    char * val2str = calloc(1,10);
    bigToLittleEndian(val2str,rev);
    val2str[8] = '\0';
    int espa = hexToDecimal(val2str);
    //espa = espa+4; 
    unsigned char * returnval = calloc(1,5);

    int i = 0;
    for(i = 0; i < 4; i++){
        if((espa + i) > size || (espa + i) < 0){
            fprintf(stderr,"Status code : ADR. Program exiting.\n");
            exit(1);
        }
        returnval[i] = memory[espa + i];
    }

    returnval[4] = '\0';

    int x = 0;
    char * tmpa = calloc(1,5);
    char * dest = calloc(1,10);
    int count = 0;
    for(x = 0; x < 4; x++){
        sprintf(tmpa, "%02x", returnval[x]);
        dest[count] = tmpa[0];
        dest[count+1] = tmpa[1];
        count = count+2;
    }
    dest[8] = '\0';

    bigToLittleEndian(val2str,dest);
    //fprintf(stderr,"val: %s\n",val2str);
    //fprintf(stderr,"oct : %01x\n oct 2:%08x\n",395,511);
    IP = hexToDecimal(val2str);
    //fprintf(stdout,"\n-%d-\n", IP);

    espa = espa + 4;

    char * tmp = calloc(1,10);
    sprintf(tmp,"%08x",espa);
    tmp[8] = '\0';
    char * rev2 = calloc(1,10);
    bigToLittleEndian(rev2,tmp);
    storeInReg(4,rev2);
    free(rev2);
    free(tmp);
    free(dest);
    free(tmpa);
    free(returnval);
    free(val2str);
    free(rev); 
}

void doRet(char input){
    if(input == '0'){
        ret();
        //fprintf(stdout,"ret\n");
    }
    else{
        fprintf(stderr,"Status code : INS. Program exiting\n"); 
        exit(1);
    }
}
void pushl(int a){
    char * rev = calloc(1,10);
    
    getStrFromReg(rev, 4);
    rev[8] = '\0';
    char * val2str = calloc(1,10);
    bigToLittleEndian(val2str,rev);
    int espa = hexToDecimal(val2str);
    espa = espa - 4;
    
    int i = 0;
    for(i = 0; i < 4; i++){
        if((espa + i) > size || (espa + i) < 0){
            fprintf(stderr,"Status code : ADR. Program exiting.\n");
            exit(1);
        }
        memory[espa + i] = registers[a][i].byte;
    }

    char * tmp = calloc(1,10);
    sprintf(tmp,"%08x",espa);
    tmp[8] = '\0';
    bigToLittleEndian(rev,tmp);
    storeInReg(4,rev);
    free(tmp);
    free(val2str);
    free(rev);
}

void doPushl(char input, char * bitval){
    int a = bitval[0] - '0';
    if(input == '0'&& bitval[1] == 'f'){
        pushl(a);
        //fprintf(stdout,"pushl r%c\n",bitval[0]);
    }
    else{
        fprintf(stderr,"Status code : INS. Program exiting.\n");
        exit(1);
    }
}

void popl(int a){
    char * rev = calloc(1,10);
    
    getStrFromReg(rev, 4);
    rev[8] = '\0';
    char * val2str = calloc(1,10);
    bigToLittleEndian(val2str,rev);
    int espa = hexToDecimal(val2str);
   
    int i = 0;
    for(i = 0; i < 4; i++){
        if((espa + i) > size || (espa + i) < 0){
            fprintf(stderr,"Status code : ADR. Program exiting.\n");
            exit(1);
        }

        registers[a][i].byte = memory[espa + i];
    }

    espa = espa + 4;

    char * tmp = calloc(1,10);
    sprintf(tmp,"%08x",espa);
    
    bigToLittleEndian(rev,tmp);
    storeInReg(4,rev);
    free(tmp);
    free(val2str);
    free(rev);

}

void doPopl(char input, char * bitval){
    int a = bitval[0] - '0';
    if(input == '0'&& bitval[1] == 'f'){
        popl(a);
        //fprintf(stdout,"popl r%c\n",bitval[0]);
    }
    else{
        fprintf(stderr,"Status code : INS. Program exiting.\n");
        exit(1);
    }
}

void readb(int a, char * input){
    char * str = calloc(1,100);
    //fprintf(stdout,"test");
    scanf("%s", str);
    unsigned int val = atoi(str);
    unsigned char valy = val;
    //fprintf(stderr,"%s", str);
    if(str == 0 || val == 0){
        flags.ZF = 1;
    }
    else
        flags.ZF = 0;
    
    char * orig = calloc(1,10);
    getStrFromReg(orig,a);
    char * rev = calloc(1,10);
    bigToLittleEndian(rev,orig);
    //char * rev2 = malloc(10);
    //bigToLittleEndian(rev2,input);
    
    int addr  = hexToDecimal(rev);
    int displ = hexToDecimal(input);
       //fprintf(stderr,"\n");
    if((addr + displ) > size || (addr + displ) < 0){
            fprintf(stderr,"Status code : ADR. Program exiting.\n");
            exit(1);
        }
    
    memory[addr + displ] = valy;
   
    free(rev);
    free(orig);
    free(str);
}


void readl(int a, char * input){

    char * str = calloc(1,100);
    //fprintf(stdout,"test");
    scanf("%s", str);
    int val = atoi(str);
    //fprintf(stderr,"%s", str);
    if(str == 0 || val == 0){
        flags.ZF = 1;
    }
    else
        flags.ZF = 0;
    
    //fprintf(stderr, "-\n%d\n-", val);
    char * hexval = calloc(1,10);
    sprintf(hexval, "%08x", val);
    hexval[8] = '\0';
    char * hexToge = calloc(1,6);
    char * rev3 = calloc(1,10);
    bigToLittleEndian(rev3,hexval);
    rev3[8] = '\0';
    //fprintf(stderr,"%s\n", rev3);
    seperateToToge(hexToge,rev3);

    char * orig = calloc(1,10);
    getStrFromReg(orig,a);
    char * rev = calloc(1,10);
    bigToLittleEndian(rev,orig);
    //char * rev2 = malloc(10);
    //bigToLittleEndian(rev2,input);
    
    int addr  = hexToDecimal(rev);
    int displ = hexToDecimal(input);
    
    int i = 0;
    for(i = 0; i < 4; i++){
        //fprintf(stderr,"-%02x-",hexToge[i]);
        if((addr + displ + i) > size || (addr + displ + i) < 0){
            fprintf(stderr,"Status code : ADR. Program exiting.\n");
            exit(1);
        }

        memory[addr + displ + i] = hexToge[i];
    }
    //fprintf(stderr,"\n");
    free(rev);
    free(orig);
    free(rev3);
    free(hexToge);
    free(hexval);
    free(str);
}

void doReadX(char input, char * bitval){
    char * tmp = calloc(1, strlen(bitval) + 1);
    bigToLittleEndian(tmp,bitval+2);
    tmp[strlen(bitval)-2] = '\0';
    
    int a = bitval[0] - '0';

    if(input == '0' && bitval[1] == 'f'){
        readb(a,tmp);
        //fprintf(stdout,"readb 0x%s(r%c)\n",tmp,bitval[0]);
        free(tmp);
    }
    else if(input == '1' && bitval[1] == 'f'){
        //fprintf(stdout,"test");
        readl(a,tmp);
        //fprintf(stdout,"readl 0x%s(r%c)\n",tmp,bitval[0]);
        free(tmp);
    }
     else{
        fprintf(stderr,"Status code : INS. Program exiting.\n");
        exit(1);
     }
}

void writeb(int a, char * val){
    char * orig = calloc(1,10);
    getStrFromReg(orig, a);
    char * rev = calloc(1,10);
    bigToLittleEndian(rev,orig);

    int addr = hexToDecimal(rev);
    int displ = hexToDecimal(val);
    
    if((addr + displ) > size || (addr + displ) < 0){
            fprintf(stderr,"Status code : ADR. Program exiting.\n");
            exit(1);
        }

    fprintf(stdout,"%c",memory[addr + displ]);
    free(rev);
    free(orig);
}

void writel(int a, char * val){
    char * orig = calloc(1,10);
    getStrFromReg(orig, a);
    char * rev = calloc(1,10);
    bigToLittleEndian(rev,orig);

    int addr = hexToDecimal(rev);
    int displ = hexToDecimal(val);

    int i =0;
    char * tmpa = calloc(1,5);
    for(i = 0; i < 4; i++){
        if((addr + displ + i) > size || (addr + displ + i) < 0){
            fprintf(stderr,"Status code : ADR. Program exiting.\n");
            exit(1);
        }

        sprintf(tmpa,"%02x", memory[addr+displ + i]);
        rev[2 * i] = tmpa[0];
        rev[(2 * i) + 1] = tmpa[1];
    }
    
    bigToLittleEndian(orig,rev);
    int res = hexToDecimal(orig);
    fprintf(stdout,"%d",res);
    free(tmpa);
    free(rev);
    free(orig);
}

void doWriteX(char input, char * bitval){
    char * tmp = calloc(1,strlen(bitval) + 1);
    bigToLittleEndian(tmp,bitval+2);
    tmp[strlen(bitval)-2] = '\0';

    int a = bitval[0] - '0';
    
    if(input == '0' && bitval[1] == 'f'){
        writeb(a,tmp);
        //fprintf(stdout,"writeb 0x%s(r%c)\n",tmp,bitval[0]);
        free(tmp);
    }
    else if(input == '1' && bitval[1] == 'f'){
        writel(a,tmp);
        //fprintf(stdout,"writel 0x%s(r%c)\n",tmp,bitval[0]);
        free(tmp);
    }
    else{
        fprintf(stderr,"Status Code : INS. Program exiting.\n");
        exit(1);
    }
}

void doMovsbl(char input, char * bitval){
    char * tmp = calloc(1,strlen(bitval) + 1);
    bigToLittleEndian(tmp,bitval+2);
    tmp[strlen(bitval)-2] = '\0';
    int a = bitval[0] - '0';
    int b = bitval[1] - '0';
    if(input == '0'){
        movSBL(a,b,tmp);
        //fprintf(stdout,"movsbl 0x%s(r%c),r%c\n",tmp,bitval[1], bitval[0]);
        free(tmp);
    }
    else{
        fprintf(stderr,"Status code : INS. Program exiting.\n");
        exit(1);
    }
}

int main(int argc, char ** argv){
    
    signal(SIGINT, intHandler);
    while(keepRunning){

    if(argc != 2){
        fprintf(stderr,"ERROR: <Incorrect number of arguments,use -h for help>\n");
        return 0;
    } 
    else if(!strcmp(argv[1],"-h")){
        fprintf(stderr,"Usage: ./y86emul <File name>\n");
        return 0;
    }
    FILE* fp;
    int bufferSize = 10000;
    char * buff = calloc(1,10000+1);

    fp = fopen(argv[1],"r");
    
    if(fp == NULL){
        fprintf(stderr,"ERROR: <File not found>\n");
        free(buff);
        free(memory);
        //fclose(fp);
        return 0;
    }

    int sizeFound = 0;
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
            char * tmp = calloc(1,i-x + 2);
            strncpy(tmp,buff+x,i-x);
            tmp[i-x + 1] = '\0';
            size = hexToDecimal(tmp);
            bufferSize = size;
            buff = realloc(buff,bufferSize);
            //printf("%d\n", size);
            memory = calloc(1,size);
            int l = 0;
            for(l = 0; l < size; l++){
                memory[l] = 0;
            }

            //fprintf(stdout,"Memory created, intialized to 0\n");

            free(tmp);
            sizeFound = 1;
            
        }
        
        else if(buff[0] == '.' && buff[1] == 'l'  && buff[2] == 'o' && buff[3] == 'n' && buff[4] == 'g'){
            i = 5;
            //int strlent = strlen(buff);
            while(isspace(buff[i])){
                i++;
            }
            int x = i;
            while(!isspace(buff[i])){
                i++;
            }

            char * tmp = calloc(1,(i-x) + 2);
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

            char * tmp2 = calloc(1,10);
            int k = 0;
            for(k = 0; k < 8; k++){
                tmp2[k] = '0';
            }
            strncpy(tmp2+7-i+x,buff+x-1,i-x+1);
            tmp2[8] = '\0';
            
            //int longValue = hexToDecimal(tmp);

            //printf("Long Address 0x%s (dec index): %d , Value : %s\n",tmp,address,tmp2);
            i = address;
            char * tmp3 = calloc(1,3);
            for(k = address; k< address+ 8; k=k+2){
                strncpy(tmp3,tmp2-address+k,2); 
                tmp3[2] = '\0';
                memory[i] = hexToDecimal(tmp3);
                //printf("%d,%s\n",memory[i],tmp3);
                i++;
                //free(tmp3);
            }
            free(tmp3);
            free(tmp2);

            //fprintf(stdout,"Long Address 0x%08x (dec index): %d, Value : %d%d%d%d , %02x%02x%02x%02x\n", address, address, memory[address], memory[address+1], memory[address+2], memory[address+3], memory[address], memory[address+1], memory[address+2], memory[address+3]);
            free(tmp);
        }

        else if(buff[0] == '.' && buff[1] == 'b'  && buff[2] == 'y' && buff[3] == 't' && buff[4] == 'e'){
            i = 5;
            //int strlent = strlen(buff);
            while(isspace(buff[i])){
                i++;
            }
            int x = i;
            while(!isspace(buff[i])){
                i++;
            }

            char * tmp = calloc(1,(i-x) + 2);
            strncpy(tmp,buff+x,i-x);
            tmp[i-x + 1] = '\0';
            int address = hexToDecimal(tmp);
            
            while(isspace(buff[i])){
                i++;
            }

            char * tmp2 = calloc(1,4);
            strncpy(tmp2,buff+i,2);
            tmp2[2] = '\0';

            //printf("Long Address 0x%s (dec index): %d , Value : %s\n",tmp,address,tmp2);
            memory[address] = hexToDecimal(tmp2);

            //fprintf(stdout,"Byte address 0x%08x (dec index): %d, Value : %d , %02x\n",address, address, memory[address], memory[address]);
            free(tmp2);
            free(tmp);
        }

        else if(buff[0] == '.' && buff[1] == 't'  && buff[2] == 'e' && buff[3] == 'x' && buff[4] == 't'){
            i = 5;
            
            //int strlent = strlen(buff);
            while(isspace(buff[i])){
                i++;
            }
            
            int x = i;
            while(!isspace(buff[i])){
                i++;
            }

            char * tmp = calloc(1,(i-x) + 2);
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

            char * tmp2 = calloc(1,(i-x) + 2);
            strncpy(tmp2, buff+x, i-x);
            tmp2[i-x + 1] = '\0';
            //printf("\n%s\n",tmp2);
            opSize = i-x;

            //printf("\n%d\n",opSize);
            i = address;
            int k;
            char * tmp3 = calloc(1,3);
            for(k = address; k< address+ opSize; k=k+2){
                strncpy(tmp3,tmp2-address+k,2);
                tmp3[2] = '\0'; 
                memory[i] = hexToDecimal(tmp3);
                //printf("%d,%s\n",memory[i],tmp3);
                i++;
                //free(tmp3);
            }
            free(tmp3);

            //fprintf(stdout,"\nText code 0x%08x (dec index): %d,-> Value : ", address, address);
             
            /*for(k = address; k < address + (opSize/2); k++){
                printf("%d",memory[k]);
            }*/
            /*
            printf("\n");
            //printf("-%02x-\n",memory[address]);
            for(k = address; k < address + (opSize/2); ++k){
                printf("%02x",memory[k]);
            }
            printf("\n\n");
            */
            //printf("\n-%02x-\n",memory[address]);
            IP = address;
            //printf("\n-%c-\n",memory[address]);
            free(tmp2);
            free(tmp);
        }
        else if(buff[0] == '.' && buff[1] == 's'  && buff[2] == 't' && buff[3] == 'r' && buff[4] == 'i' && buff[5] == 'n' && buff[6] == 'g'){
            i = 7;
            
            //int strlent = strlen(buff);
            while(isspace(buff[i])){
                i++;
            }
            
            int x = i;
            while(!isspace(buff[i])){
                i++;
            }

            char * tmp = calloc(1,(i-x) + 2);
            strncpy(tmp,buff+x,i-x);
            tmp[i-x + 1] = '\0';
            int address = hexToDecimal(tmp);
            
            while(isspace(buff[i])){
                i++;
            }
            i++;
            x = i;
            while(buff[i] != '"'){
                //fprintf(stderr, "%c\n", buff[i]);
                i++;
            }
            i--;
            //printf("\n%s\n", buff+x);

            char * tmp2 = calloc(1,(i-x) + 2);
            strncpy(tmp2, buff+x, i-x+1);
            tmp2[i-x + 1] = '\0';
            //printf("\n%s\n",tmp2);
            int strsize = i-x+1;

            //printf("\n%d\n",opSize);
            i = address;
            int k;
            for(k = 0; k< strsize; k++){
                memory[i] = tmp2[k];
                i++;
            }


            //fprintf(stdout,"\nString 0x%08x (dec index): %d,-> Value : ", address, address);
/*
            for(i = address; i < address + strsize; i++){
                //fprintf(stdout, "%c", memory[i]);
            }
            //fprintf(stdout, "\n");
*/             
            free(tmp2);
            free(tmp);

        }

    }
    free(buff);
    fclose(fp);

    int line = 0;
    int notHalted = 1;
    //printf("%d%c\n",IP,memory[IP]);
    int insSize = (opSize/2) + IP;

    do{
        AOK = 1;
        if((IP) > size || (IP) < 0){
            fprintf(stderr,"Status code : ADR. Program exiting.\n");
            exit(1);
        }

        line++;
        char * tmp9 = calloc(1,3);
        sprintf(tmp9,"%02x",memory[IP]);
        char opcode = tmp9[0];
        //printf("*%c*\n",tmp9[0]);
        if(isspace(opcode)){
            return 0;
        }
        //printf("%d: ", line);
        
        char fncode = tmp9[1];
        //printf("*%c*\n",tmp9[1]);

        //char argcode = 0;
        //char arg2code = 0;
        char * bit = calloc(1,13);
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
        free(tmp9);
        
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
                break;
            case '9':
                doRet(fncode);
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
                //fprintf(stdout, "?????");
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
                fprintf(stderr,"Status code : INS. Program exiting.\n");
                exit(1);
                //doNoOp(fncode
        }
    free(bit);
    }
    
    while(notHalted);
    if(!notHalted){
        fprintf(stdout,"Status Code : HLT. Program exiting.\n");
    }

    free(memory);
    keepRunning = 0;
    }
    return 0;
}
