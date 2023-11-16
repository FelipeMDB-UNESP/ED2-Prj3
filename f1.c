#include "header.h"

typedef struct indice {
    char chave_primaria[19];
    short rrn;
} INDICE;

typedef struct pagina {
    short qtd_dados;
    INDICE dado[3];
    short ponteiro[4];
} PAGINA;

typedef struct arv_b {
    short raizRRN;
    FILE* arq;
} ARV_B;

enum status_da_promocao
{
    NAO,
    SIM,
    DUPLICADO
};

// Função para calcular o tamanho de um arquivo
size_t calcularTamanhoArquivo(FILE* arquivo) {
    fseek(arquivo, 0, SEEK_END);
    size_t tamanho = ftell(arquivo);
    rewind(arquivo);
    return tamanho;
}

short obter_pagina(FILE *arvore_b)
{
    fseek(arvore_b, 0, 2);
    long addr = ftell(arvore_b);
    addr -= (long)2;
    return (addr > 0) ? (short)addr / sizeof(PAGINA) : 0;
}

void iniciar_pagina(PAGINA *pag)
{
    for (int i = 0; i < 3; i++)
    {
        strcpy(pag->dado[i].chave_primaria, "@@@@");
        pag->dado[i].rrn = -1;
        pag->ponteiro[i] = -1;
    }
    pag->ponteiro[3] = -1;
}

void escrever_pagina(FILE *arvore_b, short rrn, PAGINA *pag)
{
    long addr;
    addr = (long)rrn * (long)sizeof(PAGINA) + (long)2;
    fseek(arvore_b, addr, 0);
    fwrite(pag, sizeof(PAGINA), 1, arvore_b);
}

void posicionar_em_raiz(FILE *arvore_b, short raiz)
{
    fseek(arvore_b, 0, 0);
    fwrite(&raiz, sizeof(short), 1, arvore_b);
}

short criar_raiz(FILE *arvore_b, INDICE novo_indice, short esquerda, short direita)
{
    PAGINA pag;
    short rrn = obter_pagina(arvore_b);
    iniciar_pagina(&pag);
    pag.dado[0] = novo_indice;
    pag.ponteiro[0] = esquerda;
    pag.ponteiro[1] = direita;
    pag.qtd_dados = (short)1;
    escrever_pagina(arvore_b, rrn, &pag);
    posicionar_em_raiz(arvore_b, rrn);
    return rrn;
}


short criar_arvore(FILE **arvore_b, INDICE indice)
{
    short temp = -1;
    if ((*arvore_b = fopen("arvoreB.bin", "w+b")) == NULL)
    {
        printf("\nErro ao abrir o arquivo!\n");
        return temp;
    }
    fwrite(&temp, sizeof(short), 1, *arvore_b);
    return criar_raiz(*arvore_b, indice, -1, -1);
}

void ler_pagina(FILE *arvore_b, short rrn, PAGINA *pag)
{
    long addr = (long)rrn * (long)sizeof(PAGINA) + (long)2;
    fseek(arvore_b, addr, 0);
    fread(pag, sizeof(PAGINA), 1, arvore_b);
}

short encontrar_no(INDICE indice, PAGINA *pag, short *pos) //encontrar nó
{
    int i = 0;
    for (i = 0; i < pag->qtd_dados && strcmp(indice.chave_primaria, pag->dado[i].chave_primaria) > 0; i++)
    {
        //nada;
    }
    *pos = i;
    return (*pos < pag->qtd_dados && strcmp(indice.chave_primaria, pag->dado[*pos].chave_primaria) == 0) ? SIM : NAO;
}

void separar(FILE *arvore_b, INDICE novo_indice, short rFilho, PAGINA *pagina_antiga, INDICE *promotedKey, short *promotedRChild, PAGINA *newPage)
{
    printf("\nRealizando divisão de no");

    int j;
    INDICE workKeys[3 + 1];
    short workChild[3 + 2];

    INDICE chave_nula = {.chave_primaria ="@@@@", .rrn=-1};

    for (j = 0; j < 3; j++)
    {
        workKeys[j] = pagina_antiga->dado[j]; //copia chaves e filhos
        workChild[j] = pagina_antiga->ponteiro[j];

        pagina_antiga->dado[j] = chave_nula; //apaga pagina antiga
        pagina_antiga->ponteiro[j] = -1;
    }
    workChild[j] = pagina_antiga->ponteiro[j];
    pagina_antiga->ponteiro[j] = -1;


    for (j = 3; strcmp(novo_indice.chave_primaria, workKeys[j - 1].chave_primaria) < 0 && j > 0; j--)
    {
        workKeys[j] = workKeys[j - 1]; //insere nova pagina
        workChild[j + 1] = workChild[j];
    }
    workKeys[j] = novo_indice;
    workChild[j + 1] = rFilho;

    *promotedRChild = obter_pagina(arvore_b);
    iniciar_pagina(newPage);

    const int minino_chaves = (3 + 1) / 2; //=2

    //Separa 2 chaves para cada pagina
    for (j = 0; j < minino_chaves; j++)
    {
        pagina_antiga->dado[j] = workKeys[j]; //copia metade inferior do vetor para pagina_antiga
        pagina_antiga->ponteiro[j] = workChild[j];

        newPage->dado[j] = workKeys[j + minino_chaves]; //copia metade superior vetor para newPage
        newPage->ponteiro[j] = workChild[j + minino_chaves];
    }
    //ultimos ponteiros de filhos
    newPage->ponteiro[minino_chaves] = workChild[j + minino_chaves];

    //remove chave promovida da pagina antiga
    pagina_antiga->dado[minino_chaves - 1] = chave_nula;
    pagina_antiga->ponteiro[minino_chaves] = -1;

    newPage->qtd_dados = 3 - 1;
    pagina_antiga->qtd_dados = minino_chaves - 1;

    *promotedKey = workKeys[minino_chaves - 1];

    printf("\nChave %s promovida", (*promotedKey).chave_primaria);
}

void inserir_na_pagina(FILE *arvore_b, INDICE novo_indice, short rFilho, PAGINA *pag)
{
    int j;
    for (j = pag->qtd_dados; strcmp(novo_indice.chave_primaria, pag->dado[j - 1].chave_primaria) < 0 && j > 0; j--)
    {
        pag->dado[j] = pag->dado[j - 1];
        pag->ponteiro[j + 1] = pag->ponteiro[j];
    }
    pag->qtd_dados += 1;
    pag->dado[j] = novo_indice;
    pag->ponteiro[j + 1] = rFilho;
}

short adicionar_index(FILE *arvore_b, short rrn, INDICE novo_indice, short *filho_promovido, INDICE *chave_promovida)
{
    if (rrn == -1)
    {
        *chave_promovida = novo_indice;
        *filho_promovido = -1;
        return SIM;
    }
    PAGINA pagina_atual;
    PAGINA nova_pagina;
    ler_pagina(arvore_b, rrn, &pagina_atual);

    short pos;
    int achou = encontrar_no(novo_indice, &pagina_atual, &pos);
    if (achou)
    {
        printf("\nERRO: Chave %s duplicada", novo_indice.chave_primaria);
        return DUPLICADO;
    }
    short rrr_promovido_recursivamente;
    INDICE chave_promovida_recursivamente;
    short promovido = adicionar_index(arvore_b, pagina_atual.ponteiro[pos], novo_indice, &rrr_promovido_recursivamente, &chave_promovida_recursivamente);

    if (promovido == DUPLICADO)
        return DUPLICADO;

    if (!promovido)
        return NAO;

    if (pagina_atual.qtd_dados < 3)
    {
        inserir_na_pagina(arvore_b, chave_promovida_recursivamente, rrr_promovido_recursivamente, &pagina_atual);
        escrever_pagina(arvore_b, rrn, &pagina_atual);
        return NAO;
    }
    else
    {
        separar(arvore_b, chave_promovida_recursivamente, rrr_promovido_recursivamente, &pagina_atual, chave_promovida, filho_promovido, &nova_pagina);
        escrever_pagina(arvore_b, rrn, &pagina_atual); //Atualiza pagina atual
        escrever_pagina(arvore_b, *filho_promovido, &nova_pagina);
        return SIM;
    }
}

// Função para atualizar o arquivo de índices
int add_indice_arvore_b(FILE** arvore_b, INDICE novo_indice, short *raizRRN ) {

    if (*arvore_b == NULL)
    {
        *raizRRN = criar_arvore(arvore_b, novo_indice);
        printf("\nChave %s inserida com sucesso", novo_indice.chave_primaria);
        return SIM;
    }

    short rrn_promovido = -1;
    INDICE chave_promovida;

    int promovido = adicionar_index(*arvore_b, *raizRRN, novo_indice, &rrn_promovido, &chave_promovida);
    if (promovido == SIM)
        *raizRRN = criar_raiz(*arvore_b, chave_promovida, *raizRRN, rrn_promovido);

    if (promovido == DUPLICADO)
        return DUPLICADO;
    else
    {
        printf("\nChave %s inserida com sucesso", novo_indice.chave_primaria);
        return SIM;
    }

}

void inserir_registro(PASTA pasta, FILE* arq_dados, ARV_B* arvore_b_indices){

    int posRegistro;
    printf("Qual o indice do registro que deseja inserir?");
    scanf("%d", &posRegistro);

    // Calcular tamanho do registro
    size_t tamanhoRegistro = strlen(pasta[posRegistro-1]->codCliente) + 1 +
                             strlen(pasta[posRegistro-1]->codVeiculo) + 1 +
                             strlen(pasta[posRegistro-1]->nomeCliente) + 1 +
                             strlen(pasta[posRegistro-1]->nomeVeiculo) + 1 +
                             sizeof(int) + 1;

    // Obter o byte offset atual
    size_t byteOffset = calcularTamanhoArquivo(arq_dados);

    STRING chavePrimaria = (STRING)malloc(strlen(pasta[posRegistro-1]->codCliente) + strlen(pasta[posRegistro-1]->codVeiculo) + 1);

    if (chavePrimaria != NULL) {
    strcpy(chavePrimaria, ""); // Inicialize a chavePrimaria com uma string vazia
    strcat(chavePrimaria, pasta[posRegistro - 1]->codCliente);
    strcat(chavePrimaria, pasta[posRegistro - 1]->codVeiculo);
    } else {
        printf("Falha na alocacao de memoria.\n");
    }

    //Faz a checagem se esse registro já foi inserido
    /*short existe_chave = pesquisar_dado(chavePrimaria, arvore_b_indices, arq_dados);
    if(existe_chave != -1){
        printf("A chave em questão já existe no arquivo!\n");
        return;
    }*/

    INDICE indice;
    strcpy(indice.chave_primaria, chavePrimaria);
    indice.rrn = byteOffset;

    // Atualizar o arquivo de índices com o novo byte offset
    add_indice_arvore_b(&(arvore_b_indices->arq), indice, &(arvore_b_indices->raizRRN));

    // libereção de memória não mais necessária
    free(chavePrimaria);

    // Escrever o tamanho do registro no início do registro
    fwrite(&tamanhoRegistro, 1, 1, arq_dados);

    // Crie cópias das strings com '|' como separador
    char codClienteCopy[strlen(pasta[posRegistro - 1]->codCliente) + 2]; // +2 para acomodar o '|'
    char codVeiculoCopy[strlen(pasta[posRegistro - 1]->codVeiculo) + 2];
    char nomeClienteCopy[strlen(pasta[posRegistro - 1]->nomeCliente) + 2];
    char nomeVeiculoCopy[strlen(pasta[posRegistro - 1]->nomeVeiculo) + 2];

    strcpy(codClienteCopy, pasta[posRegistro - 1]->codCliente);
    strcat(codClienteCopy, "|");

    strcpy(codVeiculoCopy, pasta[posRegistro - 1]->codVeiculo);
    strcat(codVeiculoCopy, "|");

    strcpy(nomeClienteCopy, pasta[posRegistro - 1]->nomeCliente);
    strcat(nomeClienteCopy, "|");

    strcpy(nomeVeiculoCopy, pasta[posRegistro - 1]->nomeVeiculo);
    strcat(nomeVeiculoCopy, "|");

    // Escrevendo os campos no arquivo 
    fwrite(codClienteCopy, sizeof(char), strlen(codClienteCopy), arq_dados);
    fwrite(codVeiculoCopy, sizeof(char), strlen(codVeiculoCopy), arq_dados);
    fwrite(nomeClienteCopy, sizeof(char), strlen(nomeClienteCopy), arq_dados);
    fwrite(nomeVeiculoCopy, sizeof(char), strlen(nomeVeiculoCopy), arq_dados);
    fwrite((pasta[posRegistro - 1]->quantDias), sizeof(int), 1, arq_dados);

    // Escrevendo o delimitador "|" no fim do registro
    char delimitador = '|';
    fwrite(&delimitador, sizeof(char), 1, arq_dados);


    printf("Novo registro inserido no byte offset: %lu\n", byteOffset);
}

int main() {
    //quantidade de dados no arquivo insere.bin
    int tam;
    //pasta de registros, ponteiro para registros
    PASTA pasta;
    //basicamente um ponteiro para chaves
    PARAGRAFO chaves;

    //booleano para verificar se os dados foram carregados na memoria
    bool load_de_arquivos = false;
    //opcao do menu 
    int opcao;
    
    do {
        // Exibe o menu
        printf("Menu:\n");
        printf("1. Insercao\n");
        printf("3. Carregar Arquivo\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao (0-4): ");
        scanf(" %d", &opcao);
        
        //criacao do ponteiro para os arquivos que serao criados 
        FILE* arq_data;
        FILE* arq_index;
        
        switch (opcao) {
            //executa a insercao
            case 1:
                if (load_de_arquivos) {

                    arq_data = fopen("dados.bin", "a+b");

                    arq_index = fopen("indices.bin", "a+b");
                    ARV_B arvore_b = {.arq = arq_index, .raizRRN = -1};

                    printf("\nDigite um numero entre 1 e %d\n", tam);
                    inserir_registro(pasta, arq_data, &arvore_b);   

                    fclose(arq_data);
                    fclose(arq_index);
                }
                else {
                    printf("Arquivos ainda nao foram carregados!\n");
                } 
                break;
            //carrega os dados
            case 3:
                pasta = carregar_dados("insere.bin");
                tam = tam_pasta(&pasta);
                chaves = carregar_chaves("busca_p.bin");
                atualiza_log("Arquivos carregados.");
                load_de_arquivos = true;
                break;
            case 0:
                exit(0);
        } 
    } while (opcao != 0);   
   
    return 0;
}