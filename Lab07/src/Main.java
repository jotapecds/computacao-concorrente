/*
 *   Encontrar quantidade de números pares em um vetor
 *   Exercício do laboratório 7 - Computação concorrente 22.2
 *   Aluno: João Pedro Coelho de Souza
 *   DRE: 119152051
 */

import java.util.ArrayList;
import java.util.Random;

// Contador de pares concorrente
class ContadorDePares extends Thread
{
    private int count;
    private final ArrayList<Integer> vet;
    private final int ini;
    private final int fim;

    public ContadorDePares(ArrayList<Integer> vet, int ini, int fim)
    {
        this.count = 0;
        this.vet = vet;
        this.ini = ini;
        this.fim = fim;
    }

    public int getCount() { return this.count; }

    public void run()
    {
        for(int i = this.ini; i < this.fim; i++)
        {
            if(this.vet.get(i) % 2 == 0)
                this.count++;
        }
    }
}

public class Main {

    // Contador de pares sequencial
    private static int contaPares(ArrayList<Integer> vet) {
        int count = 0;

        for(var inteiro : vet)
        {
            if(inteiro % 2 == 0)
                count++;
        }
        return count;
    }

    // Classe principal
    public static void main(String[] args) throws InterruptedException {

        // Declarando variaveis
        final var MAXIMO = 99999;
        final var TAM = 100000000;
        final var NTHREADS = 4;
        final var TAM_BLOCO = TAM / NTHREADS;

        var vet = new ArrayList<Integer>(TAM);
        var threads = new ArrayList<ContadorDePares>(NTHREADS);
        var rand = new Random();

        // Preenchendo o vetor de entrada
        for(int i = 0; i < TAM; i++)
        {
            vet.add(rand.nextInt(MAXIMO));
        }

        // Criando e rodando as threads
        for(int i = 0; i < NTHREADS; i++)
        {
            int ini = i * TAM_BLOCO;
            int fim = (i == NTHREADS-1 ? TAM : ini + TAM_BLOCO);

            threads.add(new ContadorDePares(vet, ini, fim));
            threads.get(i).start();
        }

        // Tratando o retorno das threads
        int contadorConcorrente = 0;
        for(var thread : threads) {
            thread.join();
            contadorConcorrente += thread.getCount();
        }

        // Contando sequencialmente
        int contadorSenquencial = contaPares(vet);

        // Teste final
        if(contadorConcorrente != contadorSenquencial)
        {
            System.out.println("Quantidade de pares encontrados (Com threads): " + contadorConcorrente);
            System.out.println("Quantidade de pares encontrados (Sem threads): " + contadorSenquencial);
            System.out.println(">> ERRO! <<");
        }
        else
        {
            System.out.println("Quantidade de pares encontrados: " + contadorConcorrente);
            System.out.println("Tudo certo!");
        }

    }
}
