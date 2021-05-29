#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

//        (pai)
//          |
//      +---+---+
//      |       |
//     sed    grep

// ~~~ printfs  ~~~
//        sed (ao iniciar): "sed PID %d iniciado\n"
//       grep (ao iniciar): "grep PID %d iniciado\n"
//          pai (ao iniciar): "Processo pai iniciado\n"
// pai (após filho terminar): "grep retornou com código %d,%s encontrou adamantium\n"
//                            , onde %s é
//                              - ""    , se filho saiu com código 0
//                              - " não" , caso contrário

// Obs:
// - processo pai deve esperar pelo filho
// - 1º filho, após o término do 1º deve trocar seu binário para executar
//   sed -i /silver/axamantium/g;s/adamantium/silver/g;s/axamantium/adamantium/g text
//   + dica: leia as dicas do grep
// - 2º filho deve trocar seu binário para executar "grep adamantium text"
//   + dica: use execlp(char*, char*...)
//   + dica: em "grep adamantium text",  argv = {"grep", "adamantium", "text"}

int main(int argc, char** argv) {
    
    // ....
    /*************************************************
     * Dicas:                                        *
     * 1. Leia as intruções antes do main().         *
     * 2. Faça os prints exatamente como solicitado. *
     * 3. Espere o término dos filhos                *
     *************************************************/
    
    printf("Processo pai iniciado\n");
    int pid_sed = fork();
    if (pid_sed == 0) {
        printf("sed PID %d iniciado\n", getpid());
        execlp("sed", "sed", "-i", "-e", "s/silver/axamantium/g;s/adamantium/silver/g;s/axamantium/adamantium/g", "text", (char *)NULL);
    } else if (pid_sed > 0) {
        wait(NULL);
        fflush(stdout);
        int pid_grep = fork();
        if (pid_grep == 0) {
            printf("grep PID %d iniciado\n", getpid());
            execlp("grep", "grep", "adamantium", "text", (char *)NULL);
        } else if (pid_grep > 0) {
            int status;
            wait(&status);
            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                printf("grep retornou com código 0, encontrou adamantium\n");
            } else {
                printf("grep retornou com código %d, não encontrou adamantium\n", WEXITSTATUS(status));
            }
        }
    }
    return 0;
}
