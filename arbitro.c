#include "structs.h"
#define GAMEDIR "Jogo" //caso nao exista v.ambiente fica com este valor
#define MAXPLAYER 30   //caso nao exista v.ambiente fica com este valor
int duracao;
int espera;
Arbitro a;

int FLAG_TERMINA = 0; //Flag termina o servidor(arbitro)
int TERMINA_CAMPEONATO = 0;

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
		if (a.clientes[i].sair == 0)
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
		}
	}
}

void *trata_logins()
{
	char fifo_name[50];
	Cliente aux; //cliente auxiliar para verificar se o nome do cliente ja existe
	int fd_serv;
	int fd_client;
	int bytes;

	fd_serv = open(SERV_PIPE, O_RDONLY); //abertura do pipe (read only)
	do
	{
		read(fd_serv, &aux, sizeof(Cliente)); //Leio o cliente
		if (aux.sair == 0)
		{
			if (verificaNome(aux.nome) == 0) //Verificacao do nome
			{
				a.clientes[a.nclientes] = aux;
				a.nclientes++; //vou adicionar um cliente
				if (a.clientes[a.nclientes - 1].atendido != 1)
				{
					a.clientes[a.nclientes - 1].atendido = 1;
					//GUARDAR NA ESTRUTURA CLIENTE O NOME DO PIPE DE ESCRITURA
					//sprintf(fifo_name, SERV_PIPE_WR, a.clientes[a.nclientes - 1].pid);
					//strcpy(a.clientes[a.nclientes - 1].nome_pipe_escrita, fifo_name);

					fd_client = open(a.clientes[a.nclientes - 1].nome_pipe_escrita, O_WRONLY);
					printf("O jogador %s entrou no jogo.\n", a.clientes[a.nclientes - 1].nome);

					bytes = write(fd_client, "Bem-vindo Cliente!", sizeof("Bem-vindo Cliente!"));
					if (bytes == 0)
					{
						printf("[Erro]Nao conseguiu escrever nada no pipe.\n");
					}
					close(fd_client);
				}
			}
			else
			{
				sprintf(fifo_name, SERV_PIPE_WR, aux.pid);
				fd_client = open(fifo_name, O_WRONLY);
				bytes = write(fd_client, "Ja existe um cliente com esse nome!", sizeof("Ja existe um cliente com esse nome!"));
				if (bytes == 0)
				{
					printf("[Erro]Nao conseguiu escrever nada no pipe.\n");
				}
				close(fd_client);
			}
		}
		else
		{
			if (FLAG_TERMINA == 1)
			{
				eliminaCliente(aux.nome);
			}
		}

	} while (FLAG_TERMINA == 0);
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

void *jogo(void *dados)
{
	Cliente *cli;
	cli = (Cliente *)dados;
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
	int filho;
	char resp[500];
	char resp1[500];
	char input[20];
	int tamanho = 50;
	int status;
	int pont_exit;
	char fifo_name[50];
	int fd_pipe_leitura, fd_pipe_escrita;
	fd_set fontes;
	struct timeval t;

	res = fork();
	if (res == -1)
	{
		printf("Fork falhou.\n");
		exit(0);
	}
	else if (res == 0)
	{
		for (int i = 0; i < a.n_jogosAdecorrer; i++)
		{
			if (strcmp(a.jogosAdecorrer[i].nomejogo, cli->nome_jogo) == 0)
			{
				a.jogosAdecorrer[i].pid_jogo = getpid();
			}
		}

		// Processo filho
		//Falta gerar um nr aleatorio para escolher um jogo da diretoria
		//pipe1 -> write || pipe2 -> read
		close(pipe1[1]);   //fechar parte escrita pipe1
		close(pipe2[0]);   //fechar parte de leitura do pipe2
		dup2(pipe1[0], 0); //redirecionamos a escrita do pipe1
		dup2(pipe2[1], 1); //redirecionamos a leitra do pipe2
		if (strcmp(cli->nome_jogo, "G_004"))
		{
			execl("Jogo/G_004", cli->nome_jogo, NULL);
		}
		else
		{
			execl("Jogo/G_005", cli->nome_jogo, NULL);
		}
	}
	else
	{
		//Processo Pai

		close(pipe1[0]); //pipe1 serve para comunicar escrita do arbitro -> jogo
		close(pipe2[1]); //pipe2 server para comunicar leitura do arbitro <- jogo
		sprintf(fifo_name, SERV_PIPE_WR, cli->pid);
		fd_pipe_escrita = open(fifo_name, O_WRONLY);
		fprintf(stdout, "O jogador %s vai jogar\n", cli->nome);
		do
		{

			if (TERMINA_CAMPEONATO == 1)
			{
				pthread_exit(NULL);
			}
			//Ler a informacao inicial do jogo
			bytes = read(pipe2[0], resp, sizeof(resp));
			if (bytes == -1)
			{
				fprintf(stderr, "O pipe nao conseguiu ler informacao.\n");
			}
			//ler o printf seguinte do jogo
			bytes = read(pipe2[0], resp1, sizeof(resp1));
			if (bytes == -1)
			{
				fprintf(stderr, "O pipe nao conseguiu ler informacao.\n");
			}
			//juntar a info para enviar pelo pipe
			strcat(resp, resp1);

			//vou enviar a informacao que li do jogo para o cliente
			//fprintf(stdout, "%s\t", resp);
			while (cli->suspenso == 1)
				sleep(1);
			write(fd_pipe_escrita, resp, strlen(resp));
			fd_pipe_leitura = open(cli->nome_pipe_leitura, O_RDONLY);
			printf("Abri o pipe\n");
			//vou ler a informacao enviada pelo cliente
			while (cli->suspenso == 1)
				sleep(1);

			int sele;
			do
			{
				fflush(stdout);
				printf(" ");
				fflush(stdout);
				FD_ZERO(&fontes);
				//FD_SET(0, &fontes);
				FD_SET(fd_pipe_leitura, &fontes);
				t.tv_sec = 1;
				t.tv_usec = 0;

				sele = select(fd_pipe_leitura + 1, &fontes, NULL, NULL, &t);
				printf("RES = %d\n",sele);


				if (sele == 0 )
				{ //caso acabe timeout e ele tiver de sair  && cli->sair == 1
				//	NUNCA ENTRA AQUI -> BUG NO TIMEOUT?
					printf("TIMEOUT.\n");
					if(cli->sair == 1){
						printf("Acabou o jogo. Res = 0\n");
						bytes = write(pipe1[1], "\n", strlen("\n"));
						break;
					}	
				}
				else if (sele > 0 && FD_ISSET(fd_pipe_leitura, &fontes))
				{
					printf("ANTES DO READ.\n");
					read(fd_pipe_leitura, &resp, sizeof(resp));
					printf("LEU.\n");
					strcat(resp, "\n");
					//fprintf(stdout, "NUMERO:%s", resp);
					//enviar a informacao lida para o jogo
					bytes = write(pipe1[1], &resp, strlen(resp));
					if (bytes == -1)
					{
						fprintf(stderr, "O pipe nao conseguiu escrever informacao.\n");
						exit(0);
					}
					break;
				}
			} while (1);

		} while (cli->sair == 0);
	}
	printf("Cheguei AQUI.\n");
	if (waitpid(res, &status, 0) == -1)
	{
		perror("waitpid falhou!");
		exit(0);
	}
	//NAO ESTA A CHEGAR AQUI!
	if (WIFEXITED(status))
	{
		pont_exit = WEXITSTATUS(status);
		printf("A PONTUACAO FINAL FOI: %d\n", pont_exit);
		cli->pontuacao = pont_exit;

		snprintf(resp, sizeof(resp), "A pontuacao e %d", pont_exit);
		write(fd_pipe_escrita, resp, strlen(resp));
		close(fd_pipe_leitura);
		close(fd_pipe_escrita);
	}
}

//Funcao responsavel pela duracao do campeonato
void *campeonato(void *dados)
{
	int dur = duracao, esp = espera;
	TERMINA_CAMPEONATO = 0;
	do
	{
		while (a.nclientes < 2 && FLAG_TERMINA == 0)
			sleep(1);
		do
		{
			sleep(1);
			esp--;
		} while (esp > 0 && FLAG_TERMINA == 0);
	} while (a.nclientes < 2 && FLAG_TERMINA == 0);

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
			j = rand() % a.n_jogos + 1;
			strcpy(a.clientes[r].nome_jogo, a.jogos[j].identificacao);
			printf("O jogador %s vai jogar o jogo %s\n", a.clientes[r].nome, a.jogos[j].identificacao);
		}
	} while (a.n_jogosAdecorrer < a.maxplayers && a.n_jogosAdecorrer < a.nclientes);

	printf("Comecou campeonato\n");
	//pthread_t *thread_jogo; //Thread que inicia o jogo
	int k = 0;
	for (int i = 0, k = 0; i < a.nclientes; i++)
	{
		if (strcmp(a.clientes[i].nome_jogo, "") != 0)
		{ //Se tiver um jogo associado
			strcpy(a.jogosAdecorrer[k].nomejogo, a.clientes[i].nome_jogo);
			strcpy(a.jogosAdecorrer[k].nomecliente, a.clientes[i].nome);
			//Quando a 2 thread começa a 1 pára
			pthread_create(&a.jogosAdecorrer[k].thread, NULL, jogo, (void *)&a.clientes[i]);
			k++;
		}
	}

	do
	{
		sleep(1);
		dur--;
	} while (dur > 0 && FLAG_TERMINA == 0 && TERMINA_CAMPEONATO == 0 && contaPessoasNoCampeonato() > 1);
	TERMINA_CAMPEONATO = 1;

	for (int i = 0; i < a.nclientes; i++)
	{
		printf("A pontuacao de %s foi de %d\n", a.clientes[i].nome, a.clientes[i].pontuacao);
		a.clientes[i].sair = 1;
	}
	/*
	for (int i = 0; i < a.n_jogosAdecorrer; i++)
	{
		pthread_join(a.jogosAdecorrer[i].thread, NULL);
	}
	*/

	for (int i = 0; i < a.nclientes; i++)
	{
		kill(a.clientes[i].pid, SIGUSR1);
	}
	/*

	for (int i = 0; i < a.n_jogosAdecorrer; i++)
	{
		kill(a.jogosAdecorrer[i].pid_jogo, SIGUSR1);
	}
	*/
	printf("Terminou campeonato.\n");

	//free(thread_jogo);
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

int main(int argc, char *argv[])
{
	strcpy(a.gamedir, GAMEDIR);
	a.maxplayers = MAXPLAYER;
	a.nclientes = 0;
	a.n_jogos = 0;
	char comando[TAM];

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
	duracao = atoi(argv[1]);
	espera = atoi(argv[2]);

	printf("duracao = %d;espera = %d\n", duracao, espera);

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

	setbuf(stdout, NULL);

	if (mkfifo(SERV_PIPE, 0600) == -1) //verifica se conseguiu criar o named pipe
	{
		fprintf(stderr, "[Erro]na Criação do pipe do servidor.\n");
		exit(0);
	}

	if (getenv("GAMEDIR") != NULL)
		strcpy(a.gamedir, getenv("GAMEDIR"));
	if (getenv("MAXPLAYER") != NULL)
		a.maxplayers = atoi(getenv("MAXPLAYER"));

	printf("gamedir = %s;maxplayers = %d\n", a.gamedir, a.maxplayers);

	guardaJogos();
	pthread_t *logins;
	logins = (pthread_t *)malloc(sizeof(pthread_t));

	pthread_create(logins, NULL, (void *)trata_logins, NULL);

	pthread_t *thread_campeonato; //Thread que gera o campeonato
	thread_campeonato = (pthread_t *)malloc(sizeof(pthread_t));

	pthread_create(thread_campeonato, NULL, (void *)campeonato, NULL);

	//write()pontuacao

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
				if (strcasecmp(a.clientes[i].nome, comando) == 0)
				{
					a.clientes[i].suspenso = 1;
					printf("O jogador %s foi suspenso.\n", a.clientes[i].nome);
				}
			}
		}
		else if (comando[0] == 'r')
		{
			strcpy(comando, devolve_nome(comando));
			for (int i = 0; i < a.nclientes; i++)
			{
				if (strcasecmp(a.clientes[i].nome, comando) == 0)
				{
					a.clientes[i].suspenso = 0;
					printf("O jogador %s deixou de estar suspenso.\n", a.clientes[i].nome);
				}
			}
		}
		else if (strcasecmp(comando, "end") == 0)
		{
			TERMINA_CAMPEONATO = 1;
		}
		else if (strcasecmp(comando, "exit") == 0)
		{
			FLAG_TERMINA = 1;
		}
	} while (FLAG_TERMINA == 0);
	free(thread_campeonato);
	free(logins);
	interrupcao();
	return 0;
}
