#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

unsigned char * memory;

unsigned int hexToDecimal(char * hexAddress){
    return strtol(hexAddress, NULL, 16);
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
            int l = 0;
            for(l = 0; l < size; l++){
                memory[l] = 0;
            }

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
            int opSize = i-x;

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
             for(k = address; k < address + (opSize/2); k++){
                printf("%02x",memory[k]);
            }
            printf("\n");

        }

    }



    return 0;
}
