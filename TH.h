#include <stdio.h>
#include <stdlib.h>
typedef struct tenistas{
  int id, ano_nascimento, morte, rank, YoBK, numSem, pontuacao;
  char nome[51], pais[51];
}TT;
typedef struct tenistasNaHash{
  TT tenista;
  int vivo;
}TTH;

/* ---  Hash  ---
Aposentados(2):  anoNascimento <= 1965 = aposentado
AnoNascimento(45): nasc - 1960 = hash
Nascionalidade(50):  
Vivo/morto(2):  0 -> morto, 1 -> vivo
Pontuacao(?): pont/1000 = hash               (pontuacao max: 88k)


*/
//por enquanto nao optimizada
#define TAM 101
//
#define TAM1 50
#define TAM2 89    //(pontuacao max: 88k)(00 - 88)
#define TAM3 2
//

typedef TTH* TH[TAM];

int TH_hash(int mat, int n);
void TH_inicializa(TH tab, int n);
FILE* TH_busca(TH tab, int n, int mat); //retorna o arquivo em que o cara está
void TH_insere(TH tab, int n, int mat, float cr);
void TH_libera(TH tab, int n);
float TH_retira(TH tab, int n, int mat);
void TH_imprime(TH tab, int n);



/*
Ex hash de vencedores de torneios COM ANO  (guarda o id de quem ganhou E o ANO!)

0)GrandSlam
1)ATP1000
2)ATPFinals
3)Olimpiedas


EX hash de PontuacaoPorAno (cada linha é o ano, e guarda o ID da pessoa e sua pontuacao)

*/

/*
HASHS

PontuacaoPorAno n = 35                (Q4) (Q3) (Q6!) (Q8)
Nome (alfabético a->linha 0)
VencedoresDeTorneiosComAno            (Q1) (Q2) (Q5) (Q9)
Nacionalidade                         (Q2) (Q7)


]*/