#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// considerando esta strutc 
typedef struct tenistas{
  //tenistas.txt
  int id, ano_nascimento, morte, rank, YoBK, numSem;
  char nome[51], pais[51];  //nome é nome + sobrenome
  //champions.txt
  int pontuacao;
  int anoGanhouTodosGrands; //Q(5)
  int TorneiosGanhos[15];
}TT;

void imprime_folha(const char* id_folha, int andar) {
    for (int j = 0; j < andar; j++) printf("\t");
    printf("Folha %s\n", id_folha);

    char arquivo_folha[51];
    sprintf(arquivo_folha, "infos/%s.bin", id_folha);

    FILE *fp_folha = fopen(arquivo_folha, "rb");
    if (!fp_folha) { 
        for(int j = 0; j < andar + 1; j++) printf("\t");
        printf("Erro ao abrir arquivo folha %s(linha 26(imprime))\n", arquivo_folha); 
        return; 
    }
    TT atleta_atual;
    while (fread(&atleta_atual, sizeof(TT), 1, fp_folha) == 1) {
        for (int j = 0; j < andar + 1; j++) printf("\t");
        printf("- %s (ID: %d)\n", atleta_atual.nome, atleta_atual.id);
    }
    fclose(fp_folha);
}

//  a ideia e ir tudo pra direita ate achar uma folha
void imprime_recursivo(FILE *fp_indice, int t, long pos_no, int andar){
    long tam_bloco = (sizeof(char) * 6) +          // id do no
                        sizeof(int) +             // numchaves
                        (sizeof(int) * (2 * t - 1)) + // chaves
                        (sizeof(char) * 6 * (2 * t)); // filhos
                        
    fseek(fp_indice, pos_no, SEEK_SET);
    char id_no[7] = {0};
    int num_chaves;
    if(fread(id_no, sizeof(char), 6, fp_indice) != 6){
        return; // se nao leu os 6 digitos do ID deu ruim ou fim do arq
    }    
    
    if(strncmp(id_no,"N",1) == 0){// se eh no interno    // antes(dando problema) -> strcmp(id_no[0], "N") == 0 
        fread(&num_chaves, sizeof(int), 1, fp_indice);
        int chaves[2 * t -1];
        char filhos_id[2 * t][7];
        fread(chaves, sizeof(int), (2 * t - 1), fp_indice);
        for(int i = 0; i < (2 * t); i++){
            fread(filhos_id[i], sizeof(char), 6, fp_indice);
            filhos_id[i][6] = '\0';
        }
        char* filho_mais_a_direita = filhos_id[num_chaves];
        //correcao na recursao
        if(strncmp(filho_mais_a_direita, "N", 1) == 0){
            long ultimo_filhos = atol(filho_mais_a_direita + 1);
            imprime_recursivo(fp_indice, t, ultimo_filhos * tam_bloco, andar + 1);
        }
         else if (strncmp(filho_mais_a_direita, "F", 1) == 0) {
            // Se for 'F', chama a função auxiliar para imprimir a folha
            imprime_folha(filho_mais_a_direita, andar + 1);
        }  
        
        // chaves 
        for(int i = num_chaves - 1; i >= 0; i--){
            for (int j = 0; j < andar; j++) printf("\t");
            printf("%d\n", chaves[i]);

            char *filho_atual = filhos_id[i];

            //aplicando a mesma correcao na recursao
            if (strncmp(filho_atual, "N", 1) == 0){
                long filho_no_mais_direita = atol(filho_atual + 1);
                imprime_recursivo(fp_indice, t, tam_bloco * filho_no_mais_direita, andar + 1);
            } else if (strncmp(filho_atual, "F", 1) == 0) {
                imprime_folha(filho_atual, andar + 1);
            }
        }
    }
    // mesma correcao dnv
    else if (strncmp(id_no, "F", 1) == 0) {
        imprime_folha(id_no, andar);
    }
}

void imprime(const char *arq_indice, int t){
    FILE *fp = fopen(arq_indice, "rb");
    if(!fp){
        printf("Erro ao abrir arquivo(linha 96(imprime))");
        return;
    }
    imprime_recursivo(fp, t, 0L, 0);
    fclose(fp);
} 



//teste para o imprime

TT TT_cria_vazio(){
  TT novo;
  //tenistas.txt
  novo.id = -1;
  novo.ano_nascimento = -1;
  novo.morte = -1;
  novo.rank = -1;
  novo.YoBK = -1;
  novo.numSem = -1;
  strcpy(novo.nome,"\0");
  strcpy(novo.pais,"\0");
  //champions.txt
  novo.pontuacao = 0;
  novo.anoGanhouTodosGrands=-1;
  for(int i = 0; i<15; i++) novo.TorneiosGanhos[i] = 0;
  return novo;
}

void Cria_indicesTeste(){
    FILE *fp = fopen("INDICES.bin","wb+");
    char nome[51];
    int id,num;

    //arq indice
    id = 20087967;
    num = 2;
    strcpy(nome,"N0000");
    fwrite(&nome,sizeof(char),6,fp);
    fwrite(&num,sizeof(int),1,fp);
    fwrite(&id,sizeof(int),1,fp);
    id = 2095611;
    fwrite(&id,sizeof(int),1,fp);
    id = -1;
    fwrite(&id,sizeof(int),1,fp);
    strcpy(nome,"N0001");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"N0002");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"N0003");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"\0");
    fwrite(&nome,sizeof(char),6,fp);

    id = 1035780;
    num = 1;
    strcpy(nome,"N0001");
    fwrite(&nome,sizeof(char),6,fp);
    fwrite(&num,sizeof(int),1,fp);
    fwrite(&id,sizeof(int),1,fp);
    id = -1;
    fwrite(&id,sizeof(int),1,fp);
    id = -1;
    fwrite(&id,sizeof(int),1,fp);
    strcpy(nome,"F0000");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"F0001");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"\0");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"\0");
    fwrite(&nome,sizeof(char),6,fp);

    id = 2025679;
    num = 1;
    strcpy(nome,"N0002");
    fwrite(&nome,sizeof(char),6,fp);
    fwrite(&num,sizeof(int),1,fp);
    fwrite(&id,sizeof(int),1,fp);
    id = -1;
    fwrite(&id,sizeof(int),1,fp);
    id = -1;
    fwrite(&id,sizeof(int),1,fp);
    strcpy(nome,"F0002");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"F0003");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"\0");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"\0");
    fwrite(&nome,sizeof(char),6,fp);

    id = 2205611;
    num = 1;
    strcpy(nome,"N0003");
    fwrite(&nome,sizeof(char),6,fp);
    fwrite(&num,sizeof(int),1,fp);
    fwrite(&id,sizeof(int),1,fp);
    id = -1;
    fwrite(&id,sizeof(int),1,fp);
    id = -1;
    fwrite(&id,sizeof(int),1,fp);
    strcpy(nome,"F0004");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"F0005");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"\0");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"\0");
    fwrite(&nome,sizeof(char),6,fp);

    num = 6;
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);


    //arqs folhas
    fp = fopen("./infos/F0000.bin","wb+");
    TT tenista = TT_cria_vazio();
    num = 3;
    strcpy(tenista.nome,"Emanuel");
    tenista.id = 1001010;
    fwrite(&tenista,sizeof(TT),1,fp);
    strcpy(tenista.nome,"Henrique");
    tenista.id = 1011011;
    fwrite(&tenista,sizeof(TT),1,fp);
    strcpy(tenista.nome,"Lucas");
    tenista.id = 1021010;
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("./infos/F0001.bin","wb+");
    num = 1;
    strcpy(tenista.nome,"Pedro");
    tenista.id = 1038888;
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("./infos/F0002.bin","wb+");
    num = 1;
    strcpy(tenista.nome,"Gustavo");
    tenista.id = 2015679;
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("./infos/F0003.bin","wb+");
    num = 1;
    strcpy(tenista.nome,"Joao Pedro");
    tenista.id = 2025679;
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("./infos/F0004.bin","wb+");
    num = 1;
    strcpy(tenista.nome,"Paulo");
    tenista.id = 2195611;
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("./infos/F0005.bin","wb+");
    num = 2;
    strcpy(tenista.nome,"Begona");
    tenista.id = 2205611;
    fwrite(&tenista,sizeof(TT),1,fp);
    strcpy(tenista.nome,"Paula");
    tenista.id = 2305611;
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);
}


int main(void){
    //Cria_indicesTeste();
    imprime("INDICES.bin",2);
}