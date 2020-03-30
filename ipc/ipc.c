//gcc ipc.c -Wall -o main

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/wait.h> 
#include <unistd.h>   
#include <string.h>   
#include <stdlib.h>

#define SHMSIZE 30

int main() {
    pid_t cpid;
    int wstatus;
    int shmid;
    char *shm;

    //cria memória compatilhada
    shmid = shmget(2009, SHMSIZE, 0666 | IPC_CREAT);//CHAMADA DO SISTEMA shmget. (2009 -> chave de acesso, 0666 -> leitura e escrita liberada para todos)
    if (shmid == -1){
        perror("shmget");
        exit(EXIT_FAILURE); //CHAMADA DO SISTEMA EXIT. termina a execução do processo
    }

    //inicia processo filho
    cpid = fork();//CHAMADA DO SISTEMA fork(). Cria um processo filho. Retorna 0 para o filho, e um numero maior que 0 para o pai
    if (cpid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }

    //processo filho
    if(cpid == 0) { 
        shm = shmat(shmid, 0, 0); //CHAMADA DO SISTEMA shmat. Acopla segmento de memória criado com o shmget
        if (shm == (void*)-1){
            perror("shmat - filho");
            exit(EXIT_FAILURE);
        }

        char *str = (char *) shm; //trata shm como uma string
        strcpy(str, "String do filho para o pai");

        printf ("Processo filho escreveu: %s\n",shm);
        shmdt(shm); //CHAMADA DO SISTEMA shmdt. Descopla segmento de memória criado com o shmget

        exit(EXIT_SUCCESS);
    }
    //processo pai
    else {
        shm = shmat(shmid, 0, 0); 
        waitpid(-1, &wstatus, 0); //CHAMADA DO SISTEMA waitpid. ((-1) -> espera processos filhos terminarem, salva o status de execução do filho em wstatus)

        printf("processo filho terminou a execução com status %d\n", WEXITSTATUS(wstatus));
        if (shm == (void*)-1){
            perror("shmat - pai");
            exit(EXIT_FAILURE);
        }

        if(wstatus == EXIT_SUCCESS){
            printf ("Processo pai leu: %s\n",shm); //shm representa o segmento de memória compatilhada
        }

        shmdt(shm); //CHAMADA DO SISTEMA shmdt. Descopla segmento de memória criado com o shmget
        shmctl(shmid, IPC_RMID, NULL); //CHAMADA DO SISTEMA shmctl. Destruição do segmento de memória
    }

    return 0;
}