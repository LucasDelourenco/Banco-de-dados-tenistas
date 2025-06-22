#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void teste_strncmp(){
    char nome1[6];
    char nome2[6];
    strcpy(nome1,"N0101");
    strcpy(nome2,"N");
    if(strncmp(nome1,nome2,1)==0){
        printf("\n\neh um no\n");
    }
}

void teste_sprintf(){
    char nome[6],num[2];
    for(int i = 0; i< 5; i++){
        strcpy(nome,"N000");
        sprintf(num, "%d", i); //num = i como char;  (PASSANDO INT PARA CHAR)
        strcat(nome,num);
        printf("\n%s",nome);
    }
}

void teste_fseek(){
    //criando
    /*
    N0000
    N0001
    N0002
    N0003
    N0004
    */
   
    FILE *fp = fopen("./arqs teste/filetesteparafseek.bin","wb+");
    char nome[6], num[2];
    for(int i = 0; i< 5; i++){
        strcpy(nome,"N000");
        sprintf(num, "%d", i);
        strcat(nome,num);
        fwrite(&nome,sizeof(char),6,fp);
    }
    fclose(fp);
    /*
    //lendo
    FILE *fp = fopen("./arqs teste/filetesteparafseek.bin","rb+");
    char nome[6];
    fread(&nome,sizeof(char),6,fp);  //ATENCAO, a string que eu leio é N 0 0 0 0 \0, ou seja 6 caracteres
    printf("\nN0 -> %s",nome);
    int pos = ftell(fp);
    fseek(fp,pos+(sizeof(char) * 12),SEEK_SET); //funciona
    fread(&nome,sizeof(char),6,fp);
    printf("\nN3 -> %s",nome);
    fclose(fp);*/
}

/*
void teste_tamBlocoParaArqIndices(){
    FILE *fp = fopen("./arqs teste/filetesteparafseek.bin","wb+");
    char 
}*/

void teste_strcpy(){
    char nome[6];
    strcpy(nome,"N0000");
    strcpy(nome,&nome[1]);
    printf("%s",nome);
}

int main(void){
   teste_strcpy();
}