//gcc -Wall -ggdb3  Atividade_2/prod_cons.c -o Atividade_2/prod_cons -lpthread

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>

#define FOREVER 1

#define BUFFER_SIZE 10

typedef struct th_args
{
    int tid;
    void (*WaitFunction)();
} th_args;

sem_t mutex, empty, full;

pthread_mutex_t the_mutex;
pthread_cond_t condc, condp;

int buffer[BUFFER_SIZE], count, itemCount, lastCount = 0;

void *(*Produtor)(void *);
void *(*Consumidor)(void *);


void *Produtor_Sem(void *args){
    printf("Iniciando Produtor com semaforo");
    int item = 1;

    while (FOREVER) {
        sem_wait(&empty);
        sem_wait(&mutex);

        buffer[itemCount] = item;
        itemCount++;
        printf("Produtor - item %d produzido\n", itemCount);

        sem_post(&mutex);
        sem_post(&full);
        usleep(1);
    }
}

void *Consumidor_Sem(void *args){
    printf("Iniciando Consumidor com semaforo");

    while (FOREVER){
        sem_wait(&full);
        sem_wait(&mutex);

        itemCount--;
        buffer[itemCount] = 0;
        printf("Consumidor - item %d consumido.\n", itemCount + 1);

        sem_post(&mutex);
        sem_post(&empty);
        usleep(1);
    }
}

void *Produtor_sleepWakeup(void *args)
{
    printf("Iniciando Produtor com sleep wakeup\n");
    usleep(100);

    while (FOREVER){

        if (itemCount < 0 || itemCount > BUFFER_SIZE){
            printf("-produtor fora dos limites");
            exit(EXIT_FAILURE);
        }

        if (itemCount == BUFFER_SIZE)
            printf("Produtor - Sem espaço para produzir, liberando buffer\n");
        while (itemCount == BUFFER_SIZE){
            pthread_cond_wait(&condp, &the_mutex);
        }
        

        pthread_mutex_lock(&the_mutex); /* protect buffer */
        printf("Produtor - Bloqueando Buffer\n");

        buffer[itemCount] = 1;
        printf("Produtor - item %d produzido\n", itemCount);
        lastCount = itemCount;
        itemCount++;

        pthread_mutex_unlock(&the_mutex);
        if (lastCount == 0){
            pthread_cond_signal(&condc); /* wake up consumer */
        }

        printf("Produtor - Liberando Buffer\n");
        usleep(10);
    }
}

void *Consumidor_sleepWakeup(void *args)
{
    printf("Iniciando Consumidor com sleep wakeup");
    while (FOREVER)
    {

        if (itemCount < 0 || itemCount > BUFFER_SIZE){
            printf("-consumidor fora dos limites");
            exit(EXIT_FAILURE);
        }

      	
        if (itemCount == 0)
            printf("Consumidor - Sem nada para consumir, esperando produtor\n");
      	while(itemCount == 0){
            pthread_cond_wait(&condc, &the_mutex);            
        }

        pthread_mutex_lock(&the_mutex); /* protect buffer */
        printf("Consumidor - Bloqueando Buffer\n");

        lastCount = itemCount;
        itemCount--;
        buffer[itemCount] = 0;
        printf("Consumidor - item %d consumido\n", itemCount);

        pthread_mutex_unlock(&the_mutex);
        if (lastCount == BUFFER_SIZE){
            pthread_cond_signal(&condp); /* wake up consumer */
        }

        printf("Consumidor - Liberando Buffer\n");
        usleep(10);
    }
}

int main(int argc, char *argv[]){
    int ThErr1 = 0, ThErr2 = 0;

    count = 0;
    itemCount = 0;

    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);

    if (argc > 1) {
        if (strcmp("-s", argv[1]) == 0){
            Produtor = Produtor_Sem;
            Consumidor = Consumidor_Sem;
        }
        else{
            Produtor = Produtor_sleepWakeup;
            Consumidor = Consumidor_sleepWakeup;
        }
    }else{
        Produtor = Produtor_sleepWakeup;
        Consumidor = Consumidor_sleepWakeup;
    }

    pthread_t ThHandle[2]; // Thread handles
    pthread_mutex_init(&the_mutex, NULL);
    pthread_cond_init(&condc, NULL); /* Initialize consumer condition variable */
    pthread_cond_init(&condp, NULL);

    printf("iniciando Threads\n");
    ThErr1 = pthread_create(&ThHandle[0], NULL, Produtor, NULL);
    ThErr2 = pthread_create(&ThHandle[1], NULL, Consumidor, NULL);
    if (ThErr1 != 0 || ThErr2 != 0){
        printf("Erro %d, %d. Abortando operação\n", ThErr1, ThErr2);
        exit(EXIT_FAILURE);
    }

    pthread_mutex_destroy(&the_mutex); /* Free up the_mutex */
    pthread_cond_destroy(&condc);      /* Free up consumer condition variable */
    pthread_cond_destroy(&condp);      /* Free up producer condition variable */
    // pthread_attr_destroy(&ThAttr);
    pthread_join(ThHandle[0], NULL);
    pthread_join(ThHandle[1], NULL);

    return (EXIT_SUCCESS);
}