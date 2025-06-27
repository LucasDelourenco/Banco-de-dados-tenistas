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

void NOFO_libera(NOFO *no, int t){
  free(no->vet_tenista);
  free(no);
}

void NOINT_libera(NOINT *no, int t){
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
  printf("\n");
}

void NOFO_imprime(NOFO *no, int t){
  int op;
  printf("Deseja imprimir o no %s?(Sim:0/Nao:1) ", no->rotulo);
  scanf(" %d", &op);
  if (op) return;

  printf("\n");
  printf("%s\n%d\n", no->rotulo, no->num_info);
  for(int i=0; i<no->num_info;i++) printf("%d ", no->vet_tenista[i]);
  printf("\n");
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
    strncpy(temp, nome_ent, 6);
    int num = atoi(&temp[1]) + 1;

    if (num > 9999) return 0;

    sprintf(destino, "%c%04d", temp[0], num);
    return 1;
}

int diminui_um(char *nome_ent, char *destino) {
    if (campo_vazio(nome_ent)) return 0;

    char temp[6];
    strncpy(temp, nome_ent, 6);
    int num = atoi(&temp[1]) - 1;

    if (num < 0) return 0;

    sprintf(destino, "%c%04d", temp[0], num);
    return 1;
}

long buscar_pos_no(FILE *f_indice, char *nome_no, int t){
  int tamPorBloco, N, finalDosBlocos;
  char aux[6];
  
  fseek(f_indice, 0L, SEEK_END); 
  finalDosBlocos = ftell(f_indice) - sizeof(int);  //Descobre o final do ultimo bloco
  tamPorBloco = (sizeof(char)*6 + sizeof(int) + sizeof(int)*((2*t)-1) + (sizeof(char)*6)*(2*t));
  if (nome_no[0] == 'N'){  //Se for no interno pula direto pro nó desejado
    strcpy(aux, &nome_no[1]);
    N = atoi(aux);
    return tamPorBloco * N;
  }
  else if (nome_no[0] == 'F'){  //Se for filho olha os filhos de cada nó
    int blocoSemFilho = (sizeof(char)*6 + sizeof(int) + sizeof(int)*((2*t)-1)), i;
    fseek(f_indice, blocoSemFilho, SEEK_SET);
    while (1){
      for (i=0; i<2*t; i++){
        fread(aux, sizeof(char), 6, f_indice);
        if (memcmp(aux, "\0\0\0\0\0\0", 6) == 0) continue; //Se o filho for vazio pule um loop
        else if (strlen(aux) == 1 && aux[0] == 'N') continue; //Se for no interno pule um loop
        else if (strcmp(aux, nome_no) == 0){ //Achou o rótulo do filho
          return ftell(f_indice) - sizeof(char)*6;
        }
      }
      fseek(f_indice, blocoSemFilho, SEEK_CUR); //Vai para o próximo conjunto de filhos
      if (ftell(f_indice) > finalDosBlocos - sizeof(char)*6*(2*t)) break;  //Caso a próxima leitura passe do final do ultimo bloco, parar
    }
  }

  return -1; //O parametro passado está errado, esperava: "Nxxxx\0" ou "Fxxxx\0"
}

void escreve_no(FILE *file, NOINT *no, int t){
  fwrite(no->rotulo, sizeof(char), 6, file);
  fwrite(no->nchaves, sizeof(int), 1, file);
  fwrite(no->vet_chaves, sizeof(int), (2*t)-1, file);
  fwrite(no->filhos, sizeof(char), 6*2*t, file);
}

void divisao_MS(char *indice, int i, int pos_pai, int pos_dividido, int t){ //WIP
  FILE *f_indice = fopen(indice, "rb+");
  if (!f_indice){
    perror("divisao_MS:Falha em abrir o arquivo indice no modo de leitura.\n");
    return;
  }
  //OBS: f_indice terá que ser aberto em rb+!!!!!!!!!!
  long tamBloco = sizeof(char)*6 + sizeof(int) + sizeof(int)*((2*t)-1) + sizeof(char)*6*2*t;
  //pos_pai: posicao onde o pai se encontra no arquivo indice
  //i: posicao onde o novo filho será inserido
  //pos_dividido: posicao do nó que será dividido

  char no, aux[11], aux2[11];
  int j, pont_aux; //ponteiro auxiliar, facilita o uso de fseek e ftell
  int nfolhas, ntenistas, chave_que_sobe, temp, chave_atual;
  long fimDosBlocos;

  fseek(f_indice, pos_dividido, SEEK_SET); //anda ate o no que sera dividido
  fread(&no, sizeof(char), 1, f_indice);

  NOINT *x;
  x = NOINT_cria(t);

  //Preenche x com as informalçoes
  fseek(f_indice, pos_pai, SEEK_SET); //anda ate no pra ser dividido
  fread(x->rotulo, sizeof(char), 6, f_indice); //Le o rotulo Nxxxx
  fread(&x->nchaves, sizeof(int), 1, f_indice); //Le o int numero de chaves
  fread(x->vet_chaves, sizeof(int), (2*t)-1, f_indice); //Le as 2t-1 chaves do no que vai ser dividido
  fread(x->filhos, sizeof(char), 2*t*6, f_indice); //Le os 2t filhos

  fseek(f_indice, 0L, SEEK_END);
  fimDosBlocos = ftell(f_indice) - sizeof(int);
  fseek(f_indice, fimDosBlocos, SEEK_SET);
  fread(&nfolhas, sizeof(int), 1, f_indice);

  int numPai = pos_pai/tamBloco;
  int numDividido = pos_dividido/tamBloco;
  char buffer[tamBloco];

  

  if (no != 'F'){ //Caso o dividido seja no interno
    
    NOINT *y, *z;

    y = NOINT_cria(t);
    z = NOINT_cria(t);

    //Preenche y com as informalçoes
    fseek(f_indice, pos_dividido, SEEK_SET); //anda ate no pra ser dividido
    fread(y->rotulo, sizeof(char), 6, f_indice); //Le o rotulo Nxxxx/Fxxxx
    fread(&y->nchaves, sizeof(int), 1, f_indice); //Le o int numero de chaves
    if (no != 'F') fread(y->vet_chaves, sizeof(int), (2*t)-1, f_indice); //Le as 2t-1 chaves do no que vai ser dividido
    else seek(f_indice, sizeof(int)*((2*t)-1), SEEK_CUR); 
    fread(y->filhos, sizeof(char), 2*t*6, f_indice); //Le os 2t filhos

    y->nchaves = t-1;
    z->nchaves = t-1;

    for(j=0;j<t-1;j++){
      z->vet_chaves[j] = y->vet_chaves[j+t];
      y->vet_chaves[j+t] = -1;
    }
    for(j=0;j<t;j++){
      strcpy(z->filhos[j], y->filhos[j+t]);
      memset(y->filhos[j], '\0', sizeof(char[6]));
    }

    //
    aumenta_um(y->rotulo, z->rotulo);

    for (j=x->nchaves; j>=i; j--){
      aumenta_um(x->filhos[j], aux);
      strcpy(x->filhos[j+1], aux);
    }
    strcpy(x->filhos[i], z->rotulo);
    for (j=x->nchaves; j>=i; j--) x->vet_chaves[j] = x->vet_chaves[j-1];
    x->vet_chaves[i-1] = y->vet_chaves[t-1];
    x->nchaves++;
    
    FILE *fnovo = fopen("novo.bin", "wb");
    fseek(f_indice, 0L, SEEK_SET);
    for(j=0; j<numPai; j++){ //Copiar tudo até chegar no pai do nó que está sendo dividido
      fread(buffer, 1, tamBloco, f_indice);
      fwrite(buffer, 1, tamBloco, fnovo);
    }
    fseek(f_indice, tamBloco, SEEK_CUR); //Pula o no pai antigo
    escreve_no(fnovo, x, t);  //Escreve o no pai novo

    for(j=numPai; j<numDividido-tamBloco; tamBloco){ //Copiar tudo até chegar no dividido
      fread(buffer, 1, tamBloco, f_indice); 
      fwrite(buffer, 1, tamBloco, fnovo);  
    }

    fseek(f_indice, tamBloco, SEEK_CUR);  //Pula o dividido antigo
    escreve_no(fnovo, y, t);  //Escreve o dividido novo
    escreve_no(fnovo, z, t);  //Escreve o novo no que surgiu da divisao

    for(j=((fimDosBlocos-pos_dividido)/tamBloco)-1; j>0; j--){ //Copia tudo até chegar no fim
      fread(buffer, 1, tamBloco, f_indice);
      fwrite(buffer, 1, tamBloco, fnovo);
    }
    fwrite(&nfolhas, sizeof(int), 1, fnovo);  //

    fclose(fnovo);
    fclose(f_indice);

    remove(indice);
    rename("novo.bin", indice);

    NOINT_imprime(x, t);
    NOINT_imprime(y, t);
    NOINT_imprime(z, t);

    NOINT_libera(y, t);
    NOINT_libera(z, t);

  }
  else { //Caso seja folha - Falta testar
  
    char *tmpNomeFolha1 = (char*)malloc(sizeof(char)*6);
    char *tmpNomeFolha2 = (char*)malloc(sizeof(char)*6);
    int folha_y;

    NOFO *y = NOFO_cria(t);
    NOFO *z = NOFO_cria(t);
   
    fseek(f_indice, pos_dividido, SEEK_SET);
    fread(y->rotulo, sizeof(char), 6, f_indice);

    strcpy(aux, y->rotulo);
    strcat(aux, ".bin");
    FILE *fy = fopen(aux, "rb");

    aumenta_um(y->rotulo, z->rotulo);

    y->num_info = t-1;
    z->num_info = t;
    
    fseek(fy, 0L, SEEK_END);
    pont_aux = ftell(fy) - sizeof(int);
    fread(y->num_info, sizeof(int), 1, fy);
    fseek(fy, 0L, SEEK_SET);
    for (j=0; y->num_info; j++) fread(&y->vet_tenista[j], sizeof(TT), 1, fy);

    fclose(fy);

    for (j=x->nchaves; j>=i; j--){
      aumenta_um(x->filhos[j], aux);
      strcpy(x->filhos[j+1], aux);
    }
    strcpy(x->filhos[i], z->rotulo);
    for (j=x->nchaves; j>=i; j--) x->vet_chaves[j] = x->vet_chaves[j-1];
    x->vet_chaves[i-1] = y->vet_tenista[t-1].id;
    x->nchaves++;

    for (j=0;j<t;j++){
      z->vet_tenista[j] = y->vet_tenista[j+t-1];
      memset(&y->vet_tenista[j], 0, sizeof(TT));
    }

    //Atualizar o arquivo indice
    FILE *fnovo = fopen("novo.bin", "wb");

    for(j=0; j<numPai; j++){ //Copiar tudo até chegar no pai do nó que está sendo dividido
      fread(buffer, 1, tamBloco, f_indice);
      fwrite(buffer, 1, tamBloco, fnovo);
    }
    fseek(f_indice, tamBloco, SEEK_CUR); //Pula o no pai antigo
    escreve_no(fnovo, x, t);  //Escreve o no pai novo
    
    for(j=((fimDosBlocos-pos_pai)/tamBloco)-1; j>0; j--){ //Copia tudo até chegar no fim
      fread(buffer, 1, tamBloco, f_indice);
      fwrite(buffer, 1, tamBloco, fnovo);
    }
    nfolhas++;
    fwrite(&nfolhas, sizeof(int), 1, fnovo);  //

    fclose(fnovo);
    fclose(f_indice);

    remove(indice);
    rename("novo.bin", indice);

    //Ajustar as folhas

    //Como vamos adicionar uma folha na nossa arvore
    //Vamos aumentar em 1 o valor de cada folha que esta depois da folha inserida
    //Ex: F0000 - F0001 - F0002     (Divide F0000, cria irmão z com nome F0001)
    //Move 1 pra direita: F0000 - F0002 - F0003
    //Insere z: F0000 - F0001 - F0002 - F0003

    folha_y = atoi(&y->rotulo[1]);
    gerar_nome_folha(aux, nfolhas-2); 
    aumenta_um(aux, aux2); //aux: F0002 e aux2: F0003 se for o caso do exemplo acima.
    for(j = nfolhas-1; j>folha_y; j--){
      rename(aux, aux2);
      strcpy(aux2, aux);
      diminui_um(aux, aux);
    }
    strcpy(aux, y->rotulo);
    strcat(aux, ".bin");
    remove(aux);
    
    strcpy(aux2, z->rotulo);
    strcat(aux2, ".bin");

    FILE *fy = fopen(aux, "wb"), *fz = fopen(aux2, "wb");

    for (j=0; j<t; j++){
      fwrite(&z->vet_tenista[j], sizeof(TT), 1, fz);
      if (j == t-1) continue;
      fwrite(&y->vet_tenista[j], sizeof(TT), 1, fy);
    }
    fwrite(&z->num_info, sizeof(int), 1, fz);
    fwrite(&y->num_info, sizeof(int), 1, fy);

    fclose(fz);
    fclose(fy);

    NOINT_imprime(x, t);
    NOFO_imprime(y, t);
    NOFO_imprime(z, t);

    NOFO_libera(y, t);
    NOFO_libera(z, t);

  } //Fim do caso folha

  NOINT_libera(x, t);
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
    id = 20087967;
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

    id = 30087570;
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
    tenista.id = 1001010;
    strcpy(tenista.pais,"Brazil");
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("./infos/F0002.bin","wb+");
    num = 1;
    strcpy(tenista.nome,"Leandro Barros");
    tenista.id = 1001010;
    strcpy(tenista.pais,"Brazil");
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("./infos/F0003.bin","wb+");
    num = 1;
    strcpy(tenista.nome,"Luis");
    tenista.id = 1001010;
    strcpy(tenista.pais,"Brazil");
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("./infos/F0004.bin","wb+");
    num = 1;
    strcpy(tenista.nome,"Fernando");
    tenista.id = 1001010;
    strcpy(tenista.pais,"Brazil");
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("./infos/F0005.bin","wb+");
    num = 1;
    strcpy(tenista.nome,"Pablo");
    tenista.id = 1001010;
    strcpy(tenista.pais,"Brazil");
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);
}


int main(void){
  
  Cria_indicesTeste();

  int i=0;
  FILE *fp = fopen("INDICES.bin","rb+");
  long pai = buscar_pos_no(fp,"N0000",2),y = buscar_pos_no(fp,"N0001",2);
  fclose(fp);
  divisao_MS("INDICES.bin",i,pai,y,2);
  
  return 0;
}