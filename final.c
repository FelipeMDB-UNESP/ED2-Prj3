//André Nogueira Dias
//Felipe de Britto
//Rafael Daiki
//Mestre Big Ryan

//Libs
#include "header.h"

//Constantes
#define MAXKEYS 3 //Arvore-b ordem 4
#define NULL_KEY "##################"

//nome dos arquivos
#define fileIn "insere.bin"
#define keyIn "busca.bin"
#define countFile "indicesAux.bin"
#define treeFile "indicesFinal.bin"
#define mainFile "dados.bin"

//String de mensagens
#define errStr "Erro ao fechar o arquivo!"
#define errOpenFile "\nErro ao abrir o arquivo!\n"
#define errReadF "\nErro ao ler o arquivo!\n"
#define makingFileMsg "\nCriando arquivo de dados...\n"
#define errMakeFile "\nErro ao criar arquivo!"
#define duplicateMsg "\nERRO: Chave %s duplicada"
#define splitMsg "\nDivisao de no!"
#define promotedKeyMsg "\nChave %s promovida"
#define sucessMsg "\nChave %s inserida com sucesso"
#define emptyData "\nSem registro para inserir"
#define treeEmptyMsg "\nSem dados para percorrer!"
#define searchingMSG "\nListando todos os clientes...\n"
#define notFound "\nChave %s nao encontrada\n"
#define keyFound "\nChave %s encontrada, pagina %d, posicao %d\n"


typedef struct
{
    char codCliente[12];
    char codVeiculo[8];
    char nomeCliente[50];
    char nomeVeiculo[50];
    int quantDias;
} registro;

typedef struct
{
    char keyStr[18]; //chave = string concatenada de CodCli + CodF
    int rrn;        //rrn para registro no principal
} prmKey;

typedef struct
{
    short keycount;
    prmKey key[MAXKEYS];
    short child[MAXKEYS + 1]; //rrns para paginas
} page;

enum isPromoted
{
    NO,
    YES,
    DUP
};

//Protótipos
int Menu();

registro *GetReg(int *num, int prox); //retorna vetor de registro a ser inserido.
char **GetSearchKeys(int *num, int prox);

void GetCounters(int *nexData, int *nextSearch); //Atualiza contadores de registro e busca utilizadas.
void SetCounters(int nextData, int nextSearch);
void AddReg(FILE *dataFile, FILE **bTree, registro reg, short *rootRRN); //Adicona um registro no arquivo principal e arvore b
void printReg(registro reg);                                             //Mostra as informações de um registro
void InOrdem(FILE *arq, FILE *bTree, int rrn);                           //percurso inOrdem
void printRegRRN(FILE *arq, int rrn);                                    //imprime registro dado o RRN
void Search(FILE *arq, FILE *bTree, int rrn, char *key);                 //pesquisa chave na arvore

//Arvore-B
int AddIndex(FILE **bTree, prmKey key, short *rootRRN); //Adiciona registro na arvore, cria arvore se não existe
void WritePage(FILE *bTree, short rrn, page *pag);      //escreve pag no arquivo
void PageInit(page *pag);                               //inicializa uma pagina
void PutRoot(FILE *bTree, short root);                  //atualiza header com raiz da arvore
void ReadPage(FILE *bTree, short rrn, page *pag);
void InsertInPage(FILE *bTree, prmKey key, short rrnChild, page *pag);
void Split(FILE *bTree, prmKey key, short rChild, page *oldPag, prmKey *promotedKey, short *promotedRChild, page *newPage);

short Insert(FILE *bTree, short rrn, prmKey key, short *promotedChild, prmKey *promotedKey); //insere Chave na arvore
short GetRoot(FILE *bTree);                                                                  //retorna raiz da arvore
short GetPage(FILE *bTree);                                                                  //Retorna RRN da proxima pagina
short CreateTree(FILE **bTree, prmKey Key);                                                  //cria a arvore
short CreateRoot(FILE *bTree, prmKey key, short left, short right);                          //Cria raiz
short SearchNode(prmKey key, page *pag, short *pos);


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













int main(int argc, char **argv)
{
    int nexData = 0, nextSearch = 0;
    GetCounters(&nexData, &nextSearch);

    int totalData = -1, currentData = 0;
    registro *dados = GetReg(&totalData, nexData);
    if (dados == NULL) //Erro ao abrir arquivo de dados
        return 0;

    char **searchKey = NULL;
    int totalSearch = -1, currentSearch = 0;
    searchKey = GetSearchKeys(&totalSearch, nextSearch);
    if (searchKey == NULL) //Erro ao abrir arquivo com chaves
        return 0;

    short rootRRN = -1;
    FILE *arq = NULL;
    FILE *bTree = NULL;
    

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
                if ((arq = fopen(mainFile, "r+b")) == NULL)
                {
                    printf(makingFileMsg);
                    if ((arq = fopen(mainFile, "a+b")) == NULL)
                    {
                        printf(errMakeFile);
                        return 0;
                    }
                }
                if ((bTree = fopen(treeFile, "r+b")) != NULL)
                {
                    rootRRN = GetRoot(bTree);
                    if (rootRRN == -1)
                    {
                        printf(errReadF);
                        return 0;
                    }
                }
                printf("\nDigite um numero entre 1 e %d\n", tam);
                int posRegistro;
                printf("Qual o indice do registro que deseja inserir?");
                scanf("%d", &posRegistro);

                registro reg;
                strcpy(reg.codCliente, pasta[posRegistro-1]->codCliente);
                strcpy(reg.codVeiculo, pasta[posRegistro-1]->codVeiculo);
                strcpy(reg.nomeCliente, pasta[posRegistro-1]->nomeCliente);
                strcpy(reg.nomeVeiculo, pasta[posRegistro-1]->nomeVeiculo);
                reg.quantDias = *(pasta[posRegistro-1]->quantDias);

                AddReg(arq, &bTree, reg, &rootRRN);
                fclose(arq);
                fclose(bTree);
            }
            else {
                    printf("Arquivos ainda nao foram carregados!\n");
                } 
            break;
        case 2:
            //Listar dados - percurso in ordem
            if ((arq = fopen(mainFile, "r+b")) == NULL)
            {
                printf(makingFileMsg);
                if ((arq = fopen(mainFile, "a+b")) == NULL)
                {
                    printf(errMakeFile);
                    return 0;
                }
            }
            if ((bTree = fopen(treeFile, "r+b")) != NULL)
            {
                rootRRN = GetRoot(bTree);
                if (rootRRN == -1)
                {
                    printf(errReadF);
                    return 0;
                }
            }
            printf(searchingMSG);
            InOrdem(arq, bTree, rootRRN);
            fclose(arq);
            fclose(bTree);
            break;
        case 3:
            //Pesquisar cliente - busca na árvore
            if ((arq = fopen(mainFile, "r+b")) == NULL)
            {
                printf(makingFileMsg);
                if ((arq = fopen(mainFile, "a+b")) == NULL)
                {
                    printf(errMakeFile);
                    return 0;
                }
            }
            if ((bTree = fopen(treeFile, "r+b")) != NULL)
            {
                rootRRN = GetRoot(bTree);
                if (rootRRN == -1)
                {
                    printf(errReadF);
                    return 0;
                }
            }
            if (load_de_arquivos) {
                int posChave;
                printf("Qual a posicao da chave que voce deseja pesquisar? 1-%d\n",tamChaves-1);
                scanf(" %d", &posChave);
                printf("Pesquisando a chave %s.......\n", chaves[posChave-1]);

                Search(arq, bTree, rootRRN, chaves[posChave-1]);
                currentSearch++;
                fclose(arq);
                fclose(bTree);
            }
            else {
                    printf("Arquivos ainda nao foram carregados!\n");
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
    //Salva os contadores
    SetCounters(nexData + currentData, nextSearch + currentSearch);

    //Liberar memória
    free(dados);

    for (int i = 0; i < totalSearch; i++)
        free(searchKey[i]);

    free(searchKey);

    return 0;
}

int Menu()
{
    int opt = -1;
    do
    {
        printf("\n============MENU============\n");
        printf("Selecione uma opcao:\n");
        printf("1- Inserir Registro\n");
        printf("2- Listar dados\n");
        printf("3- Pesquisar por chave primaria\n");
        printf("4- Carregar dados\n");
        printf("0- Sair\n");
        printf("============================\n");
        scanf("%d", &opt);
    } while (opt < 0 || opt > 4);

    return opt;
}

void GetCounters(int *nexData, int *nextSearch)
{
    FILE *in;
    if ((in = fopen(countFile, "rb")) == NULL)
    {
        //se não existe, indices zerados.
        *nexData = 0;
        *nextSearch = 0;
        return;
    }
    if (fread(nexData, sizeof(int), 1, in) == 0)
        *nexData = 0;
    if (fread(nextSearch, sizeof(int), 1, in) == 0)
        *nextSearch = 0;

    if (fclose(in) == EOF)
        printf(errStr);
}

void SetCounters(int nextData, int nextSearch)
{
    FILE *in;
    if ((in = fopen(countFile, "w+b")) == NULL)
    {
        printf(errOpenFile);
        return;
    }
    fwrite(&nextData, sizeof(int), 1, in);
    fwrite(&nextSearch, sizeof(int), 1, in);

    if (fclose(in) == EOF)
        printf(errStr);
}

registro *GetReg(int *num, int prox)
{
    FILE *in;
    if ((in = fopen(fileIn, "r+b")) == NULL)
    {
        printf(errOpenFile);
        return NULL;
    }

    //pula registros ja utilizados
    fseek(in, prox * sizeof(registro), 0);

    registro *dados = (registro *)malloc(sizeof(registro));

    *num = 0; //quantidade de elementos no vetor
    while (fread(&(dados[*num]), sizeof(registro), 1, in))
    {
        (*num)++;
        dados = realloc(dados, ((*num) + 1) * sizeof(registro));
    }

    if (fclose(in) == EOF)
        printf(errStr);

    return dados;
}

char **GetSearchKeys(int *num, int prox)
{
    FILE *in;
    if ((in = fopen(keyIn, "r+b")) == NULL)
    {
        printf(errOpenFile);
        return NULL;
    }

    fseek(in, prox * sizeof(char) * 3 * 2, 0); //3 char para cada código = 6 char para chave primaria

    char **dados = (char **)malloc(sizeof(char *));
    *dados = (char *)malloc(sizeof(char) * 5);

    *num = 0; //quantidade de elementos no vetor
    char temp[3];
    while (fread(temp, sizeof(char), 3, in)) //le primeiro codigo
    {
        strcpy(dados[*num], temp);
        fread(temp, sizeof(char), 3, in); //le segundo codigo
        strcat(dados[*num], temp);

        (*num)++;
        dados = realloc(dados, ((*num) + 1) * sizeof(char *)); //aumenta matriz
        dados[*num] = (char *)malloc(sizeof(char) * 5);        // vetor
    }

    if (fclose(in) == EOF)
        printf(errStr);

    return dados;
}

void printReg(registro reg)
{
    printf("\n===============RESULTADO===============\n");
    printf("    Codigo cliente: %s\n", reg.codCliente);
    printf("    Codigo Veiculo: %s\n", reg.codVeiculo);
    printf("      Nome Cliente: %s\n", reg.nomeCliente);
    printf("      Nome Veiculo: %s\n", reg.nomeVeiculo);
    printf("Quantidade de dias: %d\n", reg.quantDias);
    printf("=======================================\n");
}

//mostra o Registro dado um RRN do registro
void printRegRRN(FILE *arq, int rrn)
{
    long bof = (long)rrn * (long)sizeof(registro);
    fseek(arq, bof, 0);
    registro temp;
    fread(&temp, sizeof(registro), 1, arq);
    printReg(temp);
}

//Busca uma chave
void Search(FILE *arq, FILE *bTree, int rrn, char *key)
{
    if (rrn < 0)
    {
        printf(notFound, key);
        return;
    }

    page currentPage;
    ReadPage(bTree, rrn, &currentPage);
    int i;
    for (i = 0; i < MAXKEYS; i++)
    {
        int comp = strcmp(key, currentPage.key[i].keyStr);

        if (comp < 0)
            return Search(arq, bTree, currentPage.child[i], key); //se for menor, pesquisa a esquerda
        else if (comp == 0)
        {
            printf(keyFound, key, rrn, i);
            printRegRRN(arq, currentPage.key[i].rrn); //se for igual, encontrou
            return;
        }
    }
    return Search(arq, bTree, currentPage.child[i], key); //pesquisa a direita da ultima chave
}

//Percorre a árvo in-ordem e mostra os regsitros ordenados
void InOrdem(FILE *arq, FILE *bTree, int rrn)
{
    //percorre esquerda; raiz; direita

    if (rrn < 0)
    {
        printf(treeEmptyMsg);
        return;
    }

    page currentPage;
    ReadPage(bTree, rrn, &currentPage); //le pagina

    int i;
    for (i = 0; i < MAXKEYS; i++)
    {
        if (currentPage.child[i] != -1) //percorre esquerda da chave
            InOrdem(arq, bTree, currentPage.child[i]);

        if (currentPage.key[i].rrn == -1) //se posição vazia
            return;

        printRegRRN(arq, currentPage.key[i].rrn); //mostra a chave
        //percorre direita que será esquerda na próxima iteração
    }
    if (currentPage.child[i] != -1)
        InOrdem(arq, bTree, currentPage.child[i]); //percorre ultimo filho a direita
}

//Adiciona registro ao indice e arquivo principal
void AddReg(FILE *dataFile, FILE **bTree, registro reg, short *rootRRN)
{
    fseek(dataFile, 0, 2);                                 //vai para fim do arquivo
    int regRRN = (int)(ftell(dataFile) / sizeof(registro)); //rrn para registro no arquivo principal

    prmKey key;
    key.rrn = regRRN;
    strcpy(key.keyStr, reg.codCliente);
    strcat(key.keyStr, reg.codVeiculo);

    if (AddIndex(bTree, key, rootRRN) != DUP){        //adiciona chave do registro na árvore B
        fwrite(reg.codCliente, sizeof(char), sizeof(reg.codCliente), dataFile);
        fwrite(reg.codVeiculo, sizeof(char), sizeof(reg.codVeiculo), dataFile);
        fwrite(reg.nomeCliente, sizeof(char), sizeof(reg.nomeCliente), dataFile);
        fwrite(reg.nomeVeiculo, sizeof(char), sizeof(reg.nomeVeiculo), dataFile);
        fwrite(&(reg.quantDias), sizeof(int), 1, dataFile);
    }
}

//Arvore-B
//retorna YES caso ocorra inserção, retorna DUP caso duplicado;
int AddIndex(FILE **bTree, prmKey key, short *rootRRN)
{
    if (*bTree == NULL)
    {
        *rootRRN = CreateTree(bTree, key);
        printf(sucessMsg, key.keyStr);
        return YES;
    }

    short promotedRRN = -1;
    prmKey promotedKey;

    int promoted = Insert(*bTree, *rootRRN, key, &promotedRRN, &promotedKey);
    if (promoted == YES)
        *rootRRN = CreateRoot(*bTree, promotedKey, *rootRRN, promotedRRN);

    if (promoted == DUP)
        return DUP;
    else
    {
        printf(sucessMsg, key.keyStr);
        return YES;
    }
}

short GetRoot(FILE *bTree)
{
    short root = -1;
    fseek(bTree, 0, 0);
    if (fread(&root, sizeof(short), 1, bTree) == 0)
    {
        printf(errReadF);
        return -1;
    }
    return root;
}

short CreateTree(FILE **bTree, prmKey key)
{
    short temp = -1;
    if ((*bTree = fopen(treeFile, "w+b")) == NULL)
    {
        printf(errOpenFile);
        return temp;
    }
    fwrite(&temp, sizeof(short), 1, *bTree);
    return CreateRoot(*bTree, key, -1, -1);
}

short CreateRoot(FILE *bTree, prmKey key, short left, short right)
{
    page pag;
    short rrn = GetPage(bTree);
    PageInit(&pag);
    pag.key[0] = key;
    pag.child[0] = left;
    pag.child[1] = right;
    pag.keycount = (short)1;
    WritePage(bTree, rrn, &pag);
    PutRoot(bTree, rrn);
    return rrn;
}

short GetPage(FILE *bTree)
{
    fseek(bTree, 0, 2);
    long addr = ftell(bTree);
    addr -= (long)2;
    return (addr > 0) ? (short)addr / sizeof(page) : 0;
}

short Insert(FILE *bTree, short rrn, prmKey key, short *promotedChild, prmKey *promotedKey)
{
    if (rrn == -1)
    {
        *promotedKey = key;
        *promotedChild = -1;
        return YES;
    }
    page currentPage, newPage;
    ReadPage(bTree, rrn, &currentPage);

    short pos;
    int found = SearchNode(key, &currentPage, &pos);
    if (found)
    {
        printf(duplicateMsg, key.keyStr);
        return DUP;
    }
    short promotedRRNRecursion;
    prmKey promotedKeyRecursion;
    short promoted = Insert(bTree, currentPage.child[pos], key, &promotedRRNRecursion, &promotedKeyRecursion);

    if (promoted == DUP)
        return DUP;

    if (!promoted)
        return NO;

    if (currentPage.keycount < MAXKEYS)
    {
        InsertInPage(bTree, promotedKeyRecursion, promotedRRNRecursion, &currentPage);
        WritePage(bTree, rrn, &currentPage);
        return NO;
    }
    else
    {
        Split(bTree, promotedKeyRecursion, promotedRRNRecursion, &currentPage, promotedKey, promotedChild, &newPage);
        WritePage(bTree, rrn, &currentPage); //Atualiza pagina atual
        WritePage(bTree, *promotedChild, &newPage);
        return YES;
    }
}

short SearchNode(prmKey key, page *pag, short *pos)
{
    int i = 0;
    for (i = 0; i < pag->keycount && strcmp(key.keyStr, pag->key[i].keyStr) > 0; i++)
    {
        //nada;
    }
    *pos = i;
    return (*pos < pag->keycount && strcmp(key.keyStr, pag->key[*pos].keyStr) == 0) ? YES : NO;
}

void Split(FILE *bTree, prmKey key, short rChild, page *oldPag, prmKey *promotedKey, short *promotedRChild, page *newPage)
{
    printf(splitMsg);

    int j;
    prmKey workKeys[MAXKEYS + 1];
    short workChild[MAXKEYS + 2];

    prmKey nullKey = {.keyStr="@@@@",.rrn=-1};

    for (j = 0; j < MAXKEYS; j++)
    {
        workKeys[j] = oldPag->key[j]; //copia chaves e filhos
        workChild[j] = oldPag->child[j];

        oldPag->key[j] = nullKey; //apaga pagina antiga
        oldPag->child[j] = -1;
    }
    workChild[j] = oldPag->child[j];
    oldPag->child[j] = -1;


    for (j = MAXKEYS; strcmp(key.keyStr, workKeys[j - 1].keyStr) < 0 && j > 0; j--)
    {
        workKeys[j] = workKeys[j - 1]; //insere nova pagina
        workChild[j + 1] = workChild[j];
    }
    workKeys[j] = key;
    workChild[j + 1] = rChild;

    *promotedRChild = GetPage(bTree);
    PageInit(newPage);

    const int MINKEYS = (MAXKEYS + 1) / 2; //=2

    //Separa 2 chaves para cada pagina
    for (j = 0; j < MINKEYS; j++)
    {
        oldPag->key[j] = workKeys[j]; //copia metade inferior do vetor para oldPag
        oldPag->child[j] = workChild[j];

        newPage->key[j] = workKeys[j + MINKEYS]; //copia metade superior vetor para newPage
        newPage->child[j] = workChild[j + MINKEYS];
    }
    //ultimos ponteiros de filhos
    newPage->child[MINKEYS] = workChild[j + MINKEYS];

    //remove chave promovida da pagina antiga
    oldPag->key[MINKEYS - 1] = nullKey;
    oldPag->child[MINKEYS] = -1;

    newPage->keycount = MAXKEYS - 1;
    oldPag->keycount = MINKEYS - 1;

    *promotedKey = workKeys[MINKEYS - 1];

    printf(promotedKeyMsg, (*promotedKey).keyStr);
}

void InsertInPage(FILE *bTree, prmKey key, short rChild, page *pag)
{
    int j;
    for (j = pag->keycount; strcmp(key.keyStr, pag->key[j - 1].keyStr) < 0 && j > 0; j--)
    {
        pag->key[j] = pag->key[j - 1];
        pag->child[j + 1] = pag->child[j];
    }
    pag->keycount += 1;
    pag->key[j] = key;
    pag->child[j + 1] = rChild;
}

void ReadPage(FILE *bTree, short rrn, page *pag)
{
    long addr = (long)rrn * (long)sizeof(page) + (long)2;
    fseek(bTree, addr, 0);
    fread(pag, sizeof(page), 1, bTree);
}

void WritePage(FILE *bTree, short rrn, page *pag)
{
    long addr;
    addr = (long)rrn * (long)sizeof(page) + (long)2;
    fseek(bTree, addr, 0);
    fwrite(pag, sizeof(page), 1, bTree);
}

void PageInit(page *pag)
{
    for (int i = 0; i < MAXKEYS; i++)
    {
        strcpy(pag->key[i].keyStr, NULL_KEY);
        pag->key[i].rrn = -1;
        pag->child[i] = -1;
    }
    pag->child[MAXKEYS] = -1;
}

void PutRoot(FILE *bTree, short root)
{
    fseek(bTree, 0, 0);
    fwrite(&root, sizeof(short), 1, bTree);
}
