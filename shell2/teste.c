#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>

int flag = 0;
void sig_handler(int signo)
{
    if (signo == SIGUSR1)
    {
        if (signal(SIGUSR1, sig_handler) == SIG_ERR)
            printf("\ncan't catch SIGUSR1\n");
        printf("recebi!\n");
        flag = 1;
    }
}

int main(void)
{
    char user_in[100];
    int flag = 0;
    if (signal(SIGUSR1, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGUSR1\n");
    while (fgets(user_in, 100, stdin) && flag != 0)
    {
        sleep(1);
    }
}