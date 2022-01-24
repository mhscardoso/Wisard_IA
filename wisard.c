#include <stdio.h>
#include <stdlib.h>
#include "suporte/discriminador.c"

#ifdef _WIN32
	#define clear() system("cls")
#else
	#define clear() system("clear")
#endif

// Uma vez analisada a retina, o tamanho de todas deve ser constante durante a execucao
int linhas_retina = 1;
int colunas_retina = 0;
int elementos_de_uma_tupla;
int quantidade_de_tuplas;

// Prototipos
int aprender(char retina[linhas_retina][colunas_retina], 
             micro_t tuplas[quantidade_de_tuplas][elementos_de_uma_tupla], 
             classe *primeira);

int observar(char retina[linhas_retina][colunas_retina], 
             micro_t tuplas[quantidade_de_tuplas][elementos_de_uma_tupla], 
             classe *primeira);

int lerArquivo(char filename[], char retina[linhas_retina][colunas_retina]);

int main(int argc, char *argv[]) {

    char nome_do_arquivo[30];

    // Leitura do primeiro arquivo de retina
    if (argc == 1) {
        printf("Digite o nome de um arquivo (e seu caminho) para que eu comece a aprender: ");
        scanf("%s", nome_do_arquivo);
        int t_nome = strlen(nome_do_arquivo);
        if (strcmp((nome_do_arquivo + t_nome - 4), ".txt")) {
            printf("O arquivo deve estar no formato .txt\n");
            return 1;
        }
    }
    else if (argc == 2) {
        int t_nome = strlen(argv[1]);
        strcpy(nome_do_arquivo, argv[1]);
        if (strcmp((nome_do_arquivo + t_nome - 4), ".txt")) {
            printf("O arquivo deve estar no formato .txt\n");
            return 1;
        }
        printf("Me parece que ja tenho um arquivo! (%s)\n", nome_do_arquivo);
    }
    else {
        printf("Uso: ./main || ./main archive.txt");
        return 1;
    }

    FILE* reader = fopen(nome_do_arquivo, "r");
    if (reader == NULL) {
        fprintf(stderr, "Nao foi possivel abrir este arquivo.\n");
        return 1;
    }

    // linhas_retina e colunas_retina sao globais e definidas pela primeira leitura
    char c;
    while ((c = fgetc(reader)) != EOF) {
        if (c == '\n') {
            linhas_retina++;
        }
        if (linhas_retina == 1) {
            colunas_retina++;
        }
        
        if ((c != '1') && (c != '0') && (c != EOF) && (c != '\n')) {
            printf("Ops! Saiba que eu leio, apenas, arquivos .txt com 0's e 1's\n");
            return 1;
        }
    }

    printf("A sua retina foi definida tendo %d linhas e %d colunas.\n", linhas_retina, colunas_retina);
     
    fclose(reader);

    FILE *reader2 = fopen(nome_do_arquivo, "r");

    // Nossa retina sera do tipo char. Ela recebera os valores '0' e '1'.
    char retina[linhas_retina][colunas_retina];
    char buffer;

    for (int i = 0; i < linhas_retina; i++) {
        for (int j = 0; j <= colunas_retina; j++) {
            buffer = fgetc(reader2);
            if (buffer == '\n') {
                break;
            }
            retina[i][j] = buffer;
        }
    }
    fclose(reader2);

    // Criando a estrutura de tuplas - APENAS UMA VEZ - com a primeira classe apresentada
    int total_coordenadas = linhas_retina * colunas_retina;

    elementos_de_uma_tupla = defineTuplas(colunas_retina, linhas_retina);   // Variavel global para as tuplas da execucao
    quantidade_de_tuplas = total_coordenadas / elementos_de_uma_tupla;      // Variavel global para a tuplas da execucao

    int coordenadas[total_coordenadas][2];                                  // Para coordenadas randomicas nas tuplas
    micro_t tuplas[quantidade_de_tuplas][elementos_de_uma_tupla];           // Tuplas da execucao

    criaTupla(linhas_retina, colunas_retina, coordenadas, tuplas, retina);

    // Criando o primeiro discriminador - classe
    int colunas_discriminador = 1 << elementos_de_uma_tupla;    
    int *discriminador[quantidade_de_tuplas];
    for (int i = 0; i < quantidade_de_tuplas; i++) {
        discriminador[i] = (int*) malloc(sizeof(int) * colunas_discriminador);

        if (discriminador[i] == NULL) {
            fprintf(stderr, "impossivel alocar memoria.\n");
            return 2;
        }
        // O discriminador inicia zerado
        for (int j = 0; j < colunas_discriminador; j++) {
            discriminador[i][j] = 0;
        }
    }
    
    // Verificando o endereco segundo as tuplas
    int endereco = 0;
    for (int i = 0; i < quantidade_de_tuplas; i++) {
        for (int j = elementos_de_uma_tupla - 1; j > -1; j--) {
            endereco += (tuplas[i][elementos_de_uma_tupla - 1 - j].valor << j);
        }
        discriminador[i][endereco]++;
        endereco = 0;
    }

    // Criando a primeira classe - A DEFINIDORA
    classe *classe_definidora = (classe*) malloc(sizeof(classe));   // Alocando espaco para a primeira classe
    classe_definidora -> id = 1;                                    // A primeira classe possui id = 1

    // Alocando memoria sufuciente para o nome que o usuario digitar
    int tamanho_nome;
    printf("Decida um nome para a primeira classe: ");
    scanf(" %[^\n]%n", classe_definidora -> nome = (char*) malloc(sizeof(char) * tamanho_nome), &tamanho_nome);

    classe_definidora -> memorias = discriminador;                  // Armazenando o discriminador criado
    classe_definidora -> somador = 0;                               // Somador inicia-se nulo
    classe_definidora -> contador = 1;                              // A classe aprendeu 1 exemplo
    classe_definidora -> prox = NULL;                               // Aponta para nada.

    // Mostrando o resultado para o usuario
    printf("Esta eh a classe %s.\n", classe_definidora -> nome);  

    int escolha;                                                    // Qual a opcao o usuario escolher

    // Ponteiros para as funcoes
    int (*funcs[])(char (*)[colunas_retina], micro_t (*)[elementos_de_uma_tupla], classe*) = {aprender, observar};

    // Loop infinito, quebra dependendo da escolha do usuario
    while (1) {
        printf("\nEscolha o que eu devo fazer (1 - APRENDER; 2 - OBSERVAR; 3 - SAIR): ");
        scanf("%d", &escolha);
        if (escolha == 3) {
            break;
        }
        if ((escolha != 1) && (escolha != 2)) {
            printf("\nInvalido\n");
            continue;
        }
        clear();
        funcs[escolha - 1](retina, tuplas, classe_definidora);
    }
    return 0;
}

int lerArquivo(char filename[], char retina[linhas_retina][colunas_retina]) {
    /*
        Esta funcao se encarregara de ler os arquivos dados pelo usuario apos a primeira leitura da "main"
        e, depois, adicionar os valores lidos na retina.

        filename: nome do arquivo
        retina: para armazenar os valores
    */

    FILE *reader = fopen(filename, "r");
    if (reader == NULL) {
        fprintf(stderr, "Impossivel abrir o arquivo.\n");
        return 1;
    }

    char c;                             // Buffer
    int linhas = 1;                     // Para verificar se a quantidade de linhas eh igual
    int colunas = 0;                    // Para verificar se a quantidade de colunas eh igual

    // Zerando a retina
    for (int i = 0; i < linhas_retina; i++) {
        for (int j = 0; j < colunas_retina; j++) {
            retina[i][j] = '0';
        }
    }

    char buffer;
    // Armazenando os dados na retina
    for (int i = 0; i < linhas_retina; i++) {
        for (int j = 0; j <= colunas_retina; j++) {
            buffer = fgetc(reader);
            if (buffer == '\n') {
                break;
            }
            retina[i][j] = buffer;
        }
    }

    return 0;
}

int aprender(char retina[linhas_retina][colunas_retina], 
             micro_t tuplas[quantidade_de_tuplas][elementos_de_uma_tupla], 
             classe *primeira) {
    /*
        Funcao para aprender uma nova classe ou dar um exemplo para uma
        classe existente.

        filename: string com o nome do arquivo
        nome_classe: string com o nome da classe
        retina: para armazenar os novos dados
        tuplas: para criar o discriminador de uma nova classe
        primeira: ponteiro para os discriminadores - para as classes.
    */
    unsigned short option;
    printf("\nQuer aprender uma nova classe (1) ou dar um novo exemplo para uma classe ja existente (2): ");
    scanf("%hd", &option);

    if ((option != 1) && (option != 2)) {
        printf("Invalido\n");
        return 1;
    }

    // Lendo o arquivo digitado pelo usuario
    char nome_do_arquivo[30];
    printf("\nEscolha um arquivo para eu aprender: ");
    scanf("%s", nome_do_arquivo);

    // Armazenando na retina
    int leitura = lerArquivo(nome_do_arquivo, retina);
    if (leitura) {
        return 1;
    }

    // Aloca os valores na tupla de acordo com a nova retina
    modificaTupla(linhas_retina, colunas_retina, tuplas, retina);

    // Ponteiro para funcao
    void (*opt[])(int, int, micro_t (*)[elementos_de_uma_tupla], classe*) = {novoDiscriminador, modificaDiscriminador};

    // Acao final
    opt[option - 1](linhas_retina, colunas_retina, tuplas, primeira);

    return 0;
}

int observar(char retina[linhas_retina][colunas_retina], 
             micro_t tuplas[quantidade_de_tuplas][elementos_de_uma_tupla], 
             classe *primeira) {

    /*
        Funcao para avaliar a qual classe uma certa imagem pertence.

        retina: matriz criada na main
        tuplas: definida na main, a ser alterada para observacao
        primeira: ponteiro para a primeira classe definida na main
    */

    // Lendo o arquivo e alocando os valores na retina
    char nome_do_arquivo[30];
    printf("\nEscolha um arquivo para eu observar: ");
    scanf("%s", nome_do_arquivo);

    int verifica_leitura = lerArquivo(nome_do_arquivo, retina);
    if (verifica_leitura) {
        return 1;
    }

    int total_coordenadas = linhas_retina * colunas_retina;                           // Qtd. de coordenadas na retina
    int linhas_da_tupla = total_coordenadas / elementos_de_uma_tupla;                 // Quantidade de conjuntos de tuplas

    // A quantidade de colunas de um discriminador sera 2 ** qtd_tuplas
    int colunas_discriminador = 1 << elementos_de_uma_tupla;

    // Criando as tuplas da imagem observada
    modificaTupla(linhas_retina, colunas_retina, tuplas, retina);

    int id_classe;

    int endereco = 0;
    classe *atual = primeira;
    while (atual != NULL) {

        // Soma a endereco o valor , lembrando que eh uma representacao binaria, ou seja
        // de acordo com sua localizacao na tupla, soma-se um valor diferente
        for (int i = 0; i < quantidade_de_tuplas; i++) {
            for (int j = elementos_de_uma_tupla - 1; j > -1; j--) {
                endereco += tuplas[i][elementos_de_uma_tupla - 1 - j].valor << j;
            }
            
            // Se no endereco calculado, existe 1, adiciona +1 ao somador da classe
            if (atual -> memorias[i][endereco] == 1) {
                atual -> somador += 1;
            }
            endereco = 0;
        
        }
        // Proxima classe
        atual = atual -> prox;
    }

    // Para comparar cada classe e seus somadores
    atual = primeira;
    classe *cls_maior;
    int maior;              // Maior somador
    int id_maior;           // id do maior somador
    int indeciso = 0;       // Quantos somadores iguais sao, tambem os maiores

    while (atual != NULL) {

        // Se for a primeira classe, ou se o somador da classe for maior
        if (atual -> id == 1 || atual -> somador > maior) {
            maior = atual -> somador;
            id_maior = atual -> id;
            cls_maior = atual;
            indeciso = 0;
        }
        else if (atual -> somador == maior) {
            indeciso++;
        }

        // Mostrando o somador da classe
        printf("Somador da classe %s: %d\n", atual -> nome, atual -> somador);

        // Retornando o somador a zero
        atual -> somador = 0;
        atual = atual -> prox;
    }

    // Caso haja indecisao
    if (indeciso > 0) {
        printf("Nao consigo dizer a qual classe esta imagem pertence.\n");
        return 1;
    }

    // retorno comum
    printf("Esta imagem pertence a classe %s.\n", cls_maior -> nome);
    return id_classe;
}


