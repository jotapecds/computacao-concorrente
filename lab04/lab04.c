/*
 *   Encontrando valores Max e Min dentro de um vetor
 *   Exercício do laboratório 4 - Computação concorrente 22.2
 *   Aluno: João Pedro Coelho de Souza
 *   DRE: 119152051
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

#define LLINT long long int
#define RAND_INT (int)rand()/(RAND_MAX/9999) 

int *vet_entrada;       // Vetor com os dados de entrada
float *vet_saida;       // Vetor com o resultado da execucao concorrente
LLINT DIM;              // Dimensao dos vetores
int NTHREADS;           // Numero de threads

// Inicializa vetor de entrada com inteiros aleatorios
void  incializa_vetor ()
{
    for (int i=0; i<DIM; i ++){
        *(vet_entrada +i) = RAND_INT;
    }
}

// Descobre se o numero recebido eh primo
int eh_primo(int num) 
{
    int resultado = 0;
    
    for (int i = 2; i <= num / 2; i++) {
        if (num % i == 0) {
            resultado++;
        break;
        }
    }
    
    if (resultado == 0)
        return 1;
    else
        return 0;
}

// processa primos (sequencial)
void processa_primos(int vet_entrada[], float vet_saida[]) 
{
    for(int i=0; i<DIM; i++) 
    {
        if (eh_primo(vet_entrada[i]))
            vet_saida[i] = sqrt(vet_entrada[i]);
        else
             vet_saida[i] = vet_entrada[i];
    }
}

// processa primos (concorrente)
void* processa_primos_concorrentemente (void* arg) 
{
    int tid = *(int*) arg;

    for ( int i=tid; i < DIM; i+=NTHREADS) {
        if (eh_primo(vet_entrada[i]))
            vet_saida[i] = sqrt(vet_entrada[i]);
        else
             vet_saida[i] = vet_entrada[i];
    }
    pthread_exit(NULL);
}

// Verifica se as saídas das execucoes sequencial e concorrente sao consistentes
int verifica_corretude (float vet_saida_seq[])
{
    for(int i=0; i<DIM; i++) 
        if (vet_saida[i] != vet_saida_seq[i])
            return 0;

    return 1;
}

// *funcao auxiliar*
void imprime_vets (float v_saida[])
{
    printf("\nEntrada         saida\n");
    for (int i=0; i<DIM; i++){
        printf("|%d|          |%f|\n", *(vet_entrada+i), *(v_saida+i));
    }
}

int main (int argc, char *argv []) {
     //--Lendo e validando os dados de entrada do programa--
    if(argc < 3) {
        printf("Digite: %s <Tamanho do vetor> <Numero de threads>\n", argv[0]);
        return 1;
    }

    //--Declarando, alocando e inicializando variaveis--
    DIM = atoll(argv[1]);       // recebendo a dimensao escolhida pelo usuario
    NTHREADS = atoi(argv[2]);   // recebendo a quantidade de threads requisitada pelo usuario

    pthread_t *tid_sistema;     // Identificadores das threads no sistema
    int tid_local[NTHREADS];    // Identificadores locais das threads 
    float *vet_saida_seq;       // Vetor com o resultado da execucao sequencial

    vet_entrada = (int*) malloc (sizeof(int)*DIM);
    vet_saida = (float*) malloc (sizeof(float)*DIM);
    vet_saida_seq = (float*) malloc (sizeof(float)*DIM);
    tid_sistema = (pthread_t*) malloc(sizeof(pthread_t)*NTHREADS);
    if((vet_entrada==NULL) || (vet_saida==NULL) || (vet_saida_seq==NULL) || (tid_sistema==NULL)) { 
        puts("ERRO--malloc"); return 2; 
    }

    double inicio, fim, delta_seq, delta_conc = 0;

    //--Executando a tarefa sequencialmente--
    incializa_vetor ();
    GET_TIME(inicio);
    processa_primos (vet_entrada, vet_saida_seq);
    GET_TIME(fim);
    delta_seq = fim - inicio;
    printf("Tempo de execucao sequencial: %lf\n", delta_seq);
    // imprimir_vet();

    //--Executando a tarefa de forma concorrente--
    GET_TIME(inicio);
    for(int i=0; i<NTHREADS; i++){
        tid_local[i] = i;
        if(pthread_create(tid_sistema+i, NULL, processa_primos_concorrentemente, (void*) &tid_local[i])){
            puts("ERRO--pthread_create"); return 3;
        }
    }
    // Aguardando a execução de todas as threads
    for(int i=0; i<NTHREADS; i++){
        pthread_join(*(tid_sistema+i), NULL);
    }
    GET_TIME(fim);
    delta_conc = fim - inicio;
    printf("Tempo de execucao concorrente: %lf\n", delta_conc);  
    // imprime_vets(vet_saida);

    //--Conferindo se as tarefas fizeram seu trabalho adequadamente--
    if(verifica_corretude(vet_saida_seq) == 0)
        printf("Algo de errado não está certo :/\n");
    else
        printf("Tudo OK!!\n");
    
    free(vet_entrada);
    free(vet_saida);
    free(vet_saida_seq);
    free(tid_sistema);
}



