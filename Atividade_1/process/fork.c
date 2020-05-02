#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

//Para compilar: gcc fork.c -o fork
//Para rodar: ./fork

int main(int argc, char *argv[])
{
    pid_t cpid; //guarda a id do processo

    cpid = fork(); //cria processo filho

    if (cpid == -1) //verifica se deu erro
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) //executado pelo filho
        printf("PID do processo filho: %ld\n", (long)getpid());

    else //executado pelo pai
        printf("PID do processo pai: %ld\n", (long)getpid());
}