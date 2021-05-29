#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>

//                 (main)      
//                    |
//    +----------+----+------------+
//    |          |                 |   
// worker_1   worker_2   ....   worker_n


// ~~~ argumentos (argc, argv) ~~~
// ./program n_threads

// ~~~ printfs  ~~~
// main thread (após término das threads filhas): "Contador: %d\n"
// main thread (após término das threads filhas): "Esperado: %d\n"

// Obs:
// - pai deve criar n_threds (argv[1]) worker threads 
// - cada thread deve incrementar contador_global (operador ++) n_loops vezes
// - pai deve esperar pelas worker threads  antes de imprimir!


int contador_global = 0;

// Função que será chamada pela thread.
void* global_increment(void* arg) {
    // Recebe o valor enviado para a thread, converte void* em int* e derreferencia.
    int n_loops = *((int*)arg);
    // Loop para incrementar o contador_global n_loop vezes.
    for (int i = 0; i < n_loops; i++) {
        contador_global++;
    }
    // Encerra a thread.
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("n_threads é obrigatório!\n");
        printf("Uso: %s n_threads n_loops\n", argv[0]);
        return 1;
    }

    int n_threads = atoi(argv[1]);
    int n_loops = atoi(argv[2]);
    // Lista de threads.
    pthread_t threads[n_threads];
    // Lista de valores que serão enviados para as threads.
    int values[n_threads];
    
    // Loop para criar threads e envia valores.
    for (int i = 0; i < n_threads; i++) {
        // Define o valor que será enviado para a thread.
        values[i] = n_loops;
        // Cria a thread.
        pthread_create(&threads[i], NULL, global_increment, &values[i]);
    }

    // Loop para aguardar o termino de todas as threads.
    for (int i = 0; i < n_threads; i++) {
        // Aguarda termino da thread.
        pthread_join(threads[i], NULL);
    }

    printf("Contador: %d\n", contador_global);
    printf("Esperado: %d\n", n_threads*n_loops);
    return 0;
}
