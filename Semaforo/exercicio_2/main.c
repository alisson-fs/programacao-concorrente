#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

int produzir(int value);    //< definida em helper.c
void consumir(int produto); //< definida em helper.c
void *produtor_func(void *arg);
void *consumidor_func(void *arg);

int indice_produtor, indice_consumidor, tamanho_buffer;
int* buffer;

// Semaforos para o controle de espaco do buffer.
sem_t spaces, products;
// Semaforo para controle de acesso ao buffer.
sem_t mutex_access;
// Variavel que define se as threads produtoras foram ou não finalizadas.
int producers_finished;

// Você deve fazer as alterações necessárias nesta função e na função
// consumidor_func para que usem semáforos para coordenar a produção
// e consumo de elementos do buffer.
void *produtor_func(void *arg) {
    //arg contem o número de itens a serem produzidos
    int max = *((int*)arg);
    for (int i = 0; i < max; i++) {
        int produto;
        // Decrementa o semaforo de espacos.
        sem_wait(&spaces);
        // Decrementa o semaforo que funciona como mutex de acesso ao buffer, funcionando como um lock.
        sem_wait(&mutex_access);
        // Produz um elemento.
        produto = produzir(i);
        indice_produtor = (indice_produtor + 1) % tamanho_buffer; //calcula posição próximo elemento
        buffer[indice_produtor] = produto; //adiciona o elemento produzido à lista
        // Incrementa o semaforo que funciona como mutex de acesso ao buffer, funcionando como um unlock.
        sem_post(&mutex_access);
        // Incrementa o semaforo de produtos.
        sem_post(&products);
    }
    return NULL;
}

void *consumidor_func(void *arg) {
    while (1) {
        // Se existirem produtos no buffer, decrementa o semaforo de produtos,
        // caso contrario, verifica se as threads produtoras finalizaram e se
        // for o caso finalizar esta thread.
        if (sem_trywait(&products) == 0) {
            // Decrementa o semaforo que funciona como mutex de acesso ao buffer, funcionando como um lock.
            sem_wait(&mutex_access);
            indice_consumidor = (indice_consumidor + 1) % tamanho_buffer; //Calcula o próximo item a consumir
            int produto = buffer[indice_consumidor]; //obtém o item da lista
            // Consome o item obtido.
            consumir(produto);
            // Incrementa o semaforo que funciona como mutex de acesso ao buffer, funcionando como um unlock.
            sem_post(&mutex_access);
            // Incrementa o semaforo de espacos.
            sem_post(&spaces);
        } else if (producers_finished == 1) {
            break;
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        printf("Uso: %s tamanho_buffer itens_produzidos n_produtores n_consumidores \n", argv[0]);
        return 0;
    }

    tamanho_buffer = atoi(argv[1]);
    int itens = atoi(argv[2]);
    int n_produtores = atoi(argv[3]);
    int n_consumidores = atoi(argv[4]);
    printf("itens=%d, n_produtores=%d, n_consumidores=%d\n",
	   itens, n_produtores, n_consumidores);

    //Iniciando buffer
    indice_produtor = 0;
    indice_consumidor = 0;
    buffer = malloc(sizeof(int) * tamanho_buffer);

    // Crie threads e o que mais for necessário para que n_produtores
    // threads criem cada uma n_itens produtos e o n_consumidores os
    // consumam.

    // Inicia o semaforo para controle de espaco.
    sem_init(&spaces, 0, tamanho_buffer);
    // Inicia o semaforo para controle de produtos.
    sem_init(&products, 0, 0);
    // Inicia o semaforo que será utilizado como um mutex para controle de acesso ao buffer.
    sem_init(&mutex_access, 0, 1);
    // Inicia como 0 indicando que as threads produtodas não foram finalizadas.
    producers_finished = 0;
    // Lista de threads
    pthread_t producers_threads[n_produtores];
    pthread_t consumers_threads[n_consumidores];
    // Loop que cria as threads produtoras.
    for (int i = 0; i < n_produtores; i++) {
        pthread_create(&producers_threads[i], NULL, produtor_func, &itens);
    }
    // Loop cria thread consumidora.
    for (int i = 0; i < n_consumidores; i++) {
    pthread_create(&consumers_threads[i], NULL, consumidor_func, NULL);
    }
    // Loop para aguardar o termino de todas as threads produtoras.
    for (int i = 0; i < n_produtores; i++) {
        pthread_join(producers_threads[i], NULL);
    }
    // Variavel definida para 1 pois as threads produtoras ja foram finalizadas.
    producers_finished = 1;
    // Loop para aguardar o termino de todas as threads consumidoras.
    for (int i = 0; i < n_consumidores; i++) {
        pthread_join(consumers_threads[i], NULL);
    }
    // Destrói os semaforos.
    sem_destroy(&spaces);
    sem_destroy(&products);
    sem_destroy(&mutex_access);
    //Libera memória do buffer
    free(buffer);
    return 0;
}

