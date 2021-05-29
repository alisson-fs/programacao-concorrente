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


// Avalia se o prod_escalar é o produto escalar dos vetores a e b. Assume-se
// que ambos a e b sejam vetores de tamanho size.
void avaliar(double* a, double* b, int size, double prod_escalar);

// Variavel global recebe o produto dos elementos dos vetores e soma ao seu valor atual.
double result = 0;

// Struct para guardar os valores que serão enviados para as threads.
typedef struct {
    // Recebe vetores a e b.
    double* a;
    double* b;
    // Recebe valor que inicia o pedaço que a thread irá somar.
    int begin;
    // Recebe valor que termina o pedaço que a thread irá somar.
    int end;
} info_product_vectors;

// Função que será chamada pela thread para somar os vetores.
void* product_vectors(void* arg) {
    // Recebe a struct com valores que foram enviados para a thread.
    info_product_vectors vectors = *((info_product_vectors*)arg);
    // Loop que soma cada posição a com a respectiva posição do vetor b e adiciona no vetor c.
    for (int i = vectors.begin; i < vectors.end; i++) {
        result += (vectors.a[i] * vectors.b[i]);
    }
    // Encerra a thread.
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

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

    // Calcula com n_threads threads.
    // Verifica se o numero de threads é maior que o tamanho dos vetores. Se for maior, n_threads 
    // será definido como igual ao tamanho do vetor para que não sejam criadas threads a mais.
    if (n_threads > a_size) {
        n_threads = a_size;
    }
    // Lista de threads.
    pthread_t threads[n_threads];
    // Lista de structs com valores que serão utilizados pelas threads.
    info_product_vectors vectors[n_threads];
    // Divide o vetor pela quantidade de threads para cada uma multiplicar uma parte do vetor.
    int division_vectors = a_size/n_threads;
    // Loop que envia os vetores e define o pedaço que cada thread irá somar.
    for (int i = 0; i< n_threads; i++) {
        vectors[i].a = a;
        vectors[i].b = b;
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
        pthread_create(&threads[i], NULL, product_vectors, &vectors[i]);
    }
    
    // Loop para aguardar o termino de todas as threads.
    for (int i = 0; i < n_threads; i++) {
        // Aguarda termino da thread.
        pthread_join(threads[i], NULL);
    }

    // Calcula com uma thread.
    //Calcula produto escalar. Paralelize essa parte
    //double result = 0;
    //for (int i = 0; i < a_size; ++i) 
    //    result += a[i] * b[i];
    
    //    +---------------------------------+
    // ** | IMPORTANTE: avalia o resultado! | **
    //    +---------------------------------+
    avaliar(a, b, a_size, result);

    //Libera memória
    free(a);
    free(b);

    return 0;
}
