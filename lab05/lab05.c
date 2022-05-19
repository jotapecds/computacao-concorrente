/*
 *   Sincronização condicional com variáveis de condição

 *   Exercício do laboratório 5 - Computação concorrente 22.2

 *   Aluno: João Pedro Coelho de Souza
 *   DRE: 119152051
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS 5

//-- Declaração das variáveis globais --
int contador = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;

//-- Declaração e definição das tarefas de cada thread --
void* tarefa_thread1(void* args)
{
    pthread_mutex_lock(&mutex);
    while(contador != 4) {
        pthread_cond_wait(&cond, &mutex);
    }

    printf("Volte sempre!\n");
    
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);

    pthread_exit(NULL);
}

void* tarefa_thread2(void* args) 
{
    pthread_mutex_lock(&mutex);
    while(contador < 1) {
        pthread_cond_wait(&cond, &mutex);
    }

    printf("Fique a vontade.\n");
    contador++;

    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);

    pthread_exit(NULL);
}

void* tarefa_thread3(void* args) 
{
    pthread_mutex_lock(&mutex);
    while(contador < 1) {
        pthread_cond_wait(&cond, &mutex);
    }

    printf("Sente-se por favor.\n");
    contador++;

    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);

    pthread_exit(NULL);
}

void* tarefa_thread4(void* args) 
{
    pthread_mutex_lock(&mutex);
    while(contador < 1) {
        pthread_cond_wait(&cond, &mutex);
    }

    printf("Aceita um copo d'agua?\n");
    contador++;

    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);

    pthread_exit(NULL);
}

void* tarefa_thread5(void* args) 
{
    pthread_mutex_lock(&mutex);
    while(contador != 0) {
        pthread_cond_wait(&cond, &mutex);
    }

    printf("Seja bem-vindo!\n");
    contador++;

    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(&cond);

    pthread_exit(NULL);
}

//-- Função principal --
int main() 
{
    //-- Delcaração das variáveis de controle das threads --
    void* args = malloc(sizeof(void));

    pthread_t tid[NTHREADS];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    //-- Criação das threads --
    pthread_create(&tid[0], NULL, tarefa_thread1, args);
    pthread_create(&tid[1], NULL, tarefa_thread2, args);
    pthread_create(&tid[2], NULL, tarefa_thread3, args);
    pthread_create(&tid[3], NULL, tarefa_thread4, args);
    pthread_create(&tid[4], NULL, tarefa_thread5, args);

    for(int i=0; i<NTHREADS; i++) {
        pthread_join(tid[i], NULL);
    }

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);

    return 0;
}