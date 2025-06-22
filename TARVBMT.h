#include <stdio.h>
#include <stdlib.h>



typedef struct tenistas{
  int id, ano_nascimento, morte, rank, YoBK, numSem, pontuacao;
  char nome[51], pais[51];
}TT;
//ID: [CPF(100-537)] [Nacion(10-59)] [AnoNasc(10-54)]
//       3 digs          2 digs          2 digs        =  7 digs total


//Títulos umas sequencia de 15 numeros dizendo se ja ganhou ou nao cada torneio

/*
typedef struct arvbmtenistas{
    char* arq;          //nome do arquivo do nó
    int folha;              //verifica se é folha ou não
    int nchaves;         //numero de chaves do nó
    unsigned long int* chave;   //id (unsigned long int) porque o id excede o INT_MAX
    char** filho;          //nomes dos arquivos dos filhos [lista de strings]
    char* prox;             //caso seja folha, tem o ponteiro para o nome do próximo arquivo
}TARVBMT;*/

typedef struct listaDeTenistas{
  TT tenista;
  struct listaDeTenistas *prox;
}TLSETT;


TARVBMT *TARVBMT_cria(int t);
TARVBMT *TARVBMT_inicializa(void);
TARVBMT *TARVBMT_busca(TARVBMT *a, int mat);
TARVBMT *TARVBMT_insere(TARVBMT *T, int mat, int t);
TARVBMT* TARVBMTretira(TARVBMT* arv, int k, int t);
void TARVBMT_libera(TARVBMT *a);
void TARVBMT_imprime(TARVBMT *a);
void TARVBMT_imprime_chaves(TARVBMT *a);

