
// Resolução do problema da seção 6.1: The search-insert-delete problem

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

/* Neste problema, temos uma solução bem simples utilizando as funções do tipo
pthread_rwlock. Assim, podemos definir locks de dois tipos: "escrita" e "leitura".
Várias threads podem pegar um mesmo lock como "leitura" ao mesmo tempo, desde que nenhuma
outra thread tenha adquirido o lock como "escrita". Assim, podemos fazer as threads buscadoras
e as insersoras compartilharem esse lock como leitura e as deletoras como escrita. Fazendo isso,
as threads buscadoras não se bloqueiam entre si e as threads insersoras não bloqueiam as buscadoras,
mas as deletoras bloqueiam todas as outras. Agora, só precisamos de mais um lock para garantir
que as insersoras terão exclusão mútua entre si e as deletoras também. Para isso, podemos usar um
mutex simples, que é compartilhado entre as insersoras e deletoras.
*/

pthread_rwlock_t rwmutex = PTHREAD_RWLOCK_INITIALIZER; // lock de read-write para as buscadoras e insersoras
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;     // lock de exclusão mútua simples para as insersoras e deletoras

void search() // função dummy de busca
{
    printf("Procurando item...\n");
    sleep(1);
}

void insert() // função dummy de inserção
{
    printf("Inserindo item...\n");
    sleep(1);
}

void del() // função dummy de deleção
{
    printf("Deletando item...\n");
    sleep(1);
}

void *searcher(void *arg)
{
    printf("Thread de busca iniciada;\n");
    pthread_rwlock_rdlock(&rwmutex); // se não há threads deletoras, pode procurar!
    search();
    pthread_rwlock_unlock(&rwmutex);
    return NULL;
}
void *inserter(void *arg)
{
    printf("Thread de inserção iniciada;\n");
    pthread_rwlock_rdlock(&rwmutex); // se não há threads deletoras, pode pegar esse lock para inserir
    pthread_mutex_lock(&mutex);      // se também não há outras insersoras nem deletoras, pode inserir!
    insert();
    pthread_mutex_unlock(&mutex);
    pthread_rwlock_unlock(&rwmutex);
    return NULL;
}

void *deleter(void *arg)
{
    printf("Thread de deleção iniciada;\n");
    pthread_rwlock_wrlock(&rwmutex); // se não há threads buscadoras nem threads insersoras, pode pegar esse lock para deletar
    pthread_mutex_lock(&mutex);      // se não há outras deletoras nem insersoras, pode deletar!
    del();
    pthread_mutex_unlock(&mutex);
    pthread_rwlock_unlock(&rwmutex);
    return NULL;
}

// Rotina de testes para a solução, com algumas threads de cada tipo, depois o programa fica em loop para nao
// terminar antes do tempo

// notar que, pelo tempo, as buscadoras não se excluem mutuamente e as insersoras não bloqueiam as procuras, então
// o comportamento é como esperado

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