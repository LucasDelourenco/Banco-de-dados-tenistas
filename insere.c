#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "TARVBMT.h"
#include "imprime.h"
#include "testeParaInsere.c"
#define t 2
/*
typedef struct tenistas{
  //tenistas.txt
  int id, ano_nascimento, morte, rank, YoBK, numSem;
  char nome[51], pais[51];  //nome é nome + sobrenome
  //champions.txt
  int pontuacao;
  int anoGanhouTodosGrands; //Q(5)
  int TorneiosGanhos[15];
}TT;
*/
void insere(const char *arq_indice, TT novo_atleta);

int cmp(const void *a, const void *b) {
    TT *tenistaA = (TT *)a;
    TT *tenistaB = (TT *)b;
    return (tenistaA->id - tenistaB->id);
}

int num_atletas_na_folha(FILE *fp_folha){
    if (!fp_folha) return 0;

    fseek(fp_folha, 0L, SEEK_END);// vai pro final do arq 
    long tam = ftell(fp_folha);// e retorna o quanto andou

    if (tam < sizeof(int)) {
        rewind(fp_folha);
        return 0;
    }
    
    // vai pro final do arq
    fseek(fp_folha, 0L, SEEK_END);
    long fim = ftell(fp_folha) - sizeof(int);
    fseek(fp_folha, fim, SEEK_SET);
    int n = 0;
    if (fread(&n, sizeof(int), 1, fp_folha) != 1){ // le a qntd de atletas
        rewind(fp_folha);
        printf("Erro ao ler a qntd de atletas na folha antiga\n");
        return 0; 
    }
    rewind(fp_folha);
    return n;
}

void insere_na_folha(const char *arq_indice, const char *arq_folha, TT novo_atleta, long pos_pai, int i){
    FILE *fp = fopen(arq_folha, "rb");
    if (!fp) {
        printf("Erro ao abrir o arquivo original para leitura(insere_na_folha)\n");
        return;
    }
    long tam_bloco = (sizeof(char) * 6) +          // rotulo
                    sizeof(int) +             // nchaves
                    (sizeof(int) * (2 * t - 1)) + // chaves
                    (sizeof(char) * 6 * (2 * t)); // filhos
    
    int num_atletas = num_atletas_na_folha(fp);
    
    if(num_atletas >= (2 * t - 1)){
        printf("\nFolha %s cheia. Chamando divisao...\n", arq_folha);
        fclose(fp);

        // Calcula a posiaoo do ponteiro para a folha a ser dividida.
        long offset_filhos = (sizeof(char) * 6) + sizeof(int) + (sizeof(int) * (2 * t - 1));
        long pos_ponteiro_filho = pos_pai + offset_filhos + (i * sizeof(char) * 6);
        
        divisao_MS(arq_indice, (i+1), pos_pai, pos_ponteiro_filho, t);
        
        // Apos a divisao, a arvore mudou. Reinicia o processo de insercaoo
        printf("Divisao completa. Reinserindo atleta ID %d.\n", novo_atleta.id);
        insere(arq_indice, novo_atleta);
        return;
    }
    
    TT *atletas = malloc(sizeof(TT) * (num_atletas + 1));
    if (fp && num_atletas > 0) {
        rewind(fp);
        fread(atletas, sizeof(TT), num_atletas, fp);
    }
    
    fclose(fp);
    atletas[num_atletas] = novo_atleta;
    qsort(atletas, num_atletas + 1, sizeof(TT), cmp); //ordena atletas
    
    //cria folha tmp
    char arq_tmp[51];
    sprintf(arq_tmp, "%s.tmp", arq_folha);
    FILE *fp_tmp = fopen(arq_tmp, "wb");
    if (!fp_tmp) {
        printf("Erro ao criar o arquivo temporario\n");
        free(atletas);
        return;
    }
    
    //escreve ordenado na nova folha
    fwrite(atletas, sizeof(TT), num_atletas + 1, fp_tmp);
    num_atletas++;
    fwrite(&num_atletas, sizeof(int), 1, fp_tmp);
    fclose(fp_tmp);
    free(atletas);

    // atualiza folha
    remove(arq_folha);
    rename(arq_tmp, arq_folha);

    printf("Atleta com ID %d inserido com sucesso em %s.\n", novo_atleta.id, arq_folha);
}



// A ideia eh gerar um pai para antiga raiz e dps chamar a divisao_MS no no antigo renomeado
void divisao_raiz(const char *arq_indice, long tam_bloco){
    FILE *fp_original = fopen(arq_indice, "rb+");
    if(!fp_original){
        printf("nao foi possivel abrir o arquivo original(divisao_raiz)\n");
        exit(1);
        return;
    }

    // Calcula tamanho do arq_indice
    fseek(fp_original, 0L, SEEK_END);
    long tamanho_arquivo = ftell(fp_original);
    long num_nos_internos = tamanho_arquivo / tam_bloco;

    // Varieveis para leitura da raiz antiga
    char rotulo_antiga_raiz[6];
    int nchaves_antiga_raiz;
    int chaves_antiga_raiz[2 * t - 1];
    char filhos_antiga_raiz[2 * t][6];

    // Leitura dos dados da antiga raiz
    fseek(fp_original, 0L, SEEK_SET);
    fread(rotulo_antiga_raiz, sizeof(char), 6, fp_original);
    fread(&nchaves_antiga_raiz, sizeof(int), 1, fp_original);
    fread(chaves_antiga_raiz, sizeof(int), (2 * t - 1), fp_original);
    fread(filhos_antiga_raiz, sizeof(char[6]), (2 * t), fp_original);

    // Criando arq indice tmp
    FILE *fp_tmp = fopen("INDICE.tmp", "wb");
     if (!fp_tmp) {
        printf("nao foi possivel criar o arquivo temporario(divisao_raiz)\n");
        fclose(fp_original);
        return;
    }

    // Variaveis para nova raiz
    char rotulo_nova_raiz[6];
    int nchaves_nova_raiz = 0;
    int chaves_nova_raiz[2 * t - 1];
    char filhos_nova_raiz[2 * t][6];
    char rotulo_copia_raiz_antiga[6];

    strcpy(rotulo_nova_raiz, "N0000");
    for(int j= 0; j < (2 * t - 1); j++) chaves_nova_raiz[j] = -1;

    // Filho 0 da nova raiz eh a antiga raiz
    sprintf(rotulo_copia_raiz_antiga, "N%04ld", num_nos_internos);// Cria um no com nome nao usado antes
    strcpy(filhos_nova_raiz[0], rotulo_copia_raiz_antiga); // A nova raiz aponta para este novo no
    for(int j = 1; j < (2 * t); j++) filhos_nova_raiz[j][0] = '\0'; //ANTES ERA j = 0

    // Escrevndo a nova raiz no arq_tmp
   fwrite(rotulo_nova_raiz, sizeof(char), 6, fp_tmp);
   fwrite(&nchaves_nova_raiz, sizeof(int), 1, fp_tmp);
   fwrite(chaves_nova_raiz, sizeof(int), (2 * t - 1), fp_tmp);
   fwrite(filhos_nova_raiz, sizeof(char[6]), (2 * t), fp_tmp);
   
   // Copia os demais nos
   if(num_nos_internos > 1){
        char buffer[tam_bloco];
        fseek(fp_original, tam_bloco, SEEK_SET); // Pula a raiz
        for(long i = 1; i < num_nos_internos; i++){
            fread(buffer, tam_bloco, 1, fp_original);
            fwrite(buffer,tam_bloco, 1, fp_tmp);
        }
   }

   // A raiz antiga vai receber novo rotulo e vai para o final do arq
   strcpy(rotulo_antiga_raiz, rotulo_copia_raiz_antiga); 
   fwrite(rotulo_antiga_raiz, sizeof(char), 6, fp_tmp);
   fwrite(&nchaves_antiga_raiz, sizeof(int), 1, fp_tmp);
   fwrite(chaves_antiga_raiz, sizeof(int), (2 * t - 1), fp_tmp);
   fwrite(filhos_antiga_raiz, sizeof(char[6]), (2 * t), fp_tmp);

   // Pega a posicao da copia da raiz e atualiza indice
   long pos_copia_raiz = num_nos_internos * tam_bloco;
   fclose(fp_original);
   fclose(fp_tmp);
   remove(arq_indice);
   rename("INDICE.tmp", arq_indice);

   // Antiga raiz eh folha e seu pai (nova raiz) esta na posicao 0
   
   divisao_MS(arq_indice, 1, 0, pos_copia_raiz, t);
}

void insere(const char *arq_indice, TT novo_atleta){
    FILE *fp_indice = fopen(arq_indice, "rb");
    if(!fp_indice) {
        perror("Erro ao abrir arquivo de indice");
        exit(1);
    }
    //if(TARVBMT_Busca(arq_indice, novo_atlta.id, t)) return;
    
    long tam_bloco = (sizeof(char) * 6) +          // rotulo
                    sizeof(int) +             // nchaves
                    (sizeof(int) * (2 * t - 1)) + // chaves
                    (sizeof(char) * 6 * (2 * t)); // filhos
    
    // Variaveis para ler os dados do no
    char id_no[6];
    int nchaves_no;
    int chaves_no[2 * t - 1];
    char filhos_no[2 * t][6];

    // Lendo raiz
    int nchaves_raiz;
    fseek(fp_indice, sizeof(char) * 6, SEEK_SET); // Pula o rotulo para ler nchaves
    fread(&nchaves_raiz, sizeof(int), 1, fp_indice);

    if(nchaves_raiz == 2 * t -1){ // raiz cheia
        fclose(fp_indice);
        printf("Raiz cheia, chamando divisao_raiz\n");
        divisao_raiz(arq_indice, tam_bloco);
        fp_indice = fopen(arq_indice, "rb+");
    }

    // Localizadores para caminhar no arq_ind
    long ptr_atual = 0L;
    long ptr_filho = -1L;
    int i = 0;
    int erro = 0;
    while(1){

        // Le o no atual campo a campo
        fseek(fp_indice, ptr_atual, SEEK_SET);
        if(fread(id_no, sizeof(char), 6, fp_indice) != 6){
            erro = 1;
            printf("Erro leitura campo id (insere)\n");
            break;
        }
        if(fread(&nchaves_no, sizeof(int), 1, fp_indice) != 1){
            printf("Erro leitura campo nchaves (insere)\n");
            erro = 1;
            break;
        }
        if(fread(chaves_no, sizeof(int), (2 * t - 1), fp_indice) != (2 * t - 1)){
            printf("Erro leitura campo chaves (insere)\n");
            erro = 1;
            break;
        }
        if(fread(filhos_no, sizeof(char[6]), (2 * t), fp_indice) != (2 * t)){
            printf("Erro leitura campo filhos (insere)\n");
            erro = 1;
            break;
        }

        // Encontra o caminho a seguir no no atual
        i = 0;
        while(i < nchaves_no && novo_atleta.id >= chaves_no[i]){
            i++;
        }

        // Verifica se o filho eh FOLHA.
        char *id_filho = filhos_no[i];
        if(strncmp(id_filho, "F", 1) == 0){
            break; // Encontramos o ponteiro para a folha correta. Sai do while
        }
        
        // Olha para o no filho e confere se esta cheio
        long id_num_filho = atol(id_filho + 1);
        ptr_filho = tam_bloco * id_num_filho;
        int nchaves_filho;
        fseek(fp_indice, ptr_filho + sizeof(char) * 6, SEEK_SET);
        if(fread(&nchaves_filho, sizeof(int), 1, fp_indice) != 1){
            printf("Erro leitura campo nchaves filho (insere)\n");
            erro = 1;
            break;
        }
        
        // Verifica se o no filho esta cheio
        if(nchaves_filho == (2 * t - 1)){
            printf("No: %s  cheio, chamando divisao_MS\n", id_no);
            fclose(fp_indice);
            divisao_MS(arq_indice, (i+1), ptr_atual, ptr_filho, t);
            fp_indice = fopen(arq_indice, "rb+");
            if (!fp_indice) {
                printf("Erro ao reabrir indice apos divisao (insere)\n");
                erro = 1;
                break;
            }
            ptr_atual = 0L; // Reinicia a busca, visto que a arvore fora modificada
            continue;
        }else{ // Se nao estiver cheio, continuamos descendo
            ptr_atual = ptr_filho;
        }
    }
    if(erro){
        fclose(fp_indice);
        return; // Se houve erro, nao pode continuar
    }
    
    //  O loop terminou. 'ptr_atual' aponta para o pai da folha correta.
    char *id_folha_final = filhos_no[i];
    char cam_folha[51];
    sprintf(cam_folha, "infos/%s.bin", id_folha_final);
    
    fclose(fp_indice);
    printf("Inserindo na folha: %s", cam_folha);
    insere_na_folha(arq_indice, cam_folha, novo_atleta, ptr_atual, i);
}



//
//
//
//testando o insere
//
//
//

int main(void){
    TT tenista = TT_cria_vazio();
    int continua;

    tenista.id = 1001010;
    strcpy(tenista.nome,"Gustavo Um");
    insere("INDICES.bin",tenista);
    imprime("INDICES.bin",t);
    scanf("%d",&continua);

    tenista.id = 1254264;
    strcpy(tenista.nome,"Pedro Dois");
    insere("INDICES.bin",tenista);
    imprime("INDICES.bin",t);
    scanf("%d",&continua);

    tenista.id = 1302011;
    strcpy(tenista.nome,"Lucas Tres");
    insere("INDICES.bin",tenista);
    imprime("INDICES.bin",t);
    scanf("%d",&continua);

    tenista.id = 1607217;
    strcpy(tenista.nome,"Mamao Quatro");
    insere("INDICES.bin",tenista);
    imprime("INDICES.bin",t);
    scanf("%d",&continua);

    tenista.id = 5702010;
    strcpy(tenista.nome,"Emanuel Cinco");
    insere("INDICES.bin",tenista);
    imprime("INDICES.bin",t);
    scanf("%d",&continua);

    tenista.id = 1572044;
    strcpy(tenista.nome,"Ryan Gosling Seis");
    insere("INDICES.bin",tenista);
    imprime("INDICES.bin",t);
    scanf("%d",&continua);

    tenista.id = 2805341;
    strcpy(tenista.nome,"Henrique Sete");
    insere("INDICES.bin",tenista);
    imprime("INDICES.bin",t);
    scanf("%d",&continua);

    tenista.id = 8351729;
    strcpy(tenista.nome,"Paulao oito");
    insere("INDICES.bin",tenista);
    imprime("INDICES.bin",t);
    scanf("%d",&continua);
    
    return 0;
}
