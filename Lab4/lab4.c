#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

// Variáveis globais
long long atual; //variável que estará sendo lida pelas threads
pthread_mutex_t mutex;
long long N;
int totalPrimos; 

// Função de cálculo de primalidade
int ehPrimo(long long n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (long long i = 3; i <= sqrt(n); i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

// Função executada pelas threads
void* ExecutaTarefa(void* arg) {
    long int id = (long int)arg;
    int contLocal = 0;

    while (1) {
        long long num;

        pthread_mutex_lock(&mutex);
        // Checando se o programa deve encerrar
        if (atual > N) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        // Se não precisar encerrar, incrementa a variável global e libera o lock
        num = atual++;
        pthread_mutex_unlock(&mutex);
        // O valor é incrementado em uma variável local, para evitar o uso de locks garantindo maior fluidez e também para posterior análise da execução de cada thread
        if (ehPrimo(num)) contLocal++;
    }

    // Ao fim de cálculo, é realizado o incremento da variável global com o lock para obter o total de primos
    pthread_mutex_lock(&mutex);
    totalPrimos += contLocal;
    pthread_mutex_unlock(&mutex);

    // Avaliação do desempenho individual da thread
    printf("Thread %ld terminou. Primos encontrados: %d\n", id, contLocal);
    pthread_exit(NULL);
}

// Fluxo principal
int main(int argc, char* argv[]) {
    pthread_t *tid;
    int nthreads;

    if (argc < 3) {
        printf("Digite: %s <N> <numero de threads>\n", argv[0]);
        return 1;
    }

    N = atoll(argv[1]);
    nthreads = atoi(argv[2]);

    // Fiz a parte sequencial para testar a corretude da versão concorrente

    // Quantidade de primos obtidos na versão sequencial
    int totalSeq = 0;
    // Cálculo Sequencial
    for (long long i = 1; i <= N; i++) {
        if (ehPrimo(i)) totalSeq++;
    }

    printf("Versao Sequencial:\n");
    printf("Total de primos entre 1 e %lld: %d\n\n", N, totalSeq);

    // Iniciando a parte concorrente
    tid = (pthread_t*) malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL) { puts("ERRO--malloc"); return 2; }

    pthread_mutex_init(&mutex, NULL);
    
    // Atualizando as variáveis globais, começamos os cálculos do número 1 e o total de primos inicialmente é 0
    atual = 1;
    totalPrimos = 0;

    // Cálculo do tempo concorrente
    double start, finished, elapsed;
    GET_TIME(start);

    for (long int t = 0; t < nthreads; t++) {
        if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void*)t)) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }

    for (int t = 0; t < nthreads; t++) {
        if (pthread_join(tid[t], NULL)) {
            printf("--ERRO: pthread_join()\n"); exit(-1);
        }
    }

    GET_TIME(finished)
    elapsed = finished - start;

    pthread_mutex_destroy(&mutex);
    free(tid);

    printf("\nVersao concorrente:\n");
    printf("Total de primos entre 1 e %lld: %d\n", N, totalPrimos);
    printf("Tempo de execucao concorrente: %.9f segs\n", elapsed);;

    // Testando a corretude comparando o resultado sequencial e concorrente
    if (totalPrimos==totalSeq) puts("Corretude Confirmada!");
    else puts("Código incorreto!");
    return 0;
}
