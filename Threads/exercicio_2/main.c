#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

// Lê o conteúdo do arquivo filename e retorna um vetor E o tamanho dele
// Se filename for da forma "gen:%d", gera um vetor aleatório com %d elementos
//
// +-------> retorno da função, ponteiro para vetor malloc()ado e preenchido
// | 
// |         tamanho do vetor (usado <-----+
// |         como 2o retorno)              |
// v                                       v
double* load_vector(const char* filename, int* out_size);


// Avalia o resultado no vetor c. Assume-se que todos os ponteiros (a, b, e c)
// tenham tamanho size.
void avaliar(double* a, double* b, double* c, int size);

// Struct para guardar os valores que serão enviados para as threads.
typedef struct {
    // Recebe vetores a, b e c.
    double* a;
    double* b;
    double* c;
    // Recebe valor que inicia o pedaço que a thread irá somar.
    int begin;
    // Recebe valor que termina o pedaço que a thread irá somar.
    int end;
} info_sum_vectors;

// Função que será chamada pela thread para somar os vetores.
void* sum_vectors(void* arg) {
    // Recebe a struct com valores que foram enviados para a thread.
    info_sum_vectors vectors = *((info_sum_vectors*)arg);
    // Loop que soma cada posição a com a respectiva posição do vetor b e adiciona no vetor c.
    for (int i = vectors.begin; i < vectors.end; i++) {
        vectors.c[i] = vectors.a[i] + vectors.b[i];
    }
    // Encerra a thread.
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    // Gera um resultado diferente a cada execução do programa
    // Se **para fins de teste** quiser gerar sempre o mesmo valor
    // descomente o srand(0)
    srand(time(NULL)); //valores diferentes
    //srand(0);        //sempre mesmo valor

    //Temos argumentos suficientes?
    if(argc < 4) {
        printf("Uso: %s n_threads a_file b_file\n"
               "    n_threads    número de threads a serem usadas na computação\n"
               "    *_file       caminho de arquivo ou uma expressão com a forma gen:N,\n"
               "                 representando um vetor aleatório de tamanho N\n",
               argv[0]);
        return 1;
    }
  
    //Quantas threads?
    int n_threads = atoi(argv[1]);
    if (!n_threads) {
        printf("Número de threads deve ser > 0\n");
        return 1;
    }
    //Lê números de arquivos para vetores alocados com malloc
    int a_size = 0, b_size = 0;
    double* a = load_vector(argv[2], &a_size);
    if (!a) {
        //load_vector não conseguiu abrir o arquivo
        printf("Erro ao ler arquivo %s\n", argv[2]);
        return 1;
    }
    double* b = load_vector(argv[3], &b_size);
    if (!b) {
        printf("Erro ao ler arquivo %s\n", argv[3]);
        return 1;
    }
    
    //Garante que entradas são compatíveis
    if (a_size != b_size) {
        printf("Vetores a e b tem tamanhos diferentes! (%d != %d)\n", a_size, b_size);
        return 1;
    }
    //Cria vetor do resultado 
    double* c = malloc(a_size*sizeof(double));

    // Calcula com n_threads threads.
    
    // Verifica se o numero de threads é maior que o tamanho dos vetores. Se for maior, n_threads 
    // será definido como igual ao tamanho do vetor para que não sejam criadas threads a mais.
    if (n_threads > a_size) {
        n_threads = a_size;
    }
    // Lista de threads.
    pthread_t threads[n_threads];
    // Lista de struct com valores que serão utilizados pelas threads.
    info_sum_vectors vectors[n_threads];
    // Divide o vetor pela quantidade de threads para cada uma somar uma parte do vetor.
    int division_vectors = a_size/n_threads;
    // Loop que envia os vetores e define o pedaço que cada thread irá somar.
    for (int i = 0; i< n_threads; i++) {
        vectors[i].a = a;
        vectors[i].b = b;
        vectors[i].c = c;
        vectors[i].begin = i*division_vectors;
        if ((i == n_threads - 1) && (a_size % n_threads != 0)) {
            vectors[i].end = i*division_vectors + division_vectors + (a_size % n_threads);
        } else {
            vectors[i].end = i*division_vectors + division_vectors;
        }
    }
    
    // Loop para criar threads.
    for (int i = 0; i < n_threads; i++) {
        // Cria a thread.
        pthread_create(&threads[i], NULL, sum_vectors, &vectors[i]);
    }
    
    // Loop para aguardar o termino de todas as threads.
    for (int i = 0; i < n_threads; i++) {
        // Aguarda termino da thread.
        pthread_join(threads[i], NULL);
    }
    
    // Calcula com uma thread só. Programador original só deixou a leitura 
    // do argumento e fugiu pro caribe. É essa computação que você precisa 
    // paralelizar
    //for (int i = 0; i < a_size; ++i) 
    //    c[i] = a[i] + b[i];

    //    +---------------------------------+
    // ** | IMPORTANTE: avalia o resultado! | **
    //    +---------------------------------+
    avaliar(a, b, c, a_size);
    

    //Importante: libera memória
    free(a);
    free(b);
    free(c);

    return 0;
}
