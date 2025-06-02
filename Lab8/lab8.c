#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

// Parâmetros de entrada
int N, M, C;

// Buffer circular e controle de posição
int *buffer;
int in = 0, out = 0;

// Contadores
int produziu = 0;
int consumiu = 0;

// Semáforos e mutex
sem_t vazio, cheio;
pthread_mutex_t mutexBuffer;
pthread_mutex_t mutexContador;
pthread_mutex_t mutexPrimos;

// Contador global de primos e por thread
int primosTotal = 0;
int *primosPorThread;

// Função para verificar se número é primo
int ehPrimo(long long int n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (int i = 3; i <= sqrt(n); i += 2)
        if (n % i == 0) return 0;
    return 1;
}

// Função da thread produtora
void* produtor(void* arg) {
    for (int i = 1; i <= N; i++) {
        sem_wait(&vazio);
        pthread_mutex_lock(&mutexBuffer);

        buffer[in] = i;
        in = (in + 1) % M;

        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&cheio);
    }
    return NULL;
}

// Função da thread consumidora
void* consumidor(void* arg) {
    int id = *(int*)arg;
    while (1) {
        pthread_mutex_lock(&mutexContador);
        if (consumiu >= N) {
            pthread_mutex_unlock(&mutexContador);
            break;
        }
        consumiu++;
        pthread_mutex_unlock(&mutexContador);

        sem_wait(&cheio);
        pthread_mutex_lock(&mutexBuffer);

        int valor = buffer[out];
        out = (out + 1) % M;

        pthread_mutex_unlock(&mutexBuffer);
        sem_post(&vazio);

        if (ehPrimo(valor)) {
            pthread_mutex_lock(&mutexPrimos);
            primosTotal++;
            primosPorThread[id]++;
            pthread_mutex_unlock(&mutexPrimos);
        }
    }
    return NULL;
}

int main() {
    // Leitura dos parâmetros
    printf("Digite N (quantos números produzir): ");
    scanf("%d", &N);
    printf("Digite M (tamanho do buffer): ");
    scanf("%d", &M);
    printf("Digite C (número de threads consumidoras): ");
    scanf("%d", &C);

    // Alocações
    buffer = malloc(sizeof(int) * M);
    primosPorThread = calloc(C, sizeof(int));
    pthread_t produtoras, *consumidoras = malloc(sizeof(pthread_t) * C);
    int *ids = malloc(sizeof(int) * C);

    // Inicializações
    sem_init(&vazio, 0, M);
    sem_init(&cheio, 0, 0);
    pthread_mutex_init(&mutexBuffer, NULL);
    pthread_mutex_init(&mutexContador, NULL);
    pthread_mutex_init(&mutexPrimos, NULL);

    // Criação da thread produtora
    pthread_create(&produtoras, NULL, produtor, NULL);

    // Criação das threads consumidoras
    for (int i = 0; i < C; i++) {
        ids[i] = i;
        pthread_create(&consumidoras[i], NULL, consumidor, &ids[i]);
    }

    // Espera threads
    pthread_join(produtoras, NULL);
    for (int i = 0; i < C; i++) {
        pthread_join(consumidoras[i], NULL);
    }

    // Resultados
    printf("\nTotal de primos encontrados: %d\n", primosTotal);
    int vencedora = 0;
    for (int i = 0; i < C; i++) {
        printf("Thread %d encontrou %d primos\n", i, primosPorThread[i]);
        if (primosPorThread[i] > primosPorThread[vencedora])
            vencedora = i;
    }
    printf("Thread vencedora: %d (com %d primos)\n", vencedora, primosPorThread[vencedora]);

    // Libera recursos
    free(buffer);
    free(primosPorThread);
    free(consumidoras);
    free(ids);
    sem_destroy(&vazio);
    sem_destroy(&cheio);
    pthread_mutex_destroy(&mutexBuffer);
    pthread_mutex_destroy(&mutexContador);
    pthread_mutex_destroy(&mutexPrimos);

    return 0;
}
