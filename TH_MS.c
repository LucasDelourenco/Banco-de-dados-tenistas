#include "TH_MS.h"
#include <ctype.h>
#include <string.h>

#define TAM0 35
#define TAM1 26  
#define TAM2 4
#define TAM3 50
#define TAM4 15

THVl THVl_inicializa(){
  THVl temp;
  temp.id = -1;
  memset(temp.anos, 0, sizeof(int) * 35);
  temp.prox = -1;
  temp.status = 1;
  return temp;
}

TT TT_cria_vazio(){
  TT novo;
  memset(&novo, 0, sizeof(TT));  // zera todos os bytes da struct
  //tenistas.txt
  novo.id = -1;
  novo.ano_nascimento = -1;
  novo.morte = -1;
  novo.rank = -1;
  novo.YoBK = -1;
  novo.numSem = -1;
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
        fseek(fp,pos+(sizeof(int) * (2*t -2 -i)), SEEK_SET);//pula até o começo dos filhos
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
      fclose(fp);
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
  fclose(fp);
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
//#define ANO_INICIAL 1990
//#define ANO_FINAL 2024
#define NUM_ANOS (2024 - 1990 + 1)

typedef struct {
    int id;
    int pontuacao;
} MatTenista;
typedef struct { //caracteristicas da matriz
    int capacidade;           //qtd de espaços alocados
    int jogadores_registrados;  //qtd de jogadores
} MatCarac;

//comparação para o qsort (ordem decrescente de pontuação)
int comparar_ptos(const void* a, const void* b) {
    MatTenista* jogadorA = (MatTenista*)a;
    MatTenista* jogadorB = (MatTenista*)b;
    return (jogadorB->pontuacao - jogadorA->pontuacao);
}
// cria os arquivos da matriz e de IDs com zeros
void inicializar_matriz(int capacidade_inicial) {
    //cria o arquivo de caracsdados
    FILE* fp_caracs = fopen("./auxiliares/mat_caracs.bin", "wb");
    if (!fp_caracs) exit(1);
    MatCarac caracs = {capacidade_inicial, 0};
    fwrite(&caracs, sizeof(MatCarac), 1, fp_caracs);
    fclose(fp_caracs);

    //cria o arquivo de IDs com a capacidade inicial
    FILE* fp_ids = fopen("./auxiliares/idMap.bin", "wb");
    if (!fp_ids) exit(1);
    int* zeros_ids = (int*) calloc(capacidade_inicial, sizeof(int));
    fwrite(zeros_ids, sizeof(int), capacidade_inicial, fp_ids);
    free(zeros_ids);
    fclose(fp_ids);

    //cria o arquivo da matriz com a capacidade inicial
    FILE* fp_ptos = fopen("./auxiliares/matrizRankingPorAno.bin", "wb");
    if (!fp_ptos) exit(1);
    int* zeros_ptos_linha = (int*) calloc(NUM_ANOS, sizeof(int));
    for (int i = 0; i < capacidade_inicial; ++i) {
        fwrite(zeros_ptos_linha, sizeof(int), NUM_ANOS, fp_ptos);
    }
    free(zeros_ptos_linha);
    fclose(fp_ptos);
}
int expandir_capacidade() {
    // le as caracteristicas atuais
    MatCarac caracs;
    FILE* fp_caracs = fopen("./auxiliares/mat_caracs.bin", "rb+");
    if (!fp_caracs) return 0;
    fread(&caracs, sizeof(MatCarac), 1, fp_caracs);

    int capacidade_antiga = caracs.capacidade;
    int nova_capacidade = (capacidade_antiga > 0) ? capacidade_antiga * 1.5 : 400; //expande para 1.5 a capacidade autal ou seta para 400 se o tamanho for invalido
    int slots_a_adicionar = nova_capacidade - capacidade_antiga;

    printf("Expandindo capacidade de %d para %d jogadores...\n", capacidade_antiga, nova_capacidade);

    //expande o arquivo de IDs (a matriz)
    FILE* fp_ids = fopen("./auxiliares/idMap.bin", "ab");
    if (!fp_ids){
      fclose(fp_caracs); 
      return 0;//ocorreu algum erro na expansao
    }
    int* zeros_ids = (int*) calloc(slots_a_adicionar, sizeof(int));
    fwrite(zeros_ids, sizeof(int), slots_a_adicionar, fp_ids);
    free(zeros_ids);
    fclose(fp_ids);

    //expande o arquivo de ptos
    FILE* fp_ptos = fopen("./auxiliares/matrizRankingPorAno.bin", "ab");
    if (!fp_ptos){
      fclose(fp_caracs); 
      return 0; 
    }
    int* zeros_ptos_linha = (int*) calloc(NUM_ANOS, sizeof(int)); //tipo malloc mas ja inicializa com zeros (vetor)
    for (int i = 0; i < slots_a_adicionar; i++) {
        fwrite(zeros_ptos_linha, sizeof(int), NUM_ANOS, fp_ptos);
    }
    free(zeros_ptos_linha);
    fclose(fp_ptos);

    //atualiza os caracs com a nova capacidade
    caracs.capacidade = nova_capacidade;
    fseek(fp_caracs, 0, SEEK_SET); //volta ao início do arquivo de caracs
    fwrite(&caracs, sizeof(MatCarac), 1, fp_caracs);
    fclose(fp_caracs);

    return 1; //expandiu com sucesso
}

// Retorna o índice para um dado ID (0 à numJogadores-1)
// Se o jogador é novo, salva ele no primeiro slot vago.
int obter_ou_criar_indice_por_id(int id) {
    // Lê os caracsdados para saber os limites atuais
    MatCarac caracs;
    FILE* fp_caracs = fopen("./auxiliares/mat_caracs.bin", "rb+");
    if (!fp_caracs) return -1;
    fread(&caracs, sizeof(MatCarac), 1, fp_caracs);
    
    FILE* fp_ids = fopen("./auxiliares/idMap.bin", "rb+");
    if (!fp_ids){
      fclose(fp_caracs); 
      return -1; 
    }

    int indice_encontrado = -1;
    int primeiro_indice_vago = -1;
    int id_lido;

    // Procura por um jogador existente ou um slot vago dentro da capacidade atual
    for (int i = 0; i < caracs.capacidade; ++i) {
        fread(&id_lido, sizeof(int), 1, fp_ids);
        if (id_lido == id) { //jogador está na matriz
            indice_encontrado = i;
            break;
        }
        if (id_lido == 0 && primeiro_indice_vago == -1){ //jogador nao está e a posicao que ele vai entrar é "i"
            primeiro_indice_vago = i;
        }
    }

    if (indice_encontrado != -1) {
        fclose(fp_ids);
        fclose(fp_caracs);
        return indice_encontrado;
    }

    if (primeiro_indice_vago != -1) {
        fseek(fp_ids, primeiro_indice_vago * sizeof(int), SEEK_SET);
        fwrite(&id, sizeof(int), 1, fp_ids);
        
        //atualiza a contagem de jogadores registrados nos caracsMat
        caracs.jogadores_registrados++;
        fseek(fp_caracs, 0, SEEK_SET);
        fwrite(&caracs, sizeof(MatCarac), 1, fp_caracs);

        fclose(fp_ids);
        fclose(fp_caracs);
        return primeiro_indice_vago;
    }

    //jogador nao está e nao há espaços vagos, começa a lógica de expandir
    fclose(fp_ids);
    fclose(fp_caracs); 
    
    if (!expandir_capacidade()) {
        printf("Nao foi possivel expandir a matrizDeRanking\n");
        return -1;
    }

    //agora que está expandido vamos conseguir fazer o que queriamos
    return obter_ou_criar_indice_por_id(id);
}
void adicionar_pontuacao(int id_jogador, int pontuacao_nova, int ano_evento) {
    if (ano_evento < 1990 || ano_evento > 2024) return;

    int indice_jogador = obter_ou_criar_indice_por_id(id_jogador);
    if (indice_jogador == -1) return; // Erro ao obter índice (ex: limite de jogadores)

    FILE* fp = fopen("./auxiliares/matrizRankingPorAno.bin", "rb+");
    if (!fp) return;

    for (int ano = ano_evento; ano <= 2024; ano++) { //vai atualizando a pontuacao
        int indice_ano = ano - 1990;
        long offset = (long)(indice_jogador * NUM_ANOS + indice_ano) * sizeof(int);

        fseek(fp, offset, SEEK_SET);
        fwrite(&pontuacao_nova, sizeof(int), 1, fp); // antes -> fwrite(&pontuacao, sizeof(int), 1, fp);
    }

    fclose(fp);
}
void imprimir_top_N(int ano, int t, int N) {
    if (ano < 1990 || ano > 2024) {
        printf("Ano invalido. Insira um ano entre 1990 e 2024\n");
        return;
    }

    //ler os caracs para saber a capacidade atual
    MatCarac caracs;
    FILE* fp_caracs = fopen("./auxiliares/mat_caracs.bin", "rb");
    if (!fp_caracs) exit(1);
    fread(&caracs, sizeof(MatCarac), 1, fp_caracs);
    fclose(fp_caracs);

    //se a capacidade for 0, não há nada a fazer.
    if (caracs.capacidade == 0) {
        printf("O banco de dados esta vazio. Nao ha jogadores para classificar.\n");
        return;
    }

    /*
    //verificando se a alocação de memória falhou
    if (!id_map || !ranking_ano) {
        free(id_map);      //libera o que quer que tenha sido alocado
        free(ranking_ano);
        return;
    }*/

    FILE* fp_ids = fopen("./auxiliares/idMap.bin", "rb");
    if (!fp_ids) return;
    int* id_map = (int*) malloc(caracs.capacidade * sizeof(int)); //vetor dos ids
    if (id_map == NULL) {
    perror("Falha ao alocar memoria para id_map");
    // Você pode querer fechar outros arquivos abertos aqui antes de retornar
    return;
    }

    fread(id_map, sizeof(int), caracs.capacidade, fp_ids);
    fclose(fp_ids);

    FILE* fp_ptos = fopen("./auxiliares/matrizRankingPorAno.bin", "rb");
    if (!fp_ptos){
      free(id_map);
      return;
    }
    
    //lendo pontuacao do ano e colocando num vetor para ordenar
    MatTenista* ranking_ano = (MatTenista*) malloc(caracs.capacidade * sizeof(MatTenista));
    if (ranking_ano == NULL) {
      perror("Falha ao alocar memoria para ranking_ano");
      free(id_map); // Libera o que já foi alocado
      fclose(fp_ptos); // Fecha o arquivo aberto
      return;
    }
    int indice_ano = ano - 1990;
    for (int i = 0; i < caracs.capacidade; i++) {
        //se id == 0 o espaço está vazio
        if (id_map[i] == 0) {
            ranking_ano[i].id = 0;
            ranking_ano[i].pontuacao = 0;
            continue;
        }

        //pega o id do jogador
        ranking_ano[i].id = id_map[i]; 
        
        // Calcula a posição do ponto no arquivo
        long offset = ((long)i * NUM_ANOS + indice_ano) * sizeof(int);
        fseek(fp_ptos, offset, SEEK_SET);
        fread(&ranking_ano[i].pontuacao, sizeof(int), 1, fp_ptos);
    }
    fclose(fp_ptos);

    //ordenando
    qsort(ranking_ano, caracs.capacidade, sizeof(MatTenista), comparar_ptos);

    int ignorarVazios = 1;
    if(N == -10){//Se N é -10 ent quero TODOS os jogadores em ordem sem deixar de imprimir ngm
      N = caracs.jogadores_registrados; 
      ignorarVazios = 0;
      printf("--- RANKING desconsiderando aposentadoria ---\n");
    }else{
      printf("--- TOP até %d do Ano %d ---\n", N, ano);
    }

    //imprimindo
    
    int count_impressos = 0;
    for (int i = 0; i < caracs.capacidade && count_impressos < N; ++i) {
        //pula entradas vazias ou com pontuação zero
        if (ignorarVazios && (ranking_ano[i].id == 0 || ranking_ano[i].pontuacao == 0)) {
            continue;
        }

        printf("%3d) %s - Pontuacao: %d\n", count_impressos + 1, TARVBMT_busca(ranking_ano[i].id, t).nome,ranking_ano[i].pontuacao);
        count_impressos++;
    }
    printf("--------------------------------\n");
    free(id_map);
    free(ranking_ano);
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
  inicializar_matriz(400);
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

TLSEvl *TLSEvl_insere_inic(TLSEvl *l, int id, int *anos){
  TLSEvl *novo = (TLSEvl *) malloc(sizeof(TLSEvl));
  novo->prox = l;
  novo->id = id;
  for(int i = 0; i<35; i++) novo->anos[i] = anos[i];
  return novo;
}

TLSEvl *TLSEvl_insere_fim(TLSEvl *l, int id, int *anos){
  if(!l) return TLSEvl_insere_inic(NULL,id,anos);
  l->prox = TLSEvl_insere_fim(l->prox,id,anos);
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
    if((strcasecmp(tenista.nome,nome)==0) && (aux.status)){
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
TLSEvl *THV_busca_lista_torneio(int indiceTorneio){
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
  THVl aux;
  TLSEvl *lista = NULL;
  fread(&aux, sizeof(THVl), 1, fp);
  while(1){
    if(aux.status)lista = TLSEvl_insere_fim(lista, aux.id, aux.anos);
    if(aux.prox == -1){
      fclose(fp);
      return lista;
    }
    fseek(fp, aux.prox, SEEK_SET);
    fread(&aux, sizeof(THVl), 1, fp);
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

void insere_ult_pos_vet(int *anos, int ano){
  for(int i = 0; i < 35; i++){
    if(anos[i]==0){//se é a prim pos vazia
      anos[i] = ano;
      return;
    }
    if(anos[i]==ano) return; //se ja tem esse ano, nao insere
  }
}
void THV_insere(int id, int indiceTorneios, int ano){
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
  THVl aux = THVl_inicializa();
  while(pos != -1){
    fseek(fp, pos, SEEK_SET);
    fread(&aux, sizeof(THVl), 1, fp);
    if(aux.id == id){
      aux.status = 1;
      insere_ult_pos_vet(aux.anos,ano);
      fseek(fp, pos, SEEK_SET);
      fwrite(&aux, sizeof(THVl), 1, fp);
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
    insere_ult_pos_vet(aux.anos,ano);
    aux.prox = -1;
    aux.status = 1;
    fseek(fp, 0L, SEEK_END);
    pos = ftell(fp);
    fwrite(&aux, sizeof(THVl), 1, fp);
    if(ant != -1){
      fseek(fp, ant, SEEK_SET);
      fread(&aux, sizeof(THVl), 1, fp);
      aux.prox = pos;
      fseek(fp, ant, SEEK_SET);
      fwrite(&aux, sizeof(THVl), 1, fp);
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
  fread(&aux, sizeof(THVl), 1, fp);
  aux.id = id;
  insere_ult_pos_vet(aux.anos,ano);
  aux.status = 1;
  fseek(fp, prim_pos_livre, SEEK_SET);
  fwrite(&aux, sizeof(THVl), 1, fp);
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

void TLSEvl_libera(TLSEvl *l) {
    TLSEvl *p = l, *temp;
    while(p != NULL) {
        temp = p;
        p = p->prox;
        free(temp);
    }
}

void TLSEid_libera(TLSEid *l) {
    TLSEid *p = l, *temp;
    while(p != NULL) {
        temp = p;
        p = p->prox;
        free(temp);
    }
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

