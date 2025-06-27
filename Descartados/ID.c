//Aqui

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_LINHA 512
#define TAM_NOME 51
#define JOGADORES 600
#define NUM_NACOES 100

typedef struct id{
    long id_final;
    int id_birth, id_nation, id_name;
    char nome[51];
}ID;
ID globaal_id_jogadores[JOGADORES];
int globaal_total_de_jogadores = 0;

typedef struct nation{
    int id_nation, cont_jogador;
    char nome_pais[51];
}NATION;
int globaal_cont_nacao = 1;
NATION globaal_nation_map[NUM_NACOES]; // 'alocando espaço' para ate 100 nacoes

void gera_id(const char *arq){
    
    FILE *fp = fopen(arq, "r");
    if(!fp){
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    char linha[TAM_LINHA];
    while(fgets(linha, sizeof(linha), fp)){ 
        char *copia_da_linha = strdup(linha); // eh um malloc para linha

        char *nome_jogador = strtok(copia_da_linha, "\\"); // tipo um split(), mas que modifica a linha que foi passada
        char *nasc = strtok(NULL, "\\"); // passo null pois ela continua onde parou na chamada anterior
        strtok(NULL, "\\"); // pula morte
        char *nacao = strtok(NULL,"\\" );

        if(!nome_jogador || !nasc || !nacao){
            free(copia_da_linha);
            continue;
        }

        int id_nacao = -1;
        int id_do_mapa = -1;
        for(int i = 0; i < globaal_cont_nacao; i++){ // busca se o pais ja foi e inserido e qual eh o id dele
            if(strcmp(globaal_nation_map[i].nome_pais, nacao) == 0){
                id_do_mapa = i;
                id_nacao = globaal_nation_map[i].id_nation;
                break;
            }
        }

        if(id_do_mapa == -1){ // se a nacao eh nova
            id_do_mapa = globaal_cont_nacao;
            strcpy(globaal_nation_map[globaal_cont_nacao].nome_pais, nacao);
            globaal_nation_map[globaal_cont_nacao].id_nation = globaal_cont_nacao;
            globaal_nation_map[globaal_cont_nacao].cont_jogador = 0;
            id_nacao = globaal_cont_nacao;
            globaal_cont_nacao++;
        }

        if(id_do_mapa != -1){
            globaal_nation_map[id_do_mapa].cont_jogador++;
            int num_jogador = globaal_nation_map[id_do_mapa].cont_jogador;

            //prenche uma struct ID
            ID jogador_atual;
            strncpy(jogador_atual.nome, nome_jogador, 51);
            jogador_atual.nome[50] = '\0';
            jogador_atual.id_birth = atoi(nasc);
            jogador_atual.id_nation = id_nacao;
            jogador_atual.id_name = num_jogador;

            //criacao do ID
            int digito_ano = jogador_atual.id_birth - 1960;
            jogador_atual.id_final = (long)(jogador_atual.id_nation *100000L) + (long)(jogador_atual.id_name *100L) + digito_ano;

            globaal_id_jogadores[globaal_total_de_jogadores] = jogador_atual;
            globaal_total_de_jogadores++;

        }

        free(copia_da_linha); // num pode faltar, neh
    }
    fclose(fp);
}

int id_do_pais(const char *pais){
    for(int i = 0; i < globaal_cont_nacao; i++){
        if(strcmp(globaal_nation_map[i].nome_pais, pais) == 0){
            return i;
        }
    }
    return -1;
}

/*
//Ideia de funcao abortada e substituida por TARVBMT_criaPorTxt
TT Preenche_ID(TT tenista){
    //abrir o arq
    int id;
    //vetor de paises   char vet[nomePais,nomePais]
   


    //tenista.id = ;

    return tenista;
}
*/

