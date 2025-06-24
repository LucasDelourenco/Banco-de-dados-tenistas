#include "TARVBMT.h"
#include <string.h>
#include <stdlib.h>

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

TT TT_completaInfoChampionsTxt(TT tenista){
  //durante a coleta de infos, jogar nas hashs (ex: hash ganhadores de torneios)

  //preencher campos: Pontuacao, TorneiosGanhos, anoGanhouTodosGrands

  int indicetorneios, qtdTotTitulos=0, qtdTitulosLidos=0, ano, qtdGrandSlamNesseAno;
  int torneiosPts[15] = {2000,2000,2000,2000,0,0,1000,1000,1000,1000,1000,1000,1000,1000,1000};
  char *lido, sobrenome[51], linha[300], sobrenomeTT[51];

  FILE *fp = fopen("champions.txt","r"); //IPC lembrar desse ".txt"!!
  fgets(linha,sizeof(linha),fp); //Pula a primeira linha (aka o cabeçalho)
  //fgets(linha,sizeof(linha),fp); //Pega a primeira linha de informacoes
  while(fgets(linha, sizeof(linha), fp)){ //Enquanto existir proxima linha, pega ela e:
    lido = strtok(linha,"\\"); //pega a primeira parte até "\" que é o ano
    ano = atoi(lido);
    lido = strtok(NULL,"\\"); //pega o primeiro "bloco" de campeao
    indicetorneios = 0;
    qtdGrandSlamNesseAno = 0;
    while(lido && indicetorneios<15){
      sscanf(lido,"%[^(] (%*d/%*d)",sobrenome); //lê o sobrenome
      if(sobrenome[strlen(sobrenome) -1] == ' ') sobrenome[strlen(sobrenome) -1] = '\0'; //retira o possivel espaço em branco do final
      //Pega a parte do sobrenome do nome (Pablo Afonso Pereira -> Afonso Pereira)
      int pos = 0;
      sscanf(tenista.nome, "%*s %n", &pos);  // pega a posição após o primeiro nome
      strcpy(sobrenomeTT, tenista.nome + pos); //sobrenomeTT tem o sobrenome do TT
      //
      if(strcmp(sobrenome,sobrenomeTT)==0){ //É o tenista na posicao indiceTorneios (PREENCHENDO SUAS INFORMACOES)
        if(qtdTotTitulos==0) sscanf(lido,"%*[^(] (%*d/%d)",&qtdTotTitulos);
        tenista.pontuacao += torneiosPts[indicetorneios];
        qtdTitulosLidos++;
        tenista.TorneiosGanhos[indicetorneios]++;
        if(indicetorneios<=3) qtdGrandSlamNesseAno++;

        //!!HASH!! Inserir o caba na HASH DE VENCEDORES DE TORNEIO COM ANO ("TT.id", "TT.pontuacao" e "ano")
        

        if(qtdTitulosLidos == qtdTotTitulos) break;
      }

      
      lido = strtok(NULL,"\\"); //vai pegando o conjunto de infos (até nao ter mais) do tipo:   "Lendl (1/1)" 
      indicetorneios++;
    }
    //acabou esse ano
    if(qtdGrandSlamNesseAno==4) tenista.anoGanhouTodosGrands = ano;

    //!!HASH!! Inserir o caba na HASH DE PONTUACAO POR ANO ("TT.id", "TT.pontuacao" e "ano")
    
    if(qtdTotTitulos!=0 && qtdTitulosLidos == qtdTotTitulos) break;
  }

  fclose(fp);
  return tenista;
}

int posPaisEmVet(char *pais,char *vetPais[40]){
    for(int i = 0; i<40; i++) if(vetPais[i]!=NULL && strcmp(pais,vetPais[i]) == 0) return i;
    return -1;
}
//durante a coleta de infos, jogar nas hashs

//Lê o(s) txt(s), monta o ID e ja manda insere na arvore direto (Cria a arvore no começo do programa)
void TARVBMT_criaPorTxt(int t){ //int t só usado para o insere (se o insere nao precisar tire o parametro dessa funcao)
  FILE *fp = fopen("tennis_players.txt","r");
  if(!fp) exit(1);
  TT tenista;
  char *lido, linha[120], nome[51];
  int ano, campo, indPais=10, indCpf=100, semanatemp=-1;
  int TTidPais,TTidCpf,TTidAno;
  //char vetPais[50][51]; //50 paises de no maximo 51 caracteres
  char *vetPais[50];
  for (int i = 0; i < 50; i++) {
    vetPais[i] = malloc(51 * sizeof(char));
  }
  fgets(linha,sizeof(linha),fp); //Pula o cabeçalho
  while(fgets(linha,sizeof(linha),fp)){ //começa a ler as linhas que importam
    tenista = TT_cria_vazio();
    lido = strtok(linha,"\\");
    strcpy(tenista.nome,lido);
    lido = strtok(NULL,"\\");
    tenista.ano_nascimento = atoi(lido);
        
    TTidCpf = indCpf++;  //TTidCpf recebe o indice e DEPOIS o indice aumenta em 1 (para o proximo)
    TTidAno = tenista.ano_nascimento - 1950;

    lido = strtok(NULL,"\\");
    campo = 2;
    semanatemp = -1;
    while(lido){
      if(strcmp(lido,"-")!=0){
        switch (campo){
        case 2: //ano morte
          tenista.morte = atoi(lido);
          break;
        case 3: //nacionalidade
          strcpy(tenista.pais,lido);
          int pos = posPaisEmVet(tenista.pais,vetPais);
          if(pos >= 0) TTidPais =  pos+10;
          else{
            strcpy(vetPais[indPais-10],tenista.pais);
            TTidPais = indPais++;
          }
          break;
        case 4:
          tenista.rank = atoi(lido);
          break;
        case 5:
          sscanf(lido,"%d (%d)",&tenista.YoBK,&semanatemp);
          if(semanatemp && semanatemp>0) tenista.numSem = semanatemp;
          break;
        default:
          break;
        }
      }
      lido = strtok(NULL,"\\");
      campo++;
    }

    tenista.id = (TTidCpf * 10000) + (TTidPais * 100) + TTidAno;
    tenista = TT_completaInfoChampionsTxt(tenista);

    //!!HASH!! Inserir o caba na HASH DE NOME ("TT.id","TT.nome")
    //!!HASH!! Inserir o caba na HASH DE NACIONALIDADE ("TT.id","TT.pais")


    //!!  TARVBMT_insere(tenista, t)  !!


    //printf para debug -> printa todos os jogadores com suas infos (quase todas infos)
    //printf("%s - %d: %d | %d | %s | %d | %d | %d | %d\n",tenista.nome,tenista.id,tenista.ano_nascimento,tenista.morte,tenista.pais,tenista.rank,tenista.YoBK,tenista.numSem,tenista.pontuacao);
  }

  fclose(fp);
  for (int i = 0; i < 50; i++) { //liberando o vetor de paises
      free(vetPais[i]);
  }
}



/*"Inicializa" nao é necessario
void TARVBMT_inicializa(char *arq){ //ja deve receber o nome com .bin no final(Indices.bin)
  /*
  char nome[51];
  strcpy(nome,arq);
  strcat(nome,".bin");
  
}
*/

int int_len(int num){
  if(num == 0) return 1;
  int count=0;
  while(num>0){
    num/=10;
    count++;
  }
  return count;
}

//remove o arquivo de indices, chama remove hashs e remove todos os arquivos folhas
void TARVBMT_libera(char *arq){ //nome deve vir como Indices.bin
  FILE *fp = fopen(arq,"rb+");
  fseek(fp,0L,SEEK_END);
  int nfolhas,fim = ftell(fp) - sizeof(int);
  fseek(fp,fim,SEEK_SET);
  fread(&nfolhas,sizeof(int),1,fp);
  fclose(fp);
  remove(arq);
  liberaHashs(); //Ainda deve ser implementada
  char nome[51],string_i[6];
  for(int i = 0; i<nfolhas; i++){
    strcpy(nome,"./infos/F");
    for(int j = 0; j < 4 - int_len(i); j++) strcat(nome,"0");
    sprintf(string_i,"%d",i);
    strcat(nome,string_i);
    strcat(nome,".bin");
    remove(nome);
  }
}

//                 Indices   id   TdaArvore
TT TARVBMT_busca(char *arq,int id,int t){ //TBT (ToBeTested)
  FILE *fp = fopen(arq,"rb+");
  if(!fp) exit(1);
  char identfNo[6], filho[6];
  int numchaves,qtdLidos,i,pos,tamPorBloco = (sizeof(char)*6 + sizeof(int) + sizeof(int)*((2*t)-1) + (sizeof(char)*6)*(2*t));
  int chave;//unsigned long int chave;
  while(1){
    i=0;
    qtdLidos = fread(&identfNo,sizeof(char),6,fp);
    if(qtdLidos > 0){
      if(strncmp(identfNo,"N",1)==0){ //Se é um Nó
        fread(&numchaves,sizeof(int),1,fp);
        fread(&chave,sizeof(int),1,fp);  //antes era long
        while((i < numchaves-1) && (id >= chave)){
          i++;
          fread(&chave,sizeof(int),1,fp); //antes era long
        }
        if(i==numchaves-1 && id>=chave) i++;
        pos = ftell(fp);
        fseek(fp,pos+(sizeof(int) * (t-i)), SEEK_SET);//pula até o começo dos filhos
        pos = ftell(fp);
        fseek(fp,pos+((sizeof(char)*6)*i),SEEK_SET);
        fread(&filho,sizeof(char),6,fp);
        if(strncmp(&filho,"F",1)==0){//Se o filho for folha, o nome dele vai pra identfNo e sai do loop de procura
          strcpy(identfNo,filho);
          break;
        }//caso contrario, se o filho ainda for Nó, vai para ele e repete o argoritmo(até achar uma folha)
        strcpy(filho,&filho[1]); //Pega os os numeros do filho
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


void TARVBMT_imprime_chaves(TARVBMT *a){
  if(!a) return;
  TARVBMT *p = a;
  while(p->filho[0]) p = p->filho[0];
  while(p){
    int i;
    for(i = 0; i < p->nchaves; i++) printf("%d ", p->chave[i]);
    p = p->prox;
  }
  printf("\n");
}


void imp(TARVBMT *a, int andar){
  if(a){
    int i,j;
    imp(a->filho[a->nchaves],andar+1);
    for(i=a->nchaves-1; i >= 0; i--){
      for(j=0; j<=andar; j++) printf("\t");
      printf("%d\n", a->chave[i]);
      imp(a->filho[i],andar+1);
    }
  }
}

void TARVBMT_imprime(TARVBMT *a){
  imp(a, 0);
}


void imp0(TARVBMT *a, int andar){
  if(a){
    int i,j;
    for(i=0; i<=a->nchaves-1; i++){
      imp(a->filho[i],andar+1);
      for(j=0; j<=andar; j++) printf("\t");
      printf("%d\n", a->chave[i]);
    }
    imp(a->filho[i],andar+1);
  }
}

void TARVBMT_imprime0(TARVBMT *a){
  imp0(a, 0);
}

TARVBMT *divisao(TARVBMT *x, int i, TARVBMT* y, int t){
  TARVBMT *z = TARVBMT_cria(t);
  z->folha = y->folha;
  int j;
  if(!y->folha){
    z->nchaves = t-1;
    for(j=0;j<t-1;j++) z->chave[j] = y->chave[j+t];
    for(j=0;j<t;j++){
      z->filho[j] = y->filho[j+t];
      y->filho[j+t] = NULL;
    }
  }
  else {
    z->nchaves = t; //z possuir� uma chave a mais que y se for folha
    for(j=0;j < t;j++) z->chave[j] = y->chave[j+t-1];//Caso em que y � folha, temos q passar a info para o n� da direita
    z->prox = y->prox; //ultima revisao: 04/2020
    y->prox = z;
  }
  y->nchaves = t-1;
  for(j=x->nchaves; j>=i; j--) x->filho[j+1]=x->filho[j];
  x->filho[i] = z;
  for(j=x->nchaves; j>=i; j--) x->chave[j] = x->chave[j-1];
  x->chave[i-1] = y->chave[t-1];
  x->nchaves++;
  return x;
}


TARVBMT *insere_nao_completo(TARVBMT *x, int mat, int t){
  int i = x->nchaves-1;
  if(x->folha){
    while((i>=0) && (mat < x->chave[i])){
      x->chave[i+1] = x->chave[i];
      i--;
    }
    x->chave[i+1] = mat;
    x->nchaves++;
    return x;
  }
  while((i>=0) && (mat < x->chave[i])) i--;
  i++;
  if(x->filho[i]->nchaves == ((2*t)-1)){
    x = divisao(x, (i+1), x->filho[i], t);
    if(mat > x->chave[i]) i++;
  }
  x->filho[i] = insere_nao_completo(x->filho[i], mat, t);
  return x;
}

TARVBMT *TARVBMT_insere(TARVBMT *T, TT tenista, int t){
  if(TARVBMT_busca(T, mat)) return T;
  if(!T){
    T=TARVBMT_cria(t);
    T->chave[0] = mat;
    T->nchaves=1;
    return T;
  }
  if(T->nchaves == (2*t)-1){
    TARVBMT *S = TARVBMT_cria(t);
    S->nchaves=0;
    S->folha = 0;
    S->filho[0] = T;
    S = divisao(S,1,T,t);
    S = insere_nao_completo(S, mat, t);
    return S;
  }
  T = insere_nao_completo(T, mat, t);
  return T;
}



TARVBMT* remover(TARVBMT* arv, int ch, int t){
  if(!arv) return arv;
  int i;
  for(i = 0; i < arv->nchaves && arv->chave[i] < ch; i++);
  if((i < arv->nchaves) && (ch == arv->chave[i]) && (arv->folha)){ //CASO 1
    printf("\nCASO 1\n");
    int j;
    for(j=i; j<arv->nchaves-1;j++) arv->chave[j] = arv->chave[j+1];
    arv->nchaves--;
    if(!arv->nchaves){ //ultima revisao: 04/2020
      TARVBMT_libera(arv);
      arv = NULL;
    }
    return arv;      
  }

  if((i < arv->nchaves) && (ch == arv->chave[i])) i++;
  TARVBMT *y = arv->filho[i], *z = NULL;
  if (y->nchaves == t-1){ //CASOS 3A e 3B
    if((i < arv->nchaves) && (arv->filho[i+1]->nchaves >=t)){ //CASO 3A
      printf("\nCASO 3A: i menor que nchaves\n");
      z = arv->filho[i+1];
      if(!y->folha){
        y->chave[t-1] = arv->chave[i];   //dar a y a chave i da arv
        arv->chave[i] = z->chave[0];     //dar a arv uma chave de z
      }
      else{ 
        arv->chave[i] = z->chave[0] + 1;
        y->chave[t-1] = z->chave[0];
      }
      y->nchaves++;

      int j;
      for(j=0; j < z->nchaves-1; j++)  //ajustar chaves de z
        z->chave[j] = z->chave[j+1];
      y->filho[y->nchaves] = z->filho[0]; //enviar ponteiro menor de z para o novo elemento em y
      for(j=0; j < z->nchaves; j++)       //ajustar filhos de z
        z->filho[j] = z->filho[j+1];
      z->nchaves--;
      arv->filho[i] = remover(arv->filho[i], ch, t);
      return arv;
    }
    if((i > 0) && (!z) && (arv->filho[i-1]->nchaves >= t)){ //CASO 3A
      printf("\nCASO 3A: i igual a nchaves\n");
      z = arv->filho[i-1];
      int j;
      for(j = y->nchaves; j>0; j--)               //encaixar lugar da nova chave
        y->chave[j] = y->chave[j-1];
      for(j = y->nchaves+1; j>0; j--) //encaixar lugar dos filhos da nova chave
        y->filho[j] = y->filho[j-1];

      if(!y->folha){
        y->chave[0] = arv->chave[i-1];    //dar a y a chave i da arv
        arv->chave[i-1] = z->chave[z->nchaves - 1];   //dar a arv uma chave de z
      }
      else{ 
        arv->chave[i-1] = z->chave[z->nchaves - 1];
        y->chave[0] = z->chave[z->nchaves-1];
      }
      y->nchaves++;

      //enviar ponteiro de z para o novo elemento em y
      y->filho[0] = z->filho[z->nchaves];
      z->nchaves--;
      arv->filho[i] = remover(y, ch, t);
      return arv;
    }
    if(!z){ //CASO 3B
      if(i < arv->nchaves && arv->filho[i+1]->nchaves == t-1){
        printf("\nCASO 3B: i menor que nchaves\n");
        z = arv->filho[i+1];
        if(!y->folha){
          y->chave[t-1] = arv->chave[i];//pegar chave [i] e coloca ao final de filho[i]
          y->nchaves++;
        }
        int j = 0;
        while(j < t-1){
          if(!y->folha) y->chave[t+j] = z->chave[j];
          else y->chave[t+j-1] = z->chave[j];
          y->nchaves++;
          j++;
        }
        y->prox = z->prox;
        if(!y->folha){
          for(j=0; j<t; j++){
            y->filho[t+j] = z->filho[j];
            z->filho[j] = NULL; //ultima revisao: 04/2020
          }
          //TARVBMT_libera(z); 07/2024
        }
        TARVBMT_libera(z); // 07/2024
        for(j=i; j < arv->nchaves-1; j++){ //limpar refer�ncias de i
          arv->chave[j] = arv->chave[j+1];
          arv->filho[j+1] = arv->filho[j+2];
        }
        arv->filho[arv->nchaves] = NULL;
        arv->nchaves--;
        if(!arv->nchaves){ //ultima revisao: 04/2020
          TARVBMT *temp = arv;
          arv = arv->filho[0];
          temp->filho[0] = NULL;
          TARVBMT_libera(temp);
        }
        arv = remover(arv, ch, t);
        return arv;
      }
      if((i > 0) && (arv->filho[i-1]->nchaves == t-1)){ 
        printf("\nCASO 3B: i igual a nchaves\n");
        z = arv->filho[i-1];
        if(!y->folha){
          if(i == arv->nchaves){
            z->chave[t-1] = arv->chave[i-1]; //pegar chave[i] e poe ao final de filho[i-1]
          }else{
            z->chave[t-1] = arv->chave[i];   //pegar chave [i] e poe ao final de filho[i-1]
          }
          z->nchaves++;
        }
        int j = 0;
        while(j < t-1){
          if(!y->folha) z->chave[t+j] = y->chave[j];
          else z->chave[t+j-1] = y->chave[j];
          z->nchaves++;
          j++;
        }
        z->prox = y->prox; 
        if(!z->folha){
          for(j=0; j<t; j++){
            z->filho[t+j] = y->filho[j];
            y->filho[j] = NULL; //ultima revisao: 04/2020
          }
          //TARVBMT_libera(y); 07/2024
        }
        TARVBMT_libera(y); // 07/2024
        arv->filho[arv->nchaves] = NULL;
        arv->nchaves--;
        if(!arv->nchaves){ //ultima revisao: 04/2020
          TARVBMT *temp = arv;
          arv = arv->filho[0];
          temp->filho[0] = NULL;
          TARVBMT_libera(temp);
          arv = remover(arv, ch, t);
        }
        else{
          i--;
          arv->filho[i] = remover(arv->filho[i], ch, t);
        }
        return arv;
      }
    }
  }  
  arv->filho[i] = remover(arv->filho[i], ch, t);
  return arv;
}


TARVBMT* TARVBMT_retira(TARVBMT* arv, int k, int t){
  if(!arv || !TARVBMT_busca(arv, k)) return arv;
  return remover(arv, k, t);
}

void TARVBMT_insere_MS (TT *tenista, char arq_ind, int t){ //WIP     //O primeiro numero será o numero de filhos
  FILE *f_ind = fopen(arq_ind, "rb+");
  if (!f_ind){ //Arvore não existe
    f_ind = fopen(arq_ind, "wb");
    if (!f_ind) {
      printf("Erro ao criar arquivo indice!\n");
      return;
    }

    int num_filhos = 1;
    char folha[] = "F0000.bin";

    fwrite(folha, sizeof(char), strlen(folha) + 1, f_ind);
    fwrite(0, sizeof(int), 1, f_ind);
    fwrite(num_filhos, sizeof(int), 1, f_ind);

    FILE *f_folha = fopen(folha, "wb");
    if (!f_folha) {
      printf("Erro ao criar o arquivo folha: %s", folha);
      return;
    }

    fwrite(tenista, sizeof(TT), 1, f_folha);
    fclose(f_folha);
    return;
  }

  //Caso a arvore já exista, faz a busca na arvore
  if (TARVBMT_busca_MS(f_ind, tenista->id)) {
    fclose(f_ind);
    return;
  }
  //Caso a o primeiro no da arvore tenha 2t-1 chaves:
  //Descobre qual chave tem que subir
  int num_chaves = 0, chave;
  char caractere;
  rewind(f_ind);
  fread(caractere, sizeof(char), 1, f_ind);
  
  //Se o arq indice começa com N, as informações estão no arquivo de indice
  if (caractere == 'N'){
    fseek(f_ind, sizeof(char)*5, SEEK_CUR);
    fread(&num_chaves, sizeof(int), 1, f_ind);
  }

  //Se o arq indice começa com F, as informações estão no arquivo folha, é necessário acessar
  else if (caractere == 'F'){
    rewind(f_ind);
    char folha[] = "F0000.bin";
    FILE *f_folha = fopen(folha, "rb");
    TT *temp;
    while (fread(temp, sizeof(TT), 1, f_folha)) num_chaves ++;
    if (num_chaves == (2*t)-1){
      rewind(f_folha);
      fseek(f_folha, sizeof(TT)*((num_chaves+1)/2)-1, SEEK_CUR);
      fread(&chave, sizeof(int), 1, f_folha);
    }
    free(temp);
    fclose(f_folha);
  }
  else printf("Erro: No inicio do arquivo indice não existe nó ou folha!\n");

  if (num_chaves == (2*t)-1){
    char arq_temp[50];  //Criar um arquivo temporario
    strcpy(arq_temp, "tmp_");
    strcat(arq_temp, arq_ind);
    FILE *f_tmp = fopen(arq_temp, "wb");

    if (caractere == 'N'){
      fseek(f_ind, sizeof(int)*(t-1), SEEK_CUR);  //Achar a chave t-1 que vai subir
      fread(&chave, sizeof(int), 1, f_ind);
    }
    
    fwrite("N0000", sizeof(char), 6, f_tmp);  //Escreve no novo arquivo a chave que subiu (Simula return S)
    fwrite(1, sizeof(int), 1, f_tmp);
    fwrite(&chave, sizeof(int), 1, f_tmp);
    
    //Dificuldade em implementar o resto da inserção, fui implementar a Divisão para poder continuar
    //21/06/2025 21:37
  }
}

void insere_nao_completo_MS(FILE *f_indice, TT *tenista, int t){ //WIP
  
}

void gerar_nome_no(char *retorno, int num_nos_internos){
  char aux[5];
  strcpy(retorno, "N");
  if (num_nos_internos%1000 > 0){
    sprintf(aux, "%d", num_nos_internos);
    strcat(retorno, aux);
  }
  else if (num_nos_internos%100 > 0){
    strcat(retorno, "0");
    sprintf(aux, "%d", num_nos_internos);
    strcat(retorno, aux);
  }
  else if (num_nos_internos%10 > 0){
    strcat(retorno, "00");
    sprintf(aux, "%d", num_nos_internos);
    strcat(retorno, aux);
  }
  else {
    strcat(retorno, "000");
    sprintf(aux, "%d", num_nos_internos);
    strcat(retorno, aux);
  }
}

void gerar_nome_folha(char *retorno, int num_folhas){
  char aux[5];
  strcpy(retorno, "F");
  if (num_folhas/1000 > 0){
    sprintf(aux, "%d", num_folhas);
    strcat(retorno, aux);
  }
  else if (num_folhas/100 > 0){
    strcat(retorno, "0");
    sprintf(aux, "%d", num_folhas);
    strcat(retorno, aux);
  }
  else if (num_folhas/10 > 0){
    strcat(retorno, "00");
    sprintf(aux, "%d", num_folhas);
    strcat(retorno, aux);
  }
  else {
    strcat(retorno, "000");
    sprintf(aux, "%d", num_folhas);
    strcat(retorno, aux);
  }
}

long buscar_pos_no(FILE *f_indice, char *nome_no, int t){
  int tamPorBloco, N, finalDosBlocos;
  char aux[6];
  
  fseek(f_indice, 0L, SEEK_END); 
  finalDosBlocos = ftell(f_indice) - sizeof(int);  //Descobre o final do ultimo bloco
  tamPorBloco = (sizeof(char)*6 + sizeof(int) + sizeof(int)*((2*t)-1) + (sizeof(char)*6)*(2*t));
  if (nome_no[0] == 'N'){  //Se for no interno pula direto pro nó desejado
    strcpy(aux, &nome_no[1]);
    N = atoi(aux);
    return tamPorBloco * N;
  }
  else if (nome_no[0] == 'F'){  //Se for filho olha os filhos de cada nó
    int blocoSemFilho = (sizeof(char)*6 + sizeof(int) + sizeof(int)*((2*t)-1)), i;
    fseek(f_indice, blocoSemFilho, SEEK_SET);
    while (1){
      for (i=0; i<2*t; i++){
        fread(aux, sizeof(char), 6, f_indice);
        if (memcmp(aux, "\0\0\0\0\0\0", 6) == 0) continue; //Se o filho for vazio pule um loop
        else if (strlen(aux) == 1 && aux[0] == 'N') continue; //Se for no interno pule um loop
        else if (strcmp(aux, nome_no) == 0){ //Achou o rótulo do filho
          return ftell(f_indice) - sizeof(char)*6;
        }
      }
      fseek(f_indice, blocoSemFilho, SEEK_CUR); //Vai para o próximo conjunto de filhos
      if (ftell(f_indice) > finalDosBlocos - sizeof(char)*6*(2*t)) break;  //Caso a próxima leitura passe do final do ultimo bloco, parar
    }
  }

  return -1; //O parametro passado está errado, esperava: "Nxxxx\0" ou "Fxxxx\0"
}

int copia_arquivo(char *nome_ent, char *nome_saida){
  if (strcmp(nome_ent, nome_saida) == 0) return 0;
  char entrada[10], saida[10];
  strcpy(entrada, nome_ent);
  strcpy(saida, nome_saida);
  strcat(entrada, ".bin");
  strcat(saida, ".bin");
  FILE *f_orig = fopen(entrada, "rb");
  if (!f_orig){
    perror("Func:copia_arquivo. Erro ao abrir o arquivo de entrada\n");
    return 0;
  }

  FILE *f_dest = fopen(saida, "wb");
  if (!f_dest){
    perror("Func:copia_arquivo. Erro ao criar o arquivo de destino\n");
    fclose(f_orig);
    return 0;
  }

  char buffer[1024];
  size_t bytes;

  while ((bytes = fread(buffer, 1, sizeof(buffer), f_orig)) > 0) {
      fwrite(buffer, 1, bytes, f_dest);
  }

  fclose(f_orig);
  fclose(f_dest);
  return 1;
}

char* aumenta_um(char *nome_ent) {
    int num = atoi(&nome_ent[1]) + 1;

    // Proteção contra estouro (máximo 9999)
    if (num > 9999) return NULL;

    char *resp = malloc(6 * sizeof(char)); // 5 + '\0'
    if (!resp) return NULL;

    sprintf(resp, "%c%04d", nome_ent[0], num);
    return resp;
}

int campo_vazio(char* campo) {
  int n = strlen(campo);
  for (int i = 0; i < n; i++) {
      if (campo[i] != '\0') return 0;
  }
  return 1;
}



void divisao_MS(char *indice, long pos_pai, long pos_y, int t){ //WIP
  FILE *f_indice = fopen(indice, "rb+");
  //OBS: f_indice terá que ser aberto em rb+!!!!!!!!!!

  //pos_pai: posicao onde o pai se encontra no arquivo indice
  //i: posicao onde o novo filho será inserido
  //pos_y: posicao do nó que será dividido

  //y é folha?
  char no, nome_no[6], *aux = (char*)malloc(sizeof(char)*11), *aux2 = (char*)malloc(sizeof(char)*11);
  int pont_aux; //ponteiro auxiliar, facilita o uso de fseek e ftell
  int j, nchaves_z, *chaves_z = (int*)malloc(sizeof(int)*((2*t)-1));
  int nfolhas, ntenistas, chave_que_sobe, temp, chave_atual;
  int chave_vazia = -1;

  for (j=0;j<(2*t)-1;j++) chaves_z[j] = -1;

  char **filhos_z = (char **)malloc(sizeof(char*)*2*t);
  for (j=0;j<(2*t);j++){
    filhos_z[j] = malloc(sizeof(char)*6);
    memset(filhos_z[j], '\0', 6);
  }

  fseek(f_indice, pos_y, SEEK_SET); //anda ate o no que sera dividido
  fread(&no, sizeof(char), 1, f_indice);

  if (no != 'F'){
    nchaves_z = t-1;
    fseek(f_indice, pos_y, SEEK_SET); //anda ate no pra ser dividido
    fread(nome_no, sizeof(char), 6, f_indice); //Le o rotulo Nxxxx
    fwrite(&nchaves_z, sizeof(int), 1, f_indice); //Reescreve o numero de chaves de y;  chaves de y e z sao iguais
    fseek(f_indice, sizeof(int)*(t-1), SEEK_CUR); //Pula t-1 chaves 
    fread(&chave_que_sobe, sizeof(int), 1, f_indice); //Le a t-ésima chave que vai subir pro pai
    pont_aux = ftell(f_indice) - sizeof(int);
    fseek(f_indice, pont_aux, SEEK_SET);
    fwrite(&chave_vazia, sizeof(int), 1, f_indice); //Reescreve com -1 (retira a chave)

    for(j=0;j<t-1;j++){
      fread(&chaves_z[j], sizeof(int), 1, f_indice); //Le a chave j
      pont_aux = ftell(f_indice) - sizeof(int);
      fseek(f_indice, pont_aux, SEEK_SET);
      fwrite(&chave_vazia, sizeof(int), 1, f_indice); //Reescreve com -1 (retira a chave)
    }

    fseek(f_indice, sizeof(char)*6*(t), SEEK_CUR); //Anda t filhos
    for (j=0; j<t; j++){
      fread(filhos_z[j], sizeof(char), 6, f_indice); //Le o filho
      pont_aux = ftell(f_indice) - sizeof(char)*6;
      fseek(f_indice, pont_aux, SEEK_SET);
      fwrite("\0", sizeof(char), 6, f_indice); //Reescreve com \0 (retira o filho)
    }
    fflush(f_indice);   //Quando usamos fwrite, a escrita eh mandada para um buffer
    //                  fflush manda escrever imediatamente

    //Atualizar o numero de chaves do pai
    fseek(f_indice, pos_pai + sizeof(char)*6, SEEK_SET); //Vai até o pai e pula o rotulo
    fread(&temp, sizeof(int), 1, f_indice); //Le o nchaves do no
    temp++;                                 //Atualiza nchaves
    pont_aux = ftell(f_indice) - sizeof(int); //Volta um inteiro para poder reescrever 
    fseek(f_indice, pont_aux, SEEK_SET);
    fwrite(&temp, sizeof(int), 1, f_indice); //Reescreve o nchaves do no

    fseek(f_indice, sizeof(int)*((2*t)-3), SEEK_CUR); //Prepara para atualizar

    //Atualizar as chaves do pai 
    for (j=0; j<(2*t)-1; j++){
      if (j == (2*t)-2){
        fwrite(&chave_que_sobe, sizeof(int), 1, f_indice);
        continue;
      }
      fread(&chave_atual, sizeof(int), 1, f_indice);
      if (chave_que_sobe > chave_atual){
        fwrite(&chave_que_sobe, sizeof(int), 1, f_indice);
        break;
      }
      else if (chave_que_sobe < chave_atual){
        fwrite(&chave_atual, sizeof(int), 1, f_indice);
      }
      pont_aux = ftell(f_indice) - sizeof(int)*3;
      fseek(f_indice, pont_aux, SEEK_SET);
    }

    //Atualizar filhos do pai
    //Como o numero de filhos será menor que 2t, para um no com n filhos, adicionar o filho n+1

    fseek(f_indice, sizeof(int)*((2*t)-1-temp), SEEK_CUR); //Vai para o primeiro filho do pai

    for (j=0; j<2*t; j++){
      fread(aux, sizeof(char), 6, f_indice);
      if (campo_vazio(aux)){  //Se achar um espaço vazio, escrever o novo filho
        pont_aux = ftell(f_indice) - sizeof(char)*6*2; //Calcula a pos do filho anterior ao espaço vazio
        fseek(f_indice, pont_aux, SEEK_SET);  //Pula para este filho
        fread(aux, sizeof(char), 6, f_indice); //Le o filho
        char *aux2 = aumenta_um(aux);
        fwrite(aux2, sizeof(char), 6, f_indice); //Escreve no espaco vazio
        free(aux2);
        break;
      }
    }

    //Atualizar todos os nos de f_indice que vem depois do no que esta sendo dividido
    //Se N0005 esta sendo dividido, todos os nos depois aumentam em 1 para poder inserir o novo N0006
    //... - N0004 - N0005 - N0007 - ...
    int numBloco = 0, numBlocoRest;
    fseek(f_indice, 0L, SEEK_END);
    pont_aux = ftell(f_indice) - sizeof(int);
    fseek(f_indice, pont_aux, SEEK_SET);
    fread(&numBloco, sizeof(int), 1, f_indice);

    int tamBloco = sizeof(char)*6 + sizeof(int) + sizeof(int)*(2*t)-1 + sizeof(char)*6*(2*t);
    temp = atoi(&nome_no[1]);
    temp++;
    numBlocoRest = numBloco - temp;
    fseek(f_indice, (temp)*tamBloco, SEEK_SET);

    for (j=0; j<numBlocoRest; j++){
      fread(aux, sizeof(char), 6, f_indice);
      pont_aux = ftell(f_indice) - sizeof(char)*6;
      fseek(f_indice, pont_aux, SEEK_SET);
      aux2 = aumenta_um(aux);
      fwrite(aux2, sizeof(char), 6, f_indice);
      free(aux2);
      fseek(f_indice, tamBloco-sizeof(char)*6, SEEK_CUR);
    }
    fflush(f_indice);

    //Escrever novo arq_ind em um temp e depois trocar o nome
    FILE *fnovo = fopen("novo.bin", "wb");
    size_t bytes; 
    char buffer[tamBloco];
    fseek(f_indice, 0L, SEEK_SET);

    while (bytes = fread(buffer, 1, tamBloco, f_indice) > 0 && temp > 0){ //Escreve tudo até passar do nó que foi dividido
      fwrite(buffer, 1, bytes, fnovo);
      temp --;
    }

    if (temp > 0){
      printf("Erro na escrita de um novo arquivo indice!!!!!!\n");
      free(chaves_z);
      free(aux);
      for (j = 0; j < 2*t; j++) {
        free(filhos_z[j]); // libera cada string, se tiver sido alocada
      }
      free(filhos_z); // libera o vetor de ponteiros
      fclose(fnovo);
      return;
    }

    //Escreve o novo no
    strcpy(aux, nome_no);
    aux2 = aumenta_um(aux);
    fwrite(aux, sizeof(char), 6, fnovo);
    free(aux2);
    fwrite(&nchaves_z, sizeof(int), 1, fnovo);
    for (j=0; j<(2*t)-1; j++){
      fwrite(&chaves_z[j], sizeof(int), 1, fnovo);
    }
    for (j=0; j<2*t; j++){
      fwrite(filhos_z[j], sizeof(char), 6, fnovo);
    }
    
    temp = numBlocoRest;
    while (bytes = fread(buffer, 1, tamBloco, f_indice) > 0 && temp > 0){ //Escreve o restante dos nos
      fwrite(buffer, 1, bytes, fnovo);
      temp --;
    }

    //Criou um novo no interno, atualizar o num de no interno
    numBloco++;
    fwrite(&numBloco, sizeof(int), 1, fnovo);

    fclose(f_indice);
    fclose(fnovo);

    remove(indice);
    strcpy(aux, "novo.bin");
    rename(aux, indice);

  }
  else { //Caso seja folha - Falta testar
    char *tmpNomeFolha1 = (char*)malloc(sizeof(char)*6);
    char *tmpNomeFolha2 = (char*)malloc(sizeof(char)*6);
    int folha_y;

    //nchaves = t;
    fseek(f_indice, pos_y, SEEK_SET); //anda ate no pra ser dividido
    fread(nome_no, sizeof(char), 6, f_indice); //Le o rotulo Nxxxx
    fseek(f_indice, sizeof(int), SEEK_CUR); //Pula o numero de chaves
    fseek(f_indice, sizeof(int)*(t-1), SEEK_CUR); //Pula t-1 chaves
    for(j=0;j < t;j++){
      fread(&chaves_z[j], sizeof(int), 1, f_indice); //Le a chave j
      pont_aux = ftell(f_indice) - sizeof(int);
      fseek(f_indice, pont_aux, SEEK_SET);
      fwrite(&chave_vazia, sizeof(int), 1, f_indice); //Reescreve com -1 (retira a chave)
    }
    //mexer no arquivo
    //Ajustar as folhas
    fseek(f_indice, 0L, SEEK_END);
    pont_aux = ftell(f_indice) - sizeof(int);
    fseek(f_indice, pont_aux, SEEK_SET);
    fread(&nfolhas, sizeof(int), 1, f_indice);
    folha_y = atoi(&nome_no[1]);
    for (j = nfolhas-1; j > folha_y; j--){ //Assumindo que todos os nos internos terao todos os seus filhos
      gerar_nome_folha(tmpNomeFolha1, j);
      tmpNomeFolha2 = aumenta_um(tmpNomeFolha1);
      copia_arquivo(tmpNomeFolha1, tmpNomeFolha2);

      pont_aux = buscar_pos_no(f_indice, tmpNomeFolha1, t) + sizeof(char)*6;
      fseek(f_indice, pont_aux, SEEK_SET);
      fwrite(tmpNomeFolha2, sizeof(char), 6, f_indice); //Sobreescreve o nome da folha vazia

      free(tmpNomeFolha2);
    }
    j--;
    //Criar a folha z e adicionar ela
    gerar_nome_folha(tmpNomeFolha1, j); //Cria o nome da folha, j = folha_y
    gerar_nome_folha(tmpNomeFolha2, j+1); //Cria o nome da folha, j = folha_y + 1
    strcpy(aux, tmpNomeFolha1);
    strcat(aux, ".bin");
    FILE *f1 = fopen(aux, "wb");
    strcpy(aux, tmpNomeFolha2);
    strcat(aux, ".bin");
    FILE *f2 = fopen(aux, "rb");
    FILE *ftmp = fopen("temp.bin", "wb");
    TT tenista;
    int cont1=0, conttmp=0;

    fseek(f2, 0L, SEEK_END);
    pont_aux = ftell(f2) - sizeof(int);
    fseek(f2, pont_aux, SEEK_SET);
    fread(&ntenistas, sizeof(int), 1, f2);
    fseek(f2, 0L, SEEK_SET);

    for (j=0;j<ntenistas;j++){
      fread(&tenista, sizeof(TT), 1, f2);
      if (j<t-1){
        fwrite(&tenista, sizeof(TT), 1, f1);
        cont1 ++;
      }
      else {
        fwrite(&tenista, sizeof(TT), 1, ftmp);
        conttmp ++;
      }
    }
    fwrite(&cont1, sizeof(int), 1, f1);
    fwrite(&conttmp, sizeof(int), 1, ftmp);
    fclose(f2);
    fclose(ftmp);
    fclose(f1);
    
    strcpy(aux, tmpNomeFolha2);
    strcat(aux, ".bin");
    remove(aux);
    rename("temp.bin", aux);

    fflush(f_indice);   //Quando usamos fwrite, a escrita eh mandada para um buffer
    //                  fflush manda escrever imediatamente

    free(tmpNomeFolha1);
    free(tmpNomeFolha2);
  } //Fim do caso folha

  free(chaves_z);
  free(aux);
  for (j = 0; j < 2*t; j++) {
    free(filhos_z[j]); // libera cada string, se tiver sido alocada
  }
  free(filhos_z); // libera o vetor de ponteiros

}


int main(void){
  //divisao_MS();
  FILE *fp = fopen("INDICES.bin","rb+");
  long num = buscar_pos_no(fp,"N0000",2);
  printf("%ld\n",num);
}