

//Libs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Constantes
#define MAXKEYS 3 //Arvore-b ordem 4
#define NULL_KEY "@@@@"

//nome dos arquivos
#define fileIn "insere.bin"
#define keyIn "busca.bin"
#define countFile "indicesAux.bin"
#define treeFile "arvoreB.bin"
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
    char CodCli[3];
    char CodF[3];
    char NomeCli[50];
    char NomeF[50];
    char Genero[50];
} registro;

typedef struct
{
    char keyStr[5]; //chave = string concatenada de CodCli + CodF
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
    FILE *bTree = NULL;
    if ((bTree = fopen(treeFile, "r+b")) != NULL)
    {
        rootRRN = GetRoot(bTree);
        if (rootRRN == -1)
        {
            printf(errReadF);
            return 0;
        }
    }

    FILE *arq = NULL;
    if ((arq = fopen(mainFile, "r+b")) == NULL)
    {
        printf(makingFileMsg);
        if ((arq = fopen(mainFile, "w+b")) == NULL)
        {
            printf(errMakeFile);
            return 0;
        }
    }

    //MENU
    int option = -1;
    while (option != 0)
    {
        option = Menu();
        switch (option)
        {
        case 1:
            //Inserir
            if (currentData < totalData)
            {
                AddReg(arq, &bTree, dados[currentData], &rootRRN);
                ++currentData;
            }
            else
            {
                printf(emptyData);
            }
            break;
        case 2:
            //Listar dados - percurso in ordem
            printf(searchingMSG);
            InOrdem(arq, bTree, rootRRN);
            break;
        case 3:
            //Pesquisar cliente - busca na árvore
            if (currentSearch < totalSearch)
            {
                Search(arq, bTree, rootRRN, searchKey[currentSearch]);
                currentSearch++;
            }
            else
            {
                printf("Sem chaves para buscar!");
            }
            break;
        }
    }
    //Salva os contadores
    SetCounters(nexData + currentData, nextSearch + currentSearch);

    //Fechar arquivos
    if (fclose(bTree) == EOF)
        printf(errStr);

    if (fclose(arq) == EOF)
        printf(errStr);

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
        printf("2- Listar Clientes\n");
        printf("3- Pesquisar Cliente\n");
        printf("0- Sair\n");
        printf("============================\n");
        scanf("%d", &opt);
    } while (opt < 0 || opt > 3);

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
    printf("Codigo cliente: %s\n", reg.CodCli);
    printf("  Codigo Filme: %s\n", reg.CodF);
    printf("  Nome Cliente: %s\n", reg.NomeCli);
    printf("    Nome Filme: %s\n", reg.NomeF);
    printf("        Genero: %s\n", reg.Genero);
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
    fseek(dataFile, 0, 2);                                  //vai para fim do arquivo
    int regRRN = (int)(ftell(dataFile) / sizeof(registro)); //rrn para registro no arquivo principal

    prmKey key;
    key.rrn = regRRN;
    strcpy(key.keyStr, reg.CodCli);
    strcat(key.keyStr, reg.CodF);

    if (AddIndex(bTree, key, rootRRN) != DUP)        //adiciona chave do registro na árvore B
        fwrite(&reg, sizeof(registro), 1, dataFile); //se não duplicado, grava registro no arquivo principal
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
