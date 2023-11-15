#include "header.h"

//armazenamento chave-endereco usado no indice
typedef struct indice {
    char string[19];
    size_t endereco;
} INDICE;

//bloco considerado o "no" da arvore b
typedef struct pagina {
    short qtd_dados;
    INDICE dado[3];
    short ponteiro[4];
} PAGINA;

//struct para manipular o arquivo e a raiz da arvore b a ser utilizada
typedef struct arv_b {
    short raiz;
    FILE* arq;
} ARV_B;

//converte o rrn(ponteiro) em endereco no arquivo de indices
size_t rrn_converter(short ponteiro) {
    return sizeof(size_t)+sizeof(PAGINA)*ponteiro;
}

//realiza a leitura da pagina previamente posicionada com fseek
void extrair_pagina(PAGINA* pag, ARV_B* arvore) {
    fread(pag, sizeof(PAGINA), 1, arvore->arq);
}

//impressao do registro do arquido de dados original com base no endereco de memoria
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

//funcao usada para imprimir os registros de uma dada arvore b
void listar_pagina(FILE* arq_dados, ARV_B* arvore) {

    //extracao da pagina previamente posicionada
    PAGINA pag;
    extrair_pagina(&pag, arvore);

    //checagem caso os rrn (ponteiros) existam, procurando os menores itens da arvore
    if (pag.ponteiro[0] !=-1) {
        fseek(arvore->arq, rrn_converter(pag.ponteiro[0]), SEEK_SET);
        listar_pagina(arq_dados, arvore);
    }

    //impressao do primeiro item apos as recursoes
    imprimir_registro(pag.dado[0].endereco, arq_dados);

    //logica similar as anteriores
    if (pag.ponteiro[1] !=-1) {
        fseek(arvore->arq, rrn_converter(pag.ponteiro[1]), SEEK_SET);
        listar_pagina(arq_dados, arvore);
    }

    if (pag.qtd_dados > 1)
        imprimir_registro(pag.dado[1].endereco, arq_dados);

    if (pag.ponteiro[2] !=-1) {
        fseek(arvore->arq, rrn_converter(pag.ponteiro[2]), SEEK_SET);
        listar_pagina(arq_dados, arvore);
    }

    if (pag.qtd_dados > 2)
        imprimir_registro(pag.dado[2].endereco, arq_dados);

    if (pag.ponteiro[3] !=-1) {
        fseek(arvore->arq, rrn_converter(pag.ponteiro[3]), SEEK_SET);
        listar_pagina(arq_dados, arvore);
    }
}

//lista todos os dados se houver dados na arvore b
void listar_dados(FILE* arq_dados, ARV_B* arvore) {
    if (arvore->raiz != -1) {
        fseek(arvore->arq, rrn_converter(arvore->raiz), SEEK_SET);
        listar_pagina(arq_dados, arvore);
    }
}

//funcao para usada para pesquisa de uma chave na arvore b, retorna a posicao da chave e, caso inexistente, -1.
short pesquisar_dado(STRING* chave, short pagina, ARV_B* arvore) {
    PAGINA pag;
    extrair_pagina(&pag, arvore);
    
    //compara a chave com o primeiro valor de indices
    //se for menor, checa o rrn anterior a ela
    if(strcpm(*chave, pag.dado[0].string) == -1)  {
        if (pag.ponteiro[0] != -1) {
            fseek(arvore->arq, rrn_converter(pag.ponteiro[0]), SEEK_SET);
            return pesquisar_dado(chave, pag.ponteiro[0], arvore);
        } else {
            printf("\nChave nao encontrada: %s.\n", *chave); //imprimi caso seja = -1 (chave nao existe)
            return -1;
        }
    }

    //se for igual, printa que encontrou e retorna uma compilacao de sua posicao
    if(strcpm(*chave, pag.dado[0].string) == 0) {
        printf("\nChave encontrada: %s. (Pag: %d | Pos: %d)\n", *chave, pagina, 0); //imprimi em caso de exista
        return pagina*10;
    }

    //se for maior compara com o proximo (se ele existir)

    if(strcpm(*chave, pag.dado[1].string) == -1)  {
        if (pag.ponteiro[1] != -1) {
            fseek(arvore->arq, rrn_converter(pag.ponteiro[1]), SEEK_SET);
            return pesquisar_dado(chave, pag.ponteiro[1], arvore);
        } else {
            printf("\nChave nao encontrada: %s.\n", *chave);
            return -1;
        }
    }

    if(strcpm(*chave, pag.dado[1].string) == 0 && pag.qtd_dados > 1) {
        printf("\nChave encontrada: %s. (Pag: %d | Pos: %d)\n", *chave, pagina, 1);
        return pagina*10 + 1;
    }

    if(strcpm(*chave, pag.dado[2].string) == -1)  {
        if (pag.ponteiro[2] != -1) {
            fseek(arvore->arq, rrn_converter(pag.ponteiro[2]), SEEK_SET);
            return pesquisar_dado(chave, pag.ponteiro[2], arvore);
        } else {
            printf("\nChave nao encontrada: %s.\n", *chave);
            return -1;
        }
    }

    if(strcpm(*chave, pag.dado[2].string) == 0) {
        printf("\nChave encontrada: %s. (Pag: %d | Pos: %d)\n", *chave, pagina, 2);
        return pagina*10 + 2;
    }


    if (pag.ponteiro[3] != -1) {
        fseek(arvore->arq, rrn_converter(pag.ponteiro[3]), SEEK_SET);
        return pesquisar_dado(chave, pag.ponteiro[3], arvore);
    } else {
        printf("\nChave nao encontrada: %s.\n", *chave);
        return -1;
    }
}

void pesquisar_chave(STRING* chave, ARV_B* arvore, FILE* arq_dados) {
    short contrasenha;
    if (arvore->raiz != -1) {
        fseek(arvore->arq, rrn_converter(arvore->raiz), SEEK_SET);
        if ((contrasenha == pesquisar_dado(chave, arvore->raiz, arvore->arq)) != -1) {
            PAGINA pag;
            fseek(arvore->arq, rrn_converter(contrasenha/10), SEEK_SET);
            extrair_pagina(&pag, arvore);
            imprimir_registro(pag.dado[contrasenha%10].endereco, arq_dados);
        }
    } else {
        printf("\nChave nao encontrada: %s.\n", *chave);
    }
}