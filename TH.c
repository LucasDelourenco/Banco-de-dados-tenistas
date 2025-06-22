#include <float.h>
#include "TH.h"

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


int TH_hash(TT ten, int tipoHash){ 
  unsigned long int numId = ten.id;
  switch (tipoHash){
  case 0:
    numId%=100000000000000000;   //
    numId/=1000000000000000;     //Pegando apenas o numero da posicao AnoNasc
    if(numId<=36) return 1;     //considerando o ano atual sendo 2025, se 10 -> 1960, 36 -> 1986  e 2025 - 1986 = 39
    return 0;
  case 1:
    numId%=100000000000000000;   //
    numId/=1000000000000000;     //Pegando apenas o numero da posicao AnoNasc
    return numId-10;
  case 2:
    numId%=10000000000000000000; //
    numId/=100000000000000000;     //Pegando apenas o numero da posicao Nacionalidade
    return numId-10;
  case 3:
    return ten.pontuacao/1000;
  case 4:
    if(ten.morte !=00) return 1;
    return 0;
  default:
    printf("\nTipo da hash inválido: Definido em TH.c da uma olhada la"); //MENSAGEM DEBUG! RETIRAR!
    return -1;
  }
}

void TH_inicializa(TH tab, int n){
  int i;
  for(i = 0; i < n; i++)tab[i] = NULL;
}

FILE* TH_busca(TH tab, int tipo, int mat){
  int h = TH_hash(mat, n);
  TT *p = tab[h];
  while((p) && (p->mat < mat)) p = p->prox;
  if((p) && (p->mat == mat)) return p;
  return NULL;
}

TT* TH_aloca(int mat, float cr){
  TT *novo = (TT*) malloc (sizeof (TT));
  novo->mat = mat;
  novo->cr = cr;
  novo->prox = NULL;
  return novo;
}

void TH_insere(TH tab, int n, int mat, float cr){
  int h = TH_hash(mat, n);
  TT *p = tab[h],*ant = NULL;
  while((p) && (p->mat < mat)){
    ant = p;
    p = p->prox;
  }
  if((p) && (p->mat == mat)){
    p->cr = cr;
    return;
  }
  TT *novo = TH_aloca(mat, cr);
  novo->prox = p;
  if(!ant)tab[h] = novo;
  else ant->prox = novo;
}

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
