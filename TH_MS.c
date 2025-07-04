#include "TH_MS.h"
#include <ctype.h>
#include <string.h>

#define TAM0 35
#define TAM1 26  
#define TAM2 4
#define TAM3 50
#define TAM4 15

TT TT_cria_vazio(){
  TT novo;
  //tenistas.txt
  novo.id = -1;
  novo.ano_nascimento = -1;
  novo.morte = -1;
  novo.rank = -1;
  novo.YoBK = -1;
  novo.numSem = -1;
  strcpy(novo.nome,"\0");
  strcpy(novo.pais,"\0");
  //champions.txt
  novo.pontuacao = 0;
  novo.anoGanhouTodosGrands=-1;
  for(int i = 0; i<15; i++) novo.TorneiosGanhos[i] = 0;
  return novo;
}


TT TARVBMT_busca(int id,int t){ //funcionando corretamente
  FILE *fp = fopen("INDICES.bin","rb+");
  if(!fp) return TT_cria_vazio(); //exit(1); com problemas no ubuntu
  char identfNo[6], filho[6], temp[6];
  int numchaves,qtdLidos,i,pos,tamPorBloco = (sizeof(char)*6 + sizeof(int) + sizeof(int)*((2*t)-1) + (sizeof(char)*6)*(2*t));
  int chave;
  while(1){
    i=0;
    qtdLidos = fread(&identfNo,sizeof(char),6,fp);
    if(qtdLidos > 0){
      if(strncmp(identfNo,"N",1)==0){ //Se é um Nó
        fread(&numchaves,sizeof(int),1,fp);
        fread(&chave,sizeof(int),1,fp); 
        while((i < numchaves-1) && (id >= chave)){
          i++;
          fread(&chave,sizeof(int),1,fp); 
        }
        if(i==numchaves-1 && id>=chave){ //se é a ultima chave a ainda é maior, vai descer no filho + 1
          i++;
          fread(&chave,sizeof(int),1,fp); //lê o proximo para andar com o ponteiro tbb
        }
        pos = ftell(fp);
        fseek(fp,pos+(sizeof(int) * (t-i)), SEEK_SET);//pula até o começo dos filhos
        pos = ftell(fp);
        fseek(fp,pos+((sizeof(char)*6)*i),SEEK_SET);
        fread(&filho,sizeof(char),6,fp);
        if(filho[0]=='F'){//Se o filho for folha, o nome dele vai pra identfNo e sai do loop de procura
          strcpy(identfNo,filho);
          break;
        }//caso contrario, se o filho ainda for Nó, vai para ele e repete o argoritmo(até achar uma folha)
        strcpy(temp,&filho[1]); //Pega os os numeros do filho
        strcpy(filho,temp);     //Uso do temp para evitar possiveis erros
        pos = atoi(filho) * tamPorBloco;//pos recebe o valor que iremos pular (ex: se quero ir para o nó N0001 vou pular 1 bloco)
        fseek(fp,pos,SEEK_SET);
      }
      else //Achou uma folha
        break;
    }
    else{
      printf("\nIdentificacao do No nao encontrado(verificar TARVBMT_buscar  )\n");// PRINT DEBUG APAGAR FUTURAMENTE!!
      return TT_cria_vazio(); //tenista erro
    }
  }
  fclose(fp);
  char arqFolha[30];
  TT tenista;
  strcpy(arqFolha,"./infos/");   
  strcat(arqFolha,identfNo);
  strcat(arqFolha,".bin");
  fp = fopen(arqFolha,"rb+");
  qtdLidos = fread(&tenista,sizeof(TT),1,fp);
  while((qtdLidos>0) && (tenista.id != id)) qtdLidos = fread(&tenista,sizeof(TT),1,fp);
  if(qtdLidos<=0) return TT_cria_vazio(); //tenista erro
  return tenista;
}


/*
Ideias para extra:
-Poder avancar o ano atual, fazendo novos tenistas se aposentarem
*/

/*
TIPOS DE HASH:
0) Aposentados -> n = 2        (0 -> nao aposentado, 1->aposentado)
1) AnoNascimento -> n = 45
2) Nascionalidade -> n = 50 
3) Pontuacao -> n = 89
4) Mortos -> n = 2         (0 -> vivo, 1 -> morto)

sem tipo (aka tipo -1)
 Na hash de morto a lógica será feita direto no "TH_Insere"
\) VencedoresTorneios -> n = 15

ID: [CPF(100-537)] [Nacion(10-59)] [AnoNasc(10-54)] [Titulos(15)]
       3 digs          2 digs          2 digs        15 digs      =  22 digs total
*/

//
#define ANO_INICIAL 1990
#define ANO_FINAL 2024
#define NUM_JOGADORES 400
#define NUM_ANOS (ANO_FINAL - ANO_INICIAL + 1)

// <-- MUDANÇA: Novo arquivo para mapear índice para ID
const char* ARQUIVO_IDS = "./auxiliares/id_map.bin"; 
const char* ARQUIVO_SCORES = "./auxiliares/matrizRankingPorAno.bin";

typedef struct {
    int id;
    int pontuacao;
} JogadorScore;

// Função de comparação para o qsort (ordem decrescente de pontuação)
int comparar_scores(const void* a, const void* b) {
    JogadorScore* jogadorA = (JogadorScore*)a;
    JogadorScore* jogadorB = (JogadorScore*)b;
    return (jogadorB->pontuacao - jogadorA->pontuacao);
}
// Cria os arquivos de scores e de IDs com zeros. Executar uma vez.
void inicializar_matriz() { 
    // Inicializa arquivo de scores
    FILE *fp_scores = fopen(ARQUIVO_SCORES, "wb");
    if (!fp_scores) {
        perror("Nao foi possivel criar o arquivo de scores");
        exit(1);
    }
    int *zeros_scores = (int*) calloc(NUM_ANOS, sizeof(int));
    for (int i = 0; i < NUM_JOGADORES; ++i) {
        fwrite(zeros_scores, sizeof(int), NUM_ANOS, fp_scores);
    }
    free(zeros_scores);
    fclose(fp_scores);

    // <-- MUDANÇA: Inicializa arquivo de IDs com 0
    // Um ID 0 pode significar um "slot" de jogador vago.
    FILE *fp_ids = fopen(ARQUIVO_IDS, "wb");
    if (!fp_ids) {
        perror("Nao foi possivel criar o arquivo de IDs");
        exit(1);
    }
    int *zeros_ids = (int*) calloc(NUM_JOGADORES, sizeof(int));
    fwrite(zeros_ids, sizeof(int), NUM_JOGADORES, fp_ids);
    free(zeros_ids);
    fclose(fp_ids);

    printf("Arquivos de dados inicializados com sucesso.\n");
}
// <-- MUDANÇA: Nova função para encontrar/registrar um jogador
// Retorna o índice (0 a NUM_JOGADORES-1) para um dado ID.
// Se o jogador é novo, o registra no primeiro slot vago.
int obter_ou_criar_indice_por_id(int id) {
    FILE* fp = fopen(ARQUIVO_IDS, "rb+");
    if (!fp) {
        perror("Arquivo de IDs nao encontrado. Execute a inicializacao.");
        return -1; // Retorna erro
    }

    int indice_encontrado = -1;
    int primeiro_indice_vago = -1;
    int id_lido = 0;

    for (int i = 0; i < NUM_JOGADORES; ++i) {
        fread(&id_lido, sizeof(int), 1, fp);
        if (id_lido == id) {
            indice_encontrado = i;
            break; // Encontrou, pode parar de procurar
        }
        // Se encontrarmos um slot vago (ID 0) e ainda não tivermos um, guardamos
        if (id_lido == 0 && primeiro_indice_vago == -1) {
            primeiro_indice_vago = i;
        }
    }

    if (indice_encontrado != -1) {
        fclose(fp);
        return indice_encontrado;
    } else if (primeiro_indice_vago != -1) {
        // Jogador não encontrado, vamos registrá-lo no primeiro slot vago
        fseek(fp, primeiro_indice_vago * sizeof(int), SEEK_SET);
        fwrite(&id, sizeof(int), 1, fp);
        fclose(fp);
        return primeiro_indice_vago;
    } else {
        // Não encontrou o jogador e não há mais espaço
        fclose(fp);
        printf("Erro: Limite de jogadores (%d) atingido. Nao foi possivel adicionar jogador %d\n", NUM_JOGADORES, id);
        return -1;
    }
}
void adicionar_pontuacao(int id_jogador, int pontuacao_nova, int ano_evento) {
    if (ano_evento < ANO_INICIAL || ano_evento > ANO_FINAL) return;

    // <-- MUDANÇA: Obtém o índice real do jogador
    int indice_jogador = obter_ou_criar_indice_por_id(id_jogador);
    if (indice_jogador == -1) {
        return; // Erro ao obter índice (ex: limite de jogadores)
    }

    FILE* fp = fopen(ARQUIVO_SCORES, "rb+");
    if (!fp) {
        perror("Erro ao abrir arquivo de scores.");
        return;
    }

    // O resto da lógica permanece a mesma, mas usando o 'indice_jogador' correto
    for (int ano = ano_evento; ano <= ANO_FINAL; ++ano) {
        int indice_ano = ano - ANO_INICIAL;
        long offset = (long)(indice_jogador * NUM_ANOS + indice_ano) * sizeof(int);
        fseek(fp, offset, SEEK_SET);
        
        int pontuacao_antiga = 0;
        fread(&pontuacao_antiga, sizeof(int), 1, fp);
        
        int pontuacao_atualizada = pontuacao_antiga + pontuacao_nova;
        
        fseek(fp, offset, SEEK_SET);
        fwrite(&pontuacao_atualizada, sizeof(int), 1, fp);
    }

    fclose(fp);
}
void imprimir_top_25(int ano, int t) {
    if (ano < ANO_INICIAL || ano > ANO_FINAL) {
        printf("Ano invalido.\n");
        return;
    }

    // <-- MUDANÇA: Carrega o mapa de IDs para a memória
    int id_map[NUM_JOGADORES];
    FILE* fp_ids = fopen(ARQUIVO_IDS, "rb");
    if (!fp_ids) {
        perror("Arquivo de IDs nao encontrado");
        return;
    }
    fread(id_map, sizeof(int), NUM_JOGADORES, fp_ids);
    fclose(fp_ids);

    FILE* fp_scores = fopen(ARQUIVO_SCORES, "rb");
    if (!fp_scores) {
        perror("Arquivo de scores nao encontrado");
        return;
    }

    JogadorScore ranking_ano[NUM_JOGADORES];
    int indice_ano = ano - ANO_INICIAL;

    for (int i = 0; i < NUM_JOGADORES; ++i) {
        // Se o ID no mapa for 0, significa que este slot não foi usado, então pulamos
        if (id_map[i] == 0) {
            ranking_ano[i].id = 0;
            ranking_ano[i].pontuacao = 0;
            continue;
        }

        long offset = (long)(i * NUM_ANOS + indice_ano) * sizeof(int);
        fseek(fp_scores, offset, SEEK_SET);
        
        // <-- MUDANÇA: Atribui o ID REAL vindo do mapa
        ranking_ano[i].id = id_map[i]; 
        fread(&ranking_ano[i].pontuacao, sizeof(int), 1, fp_scores);
    }
    fclose(fp_scores);

    qsort(ranking_ano, NUM_JOGADORES, sizeof(JogadorScore), comparar_scores);

    printf("--- TOP 25 para o Ano de %d ---\n", ano);
    for (int i = 0; i < 25 && i < NUM_JOGADORES; ++i) {
        // Não exibe jogadores com pontuação 0, a menos que não haja outros
        if (ranking_ano[i].pontuacao == 0 || ranking_ano[i].id == 0) continue;
        
        // <-- MUDANÇA: Agora o printf usa o ID correto!
        printf("%2d. Jogador : %s - Pontuacao: %d\n", 
               i + 1, 
               TARVBMT_busca(ranking_ano[i].id,t).nome, 
               ranking_ano[i].pontuacao);
    }
    printf("--------------------------------\n");
}
//




//HASHS
int THP_hash(int ano){
  return ano-1990;
}
int THNOM_hash(char nome[51]) {
    char primLetra = tolower(nome[0]);
    if(primLetra >= 'a' && primLetra <= 'z')
        return primLetra - 'a';  // 'a' → 0, 'b' → 1, ..., 'z' → 25
    else
        return -1;  // Caso o caractere não seja uma letra
}
int THV_hash(int indiceTorneios){
  if(indiceTorneios <=3) return 0;
  else if(indiceTorneios==4) return 1;
  else if(indiceTorneios==5) return 2;
  return 3;
}
int THNAC_hash(int id){
  int hash = id%10000;
  hash /= 100;
  return hash-10;
}
int THVT_hash(int indiceTorneios){ //Wow
  return indiceTorneios;
}

void TH_inicializa(char *tabHash, char *dados, int n){
  FILE *fp = fopen(dados, "wb");
  if(!fp) exit(1);
  fclose(fp);
  fp = fopen(tabHash, "wb");
  if(!fp) exit(1);
  int i, elem = -1;
  for(i = 0; i < n; i++)fwrite(&elem, sizeof(int), 1, fp);
  fclose(fp);
}
void InicializaHashs(){
  remove("./hashs/THP.bin");
  remove("./hashs/THNOM.bin");
  remove("./hashs/THV.bin");
  remove("./hashs/THNAC.bin");
  remove("./hashs/THVT.bin");
  remove("./hashs/THP_dados.bin");
  remove("./hashs/THNOM_dados.bin");
  remove("./hashs/THV_dados.bin");
  remove("./hashs/THNAC_dados.bin");
  remove("./hashs/THVT_dados.bin");
  
  //TH_inicializa("./hashs/THP.bin","./hashs/THP_dados.bin",35);
  inicializar_matriz();
  TH_inicializa("./hashs/THNOM.bin","./hashs/THNOM_dados.bin",26);
  TH_inicializa("./hashs/THV.bin","./hashs/THV_dados.bin",4);
  TH_inicializa("./hashs/THNAC.bin","./hashs/THNAC_dados.bin",57);
  TH_inicializa("./hashs/THVT.bin","./hashs/THVT_dados.bin",15);
}

/*
THTpts* THP_aloca(int id, int pontuacao){
  THTpts *novo = (THTpts*) malloc (sizeof (THTpts));
  novo->id = id;
  novo->pontuacao = pontuacao;
  novo->prox = NULL;
  return novo;
}
THT* THNOM_aloca(int id){
  THT *novo = (THT*) malloc (sizeof (THT));
  novo->id = id;
  novo->prox = NULL;
  return novo;
}
THT* THV_aloca(int id, int ano, int qtd){
  THT *novo = (THT*) malloc (sizeof (THT));
  novo->id = id;
  //novo->qtdNoAno = qtd;
  //novo->ano = ano;
  novo->prox = NULL;
  return novo;
}
THT* THNAC_aloca(int id){
  THT *novo = (THT*) malloc (sizeof (THT));
  novo->id = id;
  novo->prox = NULL;
  return novo;
}
THT* THVT_aloca(int id){
  THT *novo = (THT*) malloc (sizeof (THT));
  novo->id = id;
  //novo->pontuacao = pontuacao;
  novo->prox = NULL;
  return novo;
}*/

TLSEid *TLSEid_insere_inic(TLSEid *l, int id){
  TLSEid *novo = (TLSEid *) malloc(sizeof(TLSEid));
  novo->prox = l;
  novo->id = id;
  return novo;
}

TLSEid *TLSEid_insere_fim(TLSEid *l, int id){
  if(!l) return TLSEid_insere_inic(NULL,id);
  l->prox = TLSEid_insere_fim(l->prox,id);
  return l;
}

//     //LEMBRAR DE LIBERAR!!!!
// TLSEid *THP_busca_primeiros_ateh_N_Do_Ano(int ano, int qtd_n){ //Q4 e Q3 //Funcionando!
//   FILE *fp = fopen("./hashs/THP.bin","rb");
//   if(!fp)exit(1);
//   int pos, h = THP_hash(ano);
//   fseek(fp, h*sizeof(int), SEEK_SET);
//   fread(&pos, sizeof(int), 1, fp);
//   fclose(fp);
//   if(pos == -1)return NULL;
//   fp = fopen("./hashs/THP_dados.bin","rb");
//   if(!fp) exit(1);
//   fseek(fp, pos, SEEK_SET);
//   THTpts aux;
//   fread(&aux, sizeof(THTpts), 1, fp);

//   int i = 0; //quase um for
//   TLSEid *lista = NULL; //inicializando TLSEid
//   while(i < qtd_n){ //antes era 25 só para Q4
//     if(aux.status){
//       lista = TLSEid_insere_fim(lista,aux.id);
//       i++;
//     }
//     if(aux.prox == -1){
//       fclose(fp);
//       return lista;
//     }
//     fseek(fp, aux.prox, SEEK_SET);
//     fread(&aux, sizeof(THTpts), 1, fp);
//   }
//   fclose(fp);
//   return lista;
// }
//ja retorna o cara usando TARVBMT_busca
TT THNOM_busca(char nome[51],int t){ //Ou ponteiro para poder retornar NULL
  FILE *fp = fopen("./hashs/THNOM.bin","rb");
  if(!fp)exit(1);
  int pos, h = THNOM_hash(nome);
  fseek(fp, h*sizeof(int), SEEK_SET);
  fread(&pos, sizeof(int), 1, fp);
  fclose(fp);
  if(pos == -1)return TT_cria_vazio();
  fp = fopen("./hashs/THNOM_dados.bin","rb");
  if(!fp) exit(1);
  fseek(fp, pos, SEEK_SET);
  THT aux;
  TT tenista;
  fread(&aux, sizeof(THT), 1, fp);
  while(1){
    tenista = TARVBMT_busca(aux.id,t);
    if((strcmp(tenista.nome,nome)==0) && (aux.status)){
      fclose(fp);
      return tenista;
    }
    if(aux.prox == -1){
      fclose(fp);
      return TT_cria_vazio();
    }
    fseek(fp, aux.prox, SEEK_SET);
    fread(&aux, sizeof(THT), 1, fp);
  }
}
TLSEid *THV_busca_lista_torneio(int indiceTorneio){
  FILE *fp = fopen("./hashs/THV.bin","rb");
  if(!fp)exit(1);
  int pos, h = THV_hash(indiceTorneio);
  fseek(fp, h*sizeof(int), SEEK_SET);
  fread(&pos, sizeof(int), 1, fp);
  fclose(fp);
  if(pos == -1)return NULL;
  fp = fopen("./hashs/THV_dados.bin","rb");
  if(!fp) exit(1);
  fseek(fp, pos, SEEK_SET);
  THT aux;
  TLSEid *lista = NULL;
  fread(&aux, sizeof(THT), 1, fp);
  while(1){
    if(aux.status)lista = TLSEid_insere_fim(lista, aux.id);
    if(aux.prox == -1){
      fclose(fp);
      return lista;
    }
    fseek(fp, aux.prox, SEEK_SET);
    fread(&aux, sizeof(THT), 1, fp);
  }
}
//Printar o arquivo de paises e pedir para o usuario escolher por numero
TLSEid *THNAC_busca(int indice_paises_no_arq){
  FILE *fp = fopen("./hashs/THNAC.bin","rb");
  if(!fp)exit(1);
  int pos, h = indice_paises_no_arq;
  fseek(fp, h*sizeof(int), SEEK_SET);
  fread(&pos, sizeof(int), 1, fp);
  fclose(fp);
  if(pos == -1)return NULL;
  fp = fopen("./hashs/THNAC_dados.bin","rb");
  if(!fp) exit(1);
  fseek(fp, pos, SEEK_SET);
  THT aux;
  TLSEid *lista = NULL;
  fread(&aux, sizeof(THT), 1, fp);
  while(1){
    if(aux.status)lista = TLSEid_insere_fim(lista, aux.id);
    if(aux.prox == -1){
      fclose(fp);
      return lista;
    }
    fseek(fp, aux.prox, SEEK_SET);
    fread(&aux, sizeof(THT), 1, fp);
  }
}
TLSEid *THVT_busca(int indiceTorneios){
  FILE *fp = fopen("./hashs/THVT.bin","rb");
  if(!fp)exit(1);
  int pos, h = THVT_hash(indiceTorneios);
  fseek(fp, h*sizeof(int), SEEK_SET);
  fread(&pos, sizeof(int), 1, fp);
  fclose(fp);
  if(pos == -1)return NULL;
  fp = fopen("./hashs/THVT_dados.bin","rb");
  if(!fp) exit(1);
  fseek(fp, pos, SEEK_SET);
  THT aux;
  TLSEid *lista = NULL;
  fread(&aux, sizeof(THT), 1, fp);
  while(1){
    if(aux.status)lista = TLSEid_insere_fim(lista, aux.id);
    if(aux.prox == -1){
      fclose(fp);
      return lista;
    }
    fseek(fp, aux.prox, SEEK_SET);
    fread(&aux, sizeof(THT), 1, fp);
  }
}




// void THP_retira(int id){
//   FILE *fp;
//   int pos;
//   for(int i=0; i<35; i++){
//     fp = fopen("./hashs/THP.bin","rb+");
//     if(!fp) exit(1);
//     fseek(fp, i*sizeof(int), SEEK_SET);
//     fread(&pos, sizeof(int), 1, fp);
//     fclose(fp);
//     if(pos == -1)continue;
//     fp = fopen("./hashs/THP_dados.bin","rb+");
//     if(!fp)exit(1);
//     THTpts aux;
//     while(1){
//       fseek(fp, pos, SEEK_SET);
//       fread(&aux, sizeof(THTpts), 1, fp);
//       if((aux.id == id) && (aux.status)){ //achou
//         aux.status = 0;
//         fseek(fp, pos, SEEK_SET);
//         fwrite(&aux, sizeof(THTpts), 1, fp);
//         fclose(fp);
//         break;
//       }
//       if(aux.prox == -1){ //nao achou
//         fclose(fp);
//         break;
//       }
//       pos = aux.prox;
//     }
//   }
// }
void THNOM_retira(int id, char nome[51]){
  FILE *fp = fopen("./hashs/THNOM.bin","rb+");
  if(!fp) exit(1);
  int pos, h = THNOM_hash(nome);
  fseek(fp, h*sizeof(int), SEEK_SET);
  fread(&pos, sizeof(int), 1, fp);
  fclose(fp);
  if(pos == -1) return;
  fp = fopen("./hashs/THNOM_dados.bin","rb+");
  if(!fp)exit(1);
  THT aux;
  while(1){
    fseek(fp, pos, SEEK_SET);
    fread(&aux, sizeof(THT), 1, fp);
    if((aux.id == id) && (aux.status)){
      aux.status = 0;
      fseek(fp, pos, SEEK_SET);
      fwrite(&aux, sizeof(THT), 1, fp);
      fclose(fp);
      return;
    }
    if(aux.prox == -1){
      fclose(fp);
      return;
    }
    pos = aux.prox;
  }
}
void THV_retira(int id){
  FILE *fp;
  int pos;
  for(int i=0; i<4; i++){
    fp = fopen("./hashs/THV.bin","rb+");
    if(!fp) exit(1);
    fseek(fp, i*sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fp);
    fclose(fp);
    if(pos == -1)continue;
    fp = fopen("./hashs/THV_dados.bin","rb+");
    if(!fp)exit(1);
    THT aux;
    while(1){
      fseek(fp, pos, SEEK_SET);
      fread(&aux, sizeof(THT), 1, fp);
      if((aux.id == id) && (aux.status)){
        aux.status = 0;
        fseek(fp, pos, SEEK_SET);
        fwrite(&aux, sizeof(THT), 1, fp);
        fclose(fp);
        break;
      }
      if(aux.prox == -1){
        fclose(fp);
        break;
      }
      pos = aux.prox;
    }
  }
}
void THNAC_retira(int id){
  FILE *fp = fopen("./hashs/THNAC.bin","rb+");
  if(!fp) exit(1);
  int pos, h = THNAC_hash(id);
  fseek(fp, h*sizeof(int), SEEK_SET);
  fread(&pos, sizeof(int), 1, fp);
  fclose(fp);
  if(pos == -1) return;
  fp = fopen("./hashs/THNAC_dados.bin","rb+");
  if(!fp)exit(1);
  THT aux;
  while(1){
    fseek(fp, pos, SEEK_SET);
    fread(&aux, sizeof(THT), 1, fp);
    if((aux.id == id) && (aux.status)){
      aux.status = 0;
      fseek(fp, pos, SEEK_SET);
      fwrite(&aux, sizeof(THT), 1, fp);
      fclose(fp);
      return;
    }
    if(aux.prox == -1){
      fclose(fp);
      return;
    }
    pos = aux.prox;
  }
}
void THVT_retira(int id){
  FILE *fp;
  int pos;
  for(int i=0; i<15; i++){
    fp = fopen("./hashs/THVT.bin","rb+");
    if(!fp) exit(1);
    fseek(fp, i*sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fp);
    fclose(fp);
    if(pos == -1)continue;
    fp = fopen("./hashs/THVT_dados.bin","rb+");
    if(!fp)exit(1);
    THT aux;
    while(1){
      fseek(fp, pos, SEEK_SET);
      fread(&aux, sizeof(THT), 1, fp);
      if((aux.id == id) && (aux.status)){
        aux.status = 0;
        fseek(fp, pos, SEEK_SET);
        fwrite(&aux, sizeof(THT), 1, fp);
        fclose(fp);
        break;
      }
      if(aux.prox == -1){
        fclose(fp);
        break;
      }
      pos = aux.prox;
    }
  }
}


//Nao funcionando 100%, concertar na otimizacao (Nao é necessario para o programa)
/*
void THP_limpar(int ano){ //retira quem está inválido
  FILE *fph = fopen("./hashs/THP.bin", "rb+");
  if(!fph) exit(1);
  int pos, h = THP_hash(ano);
  fseek(fph, h*sizeof(int), SEEK_SET);
  fread(&pos, sizeof(int), 1, fph);
  int ant = -1, prox, pos_curr;
  FILE *fp = fopen("./hashs/THP_dados.bin","rb+");
  if(!fp){
    fclose(fph);
    exit(1);
  }
  THTpts aux;
  while(pos != -1){
    fseek(fp,pos,SEEK_SET);
    fread(&aux,sizeof(THTpts),1,fp);
    pos_curr = pos;
    if(!aux.status){
      prox = aux.prox;
      if(ant == -1){
        fseek(fph,h*sizeof(int),SEEK_SET);
        fwrite(&aux.prox,sizeof(int),1,fph);
      }
      while(prox != -1){
        fseek(fp,prox,SEEK_SET); //proximo
        fread(&aux,sizeof(THTpts),1,fp);
        fseek(fp,pos,SEEK_SET); //atual
        fwrite(&aux,sizeof(THTpts),1,fp);
        pos = prox;
        prox = aux.prox;
      }

      fseek(fp,pos_curr,SEEK_SET);
      fread(&aux,sizeof(THTpts),1,fp);
    }
    ant = pos_curr;
    pos = aux.prox;
  }
  fclose(fph);
  fclose(fp);
}*/


// void THP_insere(int id, int pontuacao, int ano){ //funcionando
//   if(ano < 2024) THP_insere(id,pontuacao,ano+1);
//   FILE *fph = fopen("./hashs/THP.bin", "rb+");
//   if(!fph) exit(1);
//   int pos, h = THP_hash(ano);
//   fseek(fph, h*sizeof(int), SEEK_SET);
//   fread(&pos, sizeof(int), 1, fph);
//   int ant = -1;
//   FILE *fp = fopen("./hashs/THP_dados.bin","rb+");
//   if(!fp){
//     fclose(fph);
//     exit(1);
//   }
//   THTpts aux;
//   while(pos != -1){ //vendo se ele está lá
//     fseek(fp, pos, SEEK_SET);
//     fread(&aux, sizeof(THTpts), 1, fp);
//     if(aux.id == id && aux.status){ //se está lá, remove e o insere de novo para entrar em nova ordem de pontuacao
//       fclose(fph);
//       fclose(fp);
//       THP_retira(id);
//       pontuacao += aux.pontuacao; //soma a pontuacao à antiga
//       THP_insere(id,pontuacao,ano);
//       return;
//     }
//     pos = aux.prox;
//   }
//   //Não está na hash (ou está inválido)
//   fseek(fph, h*sizeof(int), SEEK_SET); //redefinindo pos
//   fread(&pos, sizeof(int), 1, fph);

//   fseek(fp, pos, SEEK_SET);
//   fread(&aux, sizeof(THTpts), 1, fp);
//   while(aux.pontuacao >= pontuacao){
//     ant = pos;
//     pos = aux.prox;
//     if(pos == -1) break;
//     fseek(fp, pos, SEEK_SET);
//     fread(&aux, sizeof(THTpts), 1, fp);
//   }
//   aux.prox = pos;
//   aux.id = id;
//   aux.pontuacao = pontuacao;
//   aux.status = 1;
//   fseek(fp, 0L, SEEK_END);
//   pos = ftell(fp);
//   fwrite(&aux, sizeof(THTpts), 1, fp);
//   if(ant != -1){
//     fseek(fp, ant, SEEK_SET);
//     fread(&aux, sizeof(THTpts), 1, fp);
//     aux.prox = pos;
//     fseek(fp, ant, SEEK_SET);
//     fwrite(&aux, sizeof(THTpts), 1, fp);
//   }
//   else{
//     fseek(fph, h*sizeof(int), SEEK_SET);
//     fwrite(&pos, sizeof(int), 1, fph);
//   }
//   fclose(fp);
//   fclose(fph);
// }
// void THP_insere_otimizado(FILE *fph, FILE *fp, int id, int pontuacao, int ano) {
//     if (!fph || !fp) {
//         // Idealmente, o programa já teria encerrado se os arquivos não abriram.
//         // Isso é só uma verificação de segurança.
//         exit(1);
//     }

//     int h = THP_hash(ano);
    
//     // 1. Encontrar o início da lista para o 'ano' dado
//     fseek(fph, h * sizeof(int), SEEK_SET);
//     int pos_cabeca;
//     fread(&pos_cabeca, sizeof(int), 1, fph);

//     int pos_atual = pos_cabeca;
//     int pos_anterior = -1;
//     THTpts aux;

//     // 2. Travessia ÚNICA para:
//     //    a) Encontrar se o ID já existe.
//     //    b) Encontrar o local correto para inserção (mantendo a ordem).
//     while (pos_atual != -1) {
//         fseek(fp, pos_atual, SEEK_SET);
//         fread(&aux, sizeof(THTpts), 1, fp);

//         // Se o ID já existe e está ativo, atualizamos a pontuação e vamos reinserir
//         if (aux.id == id && aux.status == 1) {
//             pontuacao += aux.pontuacao; // Acumula a pontuação
            
//             // Marca o registro antigo como inválido ("removido")
//             aux.status = 0;
//             fseek(fp, pos_atual, SEEK_SET);
//             fwrite(&aux, sizeof(THTpts), 1, fp);
            
//             // Reseta a busca para inserir o novo registro (com pontuação atualizada) desde o início da lista
//             pos_atual = pos_cabeca;
//             pos_anterior = -1;
//             continue; // Continua o loop para encontrar a nova posição
//         }

//         // Se a pontuação do registro atual é menor que a nova pontuação,
//         // encontramos o ponto de inserção. O novo registro deve entrar ANTES dele.
//         if (aux.pontuacao < pontuacao) {
//             break;
//         }

//         pos_anterior = pos_atual;
//         pos_atual = aux.prox;
//     }

//     // 3. Inserir o novo nó
//     THTpts novo_no;
//     novo_no.id = id;
//     novo_no.pontuacao = pontuacao;
//     novo_no.status = 1;
//     novo_no.prox = pos_atual; // O novo nó aponta para o nó atual (onde o loop parou)

//     // Escreve o novo nó no final do arquivo de dados
//     fseek(fp, 0L, SEEK_END);
//     int nova_pos = ftell(fp);
//     fwrite(&novo_no, sizeof(THTpts), 1, fp);

//     // 4. Atualizar o ponteiro do nó anterior (ou da cabeça da lista)
//     if (pos_anterior != -1) {
//         // Inserção no meio ou no fim da lista
//         fseek(fp, pos_anterior, SEEK_SET);
//         fread(&aux, sizeof(THTpts), 1, fp); // Lê o nó anterior novamente
//         aux.prox = nova_pos;                // Atualiza seu 'prox'
//         fseek(fp, pos_anterior, SEEK_SET);
//         fwrite(&aux, sizeof(THTpts), 1, fp); // Escreve de volta
//     } else {
//         // Inserção no início da lista (a cabeça precisa ser atualizada)
//         fseek(fph, h * sizeof(int), SEEK_SET);
//         fwrite(&nova_pos, sizeof(int), 1, fph);
//     }
// }

// void THP_insere(int id, int pontuacao, int ano){
//   FILE *fph = fopen("./hashs/THP.bin","rb+");
//   FILE *fp = fopen("./hashs/THP_dados.bin","rb+");

//   // Itera do ano em que a pontuação foi ganha até o ano limite
//   for (int ano_inicial = ano; ano_inicial <= 2024; ++ano_inicial) { //2024 ano limite
//     // Para cada ano, chama a função otimizada para inserir a pontuação.
//     // Note que a pontuação passada é a *nova* pontuação. A função interna
//     // se encarrega de somar com qualquer pontuação que o ID já tenha naquele ano.
//     //printf("Processando ID %d para o ano %d...\n", id, ano_inicial);
//     THP_insere_otimizado(fph, fp, id, pontuacao, ano_inicial);
//   }

//   fclose(fph);
//   fclose(fp);
// }
void THNOM_insere(int id, char nome[51]){
  FILE *fph = fopen("./hashs/THNOM.bin", "rb+");
  if(!fph) exit(1);
  int pos, h = THNOM_hash(nome);
  fseek(fph, h*sizeof(int), SEEK_SET);
  fread(&pos, sizeof(int), 1, fph);
  int ant, prim_pos_livre;
  ant = prim_pos_livre = -1;
  FILE *fp = fopen("./hashs/THNOM_dados.bin","rb+");
  if(!fp){
    fclose(fph);
    exit(1);
  }
  THT aux;
  while(pos != -1){
    fseek(fp, pos, SEEK_SET);
    fread(&aux, sizeof(THT), 1, fp);
    if(aux.id == id){
      aux.status = 1;
      fseek(fp, pos, SEEK_SET);
      fwrite(&aux, sizeof(THT), 1, fp);
      fclose(fp);
      fclose(fph);
      return;
    }
    if((!aux.status) && (prim_pos_livre == -1))prim_pos_livre=pos;
    ant = pos;
    pos = aux.prox;
  }
  if(prim_pos_livre == -1){
    aux.id = id;
    aux.prox = -1;
    aux.status = 1;
    fseek(fp, 0L, SEEK_END);
    pos = ftell(fp);
    fwrite(&aux, sizeof(THT), 1, fp);
    if(ant != -1){
      fseek(fp, ant, SEEK_SET);
      fread(&aux, sizeof(THT), 1, fp);
      aux.prox = pos;
      fseek(fp, ant, SEEK_SET);
      fwrite(&aux, sizeof(THT), 1, fp);
    }
    else{
      fseek(fph, h*sizeof(int), SEEK_SET);
      fwrite(&pos, sizeof(int), 1, fph);
    }
    fclose(fp);
    fclose(fph);
    return;
  }
  fseek(fp, prim_pos_livre, SEEK_SET);
  fread(&aux, sizeof(THT), 1, fp);
  aux.id = id;
  aux.status = 1;
  fseek(fp, prim_pos_livre, SEEK_SET);
  fwrite(&aux, sizeof(THT), 1, fp);
  fclose(fp);
  fclose(fph);
}
void THV_insere(int id, int indiceTorneios){
  FILE *fph = fopen("./hashs/THV.bin", "rb+");
  if(!fph) exit(1);
  int pos, h = THV_hash(indiceTorneios);
  fseek(fph, h*sizeof(int), SEEK_SET);
  fread(&pos, sizeof(int), 1, fph);
  int ant, prim_pos_livre;
  ant = prim_pos_livre = -1;
  FILE *fp = fopen("./hashs/THV_dados.bin","rb+");
  if(!fp){
    fclose(fph);
    exit(1);
  }
  THT aux;
  while(pos != -1){
    fseek(fp, pos, SEEK_SET);
    fread(&aux, sizeof(THT), 1, fp);
    if(aux.id == id){
      aux.status = 1;
      fseek(fp, pos, SEEK_SET);
      fwrite(&aux, sizeof(THT), 1, fp);
      fclose(fp);
      fclose(fph);
      return;
    }
    if((!aux.status) && (prim_pos_livre == -1))prim_pos_livre=pos;
    ant = pos;
    pos = aux.prox;
  }
  if(prim_pos_livre == -1){
    aux.id = id;
    aux.prox = -1;
    aux.status = 1;
    fseek(fp, 0L, SEEK_END);
    pos = ftell(fp);
    fwrite(&aux, sizeof(THT), 1, fp);
    if(ant != -1){
      fseek(fp, ant, SEEK_SET);
      fread(&aux, sizeof(THT), 1, fp);
      aux.prox = pos;
      fseek(fp, ant, SEEK_SET);
      fwrite(&aux, sizeof(THT), 1, fp);
    }
    else{
      fseek(fph, h*sizeof(int), SEEK_SET);
      fwrite(&pos, sizeof(int), 1, fph);
    }
    fclose(fp);
    fclose(fph);
    return;
  }
  fseek(fp, prim_pos_livre, SEEK_SET);
  fread(&aux, sizeof(THT), 1, fp);
  aux.id = id;
  aux.status = 1;
  fseek(fp, prim_pos_livre, SEEK_SET);
  fwrite(&aux, sizeof(THT), 1, fp);
  fclose(fp);
  fclose(fph);
}
void THNAC_insere(int id){
  FILE *fph = fopen("./hashs/THNAC.bin", "rb+");
  if(!fph) exit(1);
  int pos, h = THNAC_hash(id);
  fseek(fph, h*sizeof(int), SEEK_SET);
  fread(&pos, sizeof(int), 1, fph);
  int ant, prim_pos_livre;
  ant = prim_pos_livre = -1;
  FILE *fp = fopen("./hashs/THNAC_dados.bin","rb+");
  if(!fp){
    fclose(fph);
    exit(1);
  }
  THT aux;
  while(pos != -1){
    fseek(fp, pos, SEEK_SET);
    fread(&aux, sizeof(THT), 1, fp);
    if(aux.id == id){
      aux.status = 1;
      fseek(fp, pos, SEEK_SET);
      fwrite(&aux, sizeof(THT), 1, fp);
      fclose(fp);
      fclose(fph);
      return;
    }
    if((!aux.status) && (prim_pos_livre == -1))prim_pos_livre=pos;
    ant = pos;
    pos = aux.prox;
  }
  if(prim_pos_livre == -1){
    aux.id = id;
    aux.prox = -1;
    aux.status = 1;
    fseek(fp, 0L, SEEK_END);
    pos = ftell(fp);
    fwrite(&aux, sizeof(THT), 1, fp);
    if(ant != -1){
      fseek(fp, ant, SEEK_SET);
      fread(&aux, sizeof(THT), 1, fp);
      aux.prox = pos;
      fseek(fp, ant, SEEK_SET);
      fwrite(&aux, sizeof(THT), 1, fp);
    }
    else{
      fseek(fph, h*sizeof(int), SEEK_SET);
      fwrite(&pos, sizeof(int), 1, fph);
    }
    fclose(fp);
    fclose(fph);
    return;
  }
  fseek(fp, prim_pos_livre, SEEK_SET);
  fread(&aux, sizeof(THT), 1, fp);
  aux.id = id;
  aux.status = 1;
  fseek(fp, prim_pos_livre, SEEK_SET);
  fwrite(&aux, sizeof(THT), 1, fp);
  fclose(fp);
  fclose(fph);
}
void THVT_insere(int id, int indiceTorneios){
  FILE *fph = fopen("./hashs/THVT.bin", "rb+");
  if(!fph) exit(1);
  int pos, h = THVT_hash(indiceTorneios);
  fseek(fph, h*sizeof(int), SEEK_SET);
  fread(&pos, sizeof(int), 1, fph);
  int ant, prim_pos_livre;
  ant = prim_pos_livre = -1;
  FILE *fp = fopen("./hashs/THVT_dados.bin","rb+");
  if(!fp){
    fclose(fph);
    exit(1);
  }
  THT aux;
  while(pos != -1){
    fseek(fp, pos, SEEK_SET);
    fread(&aux, sizeof(THT), 1, fp);
    if(aux.id == id){
      aux.status = 1;
      fseek(fp, pos, SEEK_SET);
      fwrite(&aux, sizeof(THT), 1, fp);
      fclose(fp);
      fclose(fph);
      return;
    }
    if((!aux.status) && (prim_pos_livre == -1))prim_pos_livre=pos;
    ant = pos;
    pos = aux.prox;
  }
  if(prim_pos_livre == -1){
    aux.id = id;
    aux.prox = -1;
    aux.status = 1;
    fseek(fp, 0L, SEEK_END);
    pos = ftell(fp);
    fwrite(&aux, sizeof(THT), 1, fp);
    if(ant != -1){
      fseek(fp, ant, SEEK_SET);
      fread(&aux, sizeof(THT), 1, fp);
      aux.prox = pos;
      fseek(fp, ant, SEEK_SET);
      fwrite(&aux, sizeof(THT), 1, fp);
    }
    else{
      fseek(fph, h*sizeof(int), SEEK_SET);
      fwrite(&pos, sizeof(int), 1, fph);
    }
    fclose(fp);
    fclose(fph);
    return;
  }
  fseek(fp, prim_pos_livre, SEEK_SET);
  fread(&aux, sizeof(THT), 1, fp);
  aux.id = id;
  aux.status = 1;
  fseek(fp, prim_pos_livre, SEEK_SET);
  fwrite(&aux, sizeof(THT), 1, fp);
  fclose(fp);
  fclose(fph);
}




// void THP_imprime(){
//   FILE *fp = fopen("./hashs/THP.bin", "rb");
//   if(!fp) exit(1);
//   int vet[35];
//   fread(&vet, sizeof(int), 35, fp); //interessante para a lista de paises
//   fclose(fp);
//   fp = fopen("./hashs/THP_dados.bin", "rb");
//   if(!fp) exit(1);
//   int i, pos;
//   for(i = 0; i < 35; i++){
//     int p = vet[i];
//     if(p != -1) printf("%d:\n", i);
//     THTpts x;
//     while(p != -1){
//       fseek(fp, p, SEEK_SET);
//       pos = ftell (fp);
//       fread(&x, sizeof(THTpts), 1, fp);
//       printf("$%d: id: %d\t pontos: %d\tstatus: %d\tprox_end: $%d\n", pos, x.id, x.pontuacao, x.status, x.prox);
//       p = x.prox;
//     }
//   }
//   fclose(fp);
// }


