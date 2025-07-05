
void processar_todos_os_rankings(int t) {
    MatCarac caracs;
    FILE* fp_caracs = fopen("./auxiliares/mat_caracs.bin", "rb");
    if (!fp_caracs) { perror("Erro"); exit(1); }
    fread(&caracs, sizeof(MatCarac), 1, fp_caracs);
    fclose(fp_caracs);

    if (caracs.capacidade == 0) {
        printf("Banco de dados vazio.\n");
        return;
    }

    // 1. Carregar TUDO para a memória UMA VEZ
    int* id_map = (int*) malloc(caracs.capacidade * sizeof(int));
    int* matriz_pontos = (int*) malloc(caracs.capacidade * NUM_ANOS * sizeof(int));

    // Verificações de malloc são essenciais aqui!
    if (!id_map || !matriz_pontos) {
        perror("Falha na alocação inicial");
        free(id_map);
        free(matriz_pontos);
        return;
    }
    
    FILE* fp_ids = fopen("./auxiliares/idMap.bin", "rb");
    // ... (verificar fp_ids) ...
    fread(id_map, sizeof(int), caracs.capacidade, fp_ids);
    fclose(fp_ids);

    FILE* fp_ptos = fopen("./auxiliares/matrizRankingPorAno.bin", "rb");
    // ... (verificar fp_ptos) ...
    fread(matriz_pontos, sizeof(int), caracs.capacidade * NUM_ANOS, fp_ptos);
    fclose(fp_ptos);
    
    // 2. Loop que chama a função otimizada
    for (int ano = 1990; ano <= 2024; ano++) {
        imprimir_top_N_ALT(ano, t, 25, caracs.capacidade, caracs.jogadores_registrados, id_map, matriz_pontos);
    }

    // 3. Liberar a memória UMA VEZ no final
    free(id_map);
    free(matriz_pontos);
}
void imprimir_top_N_ALT(int ano, int t, int N, int capacidade, int jogadores_registrados, const int* id_map, const int* matriz_pontos) {
    // A validação de ano e capacidade ainda é uma boa ideia
    if (ano < 1990 || ano > 2024) { /*...*/ return; }
    if (capacidade == 0) { /*...*/ return; }

    // Aloca memória apenas para o ranking do ano específico
    MatTenista* ranking_ano = (MatTenista*) malloc(capacidade * sizeof(MatTenista));
    if (!ranking_ano) {
        perror("Falha ao alocar ranking_ano");
        return;
    }

    int indice_ano = ano - 1990;
    for (int i = 0; i < capacidade; i++) {
        if (id_map[i] == 0) {
            ranking_ano[i].id = 0;
            ranking_ano[i].pontuacao = 0;
            continue;
        }
        ranking_ano[i].id = id_map[i];
        // Acesso direto à memória, sem I/O de arquivo!
        ranking_ano[i].pontuacao = matriz_pontos[i * NUM_ANOS + indice_ano];
    }

    // Ordena, formata a saída e imprime (código igual ao seu)
    qsort(ranking_ano, capacidade, sizeof(MatTenista), comparar_ptos);

    int count_impressos = 0;
    printf("--- TOP até %d do Ano %d ---\n", N, ano);
    for (int i = 0; i < capacidade && count_impressos < N; ++i) {
        //pula entradas vazias ou com pontuação zero
        if (ranking_ano[i].id == 0 || ranking_ano[i].pontuacao == 0) {
            continue;
        }
        TT tenista = TARVBMT_busca(ranking_ano[i].id, t);
        const char* nome_para_imprimir = (tenista.nome != NULL) ? tenista.nome : "Nome Nao Encontrado";

        printf("%3d) %s - Pontuacao: %d\n", count_impressos + 1, nome_para_imprimir,ranking_ano[i].pontuacao);
        count_impressos++;
    }
    printf("--------------------------------\n");

    // Libera apenas a memória alocada nesta função
    free(ranking_ano);
}