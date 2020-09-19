#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#define BUFFERSIZE 100
sigjmp_buf labelA;
void sig_handler(int signo){
	if (signo == SIGUSR1){
		if (signal(SIGUSR1, sig_handler) == SIG_ERR)
        	printf("\ncan't catch SIGUSR1\n");
		printf("recebi!\n");
		siglongjmp(labelA,1000);
	}
}

int main(void){
	char user_in[BUFFERSIZE];
	char comando[BUFFERSIZE];
	char args[BUFFERSIZE];
	int num_args = 0;
	sigsetjmp(labelA,0);
	//char* teste = malloc(10);
	if (signal(SIGUSR1, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGUSR1\n");
	
	strcpy(comando,"/bin/");
	printf("Qual comando você quer usar?\n");
	fgets(user_in,BUFFERSIZE,stdin);
	strtok(user_in,"\n");
	//printf("Entendi: %s\n",user_in);
	strcat(comando,user_in);
	printf("Comando: %s\n",comando);
	printf("Quantos argumentos você quer passar?\n");
	fgets(args,BUFFERSIZE,stdin);
	num_args = atoi(args);
	char *argvector[num_args+2];
	char read_args[num_args][BUFFERSIZE];
	strcpy(argvector[0], user_in);
	argvector[num_args+1]= NULL;
	for(int i=0;i<num_args;i++){
		printf("Digite o argumento %d:\n",i+1);
		fgets(read_args[i],BUFFERSIZE,stdin);
		strtok(read_args[i],"\n");
		argvector[i+1] = read_args[i];
	}

	pid_t child_pid = fork();

	if (child_pid ==0){
		execvp(comando,argvector);
		printf("execl returned! errno is [%d]\n",errno);
		perror("The error message is :");
		return 0;
	}
	else{
		wait(NULL);
		printf("Task is done: next!\n");
		return 0;
	}


}
