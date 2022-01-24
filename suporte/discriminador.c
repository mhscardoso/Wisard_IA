#include "tuplas.c"

/*
    O programa foi pensado assim:
        Cada classe tem as informacoes de nome, discriminador e um ponteiro para a proxima
        classe, de modo a termos uma lista encadeada, que sera utilizada na obdervacao, em que
        a wisard comparara as informacoes de discriminador em discriminador.
*/
typedef struct DISCRIMINADOR {
    int id;
    char *nome;
    int **memorias;
    int somador;
    int contador;
    struct DISCRIMINADOR *prox;
}classe;


void novoDiscriminador(int linhas,
                       int colunas,
                       micro_t tuplas[(linhas * colunas) / defineTuplas(linhas, colunas)][defineTuplas(linhas, colunas)],
                       classe *primeiro) {

    /*
        linhas: linhas da retina
        colunas: colunas da retina
        tuplas: matriz com as tuplas
        nome_da_classe: para cada classe, um identificador
    */

    int total_coordenadas = linhas * colunas;               // Qtd. de coordenadas na retina
    int qtd_tuplas = defineTuplas(linhas, colunas);         // Tuplas em cada conjunto
    int linhas_da_tupla = total_coordenadas / qtd_tuplas;   // Quantidade de conjuntos de tuplas

    // A quantidade de colunas de um discriminador sera 2 ** qtd_tuplas
    int colunas_discriminador = 1 << qtd_tuplas;

    // Procurando o ultimo elemento da lista de classes
    classe *atual = primeiro;
    while (atual -> prox != NULL) {
        atual = atual -> prox;
    }
    atual -> prox = (classe*) malloc(sizeof(classe));

    // Criando discriminador zerado
    int *discriminador[linhas_da_tupla];
    for (int i = 0; i < linhas_da_tupla; i++) {
        discriminador[i] = (int*) malloc(sizeof(int) * colunas_discriminador);
        if (discriminador[i] == NULL) {
            fprintf(stderr, "Impossivel alocar memoria\n");
            return;
        }
        for (int j = 0; j < colunas_discriminador; j++) {
            discriminador[i][j] = 0;
        }
    }   

    // Alocando o discriminador nos enderecos de acordo com as tuplas dadas.
    int endereco = 0;
    for (int i = 0; i < linhas_da_tupla; i++) {
        for (int j = qtd_tuplas - 1; j > -1; j--) {
            endereco += (tuplas[i][qtd_tuplas - 1 - j].valor << j);
        }
        discriminador[i][endereco]++;
        endereco = 0;
    }

    // Terminando
    atual -> prox -> id = atual -> id + 1;

    int numero_de_caracteres;
    printf("Decida um nome para a classe: ");
    scanf(" %[^\n]%n", atual -> prox -> nome = (char*) malloc(sizeof(char) * numero_de_caracteres), &numero_de_caracteres);

        // Alocando os valores do discriminador nas memorias da classe
    atual -> prox -> memorias = (int**) malloc(sizeof(discriminador));
    for (int i = 0; i < linhas_da_tupla; i++) {
        atual -> prox -> memorias[i] = (int*) malloc(sizeof(int) * colunas_discriminador);
        for (int j = 0; j < colunas_discriminador; j++) {
            atual -> prox -> memorias[i][j] = discriminador[i][j];
        }
    }
        // Liberando espaco
    for (int i = 0; i < linhas_da_tupla; i++) {
        free(discriminador[i]);
    }

    atual -> prox -> somador = 0;
    atual -> prox -> contador = 1;
    atual -> prox -> prox = NULL;

    printf("Esta eh a classe %s\n", atual -> prox -> nome);

    return;
}


void modificaDiscriminador(int linhas,
                           int colunas,
                           micro_t tuplas[(linhas * colunas) / defineTuplas(linhas, colunas)][defineTuplas(linhas, colunas)],
                           classe *primeiro) {
    /*
        Para exemplos de uma mesma classe. A funcao le um arquivo .txt e altera um discriminador de uma classe
        ja criada.

        linhas: linhas da retina
        colunas: colunas da retina
        tuplas: base para escrever no discriminador
        primeiro: ponteiro para a primeira classe da lista
    */

    unsigned int escolha;

    // Capturando do usuario de qual classe pertence o exemplo que ele mostrou
    classe *atual = primeiro;
    // Mostrando as opcoes ao usuario
    while (atual != NULL) {
        printf("%s - %i\n", atual -> nome, atual -> id);
        atual = atual -> prox;
    }
    printf("A qual classe pertence este exemplo? Digite o numero ao lado do nome da classe: ");
    scanf("%i", &escolha);

    // Encontrando a classe
    atual = primeiro;
    while (atual != NULL) {
        if (atual -> id == escolha) {
            break;
        }

        // Se chegar no fim e nao encontrar, a entrada foi invalida
        if (atual -> prox == NULL) {
            printf("Classe nao identificada\n");
            return;
        }
        atual = atual -> prox;
    }


    // Numeros importantes
    int elementos_da_tupla = defineTuplas(linhas, colunas);
    int quantidade_tuplas = (linhas * colunas) / elementos_da_tupla;
    int colunas_discriminador = 1 << elementos_da_tupla;

    // A partir da tupla recebida, calculamos os enderecos aos quais o numero 1 deve ser adicionado
    int endereco = 0;
    for (int i = 0; i < quantidade_tuplas; i++) {
        for (int j = elementos_da_tupla - 1; j > -1; j--) {
            endereco += (tuplas[i][elementos_da_tupla - 1 - j].valor << j);
        }
        
        // Se ja estiver com o numero 1, nao faz nada
        if (atual -> memorias[i][endereco] == 0) {
            atual -> memorias[i][endereco] += 1;
        }

        endereco = 0;
    }

    // O numero de exemplos que a classe recebeu aumenta de 1
    atual -> contador += 1;
    return;
}


void modificaDiscriminadorBleaching(int linhas,
                           int colunas,
                           micro_t tuplas[(linhas * colunas) / defineTuplas(linhas, colunas)][defineTuplas(linhas, colunas)],
                           classe *primeiro) {
    /*
        Para exemplos de uma mesma classe. A funcao le um arquivo .txt e altera um discriminador de uma classe
        ja criada.

        linhas: linhas da retina
        colunas: colunas da retina
        tuplas: base para escrever no discriminador
        primeiro: ponteiro para a primeira classe da lista
    */

    unsigned int escolha;
    
    // Capturando do usuario de qual classe pertence o exemplo que ele mostrou
    classe *atual = primeiro;
    while (atual != NULL) {
        printf("%s - %i\n", atual -> nome, atual -> id);
        atual = atual -> prox;
    }
    printf("A qual classe pertence este exemplo? Digite o numero ao lado do nome da classe: ");
    scanf("%i", &escolha);

    // Identicicando a classe
    atual = primeiro;
    while (atual != NULL) {
        if (atual -> id == escolha) {
            break;
        }
        // Se chegra no final e nao encontrar, a entrada eh invalida
        if (atual -> prox == NULL) {
            printf("Classe nao identificada\n");
            return;
        }
        atual = atual -> prox;
    }

    // Numeros importantes
    int elementos_da_tupla = defineTuplas(linhas, colunas);             // Esta funcao esta em tuplas.c
    int quantidade_tuplas = (linhas * colunas) / elementos_da_tupla;
    int colunas_discriminador = 1 << elementos_da_tupla;

    int endereco = 0;
    for (int i = 0; i < quantidade_tuplas; i++) {
        for (int j = elementos_da_tupla - 1; j > -1; j--) {
            endereco += (tuplas[i][elementos_da_tupla - 1 - j].valor << j);
        }

        // Aqui, independente se o numero no endereco ja for 1, sempre somaremos
        atual -> memorias[i][endereco] += 1; 
        endereco = 0;
    }

    // A classe aprendeu um novo exemplo
    atual -> contador += 1;
    return;
}



