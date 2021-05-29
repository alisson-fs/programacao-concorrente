#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

//                          (principal)
//                               |
//              +----------------+--------------+
//              |                               |
//           filho_1                         filho_2
//              |                               |
//    +---------+-----------+          +--------+--------+
//    |         |           |          |        |        |
// neto_1_1  neto_1_2  neto_1_3     neto_2_1 neto_2_2 neto_2_3

// ~~~ printfs  ~~~
//      principal (ao finalizar): "Processo principal %d finalizado\n"
// filhos e netos (ao finalizar): "Processo %d finalizado\n"
//    filhos e netos (ao inciar): "Processo %d, filho de %d\n"

// Obs:
// - netos devem esperar 5 segundos antes de imprmir a mensagem de finalizado (e terminar)
// - pais devem esperar pelos seu descendentes diretos antes de terminar

void create_gchild() {
    int pid1 = fork();
    if (pid1 == 0){
        printf("Processo %d, filho de %d\n", getpid(), getppid());
        sleep(5);
    } else if (pid1 > 0) {
        fflush(stdout);
        int pid2 = fork();
        if (pid2 == 0){
            printf("Processo %d, filho de %d\n", getpid(), getppid());
            sleep(5);
        } else if (pid2 > 0) {
            fflush(stdout);
            int pid3 = fork();
            if (pid3 == 0){
                printf("Processo %d, filho de %d\n", getpid(), getppid());
                sleep(5);
            }
        }
    }
}

void create_child() {
    int pid1 = fork();
    if (pid1 == 0){
        printf("Processo %d, filho de %d\n", getpid(), getppid());
        fflush(stdout);
        create_gchild();
        wait(NULL);
        wait(NULL);
        wait(NULL);
        printf("Processo %d finalizado\n", getpid());
    } else if (pid1 > 0) {
        fflush(stdout);
        int pid2 = fork();
        if (pid2 == 0){
            printf("Processo %d, filho de %d\n", getpid(), getppid());
            fflush(stdout);
            create_gchild();
            wait(NULL);
            wait(NULL);
            wait(NULL);
            printf("Processo %d finalizado\n", getpid());
        } else if (pid2 > 0) {
            wait(NULL);
            wait(NULL);
            printf("Processo principal %d finalizado\n", getpid());
        }
    }
}

int main(int argc, char** argv) {

    // ....

    /*************************************************
     * Dicas:                                        *
     * 1. Leia as intruções antes do main().         *
     * 2. Faça os prints exatamente como solicitado. *
     * 3. Espere o término dos filhos                *
     *************************************************/

    create_child();
    return 0;
}
