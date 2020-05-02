#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    pid_t cpid;  //guarda a id do processo filho
    int wstatus; //guarda o status do processo filho

    cpid = fork();  //cria o processo filho
    if (cpid == -1) //confere se foi um sucesso
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0)
    { //executado pelo filho
        printf("PID do processo filho: %ld\n", (long)getpid());
        for (int i = 0; i < 5; i++)
            printf("processo pai esta me esperando\n");

        exit(1);
    }
    else //executado pelo pai
    {
        waitpid(-1, &wstatus, 0); //espera o processo filho
        printf("status de saida=%d\n", WEXITSTATUS(wstatus));
    }

    return 0;
}
