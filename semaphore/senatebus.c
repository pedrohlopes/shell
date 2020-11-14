#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#define BUS_CAPACITY 3
#define NUM_PASSENGERS 3

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t riders_waiting = PTHREAD_COND_INITIALIZER;
pthread_cond_t bus_waiting = PTHREAD_COND_INITIALIZER;
pthread_cond_t bus_departed = PTHREAD_COND_INITIALIZER;
pthread_cond_t cleared_waiting_next = PTHREAD_COND_INITIALIZER;
int bus_boarding = 0;
int waiting_next = 0;

int riders = 0;

void *bus(void *arg)
{
    printf("Ônibus a caminho!\n");
    pthread_mutex_lock(&mutex2);
    printf("Chegou um Ônibus!\n");
    while (waiting_next > 0)
    {
        pthread_cond_wait(&cleared_waiting_next, &mutex2)
    }
    bus_boarding = 1;
    printf("Ônibus começou a embarcar os passageiros!\n");
    pthread_mutex_unlock(&mutex2);

    pthread_mutex_lock(&mutex);
    while (riders > 0)
    {
        pthread_cond_signal(&bus_waiting);
        printf("Ônibus esperando passageiros subirem!\n");
        pthread_cond_wait(&riders_waiting, &mutex);
        riders--;
        printf("Passageiro subiu! Passageiros para subir: %d\n", riders);
    }
    printf("Não há passageiros esperando. Onibus saindo!\n");

    pthread_mutex_lock(&mutex2);
    bus_boarding = 0;
    pthread_cond_broadcast(&bus_departed);
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *rider(void *arg)
{
    printf("Chegou um Passageiro!\n");
    pthread_mutex_lock(&mutex2);
    while (riders >= BUS_CAPACITY || bus_boarding == 1)
    {
        printf("Não foi possivel embarcar. Esperando próximo ônibus.\n");
        waiting_next++;
        pthread_cond_wait(&bus_departed, &mutex2);
    }
    pthread_mutex_unlock(&mutex2);
    pthread_mutex_lock(&mutex);
    riders++;
    printf("Passageiro esperando para subir. Passageiros para subir: %d\n", riders);
    pthread_cond_wait(&bus_waiting, &mutex);
    printf("Passageiro subindo...\n");
    pthread_cond_signal(&riders_waiting);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(void)
{
    pthread_t pBus1;
    pthread_t pBus2;
    pthread_t pBus3;
    pthread_t pRiders[NUM_PASSENGERS];
    pthread_create(&pBus1, NULL, bus, NULL);
    for (int i = 0; i < NUM_PASSENGERS; i++)
    {
        pthread_create(&pRiders[i], NULL, rider, NULL);
    }
    pthread_create(&pBus2, NULL, bus, NULL);
    pthread_create(&pBus3, NULL, bus, NULL);

    while (1)
        sleep(1);
    return 1;
}