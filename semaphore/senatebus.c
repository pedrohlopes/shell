#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#define BUS_CAPACITY 50
#define NUM_PASSENGERS 7

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t rider_done= PTHREAD_COND_INITIALIZER;
pthread_cond_t bus_ready = PTHREAD_COND_INITIALIZER;
pthread_cond_t bus_departed = PTHREAD_COND_INITIALIZER;
pthread_cond_t cleared_waiting_next = PTHREAD_COND_INITIALIZER;
int bus_boarding = 0;
int waiting_next = 0;

int riders = 0;

void *bus(void *arg)
{
    printf("Ônibus a caminho!\n");
    pthread_mutex_lock(&mutex3);
    pthread_mutex_lock(&mutex);
    printf("Chegou um Ônibus!\n");
    bus_boarding = 1;
    printf("Ônibus Pronto para embarcar os passageiros!\n");
    
    while (riders > 0)
    {
        pthread_cond_signal(&bus_ready);
        printf("Ônibus esperando passageiros subirem!\n");
        pthread_cond_wait(&rider_done, &mutex);
        riders--;
        printf("Passageiro subiu! Passageiros para subir: %d\n", riders);
    }
    printf("Não há passageiros prontos para embarcar. Onibus saindo!\n");

    bus_boarding = 0;
    pthread_cond_broadcast(&bus_departed);
    pthread_mutex_unlock(&mutex);

    pthread_mutex_unlock(&mutex);
    while((waiting_next%BUS_CAPACITY)>0){
        pthread_cond_wait(&cleared_waiting_next,&mutex);
    }
    pthread_mutex_unlock(&mutex);
    pthread_mutex_unlock(&mutex3);
    return NULL;
}

void *rider(void *arg)
{
    printf("Chegou um Passageiro!\n");
    pthread_mutex_lock(&mutex);
    while (bus_boarding == 1 || riders>=BUS_CAPACITY)
    {
        waiting_next++;
        pthread_cond_wait(&bus_departed, &mutex);
        waiting_next--;
        if (waiting_next%BUS_CAPACITY==0)
            pthread_cond_signal(&cleared_waiting_next);
    }
    riders++;
    printf("Passageiro esperando para subir. Passageiros para subir: %d\n", riders);
    pthread_cond_wait(&bus_ready, &mutex);
    printf("Passageiro subindo...\n");
    pthread_cond_signal(&rider_done);

    pthread_mutex_unlock(&mutex);
    
    
    return NULL;
}

int main(void)
{
    pthread_t pBus1;
    pthread_t pBus2;
    pthread_t pBus3;
    pthread_t pRiders[NUM_PASSENGERS];
   
    for (int i = 0; i < NUM_PASSENGERS; i++)
    {
        pthread_create(&pRiders[i], NULL, rider, NULL);
    }
    pthread_create(&pBus1, NULL, bus, NULL);
    pthread_create(&pBus2, NULL, bus, NULL);
    pthread_create(&pBus3, NULL, bus, NULL);

    while (1)
        sleep(1);
    return 1;
}