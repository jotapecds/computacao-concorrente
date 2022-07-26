/*
 *   Exclusão múltua e sincronização condicional com semáforos

 *   Exercício do laboratório 10 - Computação concorrente 22.2

 *   Aluno: João Pedro Coelho de Souza
 *   DRE: 119152051
 */

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_INT 2147483647 // Maior inteiro possível

// Declaração de variáveis globais para controle do programa
sem_t em_e, em_l, escr, leit;
int e = 0, l = 0;

// Denifição da função de threads escritoras
void* escritor(void* arg) {
    int tid = *(int*)arg;

    while(1) {
        sem_wait(&em_e);
        e++;
        if(e == 1) sem_wait(&leit);
        sem_post(&em_e);
        sem_wait(&escr);

        // Processamento qualquer para gastar tempo
        printf("Thread_escritora[%d]: Escrevendo.\n", tid);
        for(int i=0, valor=0; i<MAX_INT/100; i++) valor++;

        sem_post(&escr);
        sem_wait(&em_e);
        e--;
        if(e == 0) sem_post(&leit);
        sem_post(&em_e);
        sleep(1);
    }
}

// Denifição da função de threads leitoras
void* leitor(void* arg) {
    int tid = *(int*)arg;

    while(1) {
        sem_wait(&leit);
        sem_wait(&em_l);
        l++;
        if(l == 1) sem_wait(&escr);
        sem_post(&em_l);
        sem_post(&leit);

        // Processamento qualquer para gastar tempo
        printf("Thread_leitora[%d]: Lendo.\n", tid);
        for(int i=0, valor=0; i<MAX_INT; i++) valor++;

        sem_wait(&em_l);
        l--;
        if(l == 0) sem_post(&escr);
        sem_post(&em_l);
    }
}

int main (void) {
    int num_escritoras = 5;
    int num_leitoras = 5;

    pthread_t threads_escritoras[num_escritoras];
    pthread_t threads_leitoras[num_leitoras];
    int tid_esc[num_escritoras];
    int tid_leit[num_leitoras];

    // Inicializando semáforos binários
    sem_init(&leit, 0, 1);
    sem_init(&em_l, 0, 1);

    sem_init(&escr, 0, 1);
    sem_init(&em_e, 0, 1);

    // Inicializando threads
    for (int i=0, j=0; (i<num_escritoras)||(j<num_leitoras); i++, j++) {
        if(i < num_escritoras){
            printf("Thread_escritora[%d]: Criada.\n", i);
            tid_esc[i] = i+1;
            pthread_create(&threads_escritoras[i], NULL, escritor, (void*) &tid_esc[i]);
        }
        if(j < num_leitoras){
            printf("Thread_leitora[%d]: Criada.\n", j);
            tid_leit[j] = j+1;
            pthread_create(&threads_leitoras[j], NULL, leitor, (void*) &tid_leit[i]);
        }

    }

    // Esperando a execução de todas as threads
    for (int i=0, j=0; (i<num_escritoras)||(j<num_leitoras); i++, j++) {
        if(i < num_escritoras) pthread_join(threads_escritoras[i], NULL);
        if(j < num_leitoras) pthread_join(threads_leitoras[j], NULL);
    }

    return 0;
}