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
#define MAX 10000
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

int itemCount, lastCount = 0;
int maxProd, maxCons;

void *(*Produtor)(void *);
void *(*Consumidor)(void *);

list_t* begin = NULL;
list_t* last = NULL;
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

    while (FOREVER) {
        sem_wait(&empty);
        sem_wait(&mutex);

        if(itemCount == 0){
            begin = (list_t*) malloc(sizeof(list_t));
            begin->item = itemCount + 1;
            begin->next = NULL;
        }else{
            push();
        }

        itemCount++;
        maxProd++;
        printf("Produtor - item %d produzido\n", begin->item);

        sem_post(&mutex);
        sem_post(&full);

        if(maxProd == MAX){
            printf("Produtor produziu %d itens. Encerrando produtor\n", MAX);
            break;
        }

        usleep(10);
    }

    return NULL;
}

void *Consumidor_Sem(void *args){
    printf("Iniciando Consumidor com semaforo");

    while (FOREVER){
        sem_wait(&full);
        sem_wait(&mutex);

        if(itemCount == 1){
            free(begin);
            begin = NULL;
            last = NULL;
            printf("Consumidor - item 1 consumido\n");
        }else{
            printf("Consumidor - item %d consumido\n", begin->item);
            pop();
        }

        itemCount--;
        maxCons++;
        sem_post(&mutex);
        sem_post(&empty);
        
        if(maxCons == MAX){
            printf("Consumidor consumiu %d itens. Encerrando consumidor\n", MAX);
            break;
        }

        usleep(10);
    }

    return NULL;
}

void *Produtor_sleepWakeup(void *args){
    printf("Iniciando Produtor com sleep wakeup\n");
    usleep(100);

    while (FOREVER){

        if (itemCount < 0 || itemCount > BUFFER_SIZE){
            printf("-produtor fora dos limites");
            exit(EXIT_FAILURE);
        }

        
        pthread_mutex_lock(&the_mutex); /* protect buffer */
        printf("Produtor - Bloqueando Buffer\n");

        if (itemCount == BUFFER_SIZE)
            printf("Produtor - Sem espaço para produzir, liberando buffer\n");
        while (itemCount == BUFFER_SIZE){
            pthread_cond_wait(&condp, &the_mutex);
        }

        if(itemCount == 0){
            begin = (list_t*) malloc(sizeof(list_t));
            begin->item = itemCount + 1;
            begin->next = NULL;
        }else{
            push();
        }
        lastCount = itemCount;
        itemCount++;
        maxProd++;
        
        printf("Produtor - item %d produzido\n", begin->item);
        
        if (lastCount == 0){
            pthread_cond_signal(&condc); /* wake up consumer */
        }
        pthread_mutex_unlock(&the_mutex);

        printf("Produtor - Liberando Buffer\n");

        if(maxProd == MAX){
            printf("Produtor produziu %d itens. Encerrando produtor\n", MAX);
            break;
        }

        usleep(10);
    }

    return NULL;
}

void *Consumidor_sleepWakeup(void *args){
    printf("Iniciando Consumidor com sleep wakeup");
    while (FOREVER){

        if (itemCount < 0 || itemCount > BUFFER_SIZE){
            printf("-consumidor fora dos limites");
            exit(EXIT_FAILURE);
        }

        pthread_mutex_lock(&the_mutex); /* protect buffer */
        printf("Consumidor - Bloqueando Buffer\n");

        if (itemCount == 0)
            printf("Consumidor - Sem nada para consumir, esperando produtor\n");
      	while(itemCount == 0){
            pthread_cond_wait(&condc, &the_mutex);            
        }

        if(itemCount == 1){
            free(begin);
            begin = NULL;

            last = NULL;
            printf("Consumidor - item 1 consumido\n");
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
        maxCons++;

        if (lastCount == BUFFER_SIZE){
            pthread_cond_signal(&condp); /* wake up consumer */
        }
        pthread_mutex_unlock(&the_mutex);

        printf("Consumidor - Liberando Buffer\n");

        if(maxCons == MAX){
            printf("Consumidor consumiu %d itens. Encerrando consumidor\n", MAX);
            break;
        }

        usleep(10);
    }

    return NULL;
}

int main(int argc, char *argv[]){
    int ThErr1 = 0, ThErr2 = 0;

    itemCount = 0;
    maxProd = 0;
    maxCons = 0;

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

    return (EXIT_SUCCESS);
}