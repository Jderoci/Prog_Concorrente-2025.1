#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

// Variáveis globais
long long atual; //variável que estará sendo lida pelas threads
pthread_mutex_t mutex;
long long N; // valor limite
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
        // checando se o programa deve encerrar
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

    // Calculando o tempo sequencial
    struct timespec ini_seq, fim_seq;
    clock_gettime(CLOCK_MONOTONIC, &ini_seq);

    int totalSeq = 0;
    // Cálculo sequencial
    for (long long i = 1; i <= N; i++) {
        if (ehPrimo(i)) totalSeq++;
    }

    // Obtendo o tempo gasto sequencial
    clock_gettime(CLOCK_MONOTONIC, &fim_seq);
    double tempoSeq = (fim_seq.tv_sec - ini_seq.tv_sec) + (fim_seq.tv_nsec - ini_seq.tv_nsec) / 1e9;

    printf("Versao sequencial:\n");
    printf("Total de primos entre 1 e %lld: %d\n", N, totalSeq);
    printf("Tempo de execucao sequencial: %.9f segundos\n\n", tempoSeq);

    // Iniciando a parte concorrente
    tid = (pthread_t*) malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL) { puts("ERRO--malloc"); return 2; }

    pthread_mutex_init(&mutex, NULL);
    
    // Atualizando as variáveis globais, começamos os cálculos do número 1 e o total de primos inicialmente é 0
    atual = 1;
    totalPrimos = 0;

    // Inicializando o cálculo do tempo concorrente
    struct timespec inicio, fim;
    clock_gettime(CLOCK_MONOTONIC, &inicio);

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

    // Obtendo o tempo gasto concorrente
    clock_gettime(CLOCK_MONOTONIC, &fim);
    double tempoConc = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;

    pthread_mutex_destroy(&mutex);
    free(tid);

    printf("Versão Concorrente:\n");
    printf("Total de primos entre 1 e %lld: %d\n", N, totalPrimos);
    printf("Tempo de execucao concorrente: %.9f segundos\n", tempoConc);

    // Testando a corretude comparando o resultado sequencial e concorrente
    if (totalPrimos==totalSeq) puts("Corretude Confirmada!");
    else puts("Código Incorreto!");
    return 0;
}
