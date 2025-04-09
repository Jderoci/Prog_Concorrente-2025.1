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
void *prodConcorrente(void *arg) {
    t_Args *args = (t_Args *) arg;
    double soma_local = 0.0;

    for (long int i = args->id; i < args->n; i += args->nthreads) {
        soma_local += args->a[i] * args->b[i];
    }

    double *resultado = malloc(sizeof(double));
    if (resultado == NULL) {
        perror("Erro de alocação para resultado parcial");
        pthread_exit(NULL);
    }
    *resultado = soma_local;
    pthread_exit((void *) resultado);
}

// Função principal
int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Digite: %s <número de threads> <nome do arquivo>\n", argv[0]);
        return 1;
    }

    int nthreads = atoi(argv[1]);
    char *arquivo_vetores = argv[2];
    long int n;
    float *a, *b;
    double prod_esperado, prod_calculado = 0.0;

    FILE *arquivo = fopen(arquivo_vetores, "rb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo\n");
        return 2;
    }

    fread(&n, sizeof(long int), 1, arquivo);
    a = (float *)malloc(n * sizeof(float));
    b = (float *)malloc(n * sizeof(float));
    if (a == NULL || b == NULL) {
        printf("Erro de alocação de memória\n");
        return 3;
    }
    fread(a, sizeof(float), n, arquivo);
    fread(b, sizeof(float), n, arquivo);
    fread(&prod_esperado, sizeof(double), 1, arquivo);
    fclose(arquivo);

    pthread_t tid[nthreads];
    t_Args args[nthreads];

    // Criação das threads
    for (long int i = 0; i < nthreads; i++) {
        args[i].id = i;
        args[i].n = n;
        args[i].nthreads = nthreads;
        args[i].a = a;
        args[i].b = b;

        if (pthread_create(&tid[i], NULL, prodConcorrente, (void *)&args[i])) {
            printf("ERRO: pthread_create()\n");
            return 4;
        }
    }

    // Aguarda o término das threads e soma os resultados parciais
    for (int i = 0; i < nthreads; i++) {
        void *retorno;
        if (pthread_join(tid[i], &retorno)) {
            printf("ERRO: pthread_join()\n");
            return 5;
        }
        if (retorno != NULL) {
            double *resultado_parcial = (double *)retorno;
            prod_calculado += *resultado_parcial;
            free(resultado_parcial);
        }
    }

    // Cálculo da variação relativa
    double var_relativa = fabs((prod_esperado - prod_calculado) / prod_esperado);

    printf("Produto Interno Esperado = %.15lf\n", prod_esperado);
    printf("Produto Interno Calculado = %.15lf\n", prod_calculado);
    printf("Variação Relativa = %.15lf\n", var_relativa);

    free(a);
    free(b);

    return 0;
}
