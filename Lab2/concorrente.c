#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

// Estrutura para passar os argumentos para as threads
typedef struct {
    long int id;
    long int n;
    int nthreads;
    float *a;
    float *b;
} t_Args;

// Função executada pelas threads para calcular uma parte do produto interno
void *prodConc(void *arg) {
    t_Args *args = (t_Args *) arg;
    double soma = 0.0;

    for (long int i = args->id; i < args->n; i += args->nthreads) {
        soma += args->a[i] * args->b[i];
    }

    double *ret = malloc(sizeof(double));
    if (ret == NULL) {
        perror("Erro ao alocar memória");
        pthread_exit(NULL);
    }

    *ret = soma;
    pthread_exit((void *) ret);
}

// Função principal
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <nº threads> <arquivo>\n", argv[0]);
        return 1;
    }

    int nthreads = atoi(argv[1]);
    char *arq = argv[2];
    long int n;
    float *a, *b;
    double prod_esperado, prod_calculado = 0.0;

    FILE *f = fopen(arq, "rb");
    if (f == NULL) {
        printf("Erro ao abrir o arquivo\n");
        return 2;
    }

    fread(&n, sizeof(long int), 1, f);
    a = malloc(n * sizeof(float));
    b = malloc(n * sizeof(float));
    if (a == NULL || b == NULL) {
        printf("Erro de alocação\n");
        return 3;
    }
    fread(a, sizeof(float), n, f);
    fread(b, sizeof(float), n, f);
    fread(&prod_esperado, sizeof(double), 1, f);
    fclose(f);

    pthread_t tid[nthreads];
    t_Args args[nthreads];

  // Criação das threads  
  for (long int i = 0; i < nthreads; i++) {
        args[i].id = i;
        args[i].n = n;
        args[i].nthreads = nthreads;
        args[i].a = a;
        args[i].b = b;
        if (pthread_create(&tid[i], NULL, prodConc, (void *)&args[i])) {
            printf("ERRO: pthread_create()\n");
            return 4;
        }
    }

  // Aguarda o término das threads e soma os resultados parciais  
  for (int i = 0; i < nthreads; i++) {
        void *ret;
        if (pthread_join(tid[i], &ret)) {
            printf("ERRO: pthread_join()\n");
            return 5;
        }
        if (ret != NULL) {
            double *val = (double *) ret;
            prod_calculado += *val;
            free(val);
        }
    }

  // Cálculo do erro relativo
    double erro = fabs((prod_esperado - prod_calculado) / prod_esperado);

    printf("Produto Interno Esperado = %.15lf\n", prod_esperado);
    printf("Produto Interno Calculado = %.15lf\n", prod_calculado);
    printf("Erro Relativo = %.15lf\n", erro);

    free(a);
    free(b);

    return 0;
}
