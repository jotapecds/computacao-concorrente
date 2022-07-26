/*
 *   Leitores e escritores usando monitores em Java
 *   Exercício do laboratório 8 - Computação concorrente 22.2
 *   Aluno: João Pedro Coelho de Souza
 *   DRE: 119152051
 */

// Monitor que implementa a logica do padrao leitores/escritores
class LE {
    private int leit, escr;

    // Construtor
    LE() {
        this.leit = 0; //leitores lendo (0 ou mais)
        this.escr = 0; //escritor escrevendo (0 ou 1)
    }

    // Entrada para leitores
    public synchronized void EntraLeitor (int id) {
        try {
            while (this.escr > 0) {
                //if (this.escr > 0) {
                System.out.println ("le.leitorBloqueado("+id+")");
                wait();  //bloqueia pela condicao logica da aplicacao
            }
            this.leit++;  //registra que ha mais um leitor lendo
            System.out.println ("le.leitorLendo("+id+")");
        } catch (InterruptedException e) { }
    }

    // Saida para leitores
    public synchronized void SaiLeitor (int id) {
        this.leit--; //registra que um leitor saiu
        if (this.leit == 0)
            this.notify(); //libera escritor (caso exista escritor bloqueado)
        System.out.println ("le.leitorSaindo("+id+")");
    }

    // Entrada para escritores
    public synchronized void EntraEscritor (int id) {
        try {
            while ((this.leit > 0) || (this.escr > 0)) {
                //if ((this.leit > 0) || (this.escr > 0)) {
                System.out.println ("le.escritorBloqueado("+id+")");
                wait();  //bloqueia pela condicao logica da aplicacao
            }
            this.escr++; //registra que ha um escritor escrevendo
            System.out.println ("le.escritorEscrevendo("+id+")");
        } catch (InterruptedException e) { }
    }

    // Saida para escritores
    public synchronized void SaiEscritor (int id) {
        this.escr--; //registra que o escritor saiu
        notifyAll(); //libera leitores e escritores (caso existam leitores ou escritores bloqueados)
        System.out.println ("le.escritorSaindo("+id+")");
    }
}

class Recurso {
    private int valor;

    Recurso() { this.valor = 0; }
    int get() { return this.valor; }
    void set(int novo_valor) { this.valor = novo_valor; }
    void incrementa() { this.valor++; }
}

// Thread tipo 1: Apenas Escrita
class T1 extends Thread {
    int tid;            //identificador da thread
    Recurso recurso;    //recurso compatilhado entre as threads
    LE monitor;         //objeto monitor para coordenar a lógica de execução das threads

    // Construtor
    T1 (int id, Recurso rec, LE m) {
        this.tid = id;
        this.recurso = rec;
        this.monitor = m;
    }

    // Método executado pela thread
    public void run () {
        //System.out.println(">> Entrou na T1["+tid+"]: Recurso = "+recurso.get());
        this.monitor.EntraEscritor(this.tid);
        {
            recurso.incrementa();
            //System.out.println(">> Saiu da t1["+tid+"]: Recruso = "+recurso.get());
        }
        this.monitor.SaiEscritor(this.tid);
    }
}

// Thread tipo 2: Apenas Leitura
class T2 extends Thread {
    int tid;            //identificador da thread
    Recurso recurso;    //recurso compatilhado entre as threads
    LE monitor;         //objeto monitor para coordenar a lógica de execução das threads

    // Construtor
    T2 (int id, Recurso rec, LE m) {
        this.tid = id;
        this.recurso = rec;
        this.monitor = m;
    }

    // Método executado pela thread
    public void run () {
        //System.out.println(">> Entrou na T2["+tid+"]: Recurso = "+recurso.get());
        this.monitor.EntraLeitor(this.tid);
        {
            int valor = recurso.get();
            if (valor % 2 == 0)
                System.out.println("Valor do recurso: " + valor + ". EH PAR!");
            else
                System.out.println("Valor do recurso: " + valor + ". EH IMPAR!");
        }
        this.monitor.SaiLeitor(this.tid);
    }
}

// Thread tipo 3: Leitura e Escrita
class T3 extends Thread {
    int tid;            //identificador da thread
    Recurso recurso;    //recurso compatilhado entre as threads
    LE monitor;         //objeto monitor para coordenar a lógica de execução das threads

    // Construtor
    T3 (int id, Recurso rec, LE m) {
        this.tid = id;
        this.recurso = rec;
        this.monitor = m;
    }

    // Método executado pela thread
    public void run () {
        //System.out.println(">> Entrou na T3["+tid+"]: Recurso = "+recurso.get());
        this.monitor.EntraLeitor(this.tid);
        {
            int valor = recurso.get();
            System.out.println("T3["+tid+"]: Recurso = "+valor);

            while (valor < Integer.MAX_VALUE) {
                ++valor;
            }
        }
        this.monitor.SaiLeitor(this.tid);

        this.monitor.EntraEscritor(this.tid);
        {
            recurso.set(tid);
            //System.out.println(">> Saiu da t3["+tid+"]: Recruso = "+recurso.get());

        }
        this.monitor.SaiEscritor(this.tid);
    }
}

//--------------------------------------------------------
// Classe principal
class Main {
    static final int NUM_T1 = 5;
    static final int NUM_T2 = 3;
    static final int NUM_T3 = 4;

    public static void main (String[] args) {
        Recurso recurso = new Recurso();            // Recurso compartilhado entre as threads
        LE monitor = new LE();                      // Monitor (objeto compartilhado entre leitores e escritores)
        T1[] escritoras = new T1[NUM_T1];           // Threads leitores
        T2[] leitoras = new T2[NUM_T2];             // Threads escritores
        T3[] leitoras_escritoras = new T3[NUM_T3];  // Threads escritores

        for (int i=0, j=0, k=0; (i<NUM_T1)||(j<NUM_T2)||(k<NUM_T3); i++, j++, k++) {
            if(i<NUM_T1){
                escritoras[i] = new T1(i+1, recurso, monitor);
                escritoras[i].start();
            }
            if(j<NUM_T2){
                leitoras[j] = new T2(j+1, recurso, monitor);
                leitoras[j].start();
            }
            if(k<NUM_T3){
                leitoras_escritoras[k] = new T3(k+1, recurso, monitor);
                leitoras_escritoras[k].start();
            }
        }
    }
}
