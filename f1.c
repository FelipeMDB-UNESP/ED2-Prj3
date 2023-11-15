#include "header.h"

typedef struct indice {
    char string[19];
    size_t endereco;
} INDICE;

typedef struct pagina {
    INDICE dados[3];
    char ponteiro[4];
} PAGINA;

typedef struct arv_b {
    size_t raiz;
    FILE* arq;
} ARV_B;

void inserir_registro(PASTA pasta, FILE* arq_dados, ARV_B* arvore);