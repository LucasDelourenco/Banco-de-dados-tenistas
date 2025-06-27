#include "TH_MS.h"
#define TAM0 35
#define TAM1 26  
#define TAM2 4
#define TAM3 50
#define TAM4 15

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

//HASHS
int THP_hash(int ano){
  return ano-1960;
}
int THNOM_hash(char nome[51]){
  if(strncmp(nome,"a",1)==0 || strncmp(nome,"A",1)==0) return 0;
  else if(strncmp(nome,"b",1)==0 || strncmp(nome,"B",1)==0) return 1;
  else if(strncmp(nome,"c",1)==0 || strncmp(nome,"C",1)==0) return 2;
  else if(strncmp(nome,"d",1)==0 || strncmp(nome,"D",1)==0) return 3;
  else if(strncmp(nome,"e",1)==0 || strncmp(nome,"E",1)==0) return 4;
  else if(strncmp(nome,"f",1)==0 || strncmp(nome,"F",1)==0) return 5;
  else if(strncmp(nome,"g",1)==0 || strncmp(nome,"G",1)==0) return 6;
  else if(strncmp(nome,"h",1)==0 || strncmp(nome,"H",1)==0) return 7;
  else if(strncmp(nome,"i",1)==0 || strncmp(nome,"I",1)==0) return 8;
  else if(strncmp(nome,"j",1)==0 || strncmp(nome,"J",1)==0) return 9;
  else if(strncmp(nome,"k",1)==0 || strncmp(nome,"K",1)==0) return 10;
  else if(strncmp(nome,"l",1)==0 || strncmp(nome,"L",1)==0) return 11;
  else if(strncmp(nome,"m",1)==0 || strncmp(nome,"M",1)==0) return 12;
  else if(strncmp(nome,"n",1)==0 || strncmp(nome,"N",1)==0) return 13;
  else if(strncmp(nome,"o",1)==0 || strncmp(nome,"O",1)==0) return 14;
  else if(strncmp(nome,"p",1)==0 || strncmp(nome,"P",1)==0) return 15;
  else if(strncmp(nome,"q",1)==0 || strncmp(nome,"Q",1)==0) return 16;
  else if(strncmp(nome,"r",1)==0 || strncmp(nome,"R",1)==0) return 17;
  else if(strncmp(nome,"s",1)==0 || strncmp(nome,"S",1)==0) return 18;
  else if(strncmp(nome,"t",1)==0 || strncmp(nome,"T",1)==0) return 19;
  else if(strncmp(nome,"u",1)==0 || strncmp(nome,"U",1)==0) return 20;
  else if(strncmp(nome,"v",1)==0 || strncmp(nome,"V",1)==0) return 21;
  else if(strncmp(nome,"w",1)==0 || strncmp(nome,"W",1)==0) return 22;
  else if(strncmp(nome,"x",1)==0 || strncmp(nome,"X",1)==0) return 23;
  else if(strncmp(nome,"y",1)==0 || strncmp(nome,"Y",1)==0) return 24;
  else if(strncmp(nome,"z",1)==0 || strncmp(nome,"Z",1)==0) return 25;
}
int THV_hash(int indiceTorneios){
  if(indiceTorneios <=3) return 0;
  else if(indiceTorneios==4) return 1;
  else if(indiceTorneios==5) return 2;
  return 3;
}
int THNAC_hash(int indicePais){
  return indicePais-10;
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
  TH_inicializa("./hashs/THP.bin","./hashs/THP_dados.bin",35);
  TH_inicializa("./hashs/THNOM.bin","./hashs/THNOM_dados.bin",35);
  TH_inicializa("./hashs/THV.bin","./hashs/THV_dados.bin",35);
  TH_inicializa("./hashs/THNAC.bin","./hashs/THNAC_dados.bin",35);
  TH_inicializa("./hashs/THVT.bin","./hashs/THVT_dados.bin",35);
}


THT* THP_aloca(int id){ //THT* THP_aloca(int id, int pontuacao)
  THT *novo = (THT*) malloc (sizeof (THT));
  novo->id = id;
  //novo->pontuacao = pontuacao;
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
}

TLSETHT *TLSETHT_insere_inic(TLSETHT *l, THT elem){
  TLSETHT *novo = (TLSETHT *) malloc(sizeof(TLSETHT));
  novo->prox = l;
  novo->elemHash = elem;
  return novo;
}

TLSETHT *TLSETHT_insere_fim(TLSETHT *l, THT elem){
  if(!l) return TLSETHT_insere_inic(NULL,elem);
  l->prox = TLSE_insere_fim(l->prox,elem);
  return l;
}

    //LEMBRAR DE LIBERAR!!!!
TLSETHT *THP_busca_primeiros25DoAno(int ano){ //Q4 //NAO TESTADO!
  FILE *fp = fopen("./hashs/THP.bin","rb");
  if(!fp)exit(1);
  int pos, h = THP_hash(ano);
  fseek(fp, h*sizeof(int), SEEK_SET);
  fread(&pos, sizeof(int), 1, fp);
  fclose(fp);
  if(pos == -1)return NULL;
  fp = fopen("./hashs/THP_dados.bin","rb");
  if(!fp) exit(1);
  fseek(fp, pos, SEEK_SET);
  THT aux;
  fread(&aux, sizeof(THT), 1, fp);

  int i = 0; //quase um for
  TLSETHT *lista = NULL; //inicializando TLSETHT
  while(i < 25){
    if(aux.status){
      THT *resp = THP_aloca(aux.id);
      resp->prox = aux.prox;
      lista = TLSETHT_insere_fim(lista,aux);
    }
    if(aux.prox == -1){
      fclose(fp);
      return lista;
    }
    fseek(fp, aux.prox, SEEK_SET);
    fread(&aux, sizeof(THT), 1, fp);
  }
  fclose(fp);
  return lista;
}




float TH_retira(char *tabHash, char *arq, int n, int mat){
  FILE *fp = fopen(tabHash,"rb");
  if(!fp) exit(1);
  int pos, h = TH_hash(mat,n);
  fseek(fp, h*sizeof(int), SEEK_SET);
  fread(&pos, sizeof(int), 1, fp);
  fclose(fp);
  if(pos == -1)return -1;
  float cr = -1;
  fp = fopen(arq,"rb+");
  if(!fp)exit(1);
  TA aux;
  while(1){
    fseek(fp, pos, SEEK_SET);
    fread(&aux, sizeof(TA), 1, fp);
    if((aux.mat == mat) && (aux.status)){
      cr = aux.cr;
      aux.status = 0;
      fseek(fp, pos, SEEK_SET);
      fwrite(&aux, sizeof(TA), 1, fp);
      fclose(fp);
      return cr;
    }
    if(aux.prox == -1){
      fclose(fp);
      return cr;
    }
    pos = aux.prox;
  }
}

void TH_insere(char *tabHash, char *arq, int n, int mat, float cr){
  FILE *fph = fopen(tabHash, "rb+");
  if(!fph) exit(1);
  int pos, h = TH_hash(mat, n);
  fseek(fph, h*sizeof(int), SEEK_SET);
  fread(&pos, sizeof(int), 1, fph);
  int ant, prim_pos_livre;
  ant = prim_pos_livre = -1;
  FILE *fp = fopen(arq,"rb+");
  if(!fp){
    fclose(fph);
    exit(1);
  }
  TA aux;
  while(pos != -1){
    fseek(fp, pos, SEEK_SET);
    fread(&aux, sizeof(TA), 1, fp);
    if(aux.mat == mat){
      aux.cr = cr;
      aux.status = 1;
      fseek(fp, pos, SEEK_SET);
      fwrite(&aux, sizeof(TA), 1, fp);
      fclose(fp);
      fclose(fph);
      return;
    }
    if((!aux.status) && (prim_pos_livre == -1))prim_pos_livre=pos;
    ant = pos;
    pos = aux.prox;
  }
  if(prim_pos_livre == -1){
    aux.mat = mat;
    aux.cr = cr;
    aux.prox = -1;
    aux.status = 1;
    fseek(fp, 0L, SEEK_END);
    pos = ftell(fp);
    fwrite(&aux, sizeof(TA), 1, fp);
    if(ant != -1){
      fseek(fp, ant, SEEK_SET);
      fread(&aux, sizeof(TA), 1, fp);
      aux.prox = pos;
      fseek(fp, ant, SEEK_SET);
      fwrite(&aux, sizeof(TA), 1, fp);
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
  fread(&aux, sizeof(TA), 1, fp);
  aux.mat = mat;
  aux.cr = cr;
  aux.status = 1;
  fseek(fp, prim_pos_livre, SEEK_SET);
  fwrite(&aux, sizeof(TA), 1, fp);
  fclose(fp);
  fclose(fph);
  return;
}

void TH_imprime (char *nome_hash, char *nome_dados, int m){
  FILE *fp = fopen(nome_hash, "rb");
  if(!fp) exit(1);
  int vet[m];
  fread(&vet, sizeof(int), m, fp);
  fclose(fp);

  fp = fopen(nome_dados, "rb");
  if(!fp) exit(1);
  int i, pos;
  for(i = 0; i < m; i++){
    int p = vet[i];
    if(p != -1) printf("%d:\n", i);
    TA x;
    while(p != -1){
      fseek(fp, p, SEEK_SET);
      pos = ftell (fp);
      fread(&x, sizeof(TA), 1, fp);
      printf("$%d: matricula: %d\tcr: %f\tstatus: %d\tprox_end: $%d\n", pos, x.mat, x.cr, x.status, x.prox);
      p = x.prox;
    }
  }
  fclose(fp);
}
