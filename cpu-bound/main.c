#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <math.h>

int main()
{

    pid_t cpid;

    cpid = fork();

    if (cpid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    int wstatus;
    long unsigned int maxNumber;

    printf("number: %ld\n", maxNumber = (long unsigned int)pow(2, sizeof(long unsigned int) * 4));

    for (long unsigned int i = 0; i < maxNumber; i++)
        sqrt(i);

    if (cpid > 0)
    {
        waitpid(-1, &wstatus, 0);
        printf("status de saida=%d\n", WEXITSTATUS(wstatus));
    }

    return 0;
}