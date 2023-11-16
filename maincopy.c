//André Nogueira Dias
//Felipe de Britto
//Rafael Daiki
//Mestre Big Ryan

#include "header.h"

#pragma region codigo_feito

#pragma region criacao_alocacoes_dinamicas

//Aloca espaço para um int
NUMERO criar_numero() {
    NUMERO num = (NUMERO) malloc(sizeof(int));
    *num = -1;
    return num;
}

//Aloca espaço para um vetor de caracteres unitario
STRING criar_string() {
    STRING str = (STRING) malloc(sizeof(char));
    str[0]='\0';
    return str;
}

//Aloca espaço para um vetor de strings unitario
PARAGRAFO criar_paragrafo() {
    PARAGRAFO par = (PARAGRAFO) malloc(sizeof(STRING));
    par[0] = NULL;
    return par;
}

//Aloca espaço para um registro de dados
REGISTRO criar_registro() {
    REGISTRO reg = (REGISTRO) malloc(sizeof(DADOS));
    reg->codCliente = criar_string();
    reg->codVeiculo = criar_string();
    reg->nomeCliente = criar_string();
    reg->nomeVeiculo = criar_string();
    reg->quantDias = criar_numero();
    return reg;
}

//Aloca espaço para um vetor de registros unitario
PASTA criar_pasta() {
    PASTA pst = (PASTA) malloc(sizeof(REGISTRO));
    pst[0]=NULL;
    return pst;
}
#pragma endregion criacao_alocacoes_dinamicas

#pragma region limpeza_alocacoes_dinamicas

//Limpar a alocação de um int
void limpar_numero(NUMERO* num) {
    free(*num);
}

//Limpar a alocação de um vetor de caracteres
void limpar_string(STRING* str) {
    free(*str);
}

//Limpar a alocação de um vetor de strings
void limpar_parafrago(PARAGRAFO* par) {
    int i;
    for (i=0; (*par)[i] != NULL; i++) {
        limpar_string(&(*par)[i]);
    }
    free(*par);
}

//Limpar a alocação de um registro de dados
void limpar_registro(REGISTRO* reg) {
    limpar_string(&(*reg)->codCliente);
    limpar_string(&(*reg)->codVeiculo);
    limpar_string(&(*reg)->nomeCliente);
    limpar_string(&(*reg)->nomeVeiculo);
    limpar_numero(&(*reg)->quantDias);
    free(*reg);
}

//Limpar a alocação de um vetor de registros
void limpar_pasta(PASTA* pst) {
    int i;
    //
    for (i=0; (*pst)[i] != NULL; i++) {
        limpar_registro(&((*pst)[i]));
    }
    free(*pst);
}

#pragma endregion limpeza_alocacoes_dinamicas

#pragma region copia_alocacoes_dinamicas
// Copia o conteudo de uma string para outra
void copiar_string(STRING* new, STRING* str) {
    strcpy(*new,*str);
}

#pragma endregion copia_alocacoes_dinamicas

#pragma region tamanho_alocacoes_dinamicas
// Calcula o tamanho de um vetor de strings
int tam_paragrafo(PARAGRAFO* par) {
    int i;
    for (i=0; (*par)[i] != NULL; i++) {
    }
    return i;
}
// Calcula o tamanho de um vetor de registros
int tam_pasta(PASTA* pst) {
    int i;
    for (i=0; (*pst)[i] != NULL; i++) {
    }
    return i;
}

#pragma endregion tamanho_alocacoes_dinamicas

#pragma region insercao_alocacoes_dinamicas
// Adiciona um caractere a uma string
void add_caractere_string(STRING* str, char caractere) {
    STRING novaString;
    novaString = (STRING) malloc(sizeof(char) * strlen(*str) + 2);
    novaString[strlen(*str) + 1] = '\0';
    strcpy(novaString, *str);
    novaString[strlen(*str)] = caractere;
    limpar_string(str);
    *str = novaString;
}
// Faz a concatenação de duas strings
void add_string_string(STRING* str1, STRING* str2) {
    STRING novaString;
    novaString = (STRING) malloc(strlen(*str1) + strlen(*str2) + 1);
    copiar_string(&novaString, str1);
    strcat(novaString, *str2);
    limpar_string(str1);
    *str1 = novaString;
}
// Adiciona uma string ao vetor strings
void add_string_paragrafo(PARAGRAFO* par, STRING* str) {
    
    int i;
    int tam = tam_paragrafo(par);
    PARAGRAFO novoParagrafo;
    novoParagrafo = (PARAGRAFO) malloc(sizeof(STRING) * (tam + 2));
    
    //Loop copiando strings de par para novoParagrafo
    for (int i = 0; i < tam; i++) {
        novoParagrafo[i] = (*par)[i];
    }

    //Insere a string e o NULL do final do paragrafo
    novoParagrafo[tam] = *str;
    novoParagrafo[tam+1] = NULL;

    //Limpa a alocação anterior, sem limpar os ponteiros internos
    free(*par);

    //Coloca a string recebida apontando para NULL a fim de evitar problemas de segmentation fault
    *str = NULL;

    //Passa o novo paragrafo criado ao endereço recebido como parâmetro
    *par = novoParagrafo;
}
// Adiciona um registro no vetor de registros
void add_registro_pasta(PASTA* pst, REGISTRO* reg) {
    int i;
    int tam = tam_pasta(pst);
    PASTA novaPasta;
    novaPasta = (PASTA) malloc(sizeof(REGISTRO) * (tam + 2));
    // Copia os elementos da pasta pst para a novaPasta
    for (int i = 0; i < tam; i++) {
        novaPasta[i] = (*pst)[i];
    }

    novaPasta[tam] = *reg; // Adiciona o registro no fim da pasta
    novaPasta[tam+1] = NULL;

    free(*pst);

    *reg = NULL;
    
    *pst = novaPasta;
}

#pragma endregion insercao_alocacoes_dinamicas

#pragma endregion codigo_feito
// Faz a abertura do arquivo binário
FILE* abrir_arquivo_binario(STRING nome_do_arquivo) {

    FILE* arq;
    bool inexistencia;
    char recado[64];

    arq = fopen(nome_do_arquivo, "rb");
    inexistencia = (arq == NULL);


    if (inexistencia) {
        strcpy(recado, "Arquivo Binário \"");
        strcat(recado, nome_do_arquivo);
        strcat(recado, "\" Inexistente.");
        return NULL;
    } else {
        fclose(arq);
    }


    strcpy(recado, "Arquivo Binário \"");
    strcat(recado, nome_do_arquivo);
    strcat(recado, "\" Aberto.");

    arq = fopen(nome_do_arquivo, "r+b");
    return arq;
}

//Funcoes de trabalho com strings de tamanho variavel.

// Carrega os dados e retorna um vetor de registro
PASTA carregar_dados(STRING nomeArquivoInsercao) {
    FILE* arq;
    arq = abrir_arquivo_binario(nomeArquivoInsercao);
    if (arq == NULL) {
        // Tratamento de erro: não foi possível abrir o arquivo.
        exit(0);
    }

    int cont;
    char caractere;
    PASTA pasta = criar_pasta();
    REGISTRO registro;
    bool fim_de_arquivo = false;

    STRING inteiro;
    // Faz a leitura dos registros do arquivo e adiciona num vetor de Registros
    while (!fim_de_arquivo) {
        
        registro = criar_registro();
        inteiro = criar_string();

        while(((cont = fread(&caractere,sizeof(char),1,arq)) != 0) && caractere != '\0'){
            add_caractere_string(&(registro->codCliente),caractere);
        }

        while(((cont = fread(&caractere,sizeof(char),1,arq)) != 0) && caractere != '\0'){
            add_caractere_string(&(registro->codVeiculo),caractere);
        }

        while(((cont = fread(&caractere,sizeof(char),1,arq)) != 0) && caractere != '\0'){
            add_caractere_string(&(registro->nomeCliente),caractere);
        }

        while (((cont = fread(&caractere,sizeof(char),1,arq)) != 0) && caractere == '\0') {}

        add_caractere_string(&registro->nomeVeiculo,caractere);
        while(((cont = fread(&caractere,sizeof(char),1,arq)) != 0) && caractere != '\0'){
            add_caractere_string(&registro->nomeVeiculo,caractere);
        }

        while (((cont = fread(&caractere,sizeof(char),1,arq)) != 0) && caractere == '\0') {}

        add_caractere_string(&inteiro,caractere);
        while(((cont = fread(&caractere,sizeof(char),1,arq)) != 0) && caractere != '\0'){
            add_caractere_string(&inteiro,caractere);
        }

        while (((cont = fread(&caractere,sizeof(char),1,arq)) != 0) && caractere == '\0') {}
        fseek(arq,(-1)*sizeof(char),SEEK_CUR);

        *(registro->quantDias) = atoi(inteiro);
        limpar_string(&inteiro);

        if (cont==0) {
            fim_de_arquivo = true;
        }
        
        add_registro_pasta(&pasta,&registro);
    }
    fclose(arq);
    return pasta;
}
// Carrega as chaves e retorna um vetor de strings
PARAGRAFO carregar_chaves(STRING nomeArquivoChaves) {
    FILE* arq;
    arq = abrir_arquivo_binario(nomeArquivoChaves);
    if (arq == NULL) {
        // Tratamento de erro: não foi possível abrir o arquivo.
        exit(0);
    }
    
    int cont;
    char caractere;
    PARAGRAFO paragrafo = criar_paragrafo();
    bool fim_de_arquivo = false;

    STRING codCliente;
    STRING codVeiculo;
    // Faz a leitura das chaves e adiciona num vetor de strings
    while (!fim_de_arquivo) {
        
        codCliente = criar_string();
        codVeiculo = criar_string();

        while(((cont = fread(&caractere,sizeof(char),1,arq)) != 0) && caractere != '\0'){
            add_caractere_string(&codCliente,caractere);
        }

        while(((cont = fread(&caractere,sizeof(char),1,arq)) != 0) && caractere != '\0'){
            add_caractere_string(&codVeiculo,caractere);
        }

        if (cont==0) {
            fim_de_arquivo = true;
        }

        add_string_string(&codCliente, &codVeiculo);
        add_string_paragrafo(&paragrafo,&codCliente);
        limpar_string(&codCliente);
        limpar_string(&codVeiculo);
    }
    
    fclose(arq);
    return paragrafo;
}

#pragma region funcoes_inserir
// Função para calcular o tamanho de um arquivo
size_t calcularTamanhoArquivo(FILE* arquivo) {
    fseek(arquivo, 0, SEEK_END);
    size_t tamanho = ftell(arquivo);
    rewind(arquivo);
    return tamanho;
}

// Função para atualizar o arquivo de índices
void atualizarIndice(FILE* arquivoIndice, size_t byteOffset, STRING chavePrimaria) {
    // Mover ponteiro de para o final do arquivo de índices
    fseek(arquivoIndice, 0, SEEK_END);

    // Escrever o novo índice (byteOffset) no arquivo de índices
    fwrite(&byteOffset, sizeof(size_t), 1, arquivoIndice);
    fwrite(chavePrimaria, sizeof(char), strlen(chavePrimaria), arquivoIndice);
}
// Insere o registro desejado no arquivo de registros
void inserirRegistro(FILE* arquivoDados, FILE* arquivoIndice, PASTA pasta) {

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
    size_t byteOffset = calcularTamanhoArquivo(arquivoDados);
    
    STRING chavePrimaria = (STRING)malloc(strlen(pasta[posRegistro-1]->codCliente) + strlen(pasta[posRegistro-1]->codVeiculo) + 1);

    if (chavePrimaria != NULL) {
    strcpy(chavePrimaria, ""); // Inicialize a chavePrimaria com uma string vazia
    strcat(chavePrimaria, pasta[posRegistro - 1]->codCliente);
    strcat(chavePrimaria, pasta[posRegistro - 1]->codVeiculo);
    } else {
        printf("Falha na alocacao de memoria.\n");
    }

    //Faz a checagem se esse registro já foi inserido
    char chaveTeste[19];
    char bufferLixo[8]; 
    fseek(arquivoIndice,sizeof(char) * 8, SEEK_SET);
    while (fread(chaveTeste, sizeof(chaveTeste), 1 ,arquivoIndice) == 1)
    {
        fread(bufferLixo, sizeof(char), 8 ,arquivoIndice);
        if(strcmp(chavePrimaria, chaveTeste) == 0) // Já possui aquela chave
        {
            printf("\nRegistro ja existe!\n\n");
            free(chavePrimaria);
            return;
        }
    }

    // Atualizar o arquivo de índices com o novo byte offset
    atualizarIndice(arquivoIndice, byteOffset, chavePrimaria);

    // libereção de memória não mais necessária
    free(chavePrimaria);

    // Escrever o tamanho do registro no início do registro
    fwrite(&tamanhoRegistro, 1, 1, arquivoDados);

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
    fwrite(codClienteCopy, sizeof(char), strlen(codClienteCopy), arquivoDados);
    fwrite(codVeiculoCopy, sizeof(char), strlen(codVeiculoCopy), arquivoDados);
    fwrite(nomeClienteCopy, sizeof(char), strlen(nomeClienteCopy), arquivoDados);
    fwrite(nomeVeiculoCopy, sizeof(char), strlen(nomeVeiculoCopy), arquivoDados);
    fwrite((pasta[posRegistro - 1]->quantDias), sizeof(int), 1, arquivoDados);

    // Escrevendo o delimitador "|" no fim do registro
    char delimitador = '|';
    fwrite(&delimitador, sizeof(char), 1, arquivoDados);


    printf("Novo registro inserido no byte offset: %lu\n", byteOffset);
}
#pragma endregion funcoes_inserir

// Estrutura para armazenar informações do índice
struct Indice {
    size_t byteOffset;
    char chavePrimaria[18];
};

// Função para comparar duas chaves primárias
int compararChaves(const void* a, const void* b) {
    return strncmp(((struct Indice*)a)->chavePrimaria, ((struct Indice*)b)->chavePrimaria, 18);
}

void pesquisarPorChavePrimaria(FILE* arquivoIndice, FILE* arquivoDados, STRING chavePrimaria) {

    //determina se o registro foi localizado
    bool achouRegistro = false;
    // Lê todos os índices do arquivo de índices
    struct Indice* indices = NULL;
    size_t numIndices = 0;
    
    //o break determina a localização ou o fim do arquivo.
    while (1) {
        struct Indice indiceLido;
        size_t bytesLidos = fread(&(indiceLido.byteOffset), sizeof(size_t), 1, arquivoIndice);
        if (bytesLidos != 1) {
            break; // Fim do arquivo de índices
        }
        size_t bytesRead = fread(indiceLido.chavePrimaria, sizeof(char), 18, arquivoIndice);
        if (bytesRead != 18) {
            break; // Erro na leitura da chave primária o tamanho sempre eh 18
        }
        numIndices++;
        indices = (struct Indice*)realloc(indices, numIndices * sizeof(struct Indice));
        indices[numIndices - 1] = indiceLido;
    }

    // Ordena os índices por chave primária
    qsort(indices, numIndices, sizeof(struct Indice), compararChaves);

    // Realiza uma pesquisa binária para encontrar a chavePrimaria desejada
    struct Indice chaveBuscada;
    strncpy(chaveBuscada.chavePrimaria, chavePrimaria, 18); // Copia os primeiros 18 caracteres
    struct Indice* resultado = (struct Indice*)bsearch(&chaveBuscada, indices, numIndices, sizeof(struct Indice), compararChaves);

    //seguimos em diante somente se encontrarmos o dado desejado
    if (resultado != NULL) {
        achouRegistro = true;
        // A chavePrimaria foi encontrada, use o byteOffset para ler os dados do registro no arquivo dados.bin
        fseek(arquivoDados, resultado->byteOffset+sizeof(char), SEEK_SET);

        //cria um registro temporario para mostrarmos os dados
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

        //mostra os dados...
        printf("Registro encontrado:\n");
        printf("Codigo Cliente:%s\n", registro->codCliente);
        printf("Codigo Veiculo:%s\n", registro->codVeiculo);
        printf("Nome Cliente:%s\n", registro->nomeCliente);
        printf("Nome Veiculo:%s\n", registro->nomeVeiculo);
        printf("Quantidade de Dias: %d\n\n", *(registro->quantDias));
        free(registro);
        
    }

    //informa se nao achou o registro
    if(achouRegistro == false){
        printf("O registro procurado nao existe!\n");
    }
    // Libere a memória dos índices
    free(indices);
}


typedef struct indice {
    char chave_primaria[19];
    size_t rrn;
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

short inserir(FILE *arvore_b, short rrn, INDICE novo_indice, short *filho_promovido, INDICE *chave_promovida)
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
    short promovido = inserir(arvore_b, pagina_atual.ponteiro[pos], novo_indice, &rrr_promovido_recursivamente, &chave_promovida_recursivamente);

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
        *raizRRN = criar_arvore(*arvore_b, novo_indice);
        printf("\nChave %s inserida com sucesso", novo_indice.chave_primaria);
        return SIM;
    }

    short rrn_promovido = -1;
    INDICE chave_promovida;

    int promovido = inserir(*arvore_b, *raizRRN, novo_indice, &rrn_promovido, &chave_promovida);
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


    printf("\nNovo registro inserido no byte offset: %lu", byteOffset);
}

short obter_raiz(FILE *arvore_b)
{
    short root = -1;
    fseek(arvore_b, 0, 0);
    if (fread(&root, sizeof(short), 1, arvore_b) == 0)
    {
        printf("\nErro ao ler o arquivo!\n");
        return -1;
    }
    return root;
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
        printf("\nMenu:");
        printf("\n1. Insercao");
        printf("\n3. Carregar Arquivo");
        printf("\n0. Sair");
        printf("\nEscolha uma opcao (0-4): ");
        scanf(" %d", &opcao);
        
        //criacao do ponteiro para os arquivos que serao criados 
        FILE* arq_data;
        FILE* arq_index;
        ARV_B arvore_b = {.arq = NULL, .raizRRN = -1};
        
        switch (opcao) {
            //executa a insercao
            case 1:
                if (load_de_arquivos) {

                    arq_data = fopen("dados.bin", "a+b");

                    arq_index = fopen("indices.bin", "a+b");

                    arvore_b.arq = arq_index;

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
                chaves = carregar_chaves("busca.bin");
                load_de_arquivos = true;
                break;
            case 0:
                exit(0);
        } 
    } while (opcao != 0);   
   
    return 0;
}