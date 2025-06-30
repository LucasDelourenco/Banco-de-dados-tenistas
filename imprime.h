#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef IMPRIME_H
#define IMPRIME_H



void imprime_folha(const char* id_folha, int andar);
void imprime_recursivo(FILE *fp_indice, int t, long pos_no, int andar);
void imprime(const char *arq_indice, int t);

#endif // IMPRIME_H