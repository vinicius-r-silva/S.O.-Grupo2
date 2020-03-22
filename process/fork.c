//#include <sys/wait.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <stdio.h>
//#include <sys/types.h>
//
//
//
//int main(int argc, char *argv[])
//{
//    pid_t cpid, cpid2, w;
//    int wstatus;
//    int i=0;
//    printf("i: %d\n", i);
//
//
//    cpid = fork();
//
//    printf("cpid: %d\n", cpid);
//    //verifica se deu erro
//    if (cpid == -1) {
//        perror("fork");
//        exit(EXIT_FAILURE);
//    }
//
//
//    if (cpid == 0) {            /* Code executed by child */
//        printf("Child PID is %ld\n", (long) getpid());
//        if (argc == 1)
//            pause();                    /* Wait for signals */
//        _exit(atoi(argv[1]));
//
//    } else {                    /* Code executed by parent */
//        do {
//            if (waitpid(cpid, &wstatus, WUNTRACED | WCONTINUED) == -1) {
//                perror("waitpid");
//                exit(EXIT_FAILURE);
//            }
//
//            if (WIFEXITED(wstatus)) {
//                printf("exited, status=%d\n", WEXITSTATUS(wstatus));
//            } else if (WIFSIGNALED(wstatus)) {
//                printf("killed by signal %d\n", WTERMSIG(wstatus));
//            } else if (WIFSTOPPED(wstatus)) {
//                printf("stopped by signal %d\n", WSTOPSIG(wstatus));
//            } else if (WIFCONTINUED(wstatus)) {
//                printf("continued\n");
//            }
//        } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
//        exit(EXIT_SUCCESS);
//    }
//}
//
//
#include <stdio.h> 
#include <unistd.h> 
int main() 
{ 
    fork() && fork() || fork(); 
  
    printf("forked\n"); 
    return 0; 
} 