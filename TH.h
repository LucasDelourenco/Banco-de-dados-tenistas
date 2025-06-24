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
}THPl;
typedef struct linhaHashVencedoresDeTorneiosComAno{ //tipo 1
  int id, ano;
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

typedef THPl* THP[TAM0];
typedef THid* THNOM[TAM1];
typedef THVl* THV[TAM2];
typedef THid* THNAC[TAM3];
typedef THid* THT[TAM4];

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
1)ATPFinals
2)Olimpiedas
3)ATP1000


EX hash de PontuacaoPorAno (cada linha é o ano, e guarda o ID da pessoa e sua pontuacao)

*/

/*
HASHS

PontuacaoPorAno n = 35                (Q4) (Q3) (Q6!) (Q8)
Nome (alfabético a->linha 0)
VencedoresDeTiposDeTorneiosComAno     (Q1) (Q2) (Q5) 
Nacionalidade                         (Q2) (Q7)
VencedoresDosTorneios                 (Q9)

]*/