#include <stdio.h>
#include <stdlib.h>


typedef struct linhaHashRankingPorAno{ //tipo 0
  int id, prox, status, pontuacao; //pontuacao usada só para ordem na hash, nao como informacao buscável!!!!
}THTpts;/*
typedef struct linhaHashVencedoresDeTorneiosComAno{ //tipo 1
  int id, ano, qtdNoAno;
  struct linhaHashVencedoresDeTorneiosComAno *prox;
}THVl;*/
typedef struct linhaHashTenistas{ 
  int id, prox, status;
}THT; //Thid
typedef struct listaEncadeadatht{
  int id;
  struct listaEncadeadatht *prox;
}TLSEid;


int THP_hash(int ano);
int THNOM_hash(char nome[51]);
int THV_hash(int indiceTorneios);
int THNAC_hash(int id);
int THVT_hash(int indiceTorneios);

void TH_inicializa(char *tabHash, char *dados, int n);
void InicializaHashs();

//Nao sao usados, mas quem sabe
THTpts* THP_aloca(int id, int pontuacao);
THT* THNOM_aloca(int id);
THT* THV_aloca(int id, int ano, int qtd);
THT* THNAC_aloca(int id);
THT* THVT_aloca(int id);

TLSEid *TLSEid_insere_inic(TLSEid *l, int id);
TLSEid *TLSEid_insere_fim(TLSEid *l, int id);



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