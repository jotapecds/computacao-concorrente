/*
 *   Elevar ao quadrado os elementos de um vetor

 *   Exercício do laboratório 1 - Computação concorrente 22.2

 *   Aluno: João Pedro Coelho de Souza
 *   DRE: 119152051
 */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define NTHREADS 2  // Número de threads a serem criadas
#define DIM 20      // Dimensão do vetor

float* vetor;

typedef struct {
    int tid_local;
    int ini;
    int fim;
} tArgs;

void* eleva_ao_quadrado(void* args);
void print_vetor();

int main(void) {
    pthread_t* tid_sistema;
    tArgs* args;

    // Alocando as estruturas na memória
    tid_sistema = (pthread_t*) malloc(sizeof(pthread_t) * NTHREADS);
    args = (tArgs*) malloc(sizeof(tArgs));
    vetor = (float*) malloc(sizeof(float) * DIM);
    if ((tid_sistema == NULL) ||(args == NULL) || (vetor == NULL)) {
        printf("--ERRO: malloc()\n"); 
        exit(-1);
    }

    // Preenchendo o vetor principal
    for (int i = 0; i < DIM; i++) {
        *(vetor+i) = i;
    }

    for(int i_thread = 0; i_thread < NTHREADS; i_thread++) {
        // Preparando os argumentos
        args->tid_local = i_thread;
        if(i_thread == 0) { // Caso seja o primeiro loop
            args->ini = 0;
            args->fim = (DIM / NTHREADS) - 1;
        }
        else {
            args->ini = args->fim + 1;
            args->fim = DIM;
        }

        printf("\nInicio: %d",args->ini);
        printf("\nFim: %d",args->fim);

        if(pthread_create((tid_sistema+i_thread), NULL, eleva_ao_quadrado, (void*) (args+i_thread))) {
            printf("--ERRO: pthread_create()\n");
            exit(-1);
        }
    }

    // Esperando todas as threads terminarem
    for (int i_thread = 0; i_thread < NTHREADS; i_thread++) {
        if (pthread_join(tid_sistema[i_thread], NULL)) {
            printf("--ERRO: pthread_join() \n"); exit(-1); 
        } 
    }

    print_vetor();

    return 0;
}

void* eleva_ao_quadrado(void* args) {
    tArgs* vet = (tArgs*) args;
    printf("\n> thread %d <\n",vet->tid_local);
    for(int i = vet->ini; i < vet->fim; i++) {
        *(vetor+i) = 1;
        printf("\n Vetor[%d] => %f", i, *(vetor+i));
    }
    pthread_exit(NULL);
}

void print_vetor() {
    for(int i = 0; i < DIM; i++) {
        printf("%f\n",*(vetor+i));
    }
}