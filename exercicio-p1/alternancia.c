/*Considere uma aplicação com $M$ threads do tipo $Foo$ e $N$ threads do tipo $Bar$.
Nessa aplicação é necessário que ocorra uma alternância rigorosa entre as execuções
das threads $Foo$ e as execuções das threads $Bar$: primeiro, todas as threads
$Foo$ devem executar, depois todas as threads $Bar$ devem executar, 
depois todas as threads $Foo$ devem executar novamente, depois todas as threads $Bar$ devem executar
novamente, e assim sucessivamente.
(a) Verifique se o código das threads $Foo$ e $Bar$ mostrado abaixo
garante que essa regra de alternância entre as execuções dos grupos de threads será cumprida.
(b) Pode ocorrer da aplicação chegar a um estado de "deadlock" (todas as threads ficarem bloqueadas)? 
*/

int passoFoo=1, passoBar=0;
int contaFoo=0, contaBar=0;
pthread_cond_t condFoo, condBar; //inicializadas na main
pthread_mutex_t mutex; //inicializado na main

void * Foo(void* args) {
  while(1) {
     pthread_mutex_lock(&mutex);
     if (!passoFoo) {
        pthread_cond_wait(&condFoo, &mutex);
     }
     pthread_mutex_unlock(&mutex);
     //codigo principal da thread
     pthread_mutex_lock(&mutex);
     contaFoo++;
     if(contaFoo==M) {
        passoFoo=0;
        passoBar=1;
        contaFoo=0;
        pthread_cond_broadcast(&condBar);
     }  else  
        pthread_cond_wait(&condFoo, &mutex);
     pthread_mutex_unlock(&mutex);
  }
}

void * Bar(void* args) {
  while(1) {
     pthread_mutex_lock(&mutex);
     if (!passoBar) {
        pthread_cond_wait(&condBar, &mutex);
     }
     pthread_mutex_unlock(&mutex);
     //codigo principal da thread
     pthread_mutex_lock(&mutex);
     contaBar++;
     if(contaBar==N) {
        passoBar=0;
        passoFoo=1;
        contaBar=0;
        pthread_cond_broadcast(&condFoo);
     } else  
        pthread_cond_wait(&condBar, &mutex);
     pthread_mutex_unlock(&mutex);
  }
}
