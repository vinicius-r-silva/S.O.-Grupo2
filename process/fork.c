#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    pid_t cpid;

    //cria novo processo
    cpid = fork();

    //verifica se deu erro
    if (cpid == -1)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) //executado pelo filho
        printf("PID do processo filho: %ld\n", (long)getpid());

    else //executado pelo pai
        printf("PID do processo pai: %ld\n", (long)getpid());
}