#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "TH_MS.h"

/*
typedef struct tenistas{
  //tenistas.txt
  int id, ano_nascimento, morte, rank, YoBK, numSem;
  char nome[51], pais[51];  //nome é nome + sobrenome
  //champions.txt
  int pontuacao;
  int anoGanhouTodosGrands; //Q(5)
  int TorneiosGanhos[15];
}TT;*/

typedef struct nointerno{
  char rotulo[6];
  int nchaves;
  int *vet_chaves;
  char (*filhos)[6];
} NOINT;

typedef struct nofolha{
  char rotulo[6];
  TT *vet_tenista;
  int num_info;
} NOFO;

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
/*
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
}*/

NOFO* NOFO_cria(int t){
  NOFO *no = malloc(sizeof(NOFO));
  memset(no->rotulo, '\0', sizeof(char[6]));
  no->vet_tenista = (TT*)malloc(sizeof(TT)*(2*t - 1));
  no->num_info = 0;
  return no;
}

NOFO* NOFO_limpa(NOFO *no, int t){
  memset(no->rotulo, '\0', sizeof(char[6]));
  memset(no->vet_tenista, 0, sizeof(TT)*(2*t - 1));
  no->num_info = 0;
  return no;
}

NOINT* NOINT_cria(int t){
  NOINT *no = malloc(sizeof(NOINT));
  strcpy(no->rotulo, "\0\0\0\0\0\0");
  no->nchaves = 0;
  no->vet_chaves = (int*) malloc(sizeof(int) * (2*t - 1));
  for (int i=0;i<(2*t)-1;i++) no->vet_chaves[i] = -1;
  no->filhos = malloc(sizeof(char[6]) * (2*t));
  memset(no->filhos, '\0', sizeof(char[6]) * (2*t));
  return no; 
}

void gerar_nome_no(char *retorno, int num_nos_internos){
  sprintf(retorno, "N%04d", num_nos_internos);
}

void gerar_nome_folha(char *retorno, int num_folhas){
  sprintf(retorno, "F%04d", num_folhas);
}

void NOFO_libera(NOFO *no){
  free(no->vet_tenista);
  free(no);
}

void NOINT_libera(NOINT *no){
  free(no->vet_chaves);
  free(no->filhos);
  free(no);
}

void NOINT_imprime(NOINT *no, int t){
  int op;
  printf("Deseja imprimir o no %s?(Sim:0/Nao:1) ", no->rotulo);
  //scanf(" %d", &op);
  if (op) return;

  printf("\n");
  printf("%s\n%d\n", no->rotulo, no->nchaves);
  for(int i=0; i<no->nchaves;i++) printf("%d ", no->vet_chaves[i]);
  printf("\n");
  for(int i=0; i<(2*t);i++) printf("%s ", no->filhos[i]);
  printf("\n\n");
}

void NOFO_imprime(NOFO *no, int t){
  int op;
  printf("Deseja imprimir o no %s?(Sim:0/Nao:1) ", no->rotulo);
  //scanf(" %d", &op);
  if (op) return;

  printf("\n");
  printf("%s\n%d\n", no->rotulo, no->num_info);
  for(int i=0; i<no->num_info;i++) printf("%d ", no->vet_tenista[i].id);
  printf("\n\n");
}

int campo_vazio(char* campo) {
  int n = strlen(campo);
  for (int i = 0; i < n; i++) {
      if (campo[i] != '\0') return 0;
  }
  return 1;
}

int aumenta_um(char *nome_ent, char *destino) {
    if (campo_vazio(nome_ent)) return 0;

    char temp[6];
    strncpy(temp, nome_ent, 5);
    temp[5] = '\0';

    int num = atoi(&temp[1]) + 1;
    if (num > 9999) return 0;

    if (!(strcmp(&nome_ent[strlen(nome_ent) - 4], ".bin") == 0)) sprintf(destino, "%c%04d", temp[0], num);
    else sprintf(destino, "%c%04d.bin", temp[0], num);
    return 1;
}

int diminui_um(char *nome_ent, char *destino) {
    if (campo_vazio(nome_ent)) return 0;
    
    char temp[6];
    strncpy(temp, nome_ent, 5);
    temp[5] = '\0';

    int num = atoi(&temp[1]) - 1;
    if (num < 0) return 0;

    if (!(strcmp(&nome_ent[strlen(nome_ent) - 4], ".bin") == 0)) sprintf(destino, "%c%04d", temp[0], num);
    else sprintf(destino, "%c%04d.bin", temp[0], num);

    return 1;
}

long buscar_pos_no_int(char nome_no[6], int t){
    char aux[10];
    int N, tamPorBloco;
    tamPorBloco = (sizeof(char)*6 + sizeof(int) + sizeof(int)*((2*t)-1) + (sizeof(char)*6)*(2*t));
    strcpy(aux, &nome_no[1]);
    N = atoi(aux);
    return tamPorBloco * N;
}

long buscar_pos_no_folha(FILE *f_indice, char nome_no[6], int chave, int t){
    fseek(f_indice, 0L, SEEK_SET);
    int i;
    char filho_atual[6];
    NOINT *no = NOINT_cria(t);
    //printf("Buscando %s...\n", nome_no);
    while(1){
        i=0;
        fseek(f_indice, sizeof(char)*6, SEEK_CUR);
        if (fread(&no->nchaves, sizeof(int), 1, f_indice) != 1) printf("Falha na leitura\n");
        if (fread(no->vet_chaves, sizeof(int), (2*t)-1, f_indice) != (2*t)-1) printf("Falha na leitura\n");
        if (fread(no->filhos, sizeof(char), 6*2*t, f_indice) != 6*2*t) printf("Falha na leitura\n");

        while (i < no->nchaves && chave >= no->vet_chaves[i]){
          //printf("%d\n", no->vet_chaves[i]);
          i++;
        } 

        strcpy(filho_atual, no->filhos[i]);

        if (strcmp(filho_atual, nome_no) == 0){
          NOINT_libera(no);
          return ftell(f_indice) - sizeof(char)*6*(2*t-i);
        }
        else if (filho_atual[0] == 'N') fseek(f_indice, buscar_pos_no_int(filho_atual, t), SEEK_SET);
        else{
          printf("Erro na busca da folha!\n");
          break;
        }
    }
    NOINT_libera(no);
    return -1;
}

long buscar_pos_no(FILE *f_indice, char nome_no[6], int t){
  char aux[20];
  
  fseek(f_indice, 0L, SEEK_END); 
  if (nome_no[0] == 'N'){  //Se for no interno pula direto pro nó desejado
    return buscar_pos_no_int(nome_no, t);
  }
  else if (nome_no[0] == 'F'){  //Se for filho faz uma busca na arvore
    sprintf(aux, "./infos/%s.bin", nome_no);
    FILE *faux = fopen(aux, "rb");
    if (!faux){
      printf("buscar_pos_no: Falha ao abrir a folha %s", aux);
      return -1;
    }
    TT tenista;
    if (fread(&tenista, sizeof(TT), 1, faux) != 1){
      printf("buscar_pos_no: Falha ao ler o tenista\n");
      return -1;
    }
    fclose(faux);

    return buscar_pos_no_folha(f_indice, nome_no, tenista.id, t);
  }

  return -1; //O parametro passado está errado, esperava: "Nxxxx\0" ou "Fxxxx\0"
}


void escreve_no(FILE *file, NOINT *no, int t){
  fwrite(no->rotulo, sizeof(char), 6, file);
  fwrite(&no->nchaves, sizeof(int), 1, file);
  fwrite(no->vet_chaves, sizeof(int), (2*t)-1, file);
  fwrite(no->filhos, sizeof(char), 6*2*t, file);
}

void escreve_folha(FILE *file, NOFO *folha){
  fwrite(folha->vet_tenista, sizeof(TT), folha->num_info, file);
  fwrite(&folha->num_info, sizeof(int), 1, file);
}

int le_folha(FILE *file, NOFO **no){
  int falhou=0;
  long pont_arq;
  fseek(file, 0L, SEEK_END);
  pont_arq = ftell(file) - sizeof(int);
  fseek(file, pont_arq, SEEK_SET);
  if (fread(&(*no)->num_info, sizeof(int), 1, file) != 1){
    printf("le_folha - ERRO: Falha na leitura de num_tenista\n");
    falhou = 1;
  }
  fseek(file, 0L, SEEK_SET);
  if (fread((*no)->vet_tenista, sizeof(TT), (*no)->num_info, file) != (*no)->num_info) {
    printf("le_folha - ERRO: Falha na leitura do tenista\n");
    falhou = 1;
  }
  return falhou;
}

void div_cria_nosInt(FILE *f_ind, long pos_dividido, long tamBloco, int i, int t, NOINT **x, NOINT **y, NOINT **z){
    int j, num_nos_internos;
    long pont_aux;

    fseek(f_ind, 0L, SEEK_END);
    pont_aux = ftell(f_ind) - sizeof(int);
    num_nos_internos = pont_aux/tamBloco;

    //Preenche (*y) com as informalçoes
    fseek(f_ind, pos_dividido, SEEK_SET); //anda ate no pra ser dividido
    fread((*y)->rotulo, sizeof(char), 6, f_ind); //Le o rotulo Nxxxx/Fxxxx
    fread(&(*y)->nchaves, sizeof(int), 1, f_ind); //Le o int numero de chaves
    fread((*y)->vet_chaves, sizeof(int), (2*t)-1, f_ind); //Le as 2t-1 chaves do no que vai ser dividido
    fread((*y)->filhos, sizeof(char), 2*t*6, f_ind); //Le os 2t filhos

    (*y)->nchaves = t-1;
    (*z)->nchaves = t-1;
    
    for(j=0;j<t-1;j++){
      (*z)->vet_chaves[j] = (*y)->vet_chaves[j+t];
      (*y)->vet_chaves[j+t] = -1;
    }
    for(j=0;j<t;j++){
      strcpy((*z)->filhos[j], (*y)->filhos[j+t]);
      memset((*y)->filhos[j+t], '\0', sizeof(char[6]));
    }

    gerar_nome_no((*z)->rotulo, num_nos_internos);

    for (j=(*x)->nchaves; j>=i; j--){  //Move todos os filhos a direita da posição i e incrementa os seus rotulos/nomes em 1
      strcpy((*x)->filhos[j+1], (*x)->filhos[j]);
    }
    strcpy((*x)->filhos[i], (*z)->rotulo); // (*(*x)) recebe (*z) como filho de (*(*x)) na posição i que ficou vazia;
    
    for (j=(*x)->nchaves; j>=i; j--) (*x)->vet_chaves[j] = (*x)->vet_chaves[j-1]; //Move todas as chaves de (*(*x)) para a direita
    (*x)->vet_chaves[i-1] = (*y)->vet_chaves[t-1];  // A posição que ficou vazia recebe a chave de (*y) que vai subir 
    (*x)->nchaves++; 
}

void div_cria_folhas(FILE *f_ind, long pos_dividido, int i, int t, NOINT **x, NOFO **y, NOFO **z){
    char aux[20];
    int j;

    fseek(f_ind, pos_dividido, SEEK_SET);
    fread((*y)->rotulo, sizeof(char), 6, f_ind); //Le o nome da folha sendo dividida

    sprintf(aux, "./infos/%s.bin", (*y)->rotulo);
    FILE *fy = fopen(aux, "rb");

    aumenta_um((*y)->rotulo, (*z)->rotulo);

    if (!fy) return;

    j=0;
    while (j < (2*t)-1 && fread(&(*y)->vet_tenista[j], sizeof(TT), 1, fy) == 1) j++;

    //printf("%d\n", j);
    //le_folha(f_ind, y, t);  //Le as informações da folha para poder manipular
    fread(&(*y)->num_info, sizeof(int), 1, fy);  //Le o numero de tenistas do arquivo

    fclose(fy);

    for (j=(*x)->nchaves; j>=i; j--){
      aumenta_um((*x)->filhos[j], aux); 
      strcpy((*x)->filhos[j+1], aux);  //Move todos os filhos de (*x) 1 para a direita
    }
    strcpy((*x)->filhos[i], (*z)->rotulo); //Recebe z na posição i que ficou vazia
    for (j=(*x)->nchaves; j>=i; j--) (*x)->vet_chaves[j] = (*x)->vet_chaves[j-1]; //Move as chaves de (*x) para a direita
    (*x)->vet_chaves[i-1] = (*y)->vet_tenista[t-1].id; //Recebe a chave que subiu da folha
    (*x)->nchaves++;

    for (j=0;j<t;j++){
      (*z)->vet_tenista[j] = (*y)->vet_tenista[j+t-1]; //Passa as t chaves de y que estão a direita para z
      memset(&(*y)->vet_tenista[j+t-1], 0, sizeof(TT)); //Apaga a chave de y
    }

    (*y)->num_info = t-1; 
    (*z)->num_info = t;  //Como é folha a chave que sobe pro pai também vai pro nó dividido, por isso t chaves

    //printf("Sai da div_folhas\n");
}

void ajusta_folhas(FILE *f_indice, int nfolhas, int ponto_de_parada, int t){
    char aux[20], aux2[20], folha1[40], folha2[40];
    long pont_arq;
    int j;
    NOFO *no = NOFO_cria(t);
    FILE *ff;

    for(j = nfolhas-2; j>ponto_de_parada; j--){ //nfolhas-2: numero da ultima folha antes da divisao
      //printf("j:%d\n", j);
      gerar_nome_folha(aux, j);  //nome atual
      gerar_nome_folha(aux2, j+1);  //novo nome

      sprintf(folha1, "./infos/%s.bin", aux);
      sprintf(folha2, "./infos/%s.bin", aux2);

      pont_arq = buscar_pos_no(f_indice, aux, t);
      fseek(f_indice, pont_arq, SEEK_SET);
      fwrite(aux2, sizeof(char), 6, f_indice);

      //if (rename(folha1, folha2) != 0) printf("ERRO:Nao foi possivel renomear %s com o nome %s\n", folha1, folha2);
      ff = fopen(folha1, "rb");
      if (!ff){
        printf("ajusta_folhas: Erro ao abrir a folha - Leitura\n");
        NOFO_libera(no);
        return;
      }
      if (le_folha(ff, &no) != 0){
        NOFO_libera(no);
        return;
      }

      fclose(ff);
      remove(folha1);

      ff = fopen(folha2, "wb");
      if (!ff){
        printf("ajusta_folhas: Erro ao abrir a folha - Escrita\n");
        NOFO_libera(no);
        return;
      }
      escreve_folha(ff, no);
      no = NOFO_limpa(no, t);
      fclose(ff);

      //printf("Folha1: %s - Folha2: %s\n", folha1, folha2);
      //printf("Aux: %s - Aux2: %s\n\n", aux, aux2);
    }
    NOFO_libera(no);
}

void divisao_MS(char *indice, int i, long pos_pai, long pos_dividido, int t){

  //pos_pai: posicao onde o pai se encontra no arquivo indice
  //i: posicao onde o novo filho será inserido
  //pos_dividido: posicao do nó que será dividido

  FILE *f_indice = fopen(indice, "rb+");
  if (!f_indice){
      perror("divisao_MS:Falha em abrir o arquivo indice no modo de leitura.\n");
      return;
  }

  char no, folha1[40], folha2[40];
  int nfolhas, folha_y, j;
  long pont_aux;
  long tamBloco = (sizeof(char) * 6) +          // id do no
                  sizeof(int) +             // numchaves
                  (sizeof(int) * (2 * t - 1)) + // chaves
                  (sizeof(char) * 6 * (2 * t)); // filhos

  fseek(f_indice, pos_dividido, SEEK_SET); //anda ate o no que sera dividido
  if (fread(&no, sizeof(char), 1, f_indice) != 1) {
    printf("Erro ao ler tipo de nó\n");
    fclose(f_indice);
  return;
  }

  //printf("->%c<-\n", no);
  NOINT *x;
  x = NOINT_cria(t);
  if (!x){
    printf("Erro de alocacao!\n");
    return;
  }
  //Preenche x com as informaçoes
  fseek(f_indice, pos_pai, SEEK_SET); //anda ate no pai
  fread(x->rotulo, sizeof(char), 6, f_indice); //Le o rotulo Nxxxx
  fread(&x->nchaves, sizeof(int), 1, f_indice); //Le o int numero de chaves
  fread(x->vet_chaves, sizeof(int), (2*t)-1, f_indice); //Le as 2t-1 chaves do no que vai ser dividido
  fread(x->filhos, sizeof(char), 2*t*6, f_indice); //Le os 2t filhos

  if (no == 'N'){ //Caso o dividido seja no interno
    //printf("Caso no interno\n");
    NOINT *y, *z;

    y = NOINT_cria(t);
    z = NOINT_cria(t);
    if (!y || !z){
      printf("Erro de alocacao!\n");
      return;
    }

    div_cria_nosInt(f_indice, pos_dividido, tamBloco, i, t, &x, &y, &z);

    //Atualizacao do arquivo indice
    fseek(f_indice, pos_pai, SEEK_SET);
    escreve_no(f_indice, x, t);

    fseek(f_indice, pos_dividido, SEEK_SET);
    escreve_no(f_indice, y, t);

    fseek(f_indice, 0L, SEEK_END);
    pont_aux = ftell(f_indice) - sizeof(int);
    fseek(f_indice, pont_aux, SEEK_SET);
    fread(&nfolhas, sizeof(int), 1, f_indice);

    fseek(f_indice, pont_aux, SEEK_SET);

    escreve_no(f_indice, z, t);
    fwrite(&nfolhas, sizeof(int), 1, f_indice);

    fclose(f_indice);

    NOINT_libera(y);
    NOINT_libera(z);
  }
  else if (no == 'F'){ //Caso o dividido seja folha
    //printf("Caso Folha\n");
    
    NOFO *y, *z;

    y = NOFO_cria(t);
    z = NOFO_cria(t);

    if (!y || !z){
      printf("Erro de alocacao!\n");
      return;
    }

    div_cria_folhas(f_indice, pos_dividido, i, t, &x, &y, &z);

    fseek(f_indice, 0L, SEEK_END);
    pont_aux = ftell(f_indice) - sizeof(int);
    fseek(f_indice, pont_aux, SEEK_SET);
    fread(&nfolhas, sizeof(int), 1, f_indice);
    nfolhas++;
    fseek(f_indice, pont_aux, SEEK_SET);
    fwrite(&nfolhas, sizeof(int), 1, f_indice);

    folha_y = atoi(&y->rotulo[1]);

    //printf("Entrando no ajusta folhas\n");
    ajusta_folhas(f_indice, nfolhas, folha_y, t);
    //printf("Saindo do ajusta folhas\n");

    fseek(f_indice, pos_pai, SEEK_SET);
    escreve_no(f_indice, x, t);

    //Ajustar as folhas

    //Como vamos adicionar uma folha na nossa arvore
    //Vamos aumentar em 1 o valor de cada folha que esta depois da folha inserida
    //Ex: F0000 - F0001 - F0002     (Divide F0000, cria irmão z com nome F0001)
    //Move 1 pra direita: F0000 - F0002 - F0003
    //Insere z: F0000 - F0001 - F0002 - F0003

    sprintf(folha1, "./infos/%s.bin", y->rotulo);
    sprintf(folha2, "./infos/%s.bin", z->rotulo);

    //printf("Folha_y: %s, Folha_z: %s\n", folha1, folha2);

    FILE *fy = fopen(folha1, "wb"); 
    FILE *fz = fopen(folha2, "wb");

    for (j=0; j<t-1; j++) fwrite(&y->vet_tenista[j], sizeof(TT), 1, fy); //Escreve t-1 informações de y
    for (j=0; j<t; j++) fwrite(&z->vet_tenista[j], sizeof(TT), 1, fz); //Escreve t informações de z
        
    fwrite(&z->num_info, sizeof(int), 1, fz);
    fwrite(&y->num_info, sizeof(int), 1, fy);

    fclose(fz);
    fclose(fy);

    fclose(f_indice);

    NOFO_libera(y);
    NOFO_libera(z);
  }

  NOINT_libera(x);
}

void insere(const char *arq_indice, TT novo_atleta, int t);

int cmp(const void *a, const void *b) {
    TT *tenistaA = (TT *)a;
    TT *tenistaB = (TT *)b;
    return (tenistaA->id - tenistaB->id);
}

int num_atletas_na_folha(FILE *fp_folha){
    if (!fp_folha) return 0;

    fseek(fp_folha, 0L, SEEK_END);// vai pro final do arq 
    long tam = ftell(fp_folha);// e retorna o quanto andou

    if (tam < sizeof(int)) {
        rewind(fp_folha);
        return 0;
    }
    
    // vai pro final do arq
    fseek(fp_folha, 0L, SEEK_END);
    long fim = ftell(fp_folha) - sizeof(int);
    fseek(fp_folha, fim, SEEK_SET);
    int n = 0;
    if (fread(&n, sizeof(int), 1, fp_folha) != 1){ // le a qntd de atletas
        rewind(fp_folha);
        printf("Erro ao ler a qntd de atletas na folha antiga\n");
        return 0; 
    }
    rewind(fp_folha);
    return n;
}

void insere_na_folha(const char *arq_indice, const char *arq_folha, TT novo_atleta, long pos_pai, int i, int t){ //adicionado parametro int t
    FILE *fp = fopen(arq_folha, "rb");
    if (!fp) {
        printf("Erro ao abrir o arquivo original para leitura(insere_na_folha)\n");
        return;
    }
    long tam_bloco = (sizeof(char) * 6) +          // rotulo
                    sizeof(int) +             // nchaves
                    (sizeof(int) * (2 * t - 1)) + // chaves
                    (sizeof(char) * 6 * (2 * t)); // filhos
    
    int num_atletas = num_atletas_na_folha(fp);
    
    if(num_atletas >= (2 * t - 1)){
        //printf("\nFolha %s cheia. Chamando divisao...\n", arq_folha);
        fclose(fp);

        // Calcula a posiaoo do ponteiro para a folha a ser dividida.
        long offset_filhos = (sizeof(char) * 6) + sizeof(int) + (sizeof(int) * (2 * t - 1));
        long pos_ponteiro_filho = pos_pai + offset_filhos + (i * sizeof(char) * 6);
        
        divisao_MS((char *)arq_indice, (i+1), pos_pai, pos_ponteiro_filho, t);
        
        // Apos a divisao, a arvore mudou. Reinicia o processo de insercaoo
        //printf("Divisao completa. Reinserindo atleta ID %d.\n", novo_atleta.id);
        insere(arq_indice, novo_atleta, t);
        return;
    }
    
    TT *atletas = malloc(sizeof(TT) * (num_atletas + 1));
    if (fp && num_atletas > 0) {
        rewind(fp);
        fread(atletas, sizeof(TT), num_atletas, fp);
    }
    
    fclose(fp);
    atletas[num_atletas] = novo_atleta;
    qsort(atletas, num_atletas + 1, sizeof(TT), cmp); //ordena atletas
    
    //cria folha tmp
    char arq_tmp[51];
    sprintf(arq_tmp, "%s.tmp", arq_folha);
    FILE *fp_tmp = fopen(arq_tmp, "wb");
    if (!fp_tmp) {
        printf("Erro ao criar o arquivo temporário\n");
        free(atletas);
        return;
    }
    
    //escreve ordenado na nova folha
    fwrite(atletas, sizeof(TT), num_atletas + 1, fp_tmp);
    num_atletas++;
    fwrite(&num_atletas, sizeof(int), 1, fp_tmp);
    fclose(fp_tmp);
    free(atletas);

    // atualiza folha
    remove(arq_folha);
    rename(arq_tmp, arq_folha);

    //printf("Atleta com ID %d inserido com sucesso em %s.\n", novo_atleta.id, arq_folha);
}



// A ideia eh gerar um pai para antiga raiz e dps chamar a divisao_MS
void divisao_raiz(const char *arq_indice, long tam_bloco, int t){ //adicionado parametro int t
  //Nao funciona, quebra a arvore!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    FILE *fp_original = fopen(arq_indice, "rb+");
    int nfolhas;
    if(!fp_original){
        printf("nao foi possivel abrir o arquivo original(divisao_raiz)\n");
        exit(1);
        return;
    }

    // Calcula tamanho do arq_indice
    fseek(fp_original, 0L, SEEK_END);
    long tamanho_arquivo = ftell(fp_original); //Henrique: - sizeof(int)?  <-----------
    long num_nos_internos = tamanho_arquivo / tam_bloco;

    // Varieveis para leitura da raiz antiga
    char rotulo_antiga_raiz[6];
    int nchaves_antiga_raiz;
    int chaves_antiga_raiz[2 * t - 1];
    char filhos_antiga_raiz[2 * t][6];

    // Leitura dos dados da antiga raiz
    fseek(fp_original, 0L, SEEK_SET);
    fread(rotulo_antiga_raiz, sizeof(char), 6, fp_original);
    fread(&nchaves_antiga_raiz, sizeof(int), 1, fp_original);
    fread(chaves_antiga_raiz, sizeof(int), (2 * t - 1), fp_original);
    fread(filhos_antiga_raiz, sizeof(char[6]), (2 * t), fp_original);

    // Criando arq indice tmp
    FILE *fp_tmp = fopen("INDICE.tmp", "wb");
     if (!fp_tmp) {
        printf("nao foi possivel criar o arquivo temporario(divisao_raiz)\n");
        fclose(fp_original);
        return;
    }

    // Variaveis para nova raiz
    char rotulo_nova_raiz[6];
    int nchaves_nova_raiz = 0;
    int chaves_nova_raiz[2 * t - 1];
    char filhos_nova_raiz[2 * t][6];
    char rotulo_copia_raiz_antiga[6];

    strcpy(rotulo_nova_raiz, "N0000");
    for(int j= 0; j < (2 * t - 1); j++) chaves_nova_raiz[j] = -1;

    // Filho 0 da nova raiz eh a antiga raiz
    sprintf(rotulo_copia_raiz_antiga, "N%04ld", num_nos_internos);// Cria um no com nome nao usado antes
    strcpy(filhos_nova_raiz[0], rotulo_copia_raiz_antiga); // A nova raiz aponta para este novo no
    //for(int j = 0; j < (2 * t); j++) strcpy(filhos_nova_raiz[j], "\0");
    for(int j = 1; j < (2 * t); j++) memset(filhos_nova_raiz[j], '\0', sizeof(char)*6);

    // Escrendo a nova raiz no arq_tmp
   fwrite(rotulo_nova_raiz, sizeof(char), 6, fp_tmp);
   fwrite(&nchaves_nova_raiz, sizeof(int), 1, fp_tmp);
   fwrite(chaves_nova_raiz, sizeof(int), (2 * t - 1), fp_tmp);
   fwrite(filhos_nova_raiz, sizeof(char) * 6, (2 * t), fp_tmp);
   
   // Copia os demais nos
   if(num_nos_internos > 1){
        char buffer[tam_bloco];
        fseek(fp_original, tam_bloco, SEEK_SET); // Pula a raiz
        for(long i = 1; i < num_nos_internos; i++){
            fread(buffer, tam_bloco, 1, fp_original);
            fwrite(buffer,tam_bloco, 1, fp_tmp);
        }
   }
   fread(&nfolhas, sizeof(int), 1, fp_original); //IMPORTANTE

   // A raiz antiga vai receber novo rotulo e vai para o final do arq
   strcpy(rotulo_antiga_raiz, rotulo_copia_raiz_antiga); 
   fwrite(rotulo_antiga_raiz, sizeof(char), 6, fp_tmp);
   fwrite(&nchaves_antiga_raiz, sizeof(int), 1, fp_tmp);
   fwrite(chaves_antiga_raiz, sizeof(int), (2 * t - 1), fp_tmp);
   fwrite(filhos_antiga_raiz, sizeof(char[6]), (2 * t), fp_tmp);

   fwrite(&nfolhas, sizeof(int), 1, fp_tmp); //IMPORTANTE²

   // Pega a posicao da copia da raiz e atualiza indice
   long pos_copia_raiz = num_nos_internos * tam_bloco;
   fclose(fp_original);
   fclose(fp_tmp);
   remove(arq_indice);
   rename("INDICE.tmp", arq_indice);

   // Antiga raiz eh folha e seu pai esta na posicao 0
   
   divisao_MS((char *)arq_indice, 1, 0, pos_copia_raiz, t); // i=1 significa que desce no filho mais a esquerda!
}


void inicializa(const char *arq_indice, int t){

  // Criando folha
  char new_folha[51] = "infos/F0000.bin";
  FILE *fp_folha = fopen(new_folha, "wb");
  if (!fp_folha) {
      printf("Erro ao criar o primeiro arquivo de folha\n");
      exit(1);
  }

  int num_atletas = 0;
  fwrite(&num_atletas, sizeof(int), 1, fp_folha);
  fclose(fp_folha);

  // Criando arquivo indice e seu conteudo
  FILE *fp_indice = fopen(arq_indice, "wb");
  if(!fp_indice){
    printf("Erro ao criar o arquivo de indice\n");
    exit(1);
  }
  char rotulo[6] = "N0000"; 
  int nchaves = 0; 
  int chaves[2 * t - 1];
  memset(chaves, -1, sizeof(chaves));
  char filhos[2 * t][6];
  memset(filhos, '\0', sizeof(filhos));
  strcpy(filhos[0], "F0000");// primeiro filho folha 0
  int nfolhas = 1;

  // Escrevendo no arquivo indice
  fwrite(rotulo, sizeof(char), 6, fp_indice);
  fwrite(&nchaves, sizeof(int), 1, fp_indice);
  fwrite(chaves, sizeof(int), (2 * t - 1), fp_indice);
  fwrite(filhos, sizeof(char) * 6, (2 * t), fp_indice);
  fwrite(&nfolhas, sizeof(int), 1, fp_indice);

  fclose(fp_indice);
}

void insere(const char *arq_indice, TT novo_atleta, int t){ //adicionado parametro int t
    FILE *fp_indice = fopen(arq_indice, "rb");
    if(!fp_indice) {
        //printf("Criando arquivo indice\n");
        inicializa(arq_indice, t);
        fp_indice = fopen(arq_indice, "rb");
        if(!fp_indice){
          printf("Erro ao criar o arquivo indice\n");
          exit(1);
        }
    }
    //if(TARVBMT_Busca(arq_indice, novo_atlta.id, t)) return;
    
    long tam_bloco = (sizeof(char) * 6) +          // rotulo
                    sizeof(int) +             // nchaves
                    (sizeof(int) * (2 * t - 1)) + // chaves
                    (sizeof(char) * 6 * (2 * t)); // filhos
    
    // Variaveis para ler os dados do no
    char id_no[6];
    int nchaves_no;
    int chaves_no[2 * t - 1];
    char filhos_no[2 * t][6];

    // Lendo raiz
    int nchaves_raiz;
    fseek(fp_indice, sizeof(char) * 6, SEEK_SET); // Pula o rotulo para ler nchaves
    fread(&nchaves_raiz, sizeof(int), 1, fp_indice);
    
    if(nchaves_raiz == 2 * t -1){ // raiz cheia
        fclose(fp_indice);
        divisao_raiz(arq_indice, tam_bloco, t);
        fp_indice = fopen(arq_indice, "rb+");
    }
    
    // Localizadores para caminhar no arq_ind
    long ptr_atual = 0L;
    long ptr_filho = -1L;
    int i = 0;

    while(1){

         // Le o no atual campo a campo
        fseek(fp_indice, ptr_atual, SEEK_SET);
        fread(id_no, sizeof(char), 6, fp_indice);
        fread(&nchaves_no, sizeof(int), 1, fp_indice);
        fread(chaves_no, sizeof(int), (2 * t - 1), fp_indice);
        fread(filhos_no, sizeof(char) * 6, (2 * t), fp_indice);

        //Encontra o caminho a seguir no no atual
        i = 0;
        while(i < nchaves_no && novo_atleta.id >= chaves_no[i]){
            i++;
        }

        // Verifica se o filho eh FOLHA.
        char *id_filho = filhos_no[i];
        if(strncmp(id_filho, "F", 1) == 0){ //QUANDO ARQUIVO É VAZIO NUNCA ENTRA AQUI (LOOP INFINITO)
            break; // Encontramos o ponteiro para a folha correta. Sai do while
        }
        
        // Olha para o no filho e confere se esta cheio
        long id_num_filho = atol(id_filho + 1);
        ptr_filho = tam_bloco * id_num_filho;
        int nchaves_filho;
        fseek(fp_indice, id_num_filho * tam_bloco + sizeof(char) * 6, SEEK_SET);
        fread(&nchaves_filho, sizeof(int), 1, fp_indice);
        
        
        if(nchaves_filho == (2 * t -1 )){
            //printf("No interno cheio, chamando divisao_MS");
            fclose(fp_indice);
            divisao_MS((char *)arq_indice, (i+1), ptr_atual, ptr_filho, t); 
            fp_indice = fopen(arq_indice, "rb");
            ptr_atual = 0L;
            continue;
        }else{ // Se nao estiver cheio, continuamos descendo
            ptr_atual = ptr_filho;
        }
    }
    
    //  O loop terminou. 'ptr_atual' aponta para o pai da folha correta.
    char *id_folha_final = filhos_no[i];
    char cam_folha[51];
    sprintf(cam_folha, "infos/%s.bin", id_folha_final);
    
    fclose(fp_indice);
    insere_na_folha(arq_indice, cam_folha, novo_atleta, ptr_atual, i,t);
}

//para resolver Q1
int TT_num_titulos(TT tenista){
  int count=0;
  for(int i = 0; i<15; i++) count += tenista.TorneiosGanhos[i];
  return count;
}

//Arquivo usado para buscar por pais(dado o nome do pais, saber qual o NACid dele)
void cria_arq_paises(char *vetPaises[50]){ 
  FILE *fp = fopen("./auxiliares/paises.bin","wb+");
  for(int i = 0; i<50; i++) fwrite(vetPaises[i],sizeof(char),51,fp);
  fclose(fp);
}

//Para checar se o arquivo está certo
void ler_arq_paises(){
  FILE *fp = fopen("./auxiliares/paises.bin","rb+");
  char nome[51];
  for(int i = 0; i<50; i++){
    fread(&nome,sizeof(char)*51,1,fp);
    printf("%s\n",nome);
  }
  fclose(fp);
}

TT TT_completaInfoChampionsTxt(TT tenista){
  //durante a coleta de infos, jogar nas hashs (ex: hash ganhadores de torneios)

  //preencher campos: Pontuacao, TorneiosGanhos, anoGanhouTodosGrands

  int indicetorneios, qtdTotTitulos=0, qtdTitulosLidos=0, ano, qtdGrandSlamNesseAno;
  int torneiosPts[15] = {2000,2000,2000,2000,0,0,1000,1000,1000,1000,1000,1000,1000,1000,1000};
  char *lido, sobrenome[51], linha[300], sobrenomeTT[51];

  FILE *fp = fopen("champions.txt","r"); //IPC lembrar desse ".txt"!!
  fgets(linha,sizeof(linha),fp); //Pula a primeira linha (aka o cabeçalho)
  //fgets(linha,sizeof(linha),fp); //Pega a primeira linha de informacoes
  while(fgets(linha, sizeof(linha), fp)){ //Enquanto existir proxima linha, pega ela e:
    lido = strtok(linha,"\\"); //pega a primeira parte até "\" que é o ano
    ano = atoi(lido);
    lido = strtok(NULL,"\\"); //pega o primeiro "bloco" de campeao
    indicetorneios = 0;
    qtdGrandSlamNesseAno = 0;
    while(lido && indicetorneios<15){
      sscanf(lido,"%[^(] (%*d/%*d)",sobrenome); //lê o sobrenome
      if(sobrenome[strlen(sobrenome) -1] == ' ') sobrenome[strlen(sobrenome) -1] = '\0'; //retira o possivel espaço em branco do final
      //Pega a parte do sobrenome do nome (Pablo Afonso Pereira -> Afonso Pereira)
      int pos = 0;
      sscanf(tenista.nome, "%*s %n", &pos);  // pega a posição após o primeiro nome
      strcpy(sobrenomeTT, tenista.nome + pos); //sobrenomeTT tem o sobrenome do TT
      //
      if(strcmp(sobrenome,sobrenomeTT)==0){ //É o tenista na posicao indiceTorneios (PREENCHENDO SUAS INFORMACOES)
        if(qtdTotTitulos==0) sscanf(lido,"%*[^(] (%*d/%d)",&qtdTotTitulos);
        tenista.pontuacao += torneiosPts[indicetorneios];
        qtdTitulosLidos++;
        tenista.TorneiosGanhos[indicetorneios]++;
        if(indicetorneios<=3) qtdGrandSlamNesseAno++;

        //!!HASH!! Inserir o caba na HASH DE VENCEDORES DE TORNEIO COM ANO ("TT.id", "TT.pontuacao" e "ano")
        //THV_insere(tenista.id,indicetorneios);

        if(qtdTitulosLidos == qtdTotTitulos) break;
      }

      
      lido = strtok(NULL,"\\"); //vai pegando o conjunto de infos (até nao ter mais) do tipo:   "Lendl (1/1)" 
      indicetorneios++;
    }
    //acabou esse ano
    if(qtdGrandSlamNesseAno==4) tenista.anoGanhouTodosGrands = ano;

    //!!HASH!! Inserir o caba na HASH DE PONTUACAO POR ANO ("TT.id", "TT.pontuacao" e "ano")
    //THP_insere(tenista.id,tenista.pontuacao,ano);
    adicionar_pontuacao(tenista.id,tenista.pontuacao,ano);

    if(qtdTotTitulos!=0 && qtdTitulosLidos == qtdTotTitulos) break;
  }

  fclose(fp);
  return tenista;
}

int posPaisEmVet(char *pais,char *vetPais[50]){
    for(int i = 0; i<50; i++) if(vetPais[i]!=NULL && strcmp(pais,vetPais[i]) == 0) return i;
    return -1;
}
//durante a coleta de infos, jogar nas hashs

//Lê o(s) txt(s), monta o ID e ja manda insere na arvore direto (Cria a arvore no começo do programa)
void TARVBMT_criaPorTxt(int t){ //int t só usado para o insere (se o insere nao precisar tire o parametro dessa funcao)
  FILE *fp = fopen("tennis_players.txt","r");
  if(!fp) exit(1);
  InicializaHashs();
  TT tenista;
  char *lido, linha[120], nome[51];
  int ano, campo, indPais=10, indCpf=100, semanatemp=-1;
  int TTidPais,TTidCpf,TTidAno, tamvet = 57; //minimo com que funciona: 53
  //char vetPais[50][51]; //50 paises de no maximo 51 caracteres
  char *vetPais[tamvet];
  for (int i = 0; i < tamvet; i++) {
    vetPais[i] = malloc(51 * sizeof(char));
  }
  fgets(linha,sizeof(linha),fp); //Pula o cabeçalho
  while(fgets(linha,sizeof(linha),fp)){ //começa a ler as linhas que importam
    tenista = TT_cria_vazio();
    lido = strtok(linha,"\\");
    strcpy(tenista.nome,lido);
    lido = strtok(NULL,"\\");
    tenista.ano_nascimento = atoi(lido);
        
    TTidCpf = indCpf++;  //TTidCpf recebe o indice e DEPOIS o indice aumenta em 1 (para o proximo)
    TTidAno = tenista.ano_nascimento - 1950;

    lido = strtok(NULL,"\\");
    campo = 2;
    semanatemp = -1;
    while(lido){
      if(strcmp(lido,"-")!=0){
        switch (campo){
        case 2: //ano morte
          tenista.morte = atoi(lido);
          break;
        case 3: //nacionalidade
          strcpy(tenista.pais,lido);
          int pos = posPaisEmVet(tenista.pais,vetPais);
          if(pos >= 0) TTidPais =  pos+10;
          else{
            strcpy(vetPais[indPais-10],tenista.pais);
            TTidPais = indPais++;
          }
          break;
        case 4:
          tenista.rank = atoi(lido);
          break;
        case 5:
          sscanf(lido,"%d (%d)",&tenista.YoBK,&semanatemp);
          if(semanatemp && semanatemp>0) tenista.numSem = semanatemp;
          break;
        default:
          break;
        }
      }
      lido = strtok(NULL,"\\");
      campo++;
    }

    tenista.id = (TTidCpf * 10000) + (TTidPais * 100) + TTidAno;
    tenista = TT_completaInfoChampionsTxt(tenista);

    //!!HASH!! Inserir o caba na HASH DE NOME ("TT.id","TT.nome")
    THNOM_insere(tenista.id,tenista.nome);
    //!!HASH!! Inserir o caba na HASH DE NACIONALIDADE ("TT.id","TT.pais")
    THNAC_insere(tenista.id);

    insere("INDICES.bin",tenista, t);

    //printf para "carregando arvore . . . "
    if(!(indCpf%100)) printf(" .");

    //printf para debug -> printa todos os jogadores com suas infos (quase todas infos)
    //printf("%s - %d: %d | %d | %s | %d | %d | %d | %d\n",tenista.nome,tenista.id,tenista.ano_nascimento,tenista.morte,tenista.pais,tenista.rank,tenista.YoBK,tenista.numSem,tenista.pontuacao);
  }

  fclose(fp);
  for (int i = 0; i < tamvet; i++) { //liberando o vetor de paises
      free(vetPais[i]);
  }
}

int int_len(int num){
  if(num == 0) return 1;
  int count=0;
  while(num>0){
    num/=10;
    count++;
  }
  return count;
}
/*
TT TARVBMT_busca(int id,int t){ //funcionando corretamente
  FILE *fp = fopen("INDICES.bin","rb+");
  if(!fp) return TT_cria_vazio(); //exit(1); com problemas no ubuntu
  char identfNo[6], filho[6], temp[6];
  int numchaves,qtdLidos,i,pos,tamPorBloco = (sizeof(char)*6 + sizeof(int) + sizeof(int)*((2*t)-1) + (sizeof(char)*6)*(2*t));
  int chave;
  while(1){
    i=0;
    qtdLidos = fread(&identfNo,sizeof(char),6,fp);
    if(qtdLidos > 0){
      if(strncmp(identfNo,"N",1)==0){ //Se é um Nó
        fread(&numchaves,sizeof(int),1,fp);
        fread(&chave,sizeof(int),1,fp); 
        while((i < numchaves-1) && (id >= chave)){
          i++;
          fread(&chave,sizeof(int),1,fp); 
        }
        if(i==numchaves-1 && id>=chave){ //se é a ultima chave a ainda é maior, vai descer no filho + 1
          i++;
          fread(&chave,sizeof(int),1,fp); //lê o proximo para andar com o ponteiro tbb
        }
        pos = ftell(fp);
        fseek(fp,pos+(sizeof(int) * (t-i)), SEEK_SET);//pula até o começo dos filhos
        pos = ftell(fp);
        fseek(fp,pos+((sizeof(char)*6)*i),SEEK_SET);
        fread(&filho,sizeof(char),6,fp);
        if(filho[0]=='F'){//Se o filho for folha, o nome dele vai pra identfNo e sai do loop de procura
          strcpy(identfNo,filho);
          break;
        }//caso contrario, se o filho ainda for Nó, vai para ele e repete o argoritmo(até achar uma folha)
        strcpy(temp,&filho[1]); //Pega os os numeros do filho
        strcpy(filho,temp);     //Uso do temp para evitar possiveis erros
        pos = atoi(filho) * tamPorBloco;//pos recebe o valor que iremos pular (ex: se quero ir para o nó N0001 vou pular 1 bloco)
        fseek(fp,pos,SEEK_SET);
      }
      else //Achou uma folha
        break;
    }
    else{
      printf("\nIdentificacao do No nao encontrado(verificar TARVBMT_buscar  )\n");// PRINT DEBUG APAGAR FUTURAMENTE!!
      return TT_cria_vazio(); //tenista erro
    }
  }
  fclose(fp);
  char arqFolha[30];
  TT tenista;
  strcpy(arqFolha,"./infos/");   
  strcat(arqFolha,identfNo);
  strcat(arqFolha,".bin");
  fp = fopen(arqFolha,"rb+");
  qtdLidos = fread(&tenista,sizeof(TT),1,fp);
  while((qtdLidos>0) && (tenista.id != id)) qtdLidos = fread(&tenista,sizeof(TT),1,fp);
  if(qtdLidos<=0) return TT_cria_vazio(); //tenista erro
  return tenista;
}
  */

void TARVBMT_libera(char *arq){ //nome deve vir como Indices.bin
  FILE *fp = fopen(arq,"rb+");
  if(!fp) return;
  fseek(fp,0L,SEEK_END);
  int nfolhas,fim = ftell(fp) - sizeof(int);
  fseek(fp,fim,SEEK_SET);
  fread(&nfolhas,sizeof(int),1,fp);
  fclose(fp);
  remove(arq);
  //liberaHashs(); //Ainda deve ser implementada
  char nome[51],string_i[51]; //string_i[6] dando problema em alguns computadores
  for(int i = 0; i<nfolhas; i++){
    strcpy(nome,"./infos/F");
    for(int j = 0; j < 4 - int_len(i); j++) strcat(nome,"0");
    sprintf(string_i,"%d",i);
    strcat(nome,string_i);
    strcat(nome,".bin");
    remove(nome);
  }
}

void TLSEid_imprime(TLSEid *lista,int t){
  if(!lista) return;
  TLSEid *aux = lista;
  int i = 1;
  while(aux){
    printf("%d) %s\n",i,TARVBMT_busca(aux->id,t).nome);
    aux = aux->prox;
    i++;
  }
}

void ImprimeMenu(){
  printf(
    "\n- - - - - - - - - - - MENU - - - - - - - - - - -\n\n"

    " 1 - Imprimir árvore\n"
    " 2 - Buscar por nome completo\n"
    " 3 - Mostrar ordem e pontuacao caso não houvesse aposentados (Q3)\n"
    " 4 - Mostrar ranking por ano (Q4)\n"
    "-1 - Sair\n\n"

    "Opccao: "
  );
}

//Para compilar -> gcc MainTeste.c TH_MS.c TH_MS.h -o mainteste
int main(void){
  //setlocale(LC_ALL,"Portuguese_Brazil");
  int t, opcao;
  printf("Insira um t: ");
  scanf("%d",&t);
  TARVBMT_libera("INDICES.bin");
  printf("\nInicializando estruturas");
  TARVBMT_criaPorTxt(t);
  printf("\n");
  //imprime("INDICES.bin",t);

  /*
  printf("deseja buscar as informações de alguém? (se sim, por enquanto, insira o ID) (se nao digite -1)");
  scanf("%d", &id);
  while(id > 0){
      TT tenista = TARVBMT_busca(id, t);
      printf("tenista > %s\nNasceu em %d - %s\n",tenista.nome,tenista.ano_nascimento,tenista.pais);
      if(tenista.morte != -1){
          printf("Morreu em %d\n",tenista.morte);
      }
      printf("Fez %d pontos!\n\n",tenista.pontuacao);
      printf("deseja buscar as informações de alguém? (se sim, por enquanto, insira o ID) (se nao digite -1)");
      scanf("%d", &id);
  }
  */

  ImprimeMenu();
  scanf("%d", &opcao);
  while(opcao > 0){
    if(opcao == 1) imprime("INDICES.bin",t);
    else if(opcao == 2){
      char nome[51];
      printf("Insira o nome completo: ");
      scanf(" %[^\n]",nome);
      TT tenista = THNOM_busca(nome, t);
      if(tenista.id == -1) printf("\nTenista não encontrado\nVerifique a grafica de '%s'\n\n",nome);
      else{
        printf("\nTenista : %s\nNasceu em %d - %s\n",tenista.nome,tenista.ano_nascimento,tenista.pais);
        if(2025 - tenista.ano_nascimento >= 35)printf("Está aposentado\n");
        else printf("Está ativo\n");
        if(tenista.morte != -1){
          printf("Morreu em %d\n",tenista.morte);
        }
        printf("Fez %d pontos!\n\n",tenista.pontuacao);
      }
    }
    else if(opcao == 3){
      imprimir_top_N(2024,t,-10);//imprime até, no max, 25
    }
    else if(opcao == 4){
      int subopcao;
      printf("%d  >    .\n\t1 - Inserir um ano\n\t2 - Mostrar todos os anos\n\n\tOpccao: ",opcao);
      scanf("%d",&subopcao);
      if(subopcao == 1){
        int ano;
        printf("Insira o ano: ");
        scanf("%d",&ano);
        imprimir_top_N(ano,t,25);//imprime até, no max, 25
        //TLSEid *lista;
        //lista = THP_busca_primeiros_ateh_N_Do_Ano(ano,25);//busca até 25 do ano
        //TLSEid_imprime(lista,t);
      }
      else if(subopcao == 2){
        for(int i = 1990; i<=2024; i++)imprimir_top_N(i,t,25);
      }else{
        printf("\nInsira um valor válido\n\n");
      }
    }
    
    
    
    else printf("Opção inválida\n");
    ImprimeMenu();
    scanf("%d", &opcao);
  }
  

    
}