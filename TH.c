#include <float.h>
#include "TH.h"

#define TAM0 35
#define TAM1 26  
#define TAM2 4
#define TAM3 50
#define TAM4 15

/*
Ideias para extra:
-Poder avancar o ano atual, fazendo novos tenistas se aposentarem
*/

/*
TIPOS DE HASH:
0) Aposentados -> n = 2        (0 -> nao aposentado, 1->aposentado)
1) AnoNascimento -> n = 45
2) Nascionalidade -> n = 50 
3) Pontuacao -> n = 89
4) Mortos -> n = 2         (0 -> vivo, 1 -> morto)

sem tipo (aka tipo -1)
 Na hash de morto a lógica será feita direto no "TH_Insere"
\) VencedoresTorneios -> n = 15

ID: [CPF(100-537)] [Nacion(10-59)] [AnoNasc(10-54)] [Titulos(15)]
       3 digs          2 digs          2 digs        15 digs      =  22 digs total
*/

//HASHS
int THP_hash(int ano){
  return ano-1960;
}
int THNOM_hash(char nome[51]){
  if(strncmp(nome,"a",1)==0 || strncmp(nome,"A",1)==0) return 0;
  else if(strncmp(nome,"b",1)==0 || strncmp(nome,"B",1)==0) return 1;
  else if(strncmp(nome,"c",1)==0 || strncmp(nome,"C",1)==0) return 2;
  else if(strncmp(nome,"d",1)==0 || strncmp(nome,"D",1)==0) return 3;
  else if(strncmp(nome,"e",1)==0 || strncmp(nome,"E",1)==0) return 4;
  else if(strncmp(nome,"f",1)==0 || strncmp(nome,"F",1)==0) return 5;
  else if(strncmp(nome,"g",1)==0 || strncmp(nome,"G",1)==0) return 6;
  else if(strncmp(nome,"h",1)==0 || strncmp(nome,"H",1)==0) return 7;
  else if(strncmp(nome,"i",1)==0 || strncmp(nome,"I",1)==0) return 8;
  else if(strncmp(nome,"j",1)==0 || strncmp(nome,"J",1)==0) return 9;
  else if(strncmp(nome,"k",1)==0 || strncmp(nome,"K",1)==0) return 10;
  else if(strncmp(nome,"l",1)==0 || strncmp(nome,"L",1)==0) return 11;
  else if(strncmp(nome,"m",1)==0 || strncmp(nome,"M",1)==0) return 12;
  else if(strncmp(nome,"n",1)==0 || strncmp(nome,"N",1)==0) return 13;
  else if(strncmp(nome,"o",1)==0 || strncmp(nome,"O",1)==0) return 14;
  else if(strncmp(nome,"p",1)==0 || strncmp(nome,"P",1)==0) return 15;
  else if(strncmp(nome,"q",1)==0 || strncmp(nome,"Q",1)==0) return 16;
  else if(strncmp(nome,"r",1)==0 || strncmp(nome,"R",1)==0) return 17;
  else if(strncmp(nome,"s",1)==0 || strncmp(nome,"S",1)==0) return 18;
  else if(strncmp(nome,"t",1)==0 || strncmp(nome,"T",1)==0) return 19;
  else if(strncmp(nome,"u",1)==0 || strncmp(nome,"U",1)==0) return 20;
  else if(strncmp(nome,"v",1)==0 || strncmp(nome,"V",1)==0) return 21;
  else if(strncmp(nome,"w",1)==0 || strncmp(nome,"W",1)==0) return 22;
  else if(strncmp(nome,"x",1)==0 || strncmp(nome,"X",1)==0) return 23;
  else if(strncmp(nome,"y",1)==0 || strncmp(nome,"Y",1)==0) return 24;
  else if(strncmp(nome,"z",1)==0 || strncmp(nome,"Z",1)==0) return 25;
}
int THV_hash(int indiceTorneios){
  if(indiceTorneios <=3) return 0;
  else if(indiceTorneios==4) return 1;
  else if(indiceTorneios==5) return 2;
  return 3;
}
int THNAC_hash(int indicePais){
  return indicePais-10;
}
int THT_hash(int indiceTorneios){ //Wow
  return indiceTorneios;
}



void THP_inicializa_hashs(THP th){
  int i;
  for(i = 0; i < TAM0; i++)th[i] = NULL;
}
void THNOM_inicializa_hashs(THNOM th){
  int i;
  for(i = 0; i < TAM1; i++)th[i] = NULL;
}
void THV_inicializa_hashs(THV th){
  int i;
  for(i = 0; i < TAM2; i++)th[i] = NULL;
}
void THNAC_inicializa_hashs(THNAC th){
  int i;
  for(i = 0; i < TAM3; i++)th[i] = NULL;
}
void THT_inicializa_hashs(THT th){
  int i;
  for(i = 0; i < TAM4; i++)th[i] = NULL;
}

/*
FILE* TH_busca(TH tab, int tipo, int mat){
  int h = TH_hash(mat, n);
  TT *p = tab[h];
  while((p) && (p->mat < mat)) p = p->prox;
  if((p) && (p->mat == mat)) return p;
  return NULL;
}*/

THPl* THP_aloca(int id, int pontuacao){
  THPl *novo = (THPl*) malloc (sizeof (THPl));
  novo->id = id;
  novo->pontuacao = pontuacao;
  novo->prox = NULL;
  return novo;
}
THid* THNOM_aloca(int id){
  THid *novo = (THid*) malloc (sizeof (THid));
  novo->id = id;
  novo->prox = NULL;
  return novo;
}
THVl* THV_aloca(int id, int ano){
  THVl *novo = (THVl*) malloc (sizeof (THVl));
  novo->id = id;
  novo->ano = ano;
  novo->prox = NULL;
  return novo;
}
THid* THNAC_aloca(int id, int pontuacao){
  THid *novo = (THid*) malloc (sizeof (THid));
  novo->id = id;
  novo->prox = NULL;
  return novo;
}
THid* THT_aloca(int id, int pontuacao){
  THid *novo = (THid*) malloc (sizeof (THid));
  novo->id = id;
  novo->prox = NULL;
  return novo;
}

void THP_insere(THP tab, int id, int pontuacao, int ano){
  int h = THP_hash(ano);
  THPl *p = tab[h],*ant = NULL;
  while((p) && (p->pontuacao < pontuacao)){
    ant = p;
    p = p->prox;
  }
  if((p) && (p->pontuacao == pontuacao)){
    p->id = id;
    return;
  }
  THPl *novo = THP_aloca(id, pontuacao);
  novo->prox = p;
  if(!ant)tab[h] = novo;
  else ant->prox = novo;
}
//continuar daqui

void TH_libera(TH tab, int n){
  int i;
  for(i = 0; i < n; i++)
    if(tab[i]){
      TT *p = tab[i], *q;
      while(p){
        q = p;
        p = p->prox;
        free(q);
      }
    }
}

float TH_retira(TH tab, int n, int mat){
  int h = TH_hash(mat, n);
  if(!tab[h]) return FLT_MIN;
  TT *p = tab[h],*ant = NULL;
  float cr = FLT_MIN;
  while((p) && (p->mat < mat)){
    ant = p;
    p = p->prox;
  }
  if((!p) || (p->mat != mat)) return cr;
  if(!ant) tab[h] = p->prox;
  else ant->prox = p->prox;
  cr = p->cr;
  free(p);
  return cr;
}

void TH_imprime(TH tab, int n){
  int i;
  for(i = 0; i < n; i++){
    printf("%d: ", i);
    if(tab[i]){
      TT *p = tab[i];
      printf("\n");
      while(p){
        printf("\t%d\t%f\t%p\n", p->mat, p->cr, p->prox);
        p = p->prox;
      }
    }
    else printf("NULL\n");
  }
}
