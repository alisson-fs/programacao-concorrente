#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>

FILE* out;

// Semaforos que serao utilizados para o controle da diferenca 
// entre quantidade de As e Bs.
sem_t a, b;

void *thread_a(void *args) {
    for (int i = 0; i < *(int*)args; ++i) {
        // Decrementa o semaforo A.
        sem_wait(&a);
	//      +---> arquivo (FILE*) destino
	//      |    +---> string a ser impressa
	//      v    v
        fprintf(out, "A");
        // Importante para que vocês vejam o progresso do programa
        // mesmo que o programa trave em um sem_wait().
        fflush(stdout);
        // Incrementa o semaforo B.
        sem_post(&b);
    }
    return NULL;
}

void *thread_b(void *args) {
    for (int i = 0; i < *(int*)args; ++i) {
        // Decrementa o semaforo B.
        sem_wait(&b);
        fprintf(out, "B");
        fflush(stdout);
        // Incrementa o semaforo A.
        sem_post(&a);
    }
    return NULL;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Uso: %s iteraões\n", argv[0]);
        return 1;
    }
    int iters = atoi(argv[1]);
    srand(time(NULL));
    out = fopen("result.txt", "w");

    // Inicia os semaforos A e B.
    sem_init(&a, 0, 1);
    sem_init(&b, 0, 1);

    pthread_t ta, tb;
    // Cria threads
    pthread_create(&ta, NULL, thread_a, &iters);
    pthread_create(&tb, NULL, thread_b, &iters);

    // Espera pelas threads
    pthread_join(ta, NULL);
    pthread_join(tb, NULL);

    //Imprime quebra de linha e fecha arquivo
    fprintf(out, "\n");
    fclose(out);
  
    // Destroi os semaforos A e B.
    sem_destroy(&a);
    sem_destroy(&b);
  
    return 0;
}
