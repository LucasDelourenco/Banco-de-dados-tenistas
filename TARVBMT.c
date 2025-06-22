#include "TARVBMT.h"
#include <string.h>
#include <stdlib.h>

/*
typedef struct tenistas{
  int id, ano_nascimento, morte, rank, YoBK, numSem, pontuacao;
  char nome[51], pais[51];
}TT;
*/

TT TT_cria_vazio(){
  TT novo;
  novo.id = -1;
  novo.ano_nascimento = -1;
  novo.morte = -1;
  novo.rank = -1;
  novo.YoBK = -1;
  novo.numSem = -1;
  novo.pontuacao = -1;
  strcpy(novo.nome,"\0");
  strcpy(novo.pais,"\0");
  return novo;
}




TT *TARVBMT_cria(char *arq,int t){
  
}


TARVBMT *TARVBMT_inicializa(void){
  return NULL;
}

void TARVBMT_libera(TARVBMT *a){
  if(a){
    if(!a->folha){
      int i;
      for(i = 0; i <= a->nchaves; i++) TARVBMT_libera(a->filho[i]);
    }
    free(a->filho);
    free(a->chave);
    free(a);
  }
}

//                 Indices   id   TdaArvore
TT TARVBMT_busca(char *arq,int id,int t){ //TBT (ToBeTested)
  FILE *fp = fopen(arq,"rb+");
  if(!fp) exit(1);
  char identfNo[6], filho[6];
  int numchaves,qtdLidos,i,pos,tamPorBloco = (sizeof(char)*6 + sizeof(int) + sizeof(int)*((2*t)-1) + (sizeof(char)*6)*(2*t));
  int chave;//unsigned long int chave;
  while(1){
    i=0;
    qtdLidos = fread(&identfNo,sizeof(char),6,fp);
    if(qtdLidos > 0){
      if(strncmp(identfNo,"N",1)==0){ //Se é um Nó
        fread(&numchaves,sizeof(int),1,fp);
        fread(&chave,sizeof(int),1,fp);  //antes era long
        while((i < numchaves-1) && (id >= chave)){
          i++;
          fread(&chave,sizeof(int),1,fp); //antes era long
        }
        if(i==numchaves-1 && id>=chave) i++;
        pos = ftell(fp);
        fseek(fp,pos+(sizeof(int) * (t-i)), SEEK_SET);//pula até o começo dos filhos
        pos = ftell(fp);
        fseek(fp,pos+((sizeof(char)*6)*i),SEEK_SET);
        fread(&filho,sizeof(char),6,fp);
        if(strncmp(&filho,"F",1)==0){//Se o filho for folha, o nome dele vai pra identfNo e sai do loop de procura
          strcpy(identfNo,filho);
          break;
        }//caso contrario, se o filho ainda for Nó, vai para ele e repete o argoritmo(até achar uma folha)
        strcpy(filho,&filho[1]); //Pega os os numeros do filho
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


void TARVBMT_imprime_chaves(TARVBMT *a){
  if(!a) return;
  TARVBMT *p = a;
  while(p->filho[0]) p = p->filho[0];
  while(p){
    int i;
    for(i = 0; i < p->nchaves; i++) printf("%d ", p->chave[i]);
    p = p->prox;
  }
  printf("\n");
}


void imp(TARVBMT *a, int andar){
  if(a){
    int i,j;
    imp(a->filho[a->nchaves],andar+1);
    for(i=a->nchaves-1; i >= 0; i--){
      for(j=0; j<=andar; j++) printf("\t");
      printf("%d\n", a->chave[i]);
      imp(a->filho[i],andar+1);
    }
  }
}

void TARVBMT_imprime(TARVBMT *a){
  imp(a, 0);
}


void imp0(TARVBMT *a, int andar){
  if(a){
    int i,j;
    for(i=0; i<=a->nchaves-1; i++){
      imp(a->filho[i],andar+1);
      for(j=0; j<=andar; j++) printf("\t");
      printf("%d\n", a->chave[i]);
    }
    imp(a->filho[i],andar+1);
  }
}

void TARVBMT_imprime0(TARVBMT *a){
  imp0(a, 0);
}

TARVBMT *divisao(TARVBMT *x, int i, TARVBMT* y, int t){
  TARVBMT *z = TARVBMT_cria(t);
  z->folha = y->folha;
  int j;
  if(!y->folha){
    z->nchaves = t-1;
    for(j=0;j<t-1;j++) z->chave[j] = y->chave[j+t];
    for(j=0;j<t;j++){
      z->filho[j] = y->filho[j+t];
      y->filho[j+t] = NULL;
    }
  }
  else {
    z->nchaves = t; //z possuir� uma chave a mais que y se for folha
    for(j=0;j < t;j++) z->chave[j] = y->chave[j+t-1];//Caso em que y � folha, temos q passar a info para o n� da direita
    z->prox = y->prox; //ultima revisao: 04/2020
    y->prox = z;
  }
  y->nchaves = t-1;
  for(j=x->nchaves; j>=i; j--) x->filho[j+1]=x->filho[j];
  x->filho[i] = z;
  for(j=x->nchaves; j>=i; j--) x->chave[j] = x->chave[j-1];
  x->chave[i-1] = y->chave[t-1];
  x->nchaves++;
  return x;
}


TARVBMT *insere_nao_completo(TARVBMT *x, int mat, int t){
  int i = x->nchaves-1;
  if(x->folha){
    while((i>=0) && (mat < x->chave[i])){
      x->chave[i+1] = x->chave[i];
      i--;
    }
    x->chave[i+1] = mat;
    x->nchaves++;
    return x;
  }
  while((i>=0) && (mat < x->chave[i])) i--;
  i++;
  if(x->filho[i]->nchaves == ((2*t)-1)){
    x = divisao(x, (i+1), x->filho[i], t);
    if(mat > x->chave[i]) i++;
  }
  x->filho[i] = insere_nao_completo(x->filho[i], mat, t);
  return x;
}

TARVBMT *TARVBMT_insere(TARVBMT *T, TT tenista, int t){
  if(TARVBMT_busca(T, mat)) return T;
  if(!T){
    T=TARVBMT_cria(t);
    T->chave[0] = mat;
    T->nchaves=1;
    return T;
  }
  if(T->nchaves == (2*t)-1){
    TARVBMT *S = TARVBMT_cria(t);
    S->nchaves=0;
    S->folha = 0;
    S->filho[0] = T;
    S = divisao(S,1,T,t);
    S = insere_nao_completo(S, mat, t);
    return S;
  }
  T = insere_nao_completo(T, mat, t);
  return T;
}


TARVBMT* remover(TARVBMT* arv, int ch, int t){
  if(!arv) return arv;
  int i;
  for(i = 0; i < arv->nchaves && arv->chave[i] < ch; i++);
  if((i < arv->nchaves) && (ch == arv->chave[i]) && (arv->folha)){ //CASO 1
    printf("\nCASO 1\n");
    int j;
    for(j=i; j<arv->nchaves-1;j++) arv->chave[j] = arv->chave[j+1];
    arv->nchaves--;
    if(!arv->nchaves){ //ultima revisao: 04/2020
      TARVBMT_libera(arv);
      arv = NULL;
    }
    return arv;      
  }

  if((i < arv->nchaves) && (ch == arv->chave[i])) i++;
  TARVBMT *y = arv->filho[i], *z = NULL;
  if (y->nchaves == t-1){ //CASOS 3A e 3B
    if((i < arv->nchaves) && (arv->filho[i+1]->nchaves >=t)){ //CASO 3A
      printf("\nCASO 3A: i menor que nchaves\n");
      z = arv->filho[i+1];
      if(!y->folha){
        y->chave[t-1] = arv->chave[i];   //dar a y a chave i da arv
        arv->chave[i] = z->chave[0];     //dar a arv uma chave de z
      }
      else{ 
        arv->chave[i] = z->chave[0] + 1;
        y->chave[t-1] = z->chave[0];
      }
      y->nchaves++;

      int j;
      for(j=0; j < z->nchaves-1; j++)  //ajustar chaves de z
        z->chave[j] = z->chave[j+1];
      y->filho[y->nchaves] = z->filho[0]; //enviar ponteiro menor de z para o novo elemento em y
      for(j=0; j < z->nchaves; j++)       //ajustar filhos de z
        z->filho[j] = z->filho[j+1];
      z->nchaves--;
      arv->filho[i] = remover(arv->filho[i], ch, t);
      return arv;
    }
    if((i > 0) && (!z) && (arv->filho[i-1]->nchaves >= t)){ //CASO 3A
      printf("\nCASO 3A: i igual a nchaves\n");
      z = arv->filho[i-1];
      int j;
      for(j = y->nchaves; j>0; j--)               //encaixar lugar da nova chave
        y->chave[j] = y->chave[j-1];
      for(j = y->nchaves+1; j>0; j--) //encaixar lugar dos filhos da nova chave
        y->filho[j] = y->filho[j-1];

      if(!y->folha){
        y->chave[0] = arv->chave[i-1];    //dar a y a chave i da arv
        arv->chave[i-1] = z->chave[z->nchaves - 1];   //dar a arv uma chave de z
      }
      else{ 
        arv->chave[i-1] = z->chave[z->nchaves - 1];
        y->chave[0] = z->chave[z->nchaves-1];
      }
      y->nchaves++;

      //enviar ponteiro de z para o novo elemento em y
      y->filho[0] = z->filho[z->nchaves];
      z->nchaves--;
      arv->filho[i] = remover(y, ch, t);
      return arv;
    }
    if(!z){ //CASO 3B
      if(i < arv->nchaves && arv->filho[i+1]->nchaves == t-1){
        printf("\nCASO 3B: i menor que nchaves\n");
        z = arv->filho[i+1];
        if(!y->folha){
          y->chave[t-1] = arv->chave[i];//pegar chave [i] e coloca ao final de filho[i]
          y->nchaves++;
        }
        int j = 0;
        while(j < t-1){
          if(!y->folha) y->chave[t+j] = z->chave[j];
          else y->chave[t+j-1] = z->chave[j];
          y->nchaves++;
          j++;
        }
        y->prox = z->prox;
        if(!y->folha){
          for(j=0; j<t; j++){
            y->filho[t+j] = z->filho[j];
            z->filho[j] = NULL; //ultima revisao: 04/2020
          }
          //TARVBMT_libera(z); 07/2024
        }
        TARVBMT_libera(z); // 07/2024
        for(j=i; j < arv->nchaves-1; j++){ //limpar refer�ncias de i
          arv->chave[j] = arv->chave[j+1];
          arv->filho[j+1] = arv->filho[j+2];
        }
        arv->filho[arv->nchaves] = NULL;
        arv->nchaves--;
        if(!arv->nchaves){ //ultima revisao: 04/2020
          TARVBMT *temp = arv;
          arv = arv->filho[0];
          temp->filho[0] = NULL;
          TARVBMT_libera(temp);
        }
        arv = remover(arv, ch, t);
        return arv;
      }
      if((i > 0) && (arv->filho[i-1]->nchaves == t-1)){ 
        printf("\nCASO 3B: i igual a nchaves\n");
        z = arv->filho[i-1];
        if(!y->folha){
          if(i == arv->nchaves){
            z->chave[t-1] = arv->chave[i-1]; //pegar chave[i] e poe ao final de filho[i-1]
          }else{
            z->chave[t-1] = arv->chave[i];   //pegar chave [i] e poe ao final de filho[i-1]
          }
          z->nchaves++;
        }
        int j = 0;
        while(j < t-1){
          if(!y->folha) z->chave[t+j] = y->chave[j];
          else z->chave[t+j-1] = y->chave[j];
          z->nchaves++;
          j++;
        }
        z->prox = y->prox; 
        if(!z->folha){
          for(j=0; j<t; j++){
            z->filho[t+j] = y->filho[j];
            y->filho[j] = NULL; //ultima revisao: 04/2020
          }
          //TARVBMT_libera(y); 07/2024
        }
        TARVBMT_libera(y); // 07/2024
        arv->filho[arv->nchaves] = NULL;
        arv->nchaves--;
        if(!arv->nchaves){ //ultima revisao: 04/2020
          TARVBMT *temp = arv;
          arv = arv->filho[0];
          temp->filho[0] = NULL;
          TARVBMT_libera(temp);
          arv = remover(arv, ch, t);
        }
        else{
          i--;
          arv->filho[i] = remover(arv->filho[i], ch, t);
        }
        return arv;
      }
    }
  }  
  arv->filho[i] = remover(arv->filho[i], ch, t);
  return arv;
}


TARVBMT* TARVBMT_retira(TARVBMT* arv, int k, int t){
  if(!arv || !TARVBMT_busca(arv, k)) return arv;
  return remover(arv, k, t);
}

void TARVBMT_insere_MS (TT *tenista, char arq_ind, int t){ //WIP     //O primeiro numero será o numero de filhos
  FILE *f_ind = fopen(arq_ind, "rb+");
  if (!f_ind){ //Arvore não existe
    f_ind = fopen(arq_ind, "wb");
    if (!f_ind) {
      printf("Erro ao criar arquivo indice!\n");
      return;
    }

    int num_filhos = 1;
    char folha[] = "F0000.bin";

    fwrite(folha, sizeof(char), strlen(folha) + 1, f_ind);
    fwrite(0, sizeof(int), 1, f_ind);
    fwrite(num_filhos, sizeof(int), 1, f_ind);

    FILE *f_folha = fopen(folha, "wb");
    if (!f_folha) {
      printf("Erro ao criar o arquivo folha: %s", folha);
      return;
    }

    fwrite(tenista, sizeof(TT), 1, f_folha);
    fclose(f_folha);
    return;
  }

  //Caso a arvore já exista, faz a busca na arvore
  if (TARVBMT_busca_MS(f_ind, tenista->id)) {
    fclose(f_ind);
    return;
  }
  //Caso a o primeiro no da arvore tenha 2t-1 chaves:
  //Descobre qual chave tem que subir
  int num_chaves = 0, chave;
  char caractere;
  rewind(f_ind);
  fread(caractere, sizeof(char), 1, f_ind);
  
  //Se o arq indice começa com N, as informações estão no arquivo de indice
  if (caractere == 'N'){
    fseek(f_ind, sizeof(char)*5, SEEK_CUR);
    fread(&num_chaves, sizeof(int), 1, f_ind);
  }

  //Se o arq indice começa com F, as informações estão no arquivo folha, é necessário acessar
  else if (caractere == 'F'){
    rewind(f_ind);
    char folha[] = "F0000.bin";
    FILE *f_folha = fopen(folha, "rb");
    TT *temp;
    while (fread(temp, sizeof(TT), 1, f_folha)) num_chaves ++;
    if (num_chaves == (2*t)-1){
      rewind(f_folha);
      fseek(f_folha, sizeof(TT)*((num_chaves+1)/2)-1, SEEK_CUR);
      fread(&chave, sizeof(int), 1, f_folha);
    }
    free(temp);
    fclose(f_folha);
  }
  else printf("Erro: No inicio do arquivo indice não existe nó ou folha!\n");

  if (num_chaves == (2*t)-1){
    char arq_temp[50];  //Criar um arquivo temporario
    strcpy(arq_temp, "tmp_");
    strcat(arq_temp, arq_ind);
    FILE *f_tmp = fopen(arq_temp, "wb");

    if (caractere == 'N'){
      fseek(f_ind, sizeof(int)*(t-1), SEEK_CUR);  //Achar a chave t-1 que vai subir
      fread(&chave, sizeof(int), 1, f_ind);
    }
    
    fwrite("N0000", sizeof(char), 6, f_tmp);  //Escreve no novo arquivo a chave que subiu (Simula return S)
    fwrite(1, sizeof(int), 1, f_tmp);
    fwrite(chave, sizeof(int), 1, f_tmp);
    
    //Dificuldade em implementar o resto da inserção, fui implementar a Divisão para poder continuar
    //21/06/2025 21:37
  }
}

void insere_nao_completo_MS(FILE *f_indice, TT *tenista, int t){ //WIP
  
}

void divisao_MS(FILE *f_indice, int pos_pai, int i, int pos_y, int t){ //WIP
  //OBS: f_indice terá que ser aberto em rb+!!!!!!!!!!
  //pos_pai: posicao onde o pai se encontra no arquivo indice
  //i: posicao onde o novo filho será inserido
  //pos_y: posicao do nó que será dividido

  //y é folha?
  char no;
  int j, nchaves, *chaves = (int*)malloc(sizeof(int)*((t*2)-1));
  char **filhos, rotulo[10], aux[10];
  fread(no, sizeof(char), 1, f_indice);
  if (no != 'F'){
    nchaves = t-1;
    fseek(f_indice, pos_y, SEEK_SET); //anda até nó pra ser dividido
    fseek(f_indice, sizeof(char)*6, SEEK_CUR); //Pula o rótulo Nxxxx
    fwrite(nchaves, sizeof(int), 1, f_indice);  //Atualiza o numero de chaves
    fseek(f_indice, sizeof(int)*(t-1), SEEK_CUR); //Pula t-1 chaves 
    for(j=0;j<t-1;j++){
      fread(chaves[j], sizeof(int), 1, f_indice);
    }
    fseek(f_indice, -sizeof(int)*(t-1), SEEK_CUR);
    for(j=0;j<t-1;j++){
      fwrite(-1, sizeof(int), 1, f_indice);
    }

    for (j=0; j<t; j++){
    fread(rotulo, sizeof(char), 1, f_indice);
    if (rotulo == '\0') continue;
    
    }
  }
}