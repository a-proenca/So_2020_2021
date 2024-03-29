#include "structs.h"
#define GAMEDIR "Jogo" //caso nao exista v.ambiente fica com este valor
#define MAXPLAYER 30   //caso nao exista v.ambiente fica com este valor
Arbitro a;

void interrupcao()
{
	printf("\nO arbitro foi Interrompido!\n");
	for (int i = 0; i < a.nclientes; i++)
	{
		kill(a.clientes[i].pid, SIGINT);
	}
	unlink(SERV_PIPE);
	exit(EXIT_FAILURE);
}

//Funcao que devolve 1 se encontrar um nome igual
int verificaNome(char *nome)
{
	for (int i = 0; i < a.nclientes; i++)
	{
		if (strcasecmp(a.clientes[i].nome, nome) == 0)
			return 1;
	}
	return 0;
}
//Funcao que conta o numero total de pessoas no campeonato
int contaPessoasNoCampeonato()
{
	int conta = 0;
	for (int i = 0; i < a.nclientes; i++)
	{
		if (a.clientes[i].sair == 0 && strcmp(a.clientes[i].nome_jogo, "") != 0)
		{
			conta++;
		}
	}
	return conta;
}

void guardaJogos()
{
	FILE *f;
	ssize_t read;
	char *line = NULL;
	size_t len = 0;

	f = fopen("jogos.txt", "rt");

	if (f == NULL)
	{
		exit(EXIT_FAILURE);
	}

	while ((read = getline(&line, &len, f)) != -1)
	{
		strcpy(a.jogos[a.n_jogos].identificacao, line);
		a.n_jogos++;
	}

	fclose(f);
}

void eliminaCliente(char *nome)
{
	for (int i = 0; i < a.nclientes; i++)
	{
		if (strcasecmp(a.clientes[i].nome, nome) == 0)
		{
			for (int j = i; j < a.nclientes - 1; j++)
			{
				a.clientes[j] = a.clientes[j + 1];
			}
			a.nclientes--;
			printf("O cliente %s foi eliminado.\n", nome);
			return;
		}
	}
}

void *trata_logins()
{
	Cliente aux; //cliente auxiliar para verificar se o nome do cliente ja existe
	int fd_serv;
	int fd_client;
	int bytes;

	do
	{
		fd_serv = open(SERV_PIPE, O_RDONLY);  //abertura do pipe (read only)
		read(fd_serv, &aux, sizeof(Cliente)); //Leio o cliente

		//Verificar se o cliente quer introduzir #mygame
		if (strcmp(aux.comando, "#mygame") == 0)
		{
			char resp[700];
			for (int i = 0; i < a.nclientes; i++)
			{
				if (strcmp(aux.nome, a.clientes[i].nome) == 0)
				{
					fd_client = open(a.clientes[i].nome_pipe_escrita, O_WRONLY);
					//Escrever no pipe o nome do jogo
					strcpy(resp, "O jogo que esta a jogar e ");
					strcat(resp, a.clientes[i].nome_jogo);
					bytes = write(fd_client, resp, strlen(resp));
					if (bytes == 0)
					{
						printf("[Erro]Nao conseguiu escrever nada no pipe.\n");
					}
					close(fd_client);
				}
			}
		}
		//Trata do comando quit e dos logouts
		else if (strcmp(aux.comando, "#quit") == 0 || aux.sair == 1)
		{
			for (int i = 0; i < a.nclientes; i++)
			{
				if (strcmp(aux.nome, a.clientes[i].nome) == 0)
					a.clientes[i].sair = 1;
			}
			//Se nao tiver acontecer nenhum campeonato
			if (a.FLAG_CAMPEONATO == 0)
			{
				char resp[700];
				eliminaCliente(aux.nome);
				fd_client = open(aux.nome_pipe_escrita, O_RDWR);
				strcpy(resp, "Nao esta a decorrer nenhum jogo. Adeus!");
				write(fd_client, resp, strlen(resp));
				close(fd_client);
			}
		}
		//Trata do login do cliente
		else if (aux.sair == 0)
		{
			//Verificacao do nome
			if (verificaNome(aux.nome) == 0)
			{
				a.clientes[a.nclientes] = aux;
				printf("O jogador %s entrou no jogo.\n", a.clientes[a.nclientes].nome);

				fd_client = open(a.clientes[a.nclientes].nome_pipe_escrita, O_WRONLY);
				bytes = write(fd_client, "Bem-vindo Cliente!", sizeof("Bem-vindo Cliente!"));
				if (bytes == 0)
				{
					printf("[Erro]Nao conseguiu escrever nada no pipe.\n");
				}
				close(fd_client);

				//Vou adicionar um cliente ao vetor dos clientes
				a.nclientes++;
			}
			// O nome é repetido
			else
			{
				fd_client = open(aux.nome_pipe_escrita, O_WRONLY);
				bytes = write(fd_client, "Ja existe um cliente com esse nome!", sizeof("Ja existe um cliente com esse nome!"));
				if (bytes == 0)
				{
					printf("[Erro]Nao conseguiu escrever nada no pipe.\n");
				}
				close(fd_client);
			}
		}
		close(fd_serv);
	} while (a.FLAG_TERMINA == 0);
	pthread_exit(NULL);
}

void comandosMenu()
{
	printf("==========Configuracoes==========\n");
	printf("Listar jogadores em jogo (players)\n");
	printf("Listar jogos disponiveis (games)\n");
	printf("Remover jogador do campeonato (k+nomejogador)\n");
	printf("Suspender a comunicacao entre jogador e jogo (s+nomejogador)\n");
	printf("Retomar a comunicacao entre jogador e jogo (r+nomejogador)\n");
	printf("Encerrar o campeonato (end)\n");
	printf("Sair (exit)\n");
	printf("=================================\n");
}

//Retorna o indice do cliente vencedor
int mostraPontuacao()
{
	int n_vencedores = 0, desistentes = 0, retorna = -1;
	for (int i = 0; i < a.nclientes; i++)
	{
		if (a.clientes[i].vencedor == 1)
			n_vencedores++;
		else
			desistentes++;
		a.clientes[i].atendido = 0;
	}
	int indice = -1, max = -1;
	for (int i = 0; i < n_vencedores; i++)
	{
		max = -1;
		for (int j = 0; j < a.nclientes; j++)
			if (a.clientes[j].atendido == 0 && a.clientes[j].vencedor == 1 && a.clientes[j].pontuacao > max)
			{
				indice = j;
				max = a.clientes[j].pontuacao;
			}
		if (i == 0)
			retorna = indice;
		printf(">Vencedores< %d ) Pontuacao do %s - %d pontos\n", i + 1, a.clientes[indice].nome, a.clientes[indice].pontuacao);
		a.clientes[indice].atendido = 1; //para nao voltar a mostrar o mesmo cliente
	}
	for (int i = 0; i < desistentes; i++)
	{
		max = -1;
		for (int j = 0; j < a.nclientes; j++)
			if (a.clientes[j].atendido == 0 && a.clientes[j].vencedor == 0 && a.clientes[j].pontuacao > max)
			{
				indice = j;
				max = a.clientes[j].pontuacao;
			}
		if (i == 0 && retorna == -1)
			retorna = indice;
		printf(">Desistentes< %d ) Pontuacao do %s - %d pontos\n", i + 1, a.clientes[indice].nome, a.clientes[indice].pontuacao);
		a.clientes[indice].atendido = 1;
	}
	return retorna;
}

void *jogo(void *dados)
{
	Cliente *cli;
	cli = (Cliente *)dados;
	//pipe1 -> write || pipe2 -> read
	int pipe1[2];
	int pipe2[2];
	if (pipe(pipe1) == -1) //verifica se conseguiu criar o pipe1
	{
		fprintf(stderr, "Erro na Criação do Pipe");
		exit(0);
	}
	if (pipe(pipe2) == -1) //verifica se conseguiu criar o pipe2
	{
		fprintf(stderr, "Erro na Criação do Pipe");
		exit(0);
	}
	int res;
	int bytes;
	char resp[700];
	int status;
	int pont_exit;
	int fd_pipe_leitura, fd_pipe_escrita;
	char nomeJogo[50];
	strcpy(nomeJogo,a.gamedir);
	strcat(nomeJogo, "/");
	strcat(nomeJogo, cli->nome_jogo);
	nomeJogo[strlen(nomeJogo) - 1] = '\0';
	printf("%s\n", nomeJogo);

	res = fork();
	if (res == -1)
	{
		printf("Fork falhou.\n");
		exit(0);
	}
	else if (res == 0)
	{
		// Processo filho
		//pipe1 -> write || pipe2 -> read
		close(pipe1[1]);   //fechar parte escrita pipe1
		close(pipe2[0]);   //fechar parte de leitura do pipe2
		dup2(pipe1[0], 0); //redirecionamos a escrita do pipe1
		dup2(pipe2[1], 1); //redirecionamos a leitra do pipe2
		execl(nomeJogo, cli->nome_jogo, NULL);
	}
	else
	{
		//Processo Pai
		close(pipe1[0]); //pipe1 serve para comunicar escrita do arbitro -> jogo
		close(pipe2[1]); //pipe2 server para comunicar leitura do arbitro <- jogo
		fd_pipe_escrita = open(cli->nome_pipe_escrita, O_WRONLY);
		fprintf(stdout, "O jogador %s vai jogar\n", cli->nome);

		do
		{
			memset(resp, 0, sizeof(resp));
			//Se nao estiver a decorrer campeonato, sai
			if (a.FLAG_CAMPEONATO != 1)
			{
				break;
			}
			//Ler a informacao inicial do jogo
			bytes = read(pipe2[0], resp, sizeof(resp));
			if (bytes == -1)
			{
				fprintf(stderr, "O pipe nao conseguiu ler informacao.\n");
			}

			//vou enviar a informacao que li do jogo para o cliente
			while (cli->suspenso == 1)
				sleep(1);
			write(fd_pipe_escrita, resp, strlen(resp));

			//vou ler a informacao enviada pelo cliente
			fd_pipe_leitura = open(cli->nome_pipe_leitura, O_RDONLY);
			while (cli->suspenso == 1)
				sleep(1);

			read(fd_pipe_leitura, &resp, sizeof(resp));
			close(fd_pipe_leitura);
			strcat(resp, "\n");

			//enviar a informacao lida para o jogo
			bytes = write(pipe1[1], &resp, strlen(resp));
			if (bytes == -1)
			{
				fprintf(stderr, "O pipe nao conseguiu escrever informacao.\n");
				exit(0);
			}
		} while (cli->sair == 0);
		//Envia SIGUSR1 ao jogo
		kill(res, SIGUSR1);
	}
	if (waitpid(res, &status, 0) == -1)
	{
		perror("waitpid falhou!");
		exit(0);
	}
	//Recebe pontuacao atraves do exit_status
	if (WIFEXITED(status))
	{
		pont_exit = WEXITSTATUS(status);
		cli->pontuacao = pont_exit;
		close(fd_pipe_escrita);
	}
	pthread_exit(NULL);
}

//Funcao responsavel pela duracao do campeonato
void *campeonato(void *dados)
{
	do
	{
		int dur = a.duracao;
		do
		{
			int esp = a.espera;
			while (a.nclientes < 2 && a.FLAG_TERMINA == 0)
				sleep(1);
			do
			{
				sleep(1);
				esp--;
			} while (esp > 0 && a.FLAG_TERMINA == 0);
		} while (a.nclientes < 2 && a.FLAG_TERMINA == 0);

		if (a.FLAG_TERMINA == 1)
			break;

		a.n_jogosAdecorrer = 0;
		int r = 0;
		int j = 0;
		//Atribuir jogos a clientes
		do
		{
			r = rand() % a.nclientes;
			if (strcmp(a.clientes[r].nome_jogo, "") == 0)
			{
				a.n_jogosAdecorrer++;
				j = rand() % a.n_jogos;
				strcpy(a.clientes[r].nome_jogo, a.jogos[j].identificacao);
				printf("O jogador %s vai jogar o jogo %s\n", a.clientes[r].nome, a.jogos[j].identificacao);
			}
		} while (a.n_jogosAdecorrer < a.maxplayers && a.n_jogosAdecorrer < a.nclientes);

		if (a.FLAG_TERMINA == 1)
			break;

		printf("Comecou campeonato\n");
		a.FLAG_CAMPEONATO = 1;
		int k;
		for (int i = 0, k = 0; i < a.nclientes; i++)
		{
			if (strcmp(a.clientes[i].nome_jogo, "") != 0)
			{ //Se tiver um jogo associado
				strcpy(a.jogosAdecorrer[k].nomejogo, a.clientes[i].nome_jogo);
				strcpy(a.jogosAdecorrer[k].nomecliente, a.clientes[i].nome);
				//Inicializamos a thread para cada cliente
				if (pthread_create(&a.jogosAdecorrer[k].thread, NULL, jogo, (void *)&a.clientes[i]) != 0)
				{
					printf("Erro na criacao da Thread\n");
					exit(0);
				}
				k++;
			}
		}
		do
		{
			sleep(1);
			dur--;
			printf("[Duracao %d]\n", dur);
		} while (dur > 0 && a.FLAG_TERMINA == 0 && a.FLAG_CAMPEONATO == 1 && contaPessoasNoCampeonato() > 1);

		a.FLAG_CAMPEONATO = 0;
		printf("Terminou campeonato.\n");

		for (int i = 0; i < a.nclientes; i++)
		{
			if (a.clientes[i].sair == 0)
				a.clientes[i].vencedor = 1;
			a.clientes[i].suspenso = 0;

			//Para nao ficar a espera do numero jogado pelo cliente
			int fd_cl = open(a.clientes[i].nome_pipe_leitura, O_RDWR);
			write(fd_cl, "\n", strlen("\n"));
			close(fd_cl);
			a.clientes[i].sair = 1; //Terminar a thread
		}

		for (int i = 0; i < a.nclientes; i++)
		{
			kill(a.clientes[i].pid, SIGUSR1);
		}
		sleep(2);

		int vencedor = mostraPontuacao();

		//Avisar clientes da pontuaço e vencedor
		for (int i = 0; i < a.nclientes; i++)
		{
			char resp[700];
			int fd_cl = open(a.clientes[i].nome_pipe_escrita, O_WRONLY);
			sprintf(resp, "A sua pontuacao e %d e o jogador vencedor foi %s", a.clientes[i].pontuacao, a.clientes[vencedor].nome);
			write(fd_cl, resp, strlen(resp));
			close(fd_cl);
		}
		printf("Todos os clientes vao ser eliminados.\n");
		a.nclientes = 0;
	} while (a.FLAG_TERMINA == 0);
	pthread_exit(NULL);
}

//Funcao que tira a primeira letra do comando e devolve o nome do jogador
char *devolve_nome(char comando[TAM])
{
	int i = 0;
	for (i = 0; comando[i]; i++)
	{
		comando[i] = comando[i + 1];
	}
	return comando;
}

void TrataSinais()
{
	if (signal(SIGINT, interrupcao) == SIG_ERR)
	{
		printf("\n [ERRO] Nao foi possivel configurar o sinal SIGINT\n");
		exit(EXIT_FAILURE);
	}

	if (signal(SIGQUIT, interrupcao) == SIG_ERR)
	{
		printf("\n [ERRO] Nao foi possivel configurar o sinal SIGQUIT\n");
		exit(EXIT_FAILURE);
	}
}
int main(int argc, char *argv[])
{
	strcpy(a.gamedir, GAMEDIR);
	a.maxplayers = MAXPLAYER;
	a.FLAG_TERMINA = 0;
	a.FLAG_CAMPEONATO = 0;
	a.nclientes = 0;
	a.n_jogos = 0;
	char comando[TAM];

	srand(time(NULL));

	if (access(SERV_PIPE, F_OK) == 0) //verificar se ja existe algum servidor a correr
	{
		fprintf(stderr, "[Erro] O servidor já existe.\n");
		exit(0);
	}

	//verificacao se foram indicados os argumentos necessarios
	if (argc < 3)
	{
		printf("Nao foi indicado o numero de argumentos necessarios.\n");
		printf("Por favor indique a duração do campeonato e o tempo de espera.\n");
		return 0;
	}
	else if (argc > 3)
	{
		printf("Foram indicados argumentos em excesso.\n");
		printf("Por favor indique apenas a duração do campeonato e o tempo de espera.\n");
		return 0;
	}
	//leitura da linha de comandos
	a.duracao = atoi(argv[1]);
	a.espera = atoi(argv[2]);

	setbuf(stdout, NULL);

	TrataSinais();

	if (mkfifo(SERV_PIPE, 0600) == -1) //verifica se conseguiu criar o named pipe
	{
		fprintf(stderr, "[Erro]na Criação do pipe do servidor.\n");
		exit(0);
	}

	if (getenv("GAMEDIR") != NULL)
		strcpy(a.gamedir, getenv("GAMEDIR"));
	if (getenv("MAXPLAYER") != NULL)
		a.maxplayers = atoi(getenv("MAXPLAYER"));

	guardaJogos();

	pthread_t logins;
	pthread_create(&logins, NULL, (void *)trata_logins, NULL);

	pthread_t thread_campeonato; //Thread que gera o campeonato
	pthread_create(&thread_campeonato, NULL, (void *)campeonato, NULL);

	comandosMenu();
	do
	{
		fflush(stdout);
		printf(">>");
		fflush(stdout);
		fgets(comando, TAM, stdin);
		comando[strlen(comando) - 1] = '\0';
		if (strcasecmp(comando, "players") == 0)
		{
			if (a.nclientes == 0)
			{
				printf("\n Nenhum jogador no campeonato! \n");
			}
			else
			{
				printf("\n Jogadores no campeonato: \n");
				for (int i = 0; i < a.nclientes; i++)
				{
					if (strcasecmp(a.clientes[i].nome_jogo, "") == 0)
					{
						printf("Jogador %d: %s nao esta a jogar nenhum jogo.\n", i + 1, a.clientes[i].nome);
					}
					else
					{
						printf(" Jogador %d: %s esta a jogar o jogo %s \n", i + 1, a.clientes[i].nome, a.clientes[i].nome_jogo);
					}
				}
			}
		}
		else if (strcasecmp(comando, "games") == 0)
		{
			for (int i = 0; i < a.n_jogos; i++)
			{
				printf("%s\n", a.jogos[i].identificacao);
			}
		}
		else if (comando[0] == 'k')
		{
			strcpy(comando, devolve_nome(comando));
			for (int i = 0; i < a.nclientes; i++)
			{
				if (strcasecmp(a.clientes[i].nome, comando) == 0)
				{
					kill(a.clientes[i].pid, SIGUSR2);
				}
			}
			eliminaCliente(comando);
		}
		else if (comando[0] == 's')
		{
			strcpy(comando, devolve_nome(comando));
			for (int i = 0; i < a.nclientes; i++)
			{
				if (strcasecmp(a.clientes[i].nome, comando) == 0 && a.clientes[i].suspenso == 0)
				{
					a.clientes[i].suspenso = 1;
					printf("O jogador %s foi suspenso.\n", a.clientes[i].nome);
					int fd_pipe = open(a.clientes[i].nome_pipe_escrita, O_WRONLY);
					char resp[700];
					strcpy(resp, "Jogador suspenso!\n");
					write(fd_pipe, resp, strlen(resp));
					close(fd_pipe);
				}
			}
		}
		else if (comando[0] == 'r')
		{
			strcpy(comando, devolve_nome(comando));
			for (int i = 0; i < a.nclientes; i++)
			{
				if (strcasecmp(a.clientes[i].nome, comando) == 0 && a.clientes[i].suspenso == 1)
				{
					a.clientes[i].suspenso = 0;
					printf("O jogador %s deixou de estar suspenso.\n", a.clientes[i].nome);
					int fd_pipe = open(a.clientes[i].nome_pipe_escrita, O_WRONLY);
					char resp[700];
					strcpy(resp, "Jogador deixou de estar suspenso!\n");
					write(fd_pipe, resp, strlen(resp));
					close(fd_pipe);
				}
			}
		}
		else if (strcasecmp(comando, "end") == 0)
		{
			a.FLAG_CAMPEONATO = 0;
		}
		else if (strcasecmp(comando, "exit") == 0)
		{
			a.FLAG_TERMINA = 1;
		}
	} while (a.FLAG_TERMINA == 0);
	interrupcao();
	return 0;
}
