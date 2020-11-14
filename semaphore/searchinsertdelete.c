#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_rwlock_t rwmutex = PTHREAD_RWLOCK_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void search()
{
    printf("Procurando item...\n");
    sleep(1);
}

void insert()
{
    printf("Inserindo item...\n");
    sleep(1);
}

void del()
{
    printf("Deletando item...\n");
    sleep(1);
}

void *searcher(void *arg)
{
    printf("Thread de busca iniciada;\n");
    pthread_rwlock_rdlock(&rwmutex);
    search();
    pthread_rwlock_unlock(&rwmutex);
    return NULL;
}
void *inserter(void *arg)
{
    printf("Thread de inserção iniciada;\n");
    pthread_rwlock_rdlock(&rwmutex);
    pthread_mutex_lock(&mutex);
    insert();
    pthread_mutex_unlock(&mutex);
    pthread_rwlock_unlock(&rwmutex);
    return NULL;
}

void *deleter(void *arg)
{
    printf("Thread de deleção iniciada;\n");
    pthread_rwlock_wrlock(&rwmutex);
    pthread_mutex_lock(&mutex);
    del();
    pthread_mutex_unlock(&mutex);
    pthread_rwlock_unlock(&rwmutex);
    return NULL;
}

int main(void)
{
    pthread_t pSearcher1;
    pthread_t pSearcher2;
    pthread_t pSearcher3;
    pthread_t pInserter1;
    pthread_t pInserter2;
    pthread_t pDeleter1;
    pthread_t pDeleter2;

    pthread_create(&pInserter1, NULL, inserter, NULL);
    pthread_create(&pInserter2, NULL, inserter, NULL);
    pthread_create(&pSearcher1, NULL, searcher, NULL);
    pthread_create(&pDeleter1, NULL, deleter, NULL);
    pthread_create(&pDeleter2, NULL, deleter, NULL);
    pthread_create(&pSearcher2, NULL, searcher, NULL);
    pthread_create(&pSearcher3, NULL, searcher, NULL);
    while (1)
        sleep(1);
    return 1;
}