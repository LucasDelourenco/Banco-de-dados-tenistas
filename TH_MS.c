#include "TARVBMT.h"
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
  return ano-1990;
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
  TH_inicializa("./hashs/THP.bin","./hashs/THP_dados.bin",35);
  TH_inicializa("./hashs/THNOM.bin","./hashs/THNOM_dados.bin",26);
  TH_inicializa("./hashs/THV.bin","./hashs/THV_dados.bin",4);
  TH_inicializa("./hashs/THNAC.bin","./hashs/THNAC_dados.bin",50);
  TH_inicializa("./hashs/THVT.bin","./hashs/THVT_dados.bin",15);
}


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
}

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

    //LEMBRAR DE LIBERAR!!!!
TLSEid *THP_busca_primeiros_ateh_N_Do_Ano(int ano, int qtd_n){ //Q4 e Q3 //Funcionando!
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
  THTpts aux;
  fread(&aux, sizeof(THTpts), 1, fp);

  int i = 0; //quase um for
  TLSEid *lista = NULL; //inicializando TLSEid
  while(i < qtd_n){ //antes era 25 só para Q4
    if(aux.status){
      lista = TLSEid_insere_fim(lista,aux.id);
      i++;
    }
    if(aux.prox == -1){
      fclose(fp);
      return lista;
    }
    fseek(fp, aux.prox, SEEK_SET);
    fread(&aux, sizeof(THTpts), 1, fp);
  }
  fclose(fp);
  return lista;
}
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




void THP_retira(int id){
  FILE *fp;
  int pos;
  for(int i=0; i<35; i++){
    fp = fopen("./hashs/THP.bin","rb+");
    if(!fp) exit(1);
    fseek(fp, i*sizeof(int), SEEK_SET);
    fread(&pos, sizeof(int), 1, fp);
    fclose(fp);
    if(pos == -1)continue;
    fp = fopen("./hashs/THP_dados.bin","rb+");
    if(!fp)exit(1);
    THTpts aux;
    while(1){
      fseek(fp, pos, SEEK_SET);
      fread(&aux, sizeof(THTpts), 1, fp);
      if((aux.id == id) && (aux.status)){ //achou
        aux.status = 0;
        fseek(fp, pos, SEEK_SET);
        fwrite(&aux, sizeof(THTpts), 1, fp);
        fclose(fp);
        break;
      }
      if(aux.prox == -1){ //nao achou
        fclose(fp);
        break;
      }
      pos = aux.prox;
    }
  }
}
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


void THP_insere(int id, int pontuacao, int ano){ //funcionando
  FILE *fph = fopen("./hashs/THP.bin", "rb+");
  if(!fph) exit(1);
  int pos, h = THP_hash(ano);
  fseek(fph, h*sizeof(int), SEEK_SET);
  fread(&pos, sizeof(int), 1, fph);
  int ant = -1;
  FILE *fp = fopen("./hashs/THP_dados.bin","rb+");
  if(!fp){
    fclose(fph);
    exit(1);
  }
  THTpts aux;
  while(pos != -1){ //vendo se ele está lá
    fseek(fp, pos, SEEK_SET);
    fread(&aux, sizeof(THTpts), 1, fp);
    if(aux.id == id && aux.status){ //se está lá, remove e o insere de novo para entrar em nova ordem de pontuacao
      fclose(fph);
      fclose(fp);
      THP_retira(id);
      pontuacao += aux.pontuacao; //soma a pontuacao à antiga
      THP_insere(id,pontuacao,ano);
      return;
    }
    pos = aux.prox;
  }
  //Não está na hash (ou está inválido)
  fseek(fph, h*sizeof(int), SEEK_SET); //redefinindo pos
  fread(&pos, sizeof(int), 1, fph);

  fseek(fp, pos, SEEK_SET);
  fread(&aux, sizeof(THTpts), 1, fp);
  while(aux.pontuacao >= pontuacao){
    ant = pos;
    pos = aux.prox;
    if(pos == -1) break;
    fseek(fp, pos, SEEK_SET);
    fread(&aux, sizeof(THTpts), 1, fp);
  }
  aux.prox = pos;
  aux.id = id;
  aux.pontuacao = pontuacao;
  aux.status = 1;
  fseek(fp, 0L, SEEK_END);
  pos = ftell(fp);
  fwrite(&aux, sizeof(THTpts), 1, fp);
  if(ant != -1){
    fseek(fp, ant, SEEK_SET);
    fread(&aux, sizeof(THTpts), 1, fp);
    aux.prox = pos;
    fseek(fp, ant, SEEK_SET);
    fwrite(&aux, sizeof(THTpts), 1, fp);
  }
  else{
    fseek(fph, h*sizeof(int), SEEK_SET);
    fwrite(&pos, sizeof(int), 1, fph);
  }
  fclose(fp);
  fclose(fph);
}
void THNOM_insere(int id, char nome[51]){
  FILE *fph = fopen("./hashs/THNOM.bin", "rb+");
  if(!fph) exit(1);
  int pos, h = THNOM_hash(nome);
  fseek(fph, h*sizeof(int), SEEK_SET);
  fread(&pos, sizeof(int), 1, fph);
  int ant, prim_pos_livre;
  ant = prim_pos_livre = -1;
  FILE *fp = fopen("./hash/THNOM_dados.bin","rb+");
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
  FILE *fp = fopen("./hash/THV_dados.bin","rb+");
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
  FILE *fp = fopen("./hash/THNAC_dados.bin","rb+");
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
  FILE *fp = fopen("./hash/THVT_dados.bin","rb+");
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




void THP_imprime(){
  FILE *fp = fopen("./hashs/THP.bin", "rb");
  if(!fp) exit(1);
  int vet[35];
  fread(&vet, sizeof(int), 35, fp); //interessante para a lista de paises
  fclose(fp);
  fp = fopen("./hashs/THP_dados.bin", "rb");
  if(!fp) exit(1);
  int i, pos;
  for(i = 0; i < 35; i++){
    int p = vet[i];
    if(p != -1) printf("%d:\n", i);
    THTpts x;
    while(p != -1){
      fseek(fp, p, SEEK_SET);
      pos = ftell (fp);
      fread(&x, sizeof(THTpts), 1, fp);
      printf("$%d: id: %d\t pontos: %d\tstatus: %d\tprox_end: $%d\n", pos, x.id, x.pontuacao, x.status, x.prox);
      p = x.prox;
    }
  }
  fclose(fp);
}


