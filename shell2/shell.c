#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <setjmp.h>
#include <ctype.h>
// definindo tamanho máximo das strings e
// número máximo de argumentos
#define BUFFERSIZE 1000
#define MAX_ARGS 1000

/* Variável global para monitorar os sinais do usuário.
 Foi utilizada uma variável global devido a necessidade
 de alterá-la a partir do signal handler, que por si é
 um elemento global */
int flag = 0;

// Função para printar a mensagem de acordo com o estado do programa

void print_message(int passo, int count_args)
{
	// Vetor com mensagens necessárias
	char message[3][BUFFERSIZE] = {
		"Qual comando você quer usar?\n",
		"Quantos argumentos você quer passar?\n"
		"",
	};

	// Checando se está no passo 3, onde precisa ficar pedindo os argumentos

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
	// verifica se o sinal é o sinal que realmente devemos tratar
	if (signo == SIGUSR1)
	{
		printf("Recebi um sinal, voltando para o Passo 1:\n");
		// avisa ao programa para voltar o passo para o primeiro
		flag = 1;
		// printa novamente a mensagem do primeiro passo
		print_message(0, -1);
	}
}

int isNumeric(const char *s)
{
	if (s == NULL || *s == '\0' || isspace(*s))
		return 0;
	char *p;
	strtod(s, &p);
	return *p == '\0';
}

int main(void)
{
	// variável para guardar o comando digitado pelo usuário
	char comando[BUFFERSIZE];
	// variável para o full path do comando
	char path_comando[BUFFERSIZE];
	// variável para leitura do número de argumentos
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
	// variável para checar se a entrada do numero de argumentos é inteira
	char *check = malloc(3);
	// variável para leitura dos argumentos
	char read_args[MAX_ARGS][BUFFERSIZE];

	//colocando o path raiz na variável do path do comando
	strcpy(path_comando, "/bin/");

	if (signal(SIGUSR1, sig_handler) == SIG_ERR)
		printf("\ncan't catch SIGUSR1\n");

	while (passo_atual < 3) // loop para pegar entradas do usuário
	{
		if (passo_atual >= 2) // se já pegamos o comando e a quantidade de argumentos, printamos a mensagem com o número do argumento
		{
			print_message(passo_atual, count_args);
		}
		else // se não, printamos a mensagem do passo atual
		{
			print_message(passo_atual, -1);
		}

		fgets(buf, BUFFERSIZE, stdin); // lendo a entrada do usuário

		// se por acaso recebemos alguma interrupção ao estar esperando a entrada do usuário,
		// a proxima entrada deverá ser computada como a primeira de todas (voltamos para o passo 1)
		// aqui, independente de onde a interrupção chegar, ela nos mostrará a primeira pergunta
		// e na próxima vez que formos computar a entrada do usuário já entenderemos que essa será
		// o nome do comando (primeira entrada)
		if (flag != 0)
		{
			passo_atual = 0;
			flag = 0;
		}

		switch (passo_atual) // colocamos a entrada do usuário nas variáveis devidas dependendo do passo
		{
		case 0: // primeiro passo: lemos o comando.
			strcpy(comando, buf);
			passo_atual++;
			count_args = -1;
			break;
		case 1: // segundo passo: lemos o número de argumentos.
			strcpy(args, buf);
			*check = '\0';
			num_args = (int)strtol(args, &check, 10);
			if (*check != '\n' || num_args < 0)
			{
				printf("Erro: O número de argumentos deve ser um valor inteiro positivo.\n");
				return 1;
			}

			if (num_args == 0)
			{
				passo_atual++;
			}
			passo_atual++;
			count_args++;
			break;
		case 2: // terceiro passo: vamos lendo os argumentos, até termos num_args argumentos preenchidos.
			if (count_args >= num_args - 1)
			{
				passo_atual++;
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

	// com tudo em seu lugar, fazemos a rotina para executar o comando que o usuário pediu.

	strtok(comando, "\n");
	strcat(path_comando, comando);
	printf("Comando: %s\n", path_comando); //comando no path /bin/ +comando

	char *argvector[num_args + 2]; // variável para colocar o nome do comando e os argumentos em um vetor
	argvector[0] = (char *)malloc(BUFFERSIZE);
	strcpy(argvector[0], comando); // primeiro entra o nome do comando

	argvector[num_args + 1] = NULL;	   // execvp precisa que o ultimo elemento seja NULL para saber onde parar
	for (int i = 0; i < num_args; i++) // vai preenchendo os argumentos que foram lidos depois da posição do comando
	{
		argvector[i + 1] = (char *)malloc(BUFFERSIZE);
		argvector[i + 1] = read_args[i];
	}

	pid_t child_pid = fork(); // cria o processo filho, como vimos na disciplina

	if (child_pid == 0)
	{
		execvp(path_comando, argvector);				  // executa o comando que o usuário entrou
		printf("Comando inválido, errno: [%d]\n", errno); // se houve um erro (por ex. comando inválido/não encontrado), printa o errno e a mensagem
		perror("Mensagem de erro :");
		return 0; // finaliza o filho
	}
	else
	{
		wait(NULL); // espera o filho
		printf("Acabou!\n");
		return 0; // finaliza o pai
	}
}
