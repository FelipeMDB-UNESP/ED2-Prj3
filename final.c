//Andre Luis Dias Nogueira
//Felipe Melchior de Britto
//Rafael Daiki Kaneko
//Ryan Hideki Tadeo Guimaraes

#include "header.h"
#define MAXCHAVES 3
#define CHAVE_NULA "##################"

//arquivos
#define arq_arvore_b "indices_arvore_b.bin"
#define arq_principal "dados.bin"
#define dados_para_inserir "insere.bin"
#define dados_para_buscar "busca.bin"

typedef struct
{
    char codCliente[12];
    char codVeiculo[8];
    char nomeCliente[50];
    char nomeVeiculo[50];
    int quantDias;
} DADO;

typedef struct
{
    char chave_primaria[18]; //chave primaria = string concatenada de CodCli + CodF
    int rrn;        //rrn do item
} CHAVE_PRIMARIA;

typedef struct
{
    short qtd_indices;
    CHAVE_PRIMARIA chave[MAXCHAVES];
    short filho[MAXCHAVES + 1]; //rrns para paginas
} PAGINA_ARVORE;

enum PROMOCAO
{
    NAO,
    SIM,
    DUPLICADO
};

//Protótipos
int Menu();

void inserir_registro(FILE *dataFile, FILE **arvore_b, DADO reg, short *raizRRN); //Adicona um DADO no arquivo principal e arvore b
void printar_registro(DADO reg);                                             //Mostra as informações de um DADO
void percurso_in_ordem(FILE *arq, FILE *arvore_b, int rrn);                           //percurso inOrdem
void printar_registro_e_rrn(FILE *arq, int rrn);                                    //imprime DADO dado o RRN
void buscar(FILE *arq, FILE *arvore_b, int rrn, char *chave);                 //pesquisa chave na arvore

//Arvore-B
int adicionar_index(FILE **arvore_b, CHAVE_PRIMARIA chave, short *raizRRN); //Adiciona DADO na arvore, cria arvore se não existe
void escrever_pagina(FILE *arvore_b, short rrn, PAGINA_ARVORE *pag);      //escreve pag no arquivo
void inicializar_pagina(PAGINA_ARVORE *pag);                               //inicializa uma pagina
void posicionar_raiz(FILE *arvore_b, short raiz);                  //atualiza header com raiz da arvore
void ler_pagina(FILE *arvore_b, short rrn, PAGINA_ARVORE *pag);
void inserir_na_pagina(FILE *arvore_b, CHAVE_PRIMARIA chave, short rrnChild, PAGINA_ARVORE *pag);
void separar(FILE *arvore_b, CHAVE_PRIMARIA chave, short filho_direito, PAGINA_ARVORE *pagina_antiga, CHAVE_PRIMARIA *chave_promovida, short *filho_promovido_direito, PAGINA_ARVORE *nova_pagina);

short inserir(FILE *arvore_b, short rrn, CHAVE_PRIMARIA chave, short *filho_promovido, CHAVE_PRIMARIA *chave_promovida); //insere Chave na arvore
short obter_raiz(FILE *arvore_b);                                                                  //retorna raiz da arvore
short obter_pagina(FILE *arvore_b);                                                                  //Retorna RRN da proxima pagina
short criar_arvore(FILE **arvore_b, CHAVE_PRIMARIA chave);                                                  //cria a arvore
short criar_raiz(FILE *arvore_b, CHAVE_PRIMARIA chave, short esquerda, short direita);                          //Cria raiz
short procurar_noh(CHAVE_PRIMARIA chave, PAGINA_ARVORE *pag, short *pos);


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

//Aloca espaço para um DADO de dados
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

//Limpar a alocação de um DADO de dados
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

int tam_chaves(PARAGRAFO* parag) {
    int i;
    for (i=0; (*parag)[i] != NULL; i++) {
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
// Adiciona um DADO no vetor de registros
void add_registro_pasta(PASTA* pst, REGISTRO* reg) {
    int i;
    int tam = tam_pasta(pst);
    PASTA novaPasta;
    novaPasta = (PASTA) malloc(sizeof(REGISTRO) * (tam + 2));
    // Copia os elementos da pasta pst para a novaPasta
    for (int i = 0; i < tam; i++) {
        novaPasta[i] = (*pst)[i];
    }

    novaPasta[tam] = *reg; // Adiciona o DADO no fim da pasta
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

// Carrega os dados e retorna um vetor de DADO
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
    REGISTRO DADO;
    bool fim_de_arquivo = false;

    STRING inteiro;
    // Faz a leitura dos registros do arquivo e adiciona num vetor de Registros
    while (!fim_de_arquivo) {
        
        DADO = criar_registro();
        inteiro = criar_string();

        while(((cont = fread(&caractere,sizeof(char),1,arq)) != 0) && caractere != '\0'){
            add_caractere_string(&(DADO->codCliente),caractere);
        }

        while(((cont = fread(&caractere,sizeof(char),1,arq)) != 0) && caractere != '\0'){
            add_caractere_string(&(DADO->codVeiculo),caractere);
        }

        while(((cont = fread(&caractere,sizeof(char),1,arq)) != 0) && caractere != '\0'){
            add_caractere_string(&(DADO->nomeCliente),caractere);
        }

        while (((cont = fread(&caractere,sizeof(char),1,arq)) != 0) && caractere == '\0') {}

        add_caractere_string(&DADO->nomeVeiculo,caractere);
        while(((cont = fread(&caractere,sizeof(char),1,arq)) != 0) && caractere != '\0'){
            add_caractere_string(&DADO->nomeVeiculo,caractere);
        }

        while (((cont = fread(&caractere,sizeof(char),1,arq)) != 0) && caractere == '\0') {}

        add_caractere_string(&inteiro,caractere);
        while(((cont = fread(&caractere,sizeof(char),1,arq)) != 0) && caractere != '\0'){
            add_caractere_string(&inteiro,caractere);
        }

        while (((cont = fread(&caractere,sizeof(char),1,arq)) != 0) && caractere == '\0') {}
        fseek(arq,(-1)*sizeof(char),SEEK_CUR);

        *(DADO->quantDias) = atoi(inteiro);
        limpar_string(&inteiro);

        if (cont==0) {
            fim_de_arquivo = true;
        }
        
        add_registro_pasta(&pasta,&DADO);
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

int main(int argc, char **argv)
{
    short raizRRN = -1;
    FILE *arq = NULL;
    FILE *arvore_b = NULL;

    //quantidade de dados no arquivo insere.bin
    int tam;
    int tamChaves;
    //pasta de registros, ponteiro para registros
    PASTA pasta;
    //basicamente um ponteiro para chaves
    PARAGRAFO chaves;

    //booleano para verificar se os dados foram carregados na memoria
    bool load_de_arquivos = false;

    //MENU
    int option = -1;
   
    do{
        option = Menu();
        switch (option)
        {
        case 1:
            //Inserir
            if (load_de_arquivos)
            {
                if ((arq = fopen(arq_principal, "r+b")) == NULL)
                {
                    printf("\nCriando arquivo de dados...\n");
                    if ((arq = fopen(arq_principal, "a+b")) == NULL)
                    {
                        printf("\nErro ao criar arquivo!");
                        return 0;
                    }
                }
                if ((arvore_b = fopen(arq_arvore_b, "r+b")) != NULL)
                {
                    raizRRN = obter_raiz(arvore_b);
                    if (raizRRN == -1)
                    {
                        printf("\nErro ao ler o arquivo!\n");
                        return 0;
                    }
                }
                printf("\nDigite um numero entre 1 e %d\n", tam);
                int posRegistro;
                printf("\nQual o indice do DADO que deseja inserir?\n");
                scanf("%d", &posRegistro);

                DADO reg;
                strcpy(reg.codCliente, pasta[posRegistro-1]->codCliente);
                strcpy(reg.codVeiculo, pasta[posRegistro-1]->codVeiculo);
                strcpy(reg.nomeCliente, pasta[posRegistro-1]->nomeCliente);
                strcpy(reg.nomeVeiculo, pasta[posRegistro-1]->nomeVeiculo);
                reg.quantDias = *(pasta[posRegistro-1]->quantDias);

                inserir_registro(arq, &arvore_b, reg, &raizRRN);
                fclose(arq);
                fclose(arvore_b);
            }
            else {
                    printf("\nArquivos ainda nao foram carregados!\n");
                } 
            break;
        case 2:
            //Listar dados - percurso in ordem
            if ((arq = fopen(arq_principal, "r+b")) == NULL)
            {
                printf("\nCriando arquivo de dados...\n");
                if ((arq = fopen(arq_principal, "a+b")) == NULL)
                {
                    printf("\nErro ao criar arquivo!");
                    return 0;
                }
            }
            if ((arvore_b = fopen(arq_arvore_b, "r+b")) != NULL)
            {
                raizRRN = obter_raiz(arvore_b);
                if (raizRRN == -1)
                {
                    printf("\nErro ao ler o arquivo!\n");
                    return 0;
                }
            }
            printf("\nLista dos dados dos clientes:\n");
            percurso_in_ordem(arq, arvore_b, raizRRN);
            fclose(arq);
            fclose(arvore_b);
            break;
        case 3:
            //Pesquisar cliente - busca na árvore
            if ((arq = fopen(arq_principal, "r+b")) == NULL)
            {
                printf("\nCriando arquivo de dados...\n");
                if ((arq = fopen(arq_principal, "a+b")) == NULL)
                {
                    printf("\nErro ao criar arquivo!");
                    return 0;
                }
            }
            if ((arvore_b = fopen(arq_arvore_b, "r+b")) != NULL)
            {
                raizRRN = obter_raiz(arvore_b);
                if (raizRRN == -1)
                {
                    printf("\nErro ao ler o arquivo!\n");
                    return 0;
                }
            }
            if (load_de_arquivos) {
                int posChave;
                printf("\nQual a posicao da chave que voce deseja pesquisar? 1-%d\n",tamChaves-1);
                scanf(" %d", &posChave);
                printf("\nPesquisando a chave %s.......\n", chaves[posChave-1]);

                buscar(arq, arvore_b, raizRRN, chaves[posChave-1]);
                fclose(arq);
                fclose(arvore_b);
            }
            else {
                    printf("\nArquivos ainda nao foram carregados!\n");
            } 
            break;
        case 4:
            pasta = carregar_dados("insere.bin");
            tam = tam_pasta(&pasta);
            chaves = carregar_chaves("busca.bin");
            tamChaves = tam_chaves(&chaves);
            load_de_arquivos = true;
            break;
        case 0:
            exit(0);
        }
    } while (option != 5);

    return 0;
}

int Menu()
{
    int opt = -1;
    do
    {
        printf("\nSelecione uma opcao:\n");
        printf("1- Inserir Registro\n");
        printf("2- Listar dados\n");
        printf("3- Pesquisar por chave primaria\n");
        printf("4- Carregar dados\n");
        printf("0- Sair\n");
        scanf("%d", &opt);
    } while (opt < 0 || opt > 4);

    return opt;
}

void printar_registro(DADO reg)
{
    printf("\n===============RESULTADO===============\n");
    printf("    Codigo cliente: %s\n", reg.codCliente);
    printf("    Codigo Veiculo: %s\n", reg.codVeiculo);
    printf("      Nome Cliente: %s\n", reg.nomeCliente);
    printf("      Nome Veiculo: %s\n", reg.nomeVeiculo);
    printf("Quantidade de dias: %d\n", reg.quantDias);
    printf("=======================================\n");
}

//mostra o Registro dado um RRN do DADO
void printar_registro_e_rrn(FILE *arq, int rrn)
{
    long bof = (long)rrn * (long)sizeof(DADO);
    fseek(arq, bof, 0);
    DADO temp;
    fread(&temp, sizeof(DADO), 1, arq);
    printar_registro(temp);
}

//Busca uma chave
void buscar(FILE *arq, FILE *arvore_b, int rrn, char *chave)
{
    if (rrn < 0)
    {
        printf("\nA chave %s nao foi localizada\n", chave);
        return;
    }

    PAGINA_ARVORE pagina_atual;
    ler_pagina(arvore_b, rrn, &pagina_atual);
    int i;
    for (i = 0; i < MAXCHAVES; i++)
    {
        int comp = strcmp(chave, pagina_atual.chave[i].chave_primaria);

        if (comp < 0)
            return buscar(arq, arvore_b, pagina_atual.filho[i], chave); //se for menor, pesquisa a esquerda
        else if (comp == 0)
        {
            printf("\nA chave %s foi encontrada em pagina %d | posicao %d\n", chave, rrn, i);
            printar_registro_e_rrn(arq, pagina_atual.chave[i].rrn); //se for igual, encontrou
            return;
        }
    }
    return buscar(arq, arvore_b, pagina_atual.filho[i], chave); //pesquisa a direita da ultima chave
}

//Percorre a árvo in-ordem e mostra os regsitros ordenados
void percurso_in_ordem(FILE *arq, FILE *arvore_b, int rrn)
{
    //percorre esquerda; raiz; direita

    if (rrn < 0)
    {
        printf("\nArvore vazia!");
        return;
    }

    PAGINA_ARVORE pagina_atual;
    ler_pagina(arvore_b, rrn, &pagina_atual); //le pagina

    int i;
    for (i = 0; i < MAXCHAVES; i++)
    {
        if (pagina_atual.filho[i] != -1) //percorre esquerda da chave
            percurso_in_ordem(arq, arvore_b, pagina_atual.filho[i]);

        if (pagina_atual.chave[i].rrn == -1) //se posição vazia
            return;

        printar_registro_e_rrn(arq, pagina_atual.chave[i].rrn); //mostra a chave
        //percorre direita que será esquerda na próxima iteração
    }
    if (pagina_atual.filho[i] != -1)
        percurso_in_ordem(arq, arvore_b, pagina_atual.filho[i]); //percorre ultimo filho a direita
}

//Adiciona DADO ao indice e arquivo principal
void inserir_registro(FILE *dataFile, FILE **arvore_b, DADO reg, short *raizRRN)
{
    fseek(dataFile, 0, 2);                                 //vai para fim do arquivo
    int regRRN = (int)(ftell(dataFile) / sizeof(DADO)); //rrn para DADO no arquivo principal

    CHAVE_PRIMARIA chave;
    chave.rrn = regRRN;
    strcpy(chave.chave_primaria, reg.codCliente);
    strcat(chave.chave_primaria, reg.codVeiculo);

    if (adicionar_index(arvore_b, chave, raizRRN) != DUPLICADO){        //adiciona chave do DADO na árvore B
        fwrite(reg.codCliente, sizeof(char), sizeof(reg.codCliente), dataFile);
        fwrite(reg.codVeiculo, sizeof(char), sizeof(reg.codVeiculo), dataFile);
        fwrite(reg.nomeCliente, sizeof(char), sizeof(reg.nomeCliente), dataFile);
        fwrite(reg.nomeVeiculo, sizeof(char), sizeof(reg.nomeVeiculo), dataFile);
        fwrite(&(reg.quantDias), sizeof(int), 1, dataFile);
    }
}

//Arvore-B
//retorna SIM caso ocorra inserção, retorna DUPLICADO caso duplicado;
int adicionar_index(FILE **arvore_b, CHAVE_PRIMARIA chave, short *raizRRN)
{
    if (*arvore_b == NULL)
    {
        *raizRRN = criar_arvore(arvore_b, chave);
        printf("\nA chave %s foi inserida com sucesso\n", chave.chave_primaria);
        return SIM;
    }

    short promotedRRN = -1;
    CHAVE_PRIMARIA chave_promovida;

    int promovido = inserir(*arvore_b, *raizRRN, chave, &promotedRRN, &chave_promovida);
    if (promovido == SIM)
        *raizRRN = criar_raiz(*arvore_b, chave_promovida, *raizRRN, promotedRRN);

    if (promovido == DUPLICADO)
        return DUPLICADO;
    else
    {
        printf("\nA chave %s foi inserida com sucesso\n", chave.chave_primaria);
        return SIM;
    }
}

short obter_raiz(FILE *arvore_b)
{
    short raiz = -1;
    fseek(arvore_b, 0, 0);
    if (fread(&raiz, sizeof(short), 1, arvore_b) == 0)
    {
        printf("\nErro ao ler o arquivo!\n");
        return -1;
    }
    return raiz;
}

short criar_arvore(FILE **arvore_b, CHAVE_PRIMARIA chave)
{
    short temp = -1;
    if ((*arvore_b = fopen(arq_arvore_b, "w+b")) == NULL)
    {
        printf("\nErro tentando abrir o arquivo!\n");
        return temp;
    }
    fwrite(&temp, sizeof(short), 1, *arvore_b);
    return criar_raiz(*arvore_b, chave, -1, -1);
}

short criar_raiz(FILE *arvore_b, CHAVE_PRIMARIA chave, short esquerda, short direita)
{
    PAGINA_ARVORE pag;
    short rrn = obter_pagina(arvore_b);
    inicializar_pagina(&pag);
    pag.chave[0] = chave;
    pag.filho[0] = esquerda;
    pag.filho[1] = direita;
    pag.qtd_indices = (short)1;
    escrever_pagina(arvore_b, rrn, &pag);
    posicionar_raiz(arvore_b, rrn);
    return rrn;
}

short obter_pagina(FILE *arvore_b)
{
    fseek(arvore_b, 0, 2);
    long addr = ftell(arvore_b);
    addr -= (long)2;
    return (addr > 0) ? (short)addr / sizeof(PAGINA_ARVORE) : 0;
}

short inserir(FILE *arvore_b, short rrn, CHAVE_PRIMARIA chave, short *filho_promovido, CHAVE_PRIMARIA *chave_promovida)
{
    if (rrn == -1)
    {
        *chave_promovida = chave;
        *filho_promovido = -1;
        return SIM;
    }
    PAGINA_ARVORE pagina_atual, nova_pagina;
    ler_pagina(arvore_b, rrn, &pagina_atual);

    short pos;
    int encontrou = procurar_noh(chave, &pagina_atual, &pos);
    if (encontrou)
    {
        printf("\nERRO: Chave %s duplicada\n", chave.chave_primaria);
        return DUPLICADO;
    }
    short rrn_promovido_rec;
    CHAVE_PRIMARIA chave_promovida_rec;
    short promovido = inserir(arvore_b, pagina_atual.filho[pos], chave, &rrn_promovido_rec, &chave_promovida_rec);

    if (promovido == DUPLICADO)
        return DUPLICADO;

    if (!promovido)
        return NAO;

    if (pagina_atual.qtd_indices < MAXCHAVES)
    {
        inserir_na_pagina(arvore_b, chave_promovida_rec, rrn_promovido_rec, &pagina_atual);
        escrever_pagina(arvore_b, rrn, &pagina_atual);
        return NAO;
    }
    else
    {
        separar(arvore_b, chave_promovida_rec, rrn_promovido_rec, &pagina_atual, chave_promovida, filho_promovido, &nova_pagina);
        escrever_pagina(arvore_b, rrn, &pagina_atual); //Atualiza pagina atual
        escrever_pagina(arvore_b, *filho_promovido, &nova_pagina);
        return SIM;
    }
}

short procurar_noh(CHAVE_PRIMARIA chave, PAGINA_ARVORE *pag, short *pos)
{
    int i = 0;
    for (i = 0; i < pag->qtd_indices && strcmp(chave.chave_primaria, pag->chave[i].chave_primaria) > 0; i++)
    {
        //nada;
    }
    *pos = i;
    return (*pos < pag->qtd_indices && strcmp(chave.chave_primaria, pag->chave[*pos].chave_primaria) == 0) ? SIM : NAO;
}

void separar(FILE *arvore_b, CHAVE_PRIMARIA chave, short filho_direito, PAGINA_ARVORE *pagina_antiga, CHAVE_PRIMARIA *chave_promovida, short *filho_promovido_direito, PAGINA_ARVORE *nova_pagina)
{
    printf("\nDivisao de no!\n");

    
    CHAVE_PRIMARIA chave_atual[MAXCHAVES + 1];
    short filho_atual[MAXCHAVES + 2];
    int j;

    CHAVE_PRIMARIA chave_nula = {.chave_primaria="##################",.rrn=-1};

    for (j = 0; j < MAXCHAVES; j++)
    {
        chave_atual[j] = pagina_antiga->chave[j]; //copia chaves e filhos
        filho_atual[j] = pagina_antiga->filho[j];

        pagina_antiga->chave[j] = chave_nula; //apaga pagina antiga
        pagina_antiga->filho[j] = -1;
    }
    filho_atual[j] = pagina_antiga->filho[j];
    pagina_antiga->filho[j] = -1;


    for (j = MAXCHAVES; strcmp(chave.chave_primaria, chave_atual[j - 1].chave_primaria) < 0 && j > 0; j--)
    {
        chave_atual[j] = chave_atual[j - 1]; //insere nova pagina
        filho_atual[j + 1] = filho_atual[j];
    }
    chave_atual[j] = chave;
    filho_atual[j + 1] = filho_direito;

    *filho_promovido_direito = obter_pagina(arvore_b);
    inicializar_pagina(nova_pagina);

    const int MINIMO_CHAVES = (MAXCHAVES + 1) / 2; //=2

    //Separa 2 chaves para cada pagina
    for (j = 0; j < MINIMO_CHAVES; j++)
    {
        pagina_antiga->chave[j] = chave_atual[j]; //copia metade inferior do vetor para pagina_antiga
        pagina_antiga->filho[j] = filho_atual[j];

        nova_pagina->chave[j] = chave_atual[j + MINIMO_CHAVES]; //copia metade superior vetor para nova_pagina
        nova_pagina->filho[j] = filho_atual[j + MINIMO_CHAVES];
    }
    //ultimos ponteiros de filhos
    nova_pagina->filho[MINIMO_CHAVES] = filho_atual[j + MINIMO_CHAVES];

    //remove chave promovida da pagina antiga
    pagina_antiga->chave[MINIMO_CHAVES - 1] = chave_nula;
    pagina_antiga->filho[MINIMO_CHAVES] = -1;

    nova_pagina->qtd_indices = MAXCHAVES - 1;
    pagina_antiga->qtd_indices = MINIMO_CHAVES - 1;

    *chave_promovida = chave_atual[MINIMO_CHAVES - 1];

    printf("\nA chave %s foi promovida\n", (*chave_promovida).chave_primaria);
}

void inserir_na_pagina(FILE *arvore_b, CHAVE_PRIMARIA chave, short filho_direito, PAGINA_ARVORE *pag)
{
    int j;
    for (j = pag->qtd_indices; strcmp(chave.chave_primaria, pag->chave[j - 1].chave_primaria) < 0 && j > 0; j--)
    {
        pag->chave[j] = pag->chave[j - 1];
        pag->filho[j + 1] = pag->filho[j];
    }
    pag->qtd_indices += 1;
    pag->chave[j] = chave;
    pag->filho[j + 1] = filho_direito;
}

void ler_pagina(FILE *arvore_b, short rrn, PAGINA_ARVORE *pag)
{
    long addr = (long)rrn * (long)sizeof(PAGINA_ARVORE) + (long)2;
    fseek(arvore_b, addr, 0);
    fread(pag, sizeof(PAGINA_ARVORE), 1, arvore_b);
}

void escrever_pagina(FILE *arvore_b, short rrn, PAGINA_ARVORE *pag)
{
    long addr;
    addr = (long)rrn * (long)sizeof(PAGINA_ARVORE) + (long)2;
    fseek(arvore_b, addr, 0);
    fwrite(pag, sizeof(PAGINA_ARVORE), 1, arvore_b);
}

void inicializar_pagina(PAGINA_ARVORE *pag)
{
    for (int i = 0; i < MAXCHAVES; i++)
    {
        strcpy(pag->chave[i].chave_primaria, CHAVE_NULA);
        pag->chave[i].rrn = -1;
        pag->filho[i] = -1;
    }
    pag->filho[MAXCHAVES] = -1;
}

void posicionar_raiz(FILE *arvore_b, short raiz)
{
    fseek(arvore_b, 0, 0);
    fwrite(&raiz, sizeof(short), 1, arvore_b);
}
