#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

/*
    As tuplas serao compostas por micro_t em que, cada um guarda as informacoes de uma das
    coordenadas de retina e o dado presente ali - 0 ou 1.
*/
typedef struct {
    int coordenadas[2];
    unsigned valor : 1;
}micro_t;

int defineTuplas(int colunas_da_retina, int linhas_da_retina) {
    /*
        A funcao retorna quantos elementos a tupla tera.
        Cada elemento eh do tipo micro_t
    */
    int elementos_tupla;
    
    if (((colunas_da_retina * linhas_da_retina) % 3 == 0) && ((colunas_da_retina * linhas_da_retina) % 2 != 0)) {
        elementos_tupla = 3;
    }
    else if ((colunas_da_retina * linhas_da_retina) % 4 == 0) {
        elementos_tupla = 4;
    }
    else if ((colunas_da_retina * linhas_da_retina) % 5 == 0) {
        elementos_tupla = 5;
    }
    else if ((colunas_da_retina * linhas_da_retina) % 6 == 0) {
        elementos_tupla = 6;
    }
    else if ((colunas_da_retina * linhas_da_retina) % 7 == 0) {
        elementos_tupla = 7;
    }
    else {
        elementos_tupla = (linhas_da_retina < colunas_da_retina) ? linhas_da_retina: colunas_da_retina;
    }

    return elementos_tupla;
}

void criaCoordenadas(int linhas, int colunas, int coordenadas[][2]) {
    /*Cria coordenadas em ordem, depois, as desordena*/

    int sorteio;
    int temp[2];
    int total_coordenadas = linhas * colunas;

    for (int i = 0; i < linhas; i++) {
        for (int k = 0; k < colunas; k++) {
            coordenadas[i * colunas + k][0] = i;
            coordenadas[i * colunas + k][1] = k;
        }
    }

    // Iniciando a semente para o gerador de numeros aleatorios
    //time_t t;
    //srand((unsigned) time(&t));
    srand(time(NULL));

    // Desordenando as coordenadas
    for (int i = 0; i < total_coordenadas; i++) {
        sorteio = (rand() % (total_coordenadas - i)) + i;

        if (sorteio == i) {
            continue;
        }

        temp[0] = coordenadas[i][0];
        temp[1] = coordenadas[i][1];

        coordenadas[i][0] = coordenadas[sorteio][0];
        coordenadas[i][1] = coordenadas[sorteio][1];

        coordenadas[sorteio][0] = temp[0];
        coordenadas[sorteio][1] = temp[1];
    }
}

void criaTupla(int linhas, 
               int colunas, 
               int coordenadas[linhas * colunas][2], 
               micro_t tuplas[(linhas * colunas) / defineTuplas(linhas, colunas)][defineTuplas(linhas, colunas)], 
               char retina[linhas][colunas]) {
    /*
        Cria as tuplas, que serao fixas no programa.
        elementos: quantidades de micro_t em cada tupla
        quant_coordenadas: quantidade total de coordenadas na retina
        tuplas: estrutura vazia para ser criada
        retina: matriz com os valores dos pixels
    */

    int elementos = defineTuplas(linhas, colunas);
    int quant_coordenadas = linhas * colunas;

    // Embaralhando as coordenadas da tupla    
    criaCoordenadas(linhas, colunas, coordenadas);
    
    // Aqui, definimos as coordenadas e adicionamos os valores
    for (int i = 0, k = quant_coordenadas / elementos; i < k; i++) {
        for (int j = 0; j < elementos; j++) {
            tuplas[i][j].coordenadas[0] = coordenadas[i * elementos + j][0];
            tuplas[i][j].coordenadas[1] = coordenadas[i * elementos + j][1];
            tuplas[i][j].valor = (int) (retina[coordenadas[i * elementos + j][0]][coordenadas[i * elementos + j][1]] - '0'); 
        }
    }
}

void modificaTupla(int linhas, 
                   int colunas, 
                   micro_t tuplas[(linhas * colunas) / defineTuplas(linhas, colunas)][defineTuplas(linhas, colunas)], 
                   char retina[linhas][colunas]) {
    /*
        Modifica os valores nas tuplas para treinamento ou observacao.
        elementos: quantidades de micro_t em cada tupla
        quant_coordenadas: quantidade total de coordenadas na retina
        tuplas: estrutura com as coordenadas
        retina: matriz com os valores dos pixels
    */

    int quant_coordenadas = linhas * colunas;
    int elementos = defineTuplas(linhas, colunas);
    int quantidade_de_tuplas = quant_coordenadas / elementos;

    unsigned short verificador;

    // Aqui, apenas modificamos os valores
    for (int i = 0; i < quantidade_de_tuplas; i++) {
        for (int j = 0; j < elementos; j++) {
            tuplas[i][j].valor = (int) retina[tuplas[i][j].coordenadas[0]][tuplas[i][j].coordenadas[1]] - '0';
        }
    }
}



