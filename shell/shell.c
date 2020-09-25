#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>

// definindo tamanho máximo das strings e
// número máximo de argumentos
#define BUFFERSIZE 100
#define MAX_ARGS 100

/*Variável global para monitorar o passo do programa.
 Foi utilizada uma variável global devido a necessidade
 de alterar o mesmo a partir do signal handler, que é uma
 variável global*/
int global_passo = 0;

// Função para printar a mensagem de acordo com o estado do programa

void print_message(int passo, int count_args)
{
	// Vetor com mensagens necessárias
	char message[3][BUFFERSIZE] = {
		"Qual comando você quer usar?\n",
		"Quantos argumentos você quer passar?\n"
		"",
	};

	//Checando se está no passo 3, onde precisa ficar pedindo os argumentos

	if (count_args >= 0)
	{
		printf("Digite o argumento %d:\n", count_args + 1);
	}
	// se não está, printa uma mensagem do vetor, de acordo com o passo

	else
	{
		printf("%s", message[passo]);
	}
}

// função para lidar com o sinal do usuário
void sig_handler(int signo)
{
	//verifica se o sinal é o sinal que realmente devemos tratar
	if (signo == SIGUSR1)
	{
		printf("Recebi um sinal, voltando para o Passo 1:\n");
		// volta o passo para o primeiro
		global_passo = 0;
		//printa novamente a mensagem do primeiro passo
		print_message(global_passo, -1);
	}
}

int main(void)
{
	// variável para guardar o comando digitado pelo usuário
	char comando[BUFFERSIZE];
	// variável para o full path do comando
	char path_comando[BUFFERSIZE];
	// variável para leitura dos argumentos
	char args[BUFFERSIZE];
	// variável para leitura do buffer de entrada
	char buf[BUFFERSIZE];
	// variável para contar quantos argumentos já foram digitados
	// (só faz sentido no passo 3, então começa com -1)
	int count_args = -1;
	// variável para guardar o número de argumentos que o usuário deseja
	int num_args = 0;
	// variável para guardar o passo localmente
	int passo_atual = 0;

	char read_args[MAX_ARGS][BUFFERSIZE];

	//colocando o path raiz na variável do path do comando
	strcpy(path_comando, "/bin/");

	if (signal(SIGUSR1, sig_handler) == SIG_ERR)
		printf("\ncan't catch SIGUSR1\n");

	while (global_passo < 3)
	{

		if (global_passo >= 2)
		{
			print_message(global_passo, count_args);
		}
		else
		{
			print_message(global_passo, -1);
		}

		fgets(buf, BUFFERSIZE, stdin);
		passo_atual = global_passo;
		switch (passo_atual)
		{
		case 0:
			strcpy(comando, buf);
			global_passo++;
			count_args = -1;
			break;
		case 1:
			strcpy(args, buf);
			num_args = atoi(args);
			if (num_args == 0)
			{
				global_passo++;
			}
			global_passo++;
			count_args++;
			break;
		case 2:
			if (count_args >= num_args - 1)
			{
				global_passo++;
				strcpy(read_args[count_args], buf);
				strtok(read_args[count_args], "\n");

				count_args++;
			}
			else
			{
				strcpy(read_args[count_args], buf);
				strtok(read_args[count_args], "\n");
				count_args++;
			}
			break;
		default:
			break;
		}
	}

	strtok(comando, "\n");
	//printf("Entendi: %s\n",user_in);
	strcat(path_comando, comando);
	printf("Comando: %s\n", path_comando);
	char *argvector[num_args + 2];
	argvector[0] = (char *)malloc(BUFFERSIZE);
	//memset(argvector[0], '\0', BUFFERSIZE - 2);
	strcpy(argvector[0], comando);

	argvector[num_args + 1] = NULL;
	for (int i = 0; i < num_args; i++)
	{
		argvector[i + 1] = (char *)malloc(BUFFERSIZE);
		argvector[i + 1] = read_args[i];
	}

	pid_t child_pid = fork();

	if (child_pid == 0)
	{
		execvp(path_comando, argvector);
		printf("execl returned! errno is [%d]\n", errno);
		perror("The error message is :");
		return 0;
	}
	else
	{
		wait(NULL);
		printf("Task is done!\n");
		return 0;
	}
}
