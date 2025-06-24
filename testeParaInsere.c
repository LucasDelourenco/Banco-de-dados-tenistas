#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tenistas{
  //tenistas.txt
  int id, ano_nascimento, morte, rank, YoBK, numSem;
  char nome[51], pais[51];  //nome é nome + sobrenome
  //champions.txt
  int pontuacao;
  int anoGanhouTodosGrands; //Q(5)
  int TorneiosGanhos[15];
}TT;

void gerar_nome_no(char *retorno, int num_nos_internos){
  sprintf(retorno, "N%04d", num_nos_internos);
}

void gerar_nome_folha(char *retorno, int num_folhas){
  sprintf(retorno, "F%04d", num_folhas);
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
  char no, nome_no[6], *aux = (char*)malloc(sizeof(char)*11);
  int pont_aux; //ponteiro auxiliar, facilita o uso de fseek e ftell
  int j, nchaves_z, *chaves_z = (int*)malloc(sizeof(int)*((2*t)-1));
  int nfolhas, ntenistas, chave_que_sobe, temp, chave_atual;
  int chave_vazia = -1;

  for (j=0;j<(2*t)-1;j++) chaves_z[j] = -1;

  char **filhos_z = (char **)malloc(sizeof(char*)*2*t);
  for (j=0;j<(2*t);j++){
    filhos_z[j] = malloc(sizeof(char)*6);
    filhos_z[j] = "\0\0\0\0\0\0";
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

    //Atualizar as chaves do pai
    for (j=0; j<temp; j++){
      fread(&chave_atual, sizeof(int), 1, f_indice); //Le a chave na posicao j
      pont_aux = ftell(f_indice) - sizeof(int);
      fseek(f_indice, pont_aux, SEEK_SET);           //Volta um inteiro para poder sobrescrever

      if (chave_que_sobe < chave_atual || chave_atual == -1){ //Insere em ordem crescente; se a chave for menor que a atual
        fwrite(&chave_que_sobe, sizeof(int), 1, f_indice);     //Ou se a atual está vazia(-1)
        chave_que_sobe = chave_atual;  //A chave_atual foi apagada do arquivo, precisamos inserir ela de volta
      }
      else fseek(f_indice, sizeof(int), SEEK_CUR);
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
        aux = aumenta_um(aux);
        fwrite(aux, sizeof(char), 6, f_indice); //Escreve no espaco vazio
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
      aux = aumenta_um(aux);
      fwrite(aux, sizeof(char), 6, f_indice);
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

    strcpy(aux, nome_no);
    aux = aumenta_um(aux);
    fwrite(aux, sizeof(char), 6, fnovo);
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
      if ((j%nfolhas-1) == 0){
        pont_aux = buscar_pos_no(f_indice, tmpNomeFolha1, t) + sizeof(char)*6;
        fseek(f_indice, pont_aux, SEEK_SET);
        fwrite(tmpNomeFolha2, sizeof(char), 6, f_indice); //Sobreescreve o nome da folha vazia
      }
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
    strcpy(nome,"\0\0\0\0\0\0");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"\0\0\0\0\0\0");
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
    strcpy(nome,"\0\0\0\0\0\0");
    fwrite(&nome,sizeof(char),6,fp);
    strcpy(nome,"\0\0\0\0\0\0");
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
  
  Cria_indicesTeste();

  FILE *fp = fopen("INDICES.bin","rb+");
  long pai = buscar_pos_no(fp,"N0000",2),y = buscar_pos_no(fp,"N0001",2);
  fclose(fp);
  divisao_MS("INDICES.bin",pai,y,2);
  
  
}