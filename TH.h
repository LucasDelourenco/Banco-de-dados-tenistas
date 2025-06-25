#include <stdio.h>
#include <stdlib.h>
typedef struct tenistas{
  //tenistas.txt
  int id, ano_nascimento, morte, rank, YoBK, numSem;
  char nome[51], pais[51];  //nome é nome + sobrenome
  //champions.txt
  int pontuacao;
  int anoGanhouTodosGrands; //Q(5)
  int TorneiosGanhos[15];
}TT;


typedef struct linhaHashPontuacaoPorAno{ //tipo 0
  int id, pontuacao;
  struct linhaHashPontuacaoPorAno *prox;
}THpts;
typedef struct linhaHashVencedoresDeTorneiosComAno{ //tipo 1
  int id, ano, qtdNoAno;
  struct linhaHashVencedoresDeTorneiosComAno *prox;
}THVl;
typedef struct linhaHashSoId{ //tipo 2
  int id;
  struct linhaHashSoId *prox;
}THid;

/* ---  Hash  ---
Aposentados(2):  anoNascimento <= 1965 = aposentado
AnoNascimento(45): nasc - 1960 = hash
Nascionalidade(50):  
Vivo/morto(2):  0 -> morto, 1 -> vivo
Pontuacao(?): pont/1000 = hash               (pontuacao max: 88k)


*/
#define TAM 101
//
#define TAM0 35
#define TAM1 26  
#define TAM2 4
#define TAM3 50
#define TAM4 15
//

typedef THpts* THP[TAM0];
typedef THid* THNOM[TAM1];
typedef THVl* THV[TAM2];
typedef THid* THNAC[TAM3];
typedef THpts* THT[TAM4];



int THP_hash(int ano);
int THNOM_hash(char nome[51]);
int THV_hash(int indiceTorneios);
int THNAC_hash(int indicePais);
int THT_hash(int indiceTorneios);

void THP_inicializa_hashs(THP thp);
void THNOM_inicializa_hashs(THNOM thnom);
void THV_inicializa_hashs(THV thv);
void THNAC_inicializa_hashs(THNAC thnac);
void THT_inicializa_hashs(THT tht);
void inicializaTodasHashs(THP thp,THNOM thnom, THV thv, THNAC thnac, THT tht);

THpts* THP_aloca(int id, int pontuacao);
THid* THNOM_aloca(int id);
THVl* THV_aloca(int id, int ano, int qtd);
THid* THNAC_aloca(int id);
THpts* THT_aloca(int id, int pontuacao);

void THP_insere(THP tab, int id, int pontuacao, int ano);
void THNOM_insere(THNOM tab, int id,char nome[51]);
void THV_insere(THV tab, int id, int indiceTorneios, int ano);
void THNAC_insere(THNAC tab, int id);
void THT_insere(THT tab, int id, int indiceTorneios, int pontuacao);

void THP_libera(THP tab);
void THNOM_libera(THNOM tab);
void THV_libera(THV tab);
void THNAC_libera(THNAC tab);
void THT_libera(THT tab);

void THP_retira(THP tab, int id);
void THNOM_retira(THNOM tab, int id, char nome[51]);
void THV_retira(THV tab, int id);
void THNAC_retira(THNOM tab, int id);
void THT_retira(THP tab, int id);

/*
Ex hash de vencedores de torneios COM ANO  (guarda o id de quem ganhou E o ANO!)

0)GrandSlam
1)ATPFinals
2)Olimpiedas
3)ATP1000


EX hash de PontuacaoPorAno (cada linha é o ano, e guarda o ID da pessoa e sua pontuacao)

EX VencedoresDosTorneios  (cada linha é um dos 15 torneios, e gaurda o ID e a pontuacao da pessoa)

*/

/*

HASHS

PontuacaoPorAno n = 35                (Q4) (Q3) (Q6!) (Q8)
Nome (alfabético a->linha 0)
VencedoresDeTiposDeTorneiosComAno    -(Q1)-(Q2) (Q5) 
Nacionalidade                         (Q2) (Q7)
VencedoresDosTorneios                 (Q9)

]*/