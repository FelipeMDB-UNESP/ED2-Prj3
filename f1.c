#include "header.h"

typedef struct indice {
    char string[19];
    size_t endereco;
} INDICE;

typedef struct pagina {
    short qtd_dados;
    INDICE dado[3];
    short ponteiro[4];
} PAGINA;

typedef struct arv_b {
    short raiz;
    FILE* arq;
} ARV_B;

void inserir_registro(PASTA pasta, FILE* arq_dados, ARV_B* arvore);