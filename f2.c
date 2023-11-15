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

size_t rrn_converter(short ponteiro) {
    return sizeof(size_t)+sizeof(PAGINA)*ponteiro;
}

void extrair_pagina(PAGINA* pag, ARV_B* arvore) {

    size_t endereco = rrn_converter(arvore->raiz);
    fseek(arvore->arq, endereco, SEEK_SET);
    fread(pag, sizeof(PAGINA), 1, arvore->arq);

}

void imprimir_registro(size_t endereco, FILE* arquivoDados) {

    fseek(arquivoDados, endereco, SEEK_SET);
    
    REGISTRO registro = criar_registro();
    char caractere;

    //abaixo vai ler caractere por caractere dos campos
    while((fread(&caractere,sizeof(char),1,arquivoDados)) && caractere != '|'){
        add_caractere_string(&(registro->codCliente),caractere);
    }

    while((fread(&caractere,sizeof(char),1,arquivoDados)) && caractere != '|'){
        add_caractere_string(&(registro->codVeiculo),caractere);
    }

    while((fread(&caractere,sizeof(char),1,arquivoDados)) && caractere != '|'){
        add_caractere_string(&(registro->nomeCliente),caractere);
    }

    while((fread(&caractere,sizeof(char),1,arquivoDados)) && caractere != '|'){
        add_caractere_string(&registro->nomeVeiculo,caractere);
    }

    //aqui ele le um int inteiro, como foi armazenado no arquivo
    fread(registro->quantDias, sizeof(char)*4, 1, arquivoDados);

    //impressao do dado
    printf("Registro:\n");
    printf("Codigo Cliente:%s\n", registro->codCliente);
    printf("Codigo Veiculo:%s\n", registro->codVeiculo);
    printf("Nome Cliente:%s\n", registro->nomeCliente);
    printf("Nome Veiculo:%s\n", registro->nomeVeiculo);
    printf("Quantidade de Dias: %d\n\n", *(registro->quantDias));
    free(registro);
}

void listar_dado(FILE* arq_dados, ARV_B* arvore) {
    PAGINA pag;
    extrair_pagina(&pag,arvore);

    if (pag.ponteiro[0] !=-1) {
        fseek(arvore->arq, rrn_converter(pag.ponteiro[0]), SEEK_SET);
        listar_dado(arq_dados, arvore);
    }

    imprimir_registro(pag.dado[0].endereco, arq_dados);

    if (pag.ponteiro[1] !=-1) {
        fseek(arvore->arq, rrn_converter(pag.ponteiro[1]), SEEK_SET);
        listar_dado(arq_dados, arvore);
    }

    if (pag.qtd_dados > 1)
        imprimir_registro(pag.dado[1].endereco, arq_dados);

    if (pag.ponteiro[2] !=-1) {
        fseek(arvore->arq, rrn_converter(pag.ponteiro[2]), SEEK_SET);
        listar_dado(arq_dados, arvore);
    }

    if (pag.qtd_dados > 2)
        imprimir_registro(pag.dado[2].endereco, arq_dados);

    if (pag.ponteiro[3] !=-1) {
        fseek(arvore->arq, rrn_converter(pag.ponteiro[3]), SEEK_SET);
        listar_dado(arq_dados, arvore);
    }
}

void listar_dados(FILE* arq_dados, ARV_B* arvore) {
    if (arvore->raiz != -1) {
        fseek(arvore->arq, rrn_converter(arvore->raiz), SEEK_SET);
        listar_dado(arq_dados, arvore);
    }
}