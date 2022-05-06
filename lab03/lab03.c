/*
 *   Encontrando valores Max e Min dentro de um vetor

 *   Exercício do laboratório 3 - Computação concorrente 22.2

 *   Aluno: João Pedro Coelho de Souza
 *   DRE: 119152051
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

#define RAND_FLOAT (float)rand()/(float)(RAND_MAX/999)
#define INT_LONGO long long int //  inteiro com modificador longo

typedef struct {
    float min;
    float max;
} t_max_min; // Estrutura para armazenar os valores max e min de um intervalo

//--Variaveis globais--
float* vet_floats; //Ponteiro para o vetor de floats
INT_LONGO tam_vet; //Tamanho do vetor de floats

t_max_min* vet_threads; //Ponteiro para o vetor que armazenará o retorno de cada thread
int nthreads; //Números de treads

// inicializando vetor 
void inicializa_vet_floats () {
    for (int i=0; i< tam_vet; i++) 
        *(vet_floats+i) = RAND_FLOAT;
} 

// imprimindo vetor 
void imprime_vet () {
    for (int i=0; i< tam_vet; i++) 
        printf("[%f]\n", *(vet_floats+i));
} 

// imprimindo max e min
void imprime_max_min (t_max_min max_min) {
    printf("Minimo: [%f]", max_min.min);
    printf("\nMaximo: [%f]\n", max_min.max);
}

// Encontrando max e min sequencialmente
t_max_min encontra_max_min_sequencialmente () {
    t_max_min ret; // declaracao da estrutura  que tera os valores max e min
    ret.min = ret.max = *vet_floats; // inicializando o min e o max com o conteudo do primeiro indice do vetor

    // varrendo o vetor de floats a partir do segundo indice
    for ( int i=1; i < tam_vet; i ++) {
        if (*(vet_floats+i) < ret.min) 
            ret.min = *(vet_floats+i); // se o conteudo do vetor na posicao i for menor que o minimo, atualiza o valor de min
        else if (*(vet_floats+i) > ret.max) 
            ret.max = *(vet_floats+i); // se o conteudo do vetor na posicao i for maior que o maximo, atualiza o valor de max
    }

    return ret;
}

// Encontrando max e min concorrentemente
void* encontra_max_min_concorrentemente (void* arg) {
    int tid_local = *(int*) arg; // convertendo os argumentos recebidos por parametro

    // Inicializando os valores max e min da thread em questão
    (vet_threads+tid_local)->min = *vet_floats;
    (vet_threads+tid_local)->max = *vet_floats;

    for ( int i=tid_local; i < tam_vet; i+=nthreads) {
        if (*(vet_floats+i) < (vet_threads+tid_local)->min)
            (vet_threads+tid_local)->min = *(vet_floats+i);
        else if (*(vet_floats+i) > (vet_threads+tid_local)->max)
            (vet_threads+tid_local)->max = *(vet_floats+i);
    }
    pthread_exit(NULL);
}

// Recupera os valores max e min finais a partir do retorno da execucao de cada thread
t_max_min pega_max_min_final () {
    t_max_min max_min_final;
    max_min_final.min = vet_threads->min;
    max_min_final.max = vet_threads->max;
    for(int i=1; i<nthreads; i++){
        if(vet_threads[i].min < max_min_final.min)
            max_min_final.min = vet_threads[i].min;
        else if(vet_threads[i].max > max_min_final.max)
            max_min_final.max = vet_threads[i].max;
    }
    return max_min_final;
}

// Comparando os valores de max e min encontrados de forma sequencial e concorrente
int testa_corretude (t_max_min max_min_seq, t_max_min max_min_conc) {
    if((max_min_seq.min == max_min_conc.min) && (max_min_seq.max == max_min_conc.max))
        return 0;

    else return 1;
}

// Calcula e imprime na tela a aceleracao com base no tempo sequencial e no tempo concorrente
void calcula_aceleracao (double delta_seq, double delta_conc, int deu_errado) {
    double aceleracao = delta_seq/delta_conc;
    if(!deu_errado && (aceleracao > 0)){
        printf("\n<=======================================>\n");
        printf("As resolucoes são equivalentes!!\n");
        printf("O tempo de execucao sequencial foi de %e\n", delta_seq);
        printf("O tempo de execucao concorrente foi de %lf\n\n", delta_conc);
        printf("A aceleracao obtida pela execucao concorrente em cima da sequencial foi de %lf segundos", aceleracao);
        printf("\n<=======================================>\n");
    }
    else if (!deu_errado && (aceleracao < 0)){
        printf("\n<=======================================>\n");
        printf("As resolucoes são equivalentes!!\n");
        printf("O tempo de execucao sequencial foi de %lf\n", delta_seq);
        printf("O tempo de execucao concorrente foi de %lf\n\n", delta_conc);
        printf("A aceleracao obtida foi negativa. A execucao sequencial foi mais eficiente");
        printf("\n<=======================================>\n");
    }
    else
        printf("\n>QUEN QUEN QUEN QUEN QUENNNNNN <\n");
}

int main (int argc, char *argv[]) {
    //--Lendo e validando os dados de entrada do programa--
    if(argc < 3) {
        printf("Digite: %s <Tamanho do vetor> <Numero de threads>\n", argv[0]);
        return 1;
    }
    
    tam_vet = atoll(argv[1]);
    nthreads = atoi(argv[2]);

    //--Declarando, alocando e inicializando variaveis--
    pthread_t *tid_sistema; // Identificadores das threads no sistema
    int tid_local[nthreads]; // Identificadores locais das threads 

    vet_floats = (float*) malloc (sizeof(float)*tam_vet); // alocando memoria para o vetor de floats
    vet_threads = (t_max_min*) malloc (sizeof(t_max_min)*nthreads); // alocando memoria para o vetor de retorno das threads
    tid_sistema = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
    if((vet_floats==NULL) || (vet_threads==NULL) || (tid_sistema==NULL)) { 
        puts("ERRO--malloc"); return 2; 
    }

    double inicio, fim, delta_seq, delta_conc;
    inicio = fim = delta_seq = delta_conc = 0;

    inicializa_vet_floats(); 

    //--Encontrando o max e o min sequencialmente e medindo o tempo--
    GET_TIME(inicio);
    t_max_min max_min_seq = encontra_max_min_sequencialmente();
    GET_TIME(fim);
    delta_seq = fim - inicio;

    //--Encontrando o max e o min concorrentemente medindo o tempo--
    GET_TIME(inicio);
    for(int i=0; i<nthreads; i++){
        tid_local[i] = i;
        if(pthread_create(tid_sistema+i, NULL, encontra_max_min_concorrentemente, (void*) &tid_local[i])){
            puts("ERRO--pthread_create"); return 3;
        }
    }
    // Aguardando a execução de todas as threads
    for(int i=0; i<nthreads; i++){
        pthread_join(*(tid_sistema+i), NULL);
    }
    
    t_max_min max_min_conc = pega_max_min_final();
    GET_TIME(fim); 
    delta_conc = fim - inicio;

    calcula_aceleracao(delta_seq, delta_conc, testa_corretude(max_min_seq, max_min_conc));
    return 0;
}

