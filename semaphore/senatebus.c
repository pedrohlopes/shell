
// Resolução do problema da seção 7.4: The Senate Bus problem

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#define BUS_CAPACITY 5
#define NUM_PASSENGERS 20

/* No problema, temos que garantir que somente um ônibus embarque pessoas e,
ao mesmo tempo, que as pessoas só possam entrar pra fila de embarque quando não existem
ônibus em processo de embarque. Para isso, podemos primeiro criar um lock que garante
que apenas um ônibus estará em processo de embarque. Além disso, precisamos travar a fila
quando um ônibus começou o processo de embarque e ir soltando este lock sincronizadamente
com as threads de passageiros conforme os mesmos sobem no ônibus, então usamos outro mutex.
Também precisamos manter controle do estado do onibus (pronto para embarcar, embarcando, indo embora).
Pelo lado do passageiro, precisamos manter controle das filas e sincronizar a subida no ônibus. Além disso,
se o ônibus está em processo de embarque ou já tem mais de BUS_CAPACITY pessoas esperando, então precisamos
fazer uma outra fila para o onibus subsequente.
*/

pthread_mutex_t mutexRiders = PTHREAD_MUTEX_INITIALIZER;        // Controle da fila que vai subir no próximo ônibus que chegar
pthread_mutex_t mutexBus = PTHREAD_MUTEX_INITIALIZER;           // Lock para apenas um ônibus estar em processo de embarque
pthread_cond_t rider_done = PTHREAD_COND_INITIALIZER;           // Condição para dizer que o passageiro subiu no ônibus com sucesso
pthread_cond_t bus_ready = PTHREAD_COND_INITIALIZER;            // Condição para indicar que o ônibus começou o processo de embarque
pthread_cond_t bus_departed = PTHREAD_COND_INITIALIZER;         // Condição para dizer que acabou o processo de embarque de um ônibus
pthread_cond_t cleared_waiting_next = PTHREAD_COND_INITIALIZER; // Condição para controlar a fila do próximo ônibus

int bus_boarding = 0; // Estado do ônibus (embarcando ou nao)
int waiting_next = 0; // Se há pessoas na fila para o próximo ônibus (sem ser o que está embarcando)

int riders = 0; // Quantidade de pessoas prontas para o embarque

void *bus(void *arg)
{
    printf("Ônibus a caminho!\n");
    pthread_mutex_lock(&mutexBus);    // Garantindo que somente um ônibus estará embarcando
    pthread_mutex_lock(&mutexRiders); // Pegando o lock para garantir que ninguem vai mexer na fila nem no estado do onibus
    printf("Chegou um Ônibus!\n");
    bus_boarding = 1; // começou o processo de embarque
    printf("Ônibus Pronto para embarcar os passageiros!\n");

    while (riders > 0) // enquanto há passageiros prontos para embarcar, tente embarcar passageiros
    {
        pthread_cond_signal(&bus_ready); // avisa passageiros esperando
        printf("Ônibus esperando passageiros subirem!\n");
        pthread_cond_wait(&rider_done, &mutexRiders); // espera passageiro subir
        riders--;                                     // decrementa o num de passageiros prontos para subir
        printf("Passageiro subiu! Passageiros para subir: %d\n", riders);
    }
    printf("Não há passageiros prontos para embarcar. Onibus saindo!\n");

    bus_boarding = 0;                      // acabou o processo de embarque
    pthread_cond_broadcast(&bus_departed); // onibus saiu
    pthread_mutex_unlock(&mutexRiders);    // solto a fila, que pode ser formada novamente

    pthread_mutex_lock(&mutexRiders); // garanto que o proximo ônibus so pode começar o processo de embarque
    // quando as pessoas tiverem sido realocadas para a fila de prontos para embarcar
    while ((waiting_next % BUS_CAPACITY) > 0) // realocando pessoas na fila de prontos para o embarque de forma que no máximo BUS_CAPACITY pessoas sejam alocadas.
    // Exemplo: se tem 20 pessoas esperando para 5 lugares no onibus subsequente, precisamos colocar mais 5 na fila de prontos
    // para o embarque. Calculando o resto, conseguimos fazer isso, já que vamos começar com resto 4 e ir decrementando até 0 (19,18,17,16,15).
    {
        pthread_cond_wait(&cleared_waiting_next, &mutexRiders); // tenta colocar pessoas na fila até o resto chegar em 0
    }
    pthread_mutex_unlock(&mutexRiders); // acabou o trabalho do onibus, soltando os mutexes
    pthread_mutex_unlock(&mutexBus);
    return NULL;
}

void *rider(void *arg)
{

    pthread_mutex_lock(&mutexRiders);
    printf("Chegou um Passageiro!\n");
    while (bus_boarding == 1 || riders >= BUS_CAPACITY) // se já há ônibus embarcando ou já chegou no número máximo de pessoas, a thread fica esperando pra poder
    // entrar na fila de prontos para embarque
    {
        waiting_next++;
        pthread_cond_wait(&bus_departed, &mutexRiders);
        waiting_next--;                                 // se o onibus saiu, Entra na fila de prontos. Se ainda não dá, o while volta e esse cara volta pra fila de esperando próximo onibus.
        if (waiting_next % BUS_CAPACITY == 0)           // garantindo que a lógica do resto funciona. se ainda temos riders>= capacity, tenta decrementar então até o resto ser 0.
            pthread_cond_signal(&cleared_waiting_next); // avisa que já acabamos de esvaziar o que dava da fila de esperando próximo
    }
    riders++; // passageiro entrou para a fila de prontos para embarque
    printf("Passageiro esperando para subir. Passageiros para subir: %d\n", riders);
    pthread_cond_wait(&bus_ready, &mutexRiders); // Esperando ônibus começar rotina de embarque
    printf("Passageiro subindo...\n");
    pthread_cond_signal(&rider_done); // quando sinalizado, passageiro entra no ônibus

    pthread_mutex_unlock(&mutexRiders); // acabou! soltando o mutex

    return NULL;
}

// Rotina de testes para a solução, com algumas threads de cada tipo, depois o programa fica em loop para nao
// terminar antes do tempo

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