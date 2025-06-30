#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tenistas{
  //tenistas.txt
  int id, ano_nascimento, morte, rank, YoBK, numSem;
  char nome[51], pais[51];  //nome é nome + sobrenome
  //champions.txt
  int pontuacao;
  int anoGanhouTodosGrands; //Q(5)
  int TorneiosGanhos[15];
}TT;

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
  scanf(" %d", &op);
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
  scanf(" %d", &op);
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
    printf("Caso no interno\n");
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
    printf("Caso Folha\n");
    
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
    id = 2087967;
    num = 1;
    strcpy(nome,"N0000");
    fwrite(&nome,sizeof(char),6,fp);
    fwrite(&num,sizeof(int),1,fp);
    fwrite(&id,sizeof(int),1,fp);
    id = -1;
    fwrite(&id,sizeof(int),1,fp);
    id = -1;
    fwrite(&id,sizeof(int),1,fp);
    strcpy(nome,"N0001");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"N0002");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"\0\0\0\0\0\0");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"\0\0\0\0\0\0");
    fwrite(&nome,sizeof(char),6,fp);

    id = 1035780;
    num = 3;
    strcpy(nome,"N0001");
    fwrite(&nome,sizeof(char),6,fp);
    fwrite(&num,sizeof(int),1,fp);
    fwrite(&id,sizeof(int),1,fp);
    id = 1045780;
    fwrite(&id,sizeof(int),1,fp);
    id = 1055780;
    fwrite(&id,sizeof(int),1,fp);
    strcpy(nome,"F0000");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"F0001");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"F0002");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"F0003");
    fwrite(&nome,sizeof(char),6,fp);

    id = 3087570;
    num = 1;
    strcpy(nome,"N0002");
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
    strcpy(nome,"\0\0\0\0\0\0");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"\0\0\0\0\0\0");
    fwrite(&nome,sizeof(char),6,fp);

    num = 6;
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);


    //arqs folhas
    fp = fopen("./infos/F0000.bin","wb+");
    TT tenista = TT_cria_vazio();
    num = 1;
    strcpy(tenista.nome,"Pedro");
    tenista.id = 1001010;
    strcpy(tenista.pais,"Brazil");
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("./infos/F0001.bin","wb+");
    num = 1;
    strcpy(tenista.nome,"Gustavo");
    tenista.id = 1035888;
    strcpy(tenista.pais,"Brazil");
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("./infos/F0002.bin","wb+");
    num = 1;
    strcpy(tenista.nome,"Leandro Barros");
    tenista.id = 1054110;
    strcpy(tenista.pais,"Brazil");
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("./infos/F0003.bin","wb+");
    num = 1;
    strcpy(tenista.nome,"Luis");
    tenista.id = 1071717;
    strcpy(tenista.pais,"Brazil");
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("./infos/F0004.bin","wb+");
    num = 1;
    strcpy(tenista.nome,"Fernando");
    tenista.id = 2011010;
    strcpy(tenista.pais,"Brazil");
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("./infos/F0005.bin","wb+");
    num = 1;
    strcpy(tenista.nome,"Pablo");
    tenista.id = 4001010;
    strcpy(tenista.pais,"Brazil");
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);
}


int main(void){
  
  Cria_indicesTeste();
  //ATENCAO o valor de i eh sempre >= 1!!!! 
  int i=1;
  FILE *fp = fopen("INDICES.bin","rb+");
  long pai = buscar_pos_no(fp,"N0000",2),y = buscar_pos_no(fp,"N0001",2);
  fclose(fp);
  divisao_MS("INDICES.bin",i,pai,y,2);
  
  return 0;
}