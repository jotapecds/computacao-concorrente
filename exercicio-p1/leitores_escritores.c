//1-Verifique se esse codigo tem condição de corrida ou não, considerando que as funções implementadas poderao ser acessadas por várias threads.
//2-Verifique se os requisitos básicos do problema dos leitores e escritores são atendidos.
//3-Verifique se alguma thread pode ficar bloqueada indefinidamente.
//4-Verifique se a implementação prioriza algum tipo de operação (leitura ou escrita)


int leit=0; //contador de threads lendo
int escr=0; //contador de threads escrevendo
pthread_mutex_t mutex;
pthread_cond_t cond_leit, cond_escr;
int querEscrever = 0;
int querLer = 0;
//entrada leitura
void InicLeit (int id) {
   querLer +=1;
   pthread_mutex_lock(&mutex);
   while(escr > 0 || leit > 0) {
     pthread_cond_wait(&cond_leit, &mutex); }
   leit++;
   querLer -=1;
   pthread_mutex_unlock(&mutex);
}
//saida leitura
void FimLeit (int id) {
   pthread_mutex_lock(&mutex);
   leit--;
   if(querEscrever > 0){
     pthread_cond_signal(&cond_escr);
   }
   else{ pthread_cond_signal(&cond_leit); }
   pthread_mutex_unlock(&mutex);
}
//entrada escrita
void InicEscr (int id) {
   querEscrever += 1;
   pthread_mutex_lock(&mutex);
   while((leit>0) || (escr>0)) {
        pthread_cond_wait(&cond_escr, &mutex); }
   escr++;
   querEscrever -=1;
   pthread_mutex_unlock(&mutex);
}
//saida escrita
void FimEscr (int id) {
   pthread_mutex_lock(&mutex);
   escr--;
   if(querLer > 0){ pthread_cond_signal(&cond_leit); }
   else { pthread_cond_signal(&cond_escr); }
   pthread_mutex_unlock(&mutex);
}
