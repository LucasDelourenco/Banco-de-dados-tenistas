typedef struct tabelaComparacaoNomeID{
    unsigned long int id;
    char nome[51];
}TTCN; //tipo tabela comparacao nome

//quando o programa for iniciado é criado um vetor de tamanho sla 500
//TTCN *tabelaCompNome = (TTCN *)malloc(sizeof(TTCN) * 500)