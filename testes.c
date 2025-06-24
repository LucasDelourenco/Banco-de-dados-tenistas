#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct tenistas{
  //tenistas.txt
  int id, ano_nascimento, morte, rank, YoBK, numSem;
  char nome[51],sobrenome[51], pais[51];  //nome é nome + sobrenome
  //champions.txt
  int pontuacao;
  int anoGanhouTodosGrands; //Q(5)
  int TorneiosGanhos[15];

}TT;

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
    
    FILE *fp = fopen("champions.txt","r");
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

void teste_TT_completaInfoChampionsTxt(){ //FUNCIONA AHAHAHAHHA!!!!  :D
    TT tenista = TT_cria_vazio();
    strcpy(tenista.nome,"Michael Stich"); //Michael Stich  //Rafael Nadal
    tenista = TT_completaInfoChampionsTxt(tenista);
    printf("Pontos : %d\nQuantidade De Titulos:\n",tenista.pontuacao);
    printf("AusOpn - %d\nFrenchOpn - %d\nWimbledon - %d\nUsOpn - %d\nATPFinals - %d\nOlimpicGames - %d\nIndianWells - %d\nMiami - %d\nMonteCarlo - %d\nMadrid - %d\nRome - %d\nCanada - %d\nCincinnati - %d\nShanghai - %d\nParis - %d\n",tenista.TorneiosGanhos[0],tenista.TorneiosGanhos[1],tenista.TorneiosGanhos[2],tenista.TorneiosGanhos[3],tenista.TorneiosGanhos[4],tenista.TorneiosGanhos[5],tenista.TorneiosGanhos[6],tenista.TorneiosGanhos[7],tenista.TorneiosGanhos[8],tenista.TorneiosGanhos[9],tenista.TorneiosGanhos[10],tenista.TorneiosGanhos[11],tenista.TorneiosGanhos[12],tenista.TorneiosGanhos[13],tenista.TorneiosGanhos[14]);
}




void teste_strncmp(){
    char nome1[6];
    char nome2[6];
    strcpy(nome1,"N0101");
    strcpy(nome2,"N");
    if(strncmp(nome1,nome2,1)==0){
        printf("\n\neh um no\n");
    }
}

void teste_sprintf(){
    char nome[6],num[2];
    for(int i = 0; i< 5; i++){
        strcpy(nome,"N000");
        sprintf(num, "%d", i); //num = i como char;  (PASSANDO INT PARA CHAR)
        strcat(nome,num);
        printf("\n%s",nome);
    }
}

void cria(){
    /*
    N0000
    N0001
    N0002
    N0003
    N0004
    */

    FILE *fp = fopen("./arqs teste/filetesteparasobreescrita.bin","wb+");
    char nome[6], num[2];
    for(int i = 0; i< 5; i++){
        strcpy(nome,"N000");
        sprintf(num, "%d", i);
        strcat(nome,num);
        fwrite(&nome,sizeof(char),6,fp);
    }
    fclose(fp);
}

void teste_fseek(){
    //criando -> virou void cria();
    cria();
    //lendo
    FILE *fp = fopen("./arqs teste/filetesteparafseek.bin","rb+");
    char nome[6];
    fread(&nome,sizeof(char),6,fp);  //ATENCAO, a string que eu leio é N 0 0 0 0 \0, ou seja 6 caracteres
    printf("\nN0 -> %s",nome);
    int pos = ftell(fp);
    fseek(fp,pos+(sizeof(char) * 12),SEEK_SET); //funciona
    fread(&nome,sizeof(char),6,fp);
    printf("\nN3 -> %s",nome);
    fclose(fp);
}

void teste_appendArqBin(){ //só appenda no fim
    FILE *fp = fopen("./arqs teste/filetesteparasobreescrita.bin","ab+");
    char nome[6];
    strcpy(nome,"N0015");
    fseek(fp,sizeof(char)*6*2,SEEK_SET);
    fwrite(&nome,sizeof(char),6,fp);
    fclose(fp);
}

void ler_tudo(){
    FILE *fp = fopen("./arqs teste/filetesteparasobreescrita.bin","rb+");
    char nome[6];
    int qtdLido;
    qtdLido = fread(&nome,sizeof(char),6,fp);
    while(qtdLido>0){
        printf(" %s\n",nome);
        qtdLido = fread(&nome,sizeof(char),6,fp);
    }
    fclose(fp);
}

/*
void teste_tamBlocoParaArqIndices(){
    FILE *fp = fopen("./arqs teste/filetesteparafseek.bin","wb+");
    char 
}*/

void teste_strcpy(){
    char nome[6];
    strcpy(nome,"N0000");
    strcpy(nome,&nome[1]);
    printf("%s",nome);
}

void teste_sobreescritaArqBin(){
    FILE *fp = fopen("./arqs teste/filetesteparasobreescrita.bin","rb+");
    char nome[6];
    fseek(fp,(sizeof(char) * 6 * 3),SEEK_SET);
    strcpy(nome,"N0023");
    fwrite(&nome,sizeof(char),6,fp);
    fclose(fp);
}

int int_len(int num){
  if(num == 0) return 1;
  int count=0;
  while(num>0){
    num/=10;
    count++;
  }
  return count;
}

void teste_string1(){
    char nome[10],nome_limpo[10];
    strcpy(nome,"abcdefg");
    printf("%d",strlen(nome));
    strncpy(nome_limpo,nome,strlen(nome)-1); //nao pode usar (nome nome)
    printf("%s - %d",nome_limpo,strlen(nome_limpo));
}

void teste_string2(){
    char nomeCompleto[51],sobrenome[51];
    int pos=0;
    strcpy(nomeCompleto,"Pablo Afonso Gustavo Pereira");
    sscanf(nomeCompleto, "%*s %n", &pos);  // pega a posição após o primeiro nome
    strcpy(sobrenome, nomeCompleto + pos);
    printf("\n%s",sobrenome);
}

//lendo champions
void teste_leituraTxt(){ //FUNCIONA!!   THE ONE PIECE IS REAL!
    FILE *fp = fopen("champions.txt", "r");
    if (!fp) exit(1);
    char linha[300], *lido,sobrenome[51],sobrenome_limpo[51];
    int ano,campo,numTitulos;
    fgets(linha, sizeof(linha), fp);// Pula o cabeçalho
    //fgets(linha, sizeof(linha), fp);// pega primeira linha
    while(fgets(linha, sizeof(linha), fp)){
        lido = strtok(linha,"\\"); //pega a primeira parte até "\" que é o ano
        campo = 0;
        while(lido){
            if(campo == 0){
                ano = atoi(lido);
                printf("\nANO DE %d:\n",ano);
                campo++;
            }
            else{
                //        sem "s" dps
                sscanf(lido,"%[^(] (%*d/%d)",sobrenome,&numTitulos); // notacao "%*d" significa "ignorar esses  inteiro" //sim se vc colocar * voce pode ignorar qualquer coisa na leitura
                if(sobrenome[strlen(sobrenome) -1] == ' ') sobrenome[strlen(sobrenome) -1] = '\0';
                printf("%s - %d\n",sobrenome,numTitulos);
            }
            lido = strtok(NULL,"\\"); //vai pegando o conjunto de infos (até nao ter mais) do tipo:   "Lendl (1/1)" 
        }
    }
    /*
    //Mesma coisa só que para o proximo ano  (COMO ERA QUANDO LIA SÓ UM ANO)
    fgets(linha, sizeof(linha), fp);// pega primeira linha
    lido = strtok(linha,"\\"); //pega a primeira parte até "\" que é o ano
    campo = 0;
    while(lido){
        if(campo == 0){
            ano = atoi(lido);
            printf("\nANO DE %d:\n",ano);
            campo++;
        }
        else{
            //        sem "s" dps
            sscanf(lido,"%[^(] (%*d/%d)",sobrenome,&numTitulos); // notacao "%*d" significa "ignorar esses inteiro"
            if(sobrenome[strlen(sobrenome) -1] == ' ') sobrenome[strlen(sobrenome) -1] = '\0';
            printf("%s - %d\n",sobrenome,numTitulos);
        }
        lido = strtok(NULL,"\\"); //vai pegando o conjunto de infos (até nao ter mais) do tipo:   "Lendl (1/1)" 
    }*/


    fclose(fp);


    /*  saída quando lia só um ano
    ANO DE 1990:
    Lendl - 1
    G├│mez - 1
    Edberg - 7
    Sampras - 30
    Agassi - 27
    - - 27
    Edberg - 7
    Agassi - 27
    Chesnokov - 2
    Aguilera - 1
    Muster - 9
    Chang - 7
    Edberg - 7
    Becker - 9
    Edberg - 7
    */
}

int posPaisEmVet(char *pais,char *vetPais[40]){
    for(int i = 0; i<40; i++) if(vetPais[i]!=NULL && strcmp(pais,vetPais[i]) == 0) return i;
    return -1;
}

//lendo tenistas.txt
void teste_leituraTxt2(){ //AKA TARVBMT_criaPorTxt
    FILE *fp = fopen("tennis_players.txt","r");
    if(!fp) exit(1);
    TT tenista;
    char *lido, linha[120], nome[51];
    int ano, campo, indPais=10, indCpf=100, semanatemp=-1;
    int TTidPais,TTidCpf,TTidAno;
    //char vetPais[50][51]; //50 paises de no maximo 51 caracteres
    char *vetPais[40];
    for (int i = 0; i < 40; i++) {
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


        //!!  TARVBMT_insere(tenista)  !!

        //printf para debug -> printa todos os jogadores com suas infos (quase todas infos)
        printf("%s - %d: %d | %d | %s | %d | %d | %d | %d\n",tenista.nome,tenista.id,tenista.ano_nascimento,tenista.morte,tenista.pais,tenista.rank,tenista.YoBK,tenista.numSem,tenista.pontuacao);

    }

    fclose(fp);
    for (int i = 0; i < 40; i++) { //liberando o vetor de paises
        free(vetPais[i]);
    }
}

void Cria_indicesTeste(){
    FILE *fp = fopen("INDICES.bin","wb+");
    char nome[51];
    int id,num;

    //arq indice
    id = 20087967;
    num = 1;
    strcpy(nome,"N0000");
    fwrite(&nome,sizeof(char),6,fp);
    fwrite(&num,sizeof(int),1,fp);
    fwrite(&id,sizeof(int),1,fp);
    id = -1;
    fwrite(&id,sizeof(int),1,fp);
    id = -1;
    fwrite(&id,sizeof(int),1,fp);
    strcpy(nome,"N0001");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"N0002");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"\0");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"\0");
    fwrite(&nome,sizeof(char),6,fp);

    id = 1035780;
    num = 3;
    strcpy(nome,"N0001");
    fwrite(&nome,sizeof(char),6,fp);
    fwrite(&num,sizeof(int),1,fp);
    fwrite(&id,sizeof(int),1,fp);
    id = 1045780;
    fwrite(&id,sizeof(int),1,fp);
    id = 1055780;
    fwrite(&id,sizeof(int),1,fp);
    strcpy(nome,"F0000");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"F0001");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"F0002");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"F0003");
    fwrite(&nome,sizeof(char),6,fp);

    id = 30087570;
    num = 1;
    strcpy(nome,"N0002");
    fwrite(&nome,sizeof(char),6,fp);
    fwrite(&num,sizeof(int),1,fp);
    fwrite(&id,sizeof(int),1,fp);
    id = -1;
    fwrite(&id,sizeof(int),1,fp);
    id = -1;
    fwrite(&id,sizeof(int),1,fp);
    strcpy(nome,"F0004");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"F0005");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"\0");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"\0");
    fwrite(&nome,sizeof(char),6,fp);

    num = 6;
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);


    //arqs folhas
    fp = fopen("./infos/F0000.bin","wb+");
    TT tenista = TT_cria_vazio();
    num = 1;
    strcpy(tenista.nome,"Pedro");
    tenista.id = 1001010;
    strcpy(tenista.pais,"Brazil");
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("./infos/F0001.bin","wb+");
    num = 1;
    strcpy(tenista.nome,"Gustavo");
    tenista.id = 1001010;
    strcpy(tenista.pais,"Brazil");
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("./infos/F0002.bin","wb+");
    num = 1;
    strcpy(tenista.nome,"Leandro Barros");
    tenista.id = 1001010;
    strcpy(tenista.pais,"Brazil");
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("./infos/F0003.bin","wb+");
    num = 1;
    strcpy(tenista.nome,"Luis");
    tenista.id = 1001010;
    strcpy(tenista.pais,"Brazil");
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("./infos/F0004.bin","wb+");
    num = 1;
    strcpy(tenista.nome,"Fernando");
    tenista.id = 1001010;
    strcpy(tenista.pais,"Brazil");
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);

    fp = fopen("./infos/F0005.bin","wb+");
    num = 1;
    strcpy(tenista.nome,"Pablo");
    tenista.id = 1001010;
    strcpy(tenista.pais,"Brazil");
    fwrite(&tenista,sizeof(TT),1,fp);
    fwrite(&num,sizeof(int),1,fp);
    fclose(fp);
}

int main(void){
   //teste_appendArqBin();
   //ler_tudo();
   //printf("\n\n%d\n",int_len(5732));
   //teste_leituraTxt();
   //teste_string2();
    //teste_TT_completaInfoChampionsTxt();
    //teste_leituraTxt2();
    //Cria_indicesTeste();
   
}