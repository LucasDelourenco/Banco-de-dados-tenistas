#include <stdio.h>
#include <stdlib.h>
//#include "TARVBMT.h"

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

typedef struct tenistas{
  //tenistas.txt
  int id, ano_nascimento, morte, rank, YoBK, numSem;
  char nome[51], pais[51];  //nome é nome + sobrenome
  //champions.txt
  int pontuacao;
  int anoGanhouTodosGrands; //Q(5)
  int TorneiosGanhos[15];
}TT;

TT TT_cria_vazio();
TT TARVBMT_busca(int id,int t);

//int THP_hash(int ano);  //Substituido por matriz em MP
int THNOM_hash(char nome[51]); //funcionando
int THV_hash(int indiceTorneios);
int THNAC_hash(int id);
int THVT_hash(int indiceTorneios);

void TH_inicializa(char *tabHash, char *dados, int n);
void InicializaHashs();

//void THP_insere(int id, int pontuacao, int ano);
void THNOM_insere(int id, char nome[51]);
void THV_insere(int id, int indiceTorneios);
void THNAC_insere(int id);
void THVT_insere(int id, int indiceTorneios);

//TLSEid *THP_busca_primeiros_ateh_N_Do_Ano(int ano, int qtd_n);
TT THNOM_busca(char nome[51],int t);
TLSEid *THV_busca_lista_torneio(int indiceTorneio);
TLSEid *THNAC_busca(int indice_paises_no_arq);
TLSEid *THVT_busca(int indiceTorneios);

//Nao sao usados, mas quem sabe
/*
THTpts* THP_aloca(int id, int pontuacao);
THT* THNOM_aloca(int id);
THT* THV_aloca(int id, int ano, int qtd);
THT* THNAC_aloca(int id);
THT* THVT_aloca(int id);
*/

TLSEid *TLSEid_insere_inic(TLSEid *l, int id);
TLSEid *TLSEid_insere_fim(TLSEid *l, int id);

void adicionar_pontuacao(int id, int pontuacao_nova, int ano_evento);
void imprimir_top_25(int ano, int t);



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
typedef THid* THNAC[50];            typedef THT* THNAC[57];
typedef THpts* THVT[15];             typedef THT* THT[15];

]*/