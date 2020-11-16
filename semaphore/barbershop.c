// Resolução do problema da seção 5.2: The Barbershop Problem

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_CUSTOMERS 3 // numero n de cadeiras na espera

/* Neste problema, podemos pensar em dois recursos que as threads clientes
vão precisar: um lugar na fila e a cadeira do barbeiro.
Quando o cliente chega, ele tenta pegar um lugar na fila, chamando a função balk() 
se não conseguir e nunca mais volta. Caso consiga, ele fica esperando pela cadeira
do barbeiro ficar livre, que ocorre com um aviso do mesmo.
Pelo ponto de vista do barbeiro, precisamos nos preocupar com gerenciar a fila
e avisar aos clientes a hora certa de ir à cadeira para cortar o cabelo.

Assim, precisamos garantir uma exclusão mútua na ação de preencher a fila com
a ação de retirar clientes da fila (número de clientes esperando precisa ser protegido)
e outra exclusão mútua para quem está cortando o cabelo, em que só uma pessoa poderá
realizar essa ação em cada momento (variável da situação do barbeiro precisa aser protegida).
*/

pthread_mutex_t mutexLine = PTHREAD_MUTEX_INITIALIZER;       // usamos esse mutex para garantir a exclusão mútua na fila
pthread_mutex_t mutexChair = PTHREAD_MUTEX_INITIALIZER;      // esse aqui para a cadeira do barbeiro
pthread_cond_t barber_free = PTHREAD_COND_INITIALIZER;       // essa variável de condição serve para o barbeiro sinalizar as threads a hora de virem cortar o cabelo
pthread_cond_t customers_waiting = PTHREAD_COND_INITIALIZER; // essa variável serve para dizer ao barbeiro que ele não poderá estar dormindo
pthread_cond_t go_cut = PTHREAD_COND_INITIALIZER;            // essa variável serve para sincronizar a ação do barbeiro com a do cliente da vez
pthread_cond_t cut_done = PTHREAD_COND_INITIALIZER;          // variável que indica o final do corte para realizar a liberação do cliente

int n_waiting = 0; // variável de estado para saber quantas pessoas tem no estado de espera
int cutting = 0;   // variável de estado para dizer se está ou não acontecendo corte de cabelo

void cutHair() // função dummy de corte de cabelo - cliente
{
    printf("Cliente Cortando o cabelo...\n");
    sleep(1);
}

void getHairCut() // função dummy de corte de cabelo = barbeiro
{
    printf("Barbeiro começou o corte!\n");
}

void *barber(void *arg)
{

    printf("Thread do barbeiro iniciada;\n");
    while (1)
    {

        pthread_mutex_lock(&mutexLine); // vou verificar n_waiting, então preciso do mutex
        while (n_waiting == 0)          // se não tem cliente, barbeiro dorme
        {
            printf("Barbeiro esperando clientes!\n");
            pthread_cond_wait(&customers_waiting, &mutexLine); // barbeiro espera até ter algum cliente
        }

        pthread_mutex_unlock(&mutexLine);                     // já fiz o que queria com n_waiting
        printf("Barbeiro pronto para atender o cliente! \n"); // há clientes, então o barbeiro acordou!

        pthread_mutex_lock(&mutexChair); // vou trazer alguem pra cadeira, então vou modificar cutting
        pthread_mutex_lock(&mutexLine);  // vou tirar gente da fila, então vou modificar n_waiting
        n_waiting--;                     // tirando cliente da fila
        printf("Barbeiro trouxe o cliente para a cadeira de corte. Clientes esperando: %d\n", n_waiting);

        pthread_mutex_unlock(&mutexLine); // como podem chegar clientes enquanto estou cortando o cabelo, solto o mutex da fila
        cutting = 1;                      // comecei o processo de cortar o cabelo
        pthread_cond_signal(&go_cut);     // comunicando o cliente que ele tem que cortar o cabelo
        getHairCut();                     // função de cortar o cabelo - barbeiro
        while (cutting == 1)              // enquanto o cliente ainda nao terminou, espera
            pthread_cond_wait(&cut_done, &mutexChair);
        pthread_mutex_unlock(&mutexChair); // solto o mutex
    }
}

void balk() // função para atender o requisito que é imposto no texto (se está cheio, chame a função balk() que nunca retorna)
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
    pthread_mutex_lock(&mutexLine); // preciso incrementar o numero de pessoas na fila, entao pego o mutex
    while (n_waiting >= MAX_CUSTOMERS)
    {
        pthread_mutex_unlock(&mutexLine); // solto o mutex se o cliente n vai fazer nada
        balk();                           // nunca retorna
    }
    n_waiting++; // incrementando a fila
    printf("Cliente tomou posição na fila. Clientes esperando: %d\n", n_waiting);
    pthread_cond_signal(&customers_waiting); // avisando ao barbeiro que tem gente esperando
    pthread_mutex_unlock(&mutexLine);        // soltando o mutex pq já terminei

    pthread_mutex_lock(&mutexChair); // pegando o mutex da cadeira pra esperar o barbeiro chamar
    while (cutting == 0)             // enquanto nao chama e nao tem ninguem cortando, espera
        pthread_cond_wait(&go_cut, &mutexChair);
    cutHair();                      // se chamou, corta o cabelo
    cutting = 0;                    // acabou o corte, entao não estará mais cortando
    pthread_cond_signal(&cut_done); // avisando ao barbeiro que o corte terminou e tá tudo certo
    printf("Corte de um cliente terminou!\n");
    pthread_mutex_unlock(&mutexChair); // solta o mutex finalmente

    return NULL;
}

// Rotina de testes para a solução, com alguns clientes e o barbeiro, depois o programa fica em loop para nao
// terminar antes do tempo
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