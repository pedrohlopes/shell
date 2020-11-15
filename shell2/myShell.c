#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define BUFFERSIZE 100
#define MAX_ARGS 200

int main(void){
	char user_in[BUFFERSIZE];
	char comando[BUFFERSIZE];
	char read_arg[BUFFERSIZE];
	int num_words = 0;
	char read_args[MAX_ARGS][BUFFERSIZE];
	char * aux;
	int argcount =1;
	
	//char* teste = malloc(10);
	
	while(1){
		printf("shellPedro>");
		fgets(user_in,BUFFERSIZE,stdin);
		strcpy(comando,"/bin/");
		argcount = 1;
		strtok(user_in,"\n");
		
		strcpy(read_arg," ");
		int i=0;
		while(read_arg[0]!='\0'){
			if (i==0){
				strcpy(read_arg,strtok(user_in," "));
				strcpy(read_args[i],read_arg);
			}
			else{
				aux = strtok(NULL," ");
				if (aux != NULL){
					strcpy(read_arg,aux);
					strcpy(read_args[i],read_arg);
					argcount++;
				}
				else{
					read_arg[0] = '\0';
				}	
			}
			i++;
		}
		//printf("Entendi: %s\n",user_in);
		num_words = argcount;
		char *argvector[num_words+1];
		strcat(comando,read_args[0]);

		argvector[num_words]= NULL;
		for(int i=0;i<num_words;i++){
			argvector[i] = read_args[i];
			
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
		}

	}
	

}
