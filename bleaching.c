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

int observarBleaching(char retina[linhas_retina][colunas_retina], 
                      micro_t tuplas[quantidade_de_tuplas][elementos_de_uma_tupla], 
                      classe *primeira);

int imagemMental(char retina[linhas_retina][colunas_retina], 
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

        for (int j = 0; j < colunas_discriminador; j++) {
            discriminador[i][j] = 0;
        }
    }

    // Criando um discriminador zerado
    int endereco = 0;
    for (int i = 0; i < quantidade_de_tuplas; i++) {
        for (int j = elementos_de_uma_tupla - 1; j > -1; j--) {
            endereco += (tuplas[i][elementos_de_uma_tupla - 1 - j].valor << j);
        }
        discriminador[i][endereco]++;
        endereco = 0;
    }
    
    // Criando a primeira classe - A DEFINIDORA
    classe *classe_definidora = (classe*) malloc(sizeof(classe));

    // A primeira classe possui id = 1
    classe_definidora -> id = 1;

    // Alocando espaco suficiente para o nome que o usuario digitar
    int tamanho_nome;
    printf("Decida um nome para a classe: ");
    scanf(" %[^\n]%n", classe_definidora -> nome = (char*) malloc(sizeof(char) * tamanho_nome), &tamanho_nome);

    classe_definidora -> memorias = discriminador;                  // Alocando o discriminador criado
    classe_definidora -> somador = 0;                               // Somador inicia-se nulo
    classe_definidora -> contador = 1;                              // A classe aprendeu um exemplo
    classe_definidora -> prox = NULL;                               // Nao existe proximo, ainda.

    // Mostrando o resultado ao usuario
    printf("Esta eh a classe %s.\n", classe_definidora -> nome);
    
    // Qual a escolha do usuario
    int escolha;

    // Ponteiros para as funcoes
    int (*funcs[])(char (*)[colunas_retina], micro_t (*)[elementos_de_uma_tupla], classe*) = {aprender, observarBleaching, imagemMental};

    while (1) {
        printf("\nEscolha o que eu devo fazer (1 - APRENDER; 2 - OBSERVAR; 3 - IMAGEM MENTAL; 4 - SAIR): ");
        scanf("%d", &escolha);
        if (escolha == 4) {
            break;
        }
        if ((escolha != 1) && (escolha != 2) && (escolha != 3)) {
            printf("\nInvalido\n");
            continue;
        }
        clear();
        funcs[escolha - 1](retina, tuplas, classe_definidora);
    }
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

    char c;             // Buffer
    int linhas = 1;     // Para verificar se a quantidade de linhas eh igual
    int colunas = 0;    // Para verificar se a quantidade de colunas eh igual

    // Zerando a retina
    for (int i = 0; i < linhas_retina; i++) {
        for (int j = 0; j < colunas_retina; j++) {
            retina[i][j] = '0';
        }
    }

    char buffer;

    // Alocando os dados lidos na retina
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

    // Faz a leitura do arquivo e armazena na retina
    char nome_do_arquivo[30];
    printf("\nEscolha um arquivo para eu aprender: ");
    scanf("%s", nome_do_arquivo);

    int leitura = lerArquivo(nome_do_arquivo, retina);
    if (leitura) {
        return 1;
    }

    // Modifica as tuplas de acordo com a retina
    modificaTupla(linhas_retina, colunas_retina, tuplas, retina);

    void (*opt[])(int, int, micro_t (*)[elementos_de_uma_tupla], classe*) = {novoDiscriminador, modificaDiscriminadorBleaching};

    opt[option - 1](linhas_retina, colunas_retina, tuplas, primeira);

    return 0;
}



int observarBleaching(char retina[linhas_retina][colunas_retina], 
                      micro_t tuplas[quantidade_de_tuplas][elementos_de_uma_tupla], 
                      classe *primeira) {

    /*
        Funcao para avaliar a qual classe uma certa imagem pertence.

        retina: para armazenar os novos dados
        tuplas: para criar o discriminador de uma nova classe
        primeira: ponteiro para os discriminadores - para as classes.
    */
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

    // Criando um vetor de enderecos de acordo com as tuplas
    // Ao inves de um discriminador, criamos um vetor que mostra o endereco
    // em que estaria escrito 1 no discriminador
    int enderecos[linhas_da_tupla];
    int endereco = 0;
    for (int i = 0; i < linhas_da_tupla; i++) {
        for (int j = elementos_de_uma_tupla - 1; j > -1; j--) {
            endereco += (tuplas[i][elementos_de_uma_tupla - 1 - j].valor << j);
        }
        enderecos[i] = endereco;
        endereco = 0;
    }

    // Verificando o maior contador dentre as classes, lembrando que contador
    // se refera ao numero de exemplos que determinada classe aprendeu
    classe *atual = primeira;
    int maior_contador;
    int cont = 1;
    while (atual != NULL) {
        if (atual == primeira || atual -> contador > maior_contador) {
            maior_contador = atual -> contador;
        }
        atual = atual -> prox;
    }


    atual = primeira;
    int maior_somador;
    int id_maior;
    int indeciso = 0;

    // Aqui, ele itera ate o maior contador de uma classe
    while (cont <= maior_contador) {

        // Para cada classe, o algoritmo verifica se os enderecos do discriminador da retina,
        // enderecos estes que guardam onde a informacao 1 esta gravada.
        // Se no endereco do discriminador de determinada classe estiver escrito um valor maior
        // que "cont", que se refere ao bleaching, o somador da classe recebe mais um.
        for (int i = 0; i < linhas_da_tupla; i++) {
            if (atual -> memorias[i][enderecos[i]] >= cont) {
                atual -> somador++;
            }
        }

        // Verificando o maior somador e possiveis indecisoes
        if (atual == primeira || atual -> somador > maior_somador) {
            maior_somador = atual -> somador;
            id_maior = atual -> id;
            indeciso = 0;
        }
        else if (atual -> somador == maior_somador) {
            indeciso++;
        }

        // Proxima classe
        atual = atual -> prox;

        // Chegando ao final da lista, vejo se eh preciso voltar ao ponto de partida
        // ou se ja podemos quebrar o while, dadas as indecisoes
        if (atual == NULL) {
            atual = primeira;

            if (indeciso == 0) {
                break;
            }
            if (indeciso > 0 && cont == maior_contador) {
                break;
            }
            indeciso = 0;
            cont++;
        }
    }

    // Escrevando o resultado dos somadores, e tornando-os todos nulos
    while (atual != NULL) {
        printf("Somador da classe %s: %d\n", atual -> nome, atual -> somador);
        atual -> somador = 0;
        atual = atual -> prox;
    }

    // Se, ainda, houver indecisao.
    if (indeciso > 0) {
        printf("Nao consigo definir a classe\n");
        return 1;
    }

    // Sem indecisao.
    atual = primeira;
    while (atual != NULL) {
        if (atual -> id == id_maior) {
            printf("A imagem pertence a classe %s\n", atual -> nome);
            break;
        }
        atual = atual -> prox;
    }

    return 1;
}


int imagemMental(char retina[linhas_retina][colunas_retina], 
                 micro_t tuplas[quantidade_de_tuplas][elementos_de_uma_tupla], 
                 classe *primeira) {

    /*
        Cria um arquivo .txt com a imagem mental

        retina: para armazenar os novos dados
        tuplas: para criar o discriminador de uma nova classe
        primeira: ponteiro para os discriminadores - para as classes.
    */

    // Escolha da classe
    unsigned int escolha;
    unsigned int maior_id;          // Aloca o id da ultima classe da lista
    classe *a;                      // Inicio da iteracao

    // Escolha da classe
    for (a = primeira; a != NULL; a = a -> prox) {
        printf("%s - %d\n", a -> nome, a -> id);
        maior_id = a -> id;
    }
    printf("Escolha a classe para fazermos sua imagem mental (Digite o numero ao lado da classe): ");
    scanf("%i", &escolha);
    
    // Em caso de mau uso
    if (escolha < 1 || escolha > maior_id) {
        printf("Invalido\n");
        return 1;
    }

    // Pega a classe que o usuario escolheu
    for (a = primeira; a != NULL; a = a -> prox) {
        if (a -> id == escolha) {
            break;
        }
    }

    // Limpando a tela
    clear();

    printf("FAZENDO A IMAGEM MENTAL DA CLASSE\n");

    // Zerando a retina
    for (int i = 0; i < linhas_retina; i++) {
        for (int j = 0; j < colunas_retina; j++) {
            retina[i][j] = '0';
        }
    }

    // calculando a quantidade de colunas que tera o discriminador
    int colunas_discriminador = 1 << elementos_de_uma_tupla;

    // Itera pelo discriminador da classe escolhida
    for (int i = 0; i < quantidade_de_tuplas; i++) {
        for (int j = 0; j < colunas_discriminador; j++) {
            if (a -> memorias[i][j] > 0) {

                // Itera pelas tuplas, de acordo com os neuronios
                for (int k = elementos_de_uma_tupla - 1; k > -1; k--) {
                    // Calculando a representacao binaria e alocando os valores na retina
                    tuplas[i][elementos_de_uma_tupla - k - 1].valor = (j >> k) % 2;
                    
                    retina[tuplas[i][elementos_de_uma_tupla - k - 1].coordenadas[0]][tuplas[i][elementos_de_uma_tupla - k - 1].coordenadas[1]] += (a -> memorias[i][j] * tuplas[i][elementos_de_uma_tupla - k - 1].valor);
                }
            }
        }
    }

    // Escrevendo um arquivo .txt
    char *nome_do_arquivo = (char*) malloc(sizeof(char) * (strlen(a -> nome) + 6));
    sprintf(nome_do_arquivo, "%s.txt", a -> nome);

    FILE *escritor = fopen(nome_do_arquivo, "w");
    for (int i = 0; i < linhas_retina; i++) {
        for (int j = 0; j <= colunas_retina; j++) {

            if (j == colunas_retina) {
                fprintf(escritor, "\n");
                continue;
            }

            fprintf(escritor, "%c", retina[i][j]);
        }
    }

    fclose(escritor);
    return 0;
}





