/*
 *   Controle da ordem de execução de threads com o uso de semáforos

 *   Exercício do laboratório 9 - Computação concorrente 22.2

 *   Aluno: João Pedro Coelho de Souza
 *   DRE: 119152051
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>

// Variáveis globais
sem_t semaforo1, semaforo2;
int count = 0;

// Assinatura das threads
void* thread1(void* args);
void* thread2(void* args);
void* thread3(void* args);
void* thread4(void* args);
void* thread5(void* args);

int main (void) {
    pthread_t threads[5]; // Array de threads

    // Inicializando semáforos
    sem_init(&semaforo1, 0, 0);
    sem_init(&semaforo2, 0, 0);
 
    // Criando as threads em ordem aleatória
    pthread_create(&threads[0], NULL, thread3, malloc(sizeof(void)));
    pthread_create(&threads[1], NULL, thread1, malloc(sizeof(void)));
    pthread_create(&threads[2], NULL, thread5, malloc(sizeof(void)));
    pthread_create(&threads[3], NULL, thread2, malloc(sizeof(void)));
    pthread_create(&threads[4], NULL, thread4, malloc(sizeof(void)));

    // Aguardando o retorno das threads
    for(int i=0; i<5; i++)
        pthread_join(threads[i], NULL);

    return 0;
}

void* thread1(void* args) {

    sem_wait(&semaforo2); // Aguarda até que as as threads 2, 3 e 4 tenham sido executadas
    printf("Volte sempre!.\n");
    pthread_exit(NULL);
}

void* thread2(void* args) {

    sem_wait(&semaforo1); // Espera pela execução da thread 5 ou 4 ou 3
    printf("Fique a vontade.\n");
    sem_post(&semaforo1);

    if(++count == 3) sem_post(&semaforo2);
    pthread_exit(NULL);
}

void* thread3(void* args) {

    sem_wait(&semaforo1); // Espera pela execução da thread 5 ou 4 ou 2
    printf("Sente-se por favor.\n");
    sem_post(&semaforo1);

    if(++count == 3) sem_post(&semaforo2);
    pthread_exit(NULL);
}

void* thread4(void* args) {

    sem_wait(&semaforo1);  // Espera pela execução da thread 5 ou 3 ou 2
    printf("Aceita um copo d'agua?\n");
    sem_post(&semaforo1);

    if(++count == 3) sem_post(&semaforo2);
    pthread_exit(NULL);
}

void* thread5(void* args) {

    // Não precisa aguardar nenhuma thread.
    printf("Seja bem-vindo!\n");
    sem_post(&semaforo1); // Libera a execução das demais.
    pthread_exit(NULL);
}