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

typedef struct tist_t{

    struct tist_t *next;
    int item;

}list_t;

sem_t mutex, empty, full;

pthread_mutex_t the_mutex;
pthread_cond_t condc, condp;

int buffer[BUFFER_SIZE], count, itemCount, lastCount = 0;

void *(*Produtor)(void *);
void *(*Consumidor)(void *);

list_t* begin = NULL;
list_t* last = NULL;
list_t* current = NULL;
list_t* prev = NULL;

void push(){
    list_t *temp;

    temp = (list_t*) malloc(sizeof(list_t));
    temp->item = itemCount + 1;
    temp->next = begin;
    begin = temp;
}

void pop(){
    list_t* temp;

    temp = begin;
    begin = begin->next;

    free(temp);
}

void *Produtor_Sem(void *args){
    printf("Iniciando Produtor com semaforo");
    int item = 1;

    list_t *temp;

    while (FOREVER) {
        sem_wait(&empty);
        sem_wait(&mutex);

        // temp = begin;
        // while (temp != NULL)
        // {
        //     printf("p: %d  ", temp->item);
        //     temp = temp->next;
        // }
        // printf("\n");

        if(itemCount == 0){
            begin = (list_t*) malloc(sizeof(list_t));
            begin->item = itemCount + 1;
            begin->next = NULL;
        }else{
            push();
        }
        
        buffer[itemCount] = item;
        itemCount++;
        printf("Produtor - item %d produzido\n", begin->item);
        printf("Produtor - item %d produzido\n", itemCount);

        sem_post(&mutex);
        sem_post(&full);
        usleep(1000);
    }
}

void *Consumidor_Sem(void *args){
    printf("Iniciando Consumidor com semaforo");

    list_t *temp;
    while (FOREVER){
        sem_wait(&full);
        sem_wait(&mutex);

        // // temp = begin;
        // while (temp != NULL)
        // {
        //     printf("c: %d ", temp->item);
        //     temp = temp->next;
        // }
        // printf("\n");

        if(itemCount == 1){
            free(begin);
            begin = NULL;
            current = NULL;
            last = NULL;
            printf("Consumidor - item 1 consumido\n");
        }else{
            printf("Consumidor - item %d consumido\n", begin->item);
            pop();
        }

        itemCount--;
        buffer[itemCount] = 0;
        printf("Consumidor - item %d consumido.\n", itemCount + 1);

        sem_post(&mutex);
        sem_post(&empty);
        usleep(1000);
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

        pthread_mutex_lock(&the_mutex); /* protect buffer */
        printf("Produtor - Bloqueando Buffer\n");

        if (itemCount >= BUFFER_SIZE){
            printf("Produtor - Sem espaço para produzir, liberando buffer\n");
            while (itemCount >= BUFFER_SIZE){
                pthread_cond_wait(&condp, &the_mutex);
            }
        }
       
        if(itemCount == 0){
            begin = (list_t*) malloc(sizeof(list_t));
            begin->item = itemCount + 1;
            begin->next = NULL;
        }else{
            push();
        }

        buffer[itemCount] = 1;
        lastCount = itemCount;
        itemCount++;
        printf("Produtor - item %d produzido\n", begin->item);
        printf("Produtor - item %d produzido\n", itemCount);
        
        if (lastCount == 0){
            pthread_cond_signal(&condc); /* wake up consumer */
        }
        pthread_mutex_unlock(&the_mutex);

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

        pthread_mutex_lock(&the_mutex); /* protect buffer */
        printf("Consumidor - Bloqueando Buffer\n");

        if (itemCount <= 0){
            printf("Consumidor - Sem nada para consumir, esperando produtor\n");
            while(itemCount <= 0){
                pthread_cond_wait(&condc, &the_mutex); 
            }
        }

        if(itemCount == 1){
            printf("Consumidor - item 1 consumido\n");
            free(begin);
            begin = NULL;
            current = NULL;
            last = NULL;
        }else{
            if(begin == NULL){
                printf("Begin NULL");
                getchar();
            }
            printf("Consumidor - item %d consumido\n", begin->item);
            pop();
        }

        lastCount = itemCount;
        itemCount--;
        buffer[itemCount] = 0;
        printf("Consumidor - item %d consumido\n", itemCount+1);

        if (lastCount == BUFFER_SIZE){
            pthread_cond_signal(&condp); /* wake up consumer */
        }
        pthread_mutex_unlock(&the_mutex);

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

    pthread_join(ThHandle[0], NULL);
    pthread_join(ThHandle[1], NULL);
    pthread_mutex_destroy(&the_mutex); /* Free up the_mutex */
    pthread_cond_destroy(&condc);      /* Free up consumer condition variable */
    pthread_cond_destroy(&condp);      /* Free up producer condition variable */
    // pthread_attr_destroy(&ThAttr);

    return (EXIT_SUCCESS);
}