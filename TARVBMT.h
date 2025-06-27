#include <stdio.h>
#include <stdlib.h>



typedef struct tenistas{
  //tenistas.txt
  int id, ano_nascimento, morte, rank, YoBK, numSem;
  char nome[51], pais[51];  //nome é nome + sobrenome
  //champions.txt
  int pontuacao;
  int anoGanhouTodosGrands; //(Q5)
  //int anoVenceuGrands[35]; //(Q2) usar? vamos decidir depois
  int TorneiosGanhos[15];
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

/*

→ quantos jogadores aposentados possuem títulos? E quantos ativos? Informe-os
separadamente em ordem decrescente do número de títulos. Os títulos de grand slams devem ser
apresentados primeiro, seguido dos de ATP 1000, ATP Finals e Olimpíadas. A ordem dos torneios
deve ser respeitada: os grand slams e os ATP 1000 aparecem em ordem no arquivo de entrada; e
O ATP Finals é o último torneio do ano e só participam os OITO melhores da temporada.


→ se existem jogadores que nasceram no mesmo ano que seu compatriota (que possui um
ranking) ganhou algum grand slam?


→ se ninguém tivesse se aposentado ainda, qual seria a pontuação acumulada de cada um dos
jogadores, em ordem decrescente?


→ indique o ranking (até 25) por ano, de acordo com a pontuação obtida em cada ano.


→ se existe algum jogador que ganhou todos os grand slams no mesmo ano? Se sim, retornar
o(s) nome(s) do(s) jogador(es) e o(s) ano(s).


→ se existe algum jogador que “furou” o ranking, isto é, ganhou algum torneio, mas não estava no
ranking (até 25) para:
(a) grand slams;
(b) ATP 1000;
(c) ATP Finals; e
(d) Olimpíadas.


→ retirar todos os jogadores em atividade ou não de um determinado país.


→ indique os OITO melhores jogadores do ano, que, a princípio, participaram de cada ATP Finals,
a partir de 1991, de acordo com a pontuação obtida no ano corrente. Se os jogadores tiverem as
mesmas pontuações, os grand slams devem ser considerados primeiro, depois os ATP 1000. Se
ainda ocorrerem empates, considere a pontuação do ano anterior. Em 1990, é sabido que os
jogadores que disputaram o ATP Finals foram Stefan Edberg, André Agassi, Pete Sampras, Emílio
Sanchez, Boris Becker, Ivan Lendl, Andrés Goméz e Thomas Muster.


→ retornar todos os vencedores, em ordem decrescente, por torneio. Se ocorrem empates,
ordene-os por nome em ordem crescente.
IDEIA: primeiro ordena em ordem crescente por nome, depois por descrecente de pontos!

*/