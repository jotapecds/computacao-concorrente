/*
 *   Multiplicação de matrizes quadradas

 *   Exercício do laboratório 2 - Computação concorrente 22.2

 *   Aluno: João Pedro Coelho de Souza
 *   DRE: 119152051
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

#define RAND_FLOAT (float)rand()/(float)(RAND_MAX/999);

float* mat1;    // Primeira matriz de entrada
float* mat2;    // Segunda matriz de entrada
float* saida;   // Matriz saída
int nthreads;   // Número de threads

typedef struct {
    int id;      // Index do elemento que a thread ira processar
    int dim;     // Dimensão das estruturas de entrada
} tArgs;

/* 
 * Realiza a multiplicação sequencial entre as matrizes mat1 e mat2 e atribui à matriz saida 
 */
float* multiplicacao_sequencial(int dim) {
    float* saida;
    saida = (float*) malloc(sizeof(float) * dim * dim);

    for(int i = 0; i < dim; i++)
        for(int j = 0; j < dim; j++) {
            float aux = 0;
            for(int k=0; k < dim; k++)
                aux += (mat1+i)[k] * (mat2+k)[j];
            (saida+i)[j] = aux;
        }

    return saida;
}

/* 
 * Realiza a multiplicação concorrente entre as matrizes mat1 e mat2 e atribui à matriz saida 
 */
void* multiplicacao_concorrente(void* arg) {
    tArgs* args = (tArgs*) arg;
    for(int i = args->id; i < args->dim; i += nthreads)
        for(int j = 0; j < args->dim; j++) {
            float aux = 0;
            for(int k=0; k < args->dim; k++)
                aux += (mat1+i)[k] * (mat2+k)[j];
            (saida+i)[j] = aux;
        }
   pthread_exit(NULL);
}

/* 
 * Compara as matrizes saida (sequencial x concorrente). 
 * Caso sejam iguais a função retorna 0. Caso contrário, retorna 1.
 */
int verifica_corretude(float* saida_sequencial, int dim) {
    for(int i = 0; i < dim; i++)
        for(int j = 0; j < dim; j++)
            if((saida_sequencial+i)[j] != (saida+i)[j]) 
                return 1;
    return 0;
}

void imprime_matrizes();

/* Fluxo principal */
int main(int argc, char* argv[]) {
    int dim;         // Dimensão da matriz de entrada
    pthread_t *tid;  // Identificadores das threads no sistema
    tArgs *args;     // Identificadores locais das threads e dimensao

    double inicio_seq, fim_seq, delta_seq, inicio_conc, fim_conc, delta_conc;   // { [inicio], [fim], [variação] }

    // Lendo e validando os dados de entrada do programa
    if(argc < 3) {
        printf("Digite: %s <dimensao da matriz> <numero de threads>\n", argv[0]);
        return 1;
    }

    dim = atoi(argv[1]);
    nthreads = atoi(argv[2]);
    if (nthreads > dim) nthreads=dim;

    /* Alocação: Alocando memória para o armazenamento das estruturas */
    mat1 = (float*) malloc(sizeof(float) * dim * dim);
    mat2 = (float*) malloc(sizeof(float) * dim * dim);
    saida = (float*) malloc(sizeof(float) * dim * dim);
    if ((mat1 == NULL) || (mat2 == NULL) || (saida == NULL)) { 
        printf("ERRO--malloc\n"); 
        return 2;
    }

    /* Carregamento: Inicializando as estruturas com valores aleatórios */
    for(int i = 0; i < dim; i++) {
        for(int j = 0; j < dim; j++){
            mat1[i*dim+j] = 1;//RAND_FLOAT;    // Significa mat1[i][j]
            mat2[i*dim+j] = 1;//RAND_FLOAT; 
            saida[i*dim+j] = 0;
        }
    }

    /* Processamento (1): Multiplicando as matrizes sequencialmente e medindo o tempo */
    GET_TIME(inicio_seq);
    float* saida_sequencial = multiplicacao_sequencial(dim);
    GET_TIME(fim_seq);
    delta_seq = fim_seq - inicio_seq;


    /* Processamento (2): Multiplicando as matrizes concorrentemente e medindo o tempo */
    GET_TIME(inicio_conc);
    // Alocação das estruturas
    tid = (pthread_t*) malloc(sizeof(pthread_t) * nthreads);
    args = (tArgs*) malloc(sizeof(tArgs) * nthreads);
    if((args==NULL) || (tid==NULL)) { puts("ERRO--malloc"); return 2; }

    // Criando as threads
    for(int i = 0; i < nthreads; i++) {
        (args+i)->id = i;
        (args+i)->dim = dim;
        if(pthread_create(tid+i, NULL, multiplicacao_concorrente, (void*) (args+i))){
            puts("ERRO--pthread_create"); return 3;
        }
    }

    // Aguardando a execução de todas as threads
    for(int i = 0; i < nthreads; i++) {
        pthread_join(*(tid+i), NULL);
    }

    GET_TIME(fim_conc);   
    delta_conc = fim_conc - inicio_conc;

    //imprime_matrizes(multiplicacao_sequencial(dim), dim);

    if(!verifica_corretude(saida_sequencial, dim))
        puts("\n > boa time!! <");
    else puts("\n < deu ruim fml >");

    // Comparando o tempo de execução dos dois métodos
    printf("\nTempo sequencial: %.4f\n", delta_seq);
    printf("Tempo concorrente: %.4f\n", delta_conc);

    if(delta_seq > delta_conc) {
        double ganho = delta_seq / delta_conc;
        printf("Ganho obtido: %.4f", ganho);
    }
    else printf("Sem ganhos.");

    

    /* Liberação: Liberando as variáveis usadas da memória */
    free(mat1);
    free(mat2);
    free(saida);
    free(args);
    free(tid);
    return 0;
}

/* Exibição: Imprimindo os resultados para o usuário */
void imprime_matrizes(float* vet, int dim) {
    puts("Matriz de entrada 1:");
        for(int i = 0; i < dim; i++){
            for(int j = 0; j < dim; j++)
                printf("%.1f ", (mat1+i)[j]);
            puts("");
        }
    puts("\nMatriz de entrada 2:");
    for(int i = 0; i < dim; i++){
        for(int j = 0; j < dim; j++)
            printf("%.1f ", (mat2+i)[j]);
        puts("");
    }
    puts("\nMatriz de saida sequencial:");
    for(int i = 0; i < dim; i++){
        for(int j = 0; j < dim; j++)
            printf("%.1f ", (vet+i)[j]);
        puts("");
    }
    puts("\nMatriz de saida concorrente:");
    for(int i = 0; i < dim; i++){
        for(int j = 0; j < dim; j++)
            printf("%.1f ", (saida+i)[j]);
        puts("");
    }
}