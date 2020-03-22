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
        printf("Child PID is: %ld\n", (long)getpid());

    else //executado pelo pai
        printf("father PID is: %ld\n", (long)getpid());
}