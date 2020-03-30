#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <math.h>

int main(int argc, char *argv[])
{
    pid_t cpid; //guarda a id do processo

    cpid = fork(); //cria um processo filho

    //confere se foi possível criar um processo filho
    if (cpid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    int wstatus;                 //guarda o status do processo filho
    long unsigned int maxNumber; //guarda um inteiro grande

    //calcula o valor do inteiro
    maxNumber = (long unsigned int)pow(2, sizeof(long unsigned int) * 4);
    //printf("number: %ld\n", maxNumber);

    //processo escolhido como exemplo para o programa
    for (long unsigned int i = 0; i < maxNumber; i++)
        sqrt(i);

    //espera o processo filho terminar
    if (cpid > 0) //executado pelo pai
    {
        waitpid(-1, &wstatus, 0);
        printf("status de saida=%d\n", WEXITSTATUS(wstatus));
    }

    return 0;
}