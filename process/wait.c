#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

int main(int argc, char *argv[])
{
    pid_t cpid, w;
    int wstatus;

    cpid = fork();
    if (cpid == -1)
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
    else
    {
        waitpid(-1, &wstatus, 0);
        printf("status de saida=%d\n", WEXITSTATUS(wstatus));
    }
}
