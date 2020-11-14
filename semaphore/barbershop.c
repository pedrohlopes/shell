#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_CUSTOMERS 5

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t barber_free = PTHREAD_COND_INITIALIZER;
pthread_cond_t customers_waiting = PTHREAD_COND_INITIALIZER;
pthread_cond_t go_cut = PTHREAD_COND_INITIALIZER;
pthread_cond_t cut_done = PTHREAD_COND_INITIALIZER;

int n_waiting = 0;
int isFull = 0;
int cutting = 0;

void cutHair()
{
    printf("Cliente Cortando o cabelo...\n");
    sleep(1);
}

void getHairCut()
{
    printf("Barbeiro começou o corte!\n");
}

void *barber(void *arg)
{

    printf("Thread do barbeiro iniciada;\n");
    while (1)
    {

        pthread_mutex_lock(&mutex);
        while (n_waiting == 0)
        {
            printf("Barbeiro esperando clientes!\n");
            pthread_cond_wait(&customers_waiting, &mutex);
        }

        pthread_mutex_unlock(&mutex);
        printf("Barbeiro pronto para atender o cliente! \n");

        pthread_mutex_lock(&mutex2);
        pthread_mutex_lock(&mutex);
        n_waiting--;
        printf("Barbeiro trouxe o cliente para a cadeira de corte. Clientes esperando: %d\n", n_waiting);

        pthread_mutex_unlock(&mutex);
        cutting = 1;
        pthread_cond_signal(&go_cut);
        getHairCut();

        pthread_mutex_unlock(&mutex2);

        pthread_mutex_lock(&mutex2);
        while (cutting == 1)
            pthread_cond_wait(&cut_done, &mutex2);
        pthread_mutex_unlock(&mutex2);
    }
}

void balk()
{
    printf("Salão cheio. Cliente indo embora...\n");
    while (1)
    {
        sleep(1);
    }
}

void *customer(void *arg)
{

    printf("Thread de cliente iniciada;\n");
    pthread_mutex_lock(&mutex);
    while (n_waiting >= MAX_CUSTOMERS)
    {
        pthread_mutex_unlock(&mutex);
        balk();
    }
    n_waiting++;
    printf("Cliente tomou posição na fila. Clientes esperando: %d\n", n_waiting);
    pthread_cond_signal(&customers_waiting);
    pthread_mutex_unlock(&mutex);

    pthread_mutex_lock(&mutex2);
    while (cutting == 0)
        pthread_cond_wait(&go_cut, &mutex2);
    cutHair();
    pthread_mutex_unlock(&mutex2);

    pthread_mutex_lock(&mutex2);
    cutting = 0;
    pthread_cond_signal(&cut_done);
    printf("Corte de um cliente terminou!\n");
    pthread_mutex_unlock(&mutex2);

    return NULL;
}

int main(void)
{
    pthread_t pBarber;
    pthread_t pCustomer;
    pthread_t pCustomer2;
    pthread_t pCustomer3;
    pthread_t pCustomer4;
    pthread_t pCustomer5;
    // pthread_t pCustomer6;
    // pthread_t pCustomer7;
    // pthread_t pCustomer8;
    printf("testando\n");
    pthread_create(&pBarber, NULL, barber, NULL);
    pthread_create(&pCustomer, NULL, customer, NULL);
    pthread_create(&pCustomer2, NULL, customer, NULL);
    pthread_create(&pCustomer3, NULL, customer, NULL);
    pthread_create(&pCustomer4, NULL, customer, NULL);
    pthread_create(&pCustomer5, NULL, customer, NULL);
    // pthread_create(&pCustomer6, NULL, customer, NULL);
    // pthread_create(&pCustomer7, NULL, customer, NULL);
    // pthread_create(&pCustomer8, NULL, customer, NULL);
    while (1)
        sleep(1);
    return 1;
}