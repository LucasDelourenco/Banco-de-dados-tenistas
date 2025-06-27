#include <stdio.h>
#include <stdlib.h>

/*
typedef struct linhaHashPontuacaoPorAno{ //tipo 0
  int id, pontuacao;
  struct linhaHashPontuacaoPorAno *prox;
}THpts;
typedef struct linhaHashVencedoresDeTorneiosComAno{ //tipo 1
  int id, ano, qtdNoAno;
  struct linhaHashVencedoresDeTorneiosComAno *prox;
}THVl;*/
typedef struct linhaHashTenistas{ 
  int id, prox, status;
}THT; //Thid
typedef struct listaEncadeadatht{
  THT elemHash;
  struct listaEncadeadatht *prox;
}TLSETHT;

void TH_inicializa(char *tabHash, char *dados, int n);
THT* TH_busca(char *tabHash, char *dados, int n, int mat);
float TH_retira(char *tabHash, char *arq, int n, int mat);
void TH_insere(char *tabHash, char *arq, int n, int mat, float cr);
void TH_imprime (char *nome_hash, char *nome_dados, int m);



/*
Ex hash de vencedores de torneios COM ANO  (guarda o id de quem ganhou E o ANO!)

0)GrandSlam
1)ATPFinals
2)Olimpiedas
3)ATP1000


EX hash de RankingPorAno  (cada linha é o ano)

EX VencedoresDosTorneios  (cada linha é um dos 15 torneios)

*/

/*

HASHS

RankingPorAno n = 35                  (Q4) (Q3) (Q6!) (Q8)
Nome (alfabético a->linha 0)
VencedoresDeTiposDeTorneiosComAno    -(Q1)-(Q2) (Q5) 
Nacionalidade                         (Q2) (Q7)
VencedoresDosTorneios                 (Q9)


typedef THpts* THP[35];             typedef THT* THP[35];
typedef THid* THNOM[26];            typedef THT* THNOM[26];
typedef THVl* THV[4];        ->     typedef THT* THV[4];
typedef THid* THNAC[50];            typedef THT* THNAC[50];
typedef THpts* THVT[15];             typedef THT* THT[15];

]*/