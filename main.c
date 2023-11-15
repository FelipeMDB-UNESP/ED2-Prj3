//André Nogueira Dias
//Felipe de Britto
//Rafael Daiki
//Ryan

#include "header.h"

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
        
        atualiza_log("Execucao do Menu");

        // Exibe o menu
        printf("Menu:\n");
        printf("1. Insercao\n");
        printf("2. Listar os dados de todos os clientes\n");
        printf("3. Pesquisar por chave primária\n");
        printf("4. Carregar Arquivo\n");
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

                    printf("\nDigite um numero entre 1 e %d\n", tam);
                    inserirRegistro(arq_data, arq_index, pasta);   

                    fclose(arq_data);
                    fclose(arq_index);
                }
                else {
                    printf("Arquivos ainda nao foram carregados!\n");
                } 
                break;
            //executa a busca binaria por chave
            case 2:
                if (load_de_arquivos) {

                    arq_data = fopen("dados.bin", "a+b");

                    arq_index = fopen("indices.bin", "a+b");
                    int posChave;
                    printf("Qual a posicao da chave que voce deseja pesquisar? 1-%d\n",tam);
                    scanf(" %d", &posChave);
                    printf("Pesquisando a chave %s.......\n", chaves[posChave-1]);

                    pesquisarPorChavePrimaria(arq_index, arq_data, chaves[posChave-1]);

                    
                } break;
            //carrega os dados
            case 3:
                pasta = carregar_dados("insere.bin");
                tam = tam_pasta(&pasta);
                chaves = carregar_chaves("busca_p.bin");
                atualiza_log("Arquivos carregados.");
                load_de_arquivos = true;
                break;
            //limpa o arquivo de logs, registro das execuções do programa
            case 4:
                limpar_log();
                break;
            //fecha o programa
            case 0:
                atualiza_log("Execucao Finalizada.");
    
                exit(0);
            default:
                atualiza_log("Opcao invalida. Por favor, escolha uma opcao valida (0-4).");
                break;
        } 
    } while (opcao != 0);   
   
    return 0;
}