#include "structs.h"
#define GAMEDIR "Jogo" //caso nao exista v.ambiente fica com este valor
#define MAXPLAYER 30   //caso nao exista v.ambiente fica com este valor
int duracao;
Arbitro a;

int FLAG_TERMINA = 0; //Flag termina o servidor(arbitro)

//FAZER A VERIFICACÂO DO NOME ATRAVES DO ARBITRO
/*
void terminaJogo()
{
	close(pipe2[0]);
	kill(filho, SIGUSR2);
}
*/

void interrupcao(){
  
  printf("\nO arbitro foi Interrompido!");
  for(int i=0; i<a.nclientes; i++){
	  kill(a.clientes[i].pid, SIGUSR2);
  }
  unlink(SERV_PIPE);
  exit(EXIT_FAILURE);
}

void* trata_logins(){
	char fifo_name[50];
	do{

    int fd_serv;
	int fd_client;
	int bytes;

	fd_serv = open(SERV_PIPE, O_RDONLY); //abertura do pipe (read only)
	bytes = read(fd_serv, &a.clientes[a.nclientes], sizeof(Cliente));
	if (bytes == 0)
	{
		printf("[Erro]Nao conseguiu ler nada do pipe.\n");
	}
	a.nclientes++; //vou adicionar um cliente
	if(a.clientes->atendido!=1){
		
		sprintf(fifo_name, SERV_PIPE_WR, a.clientes[a.nclientes - 1].pid);
		a.clientes->atendido=1;
		strcpy(a.clientes->nome_pipe_escrita,fifo_name);
		fd_client=open(a.clientes->nome_pipe_escrita,O_WRONLY);
		printf("O jogador %s entrou no jogo.\n", a.clientes[a.nclientes - 1].nome);
		//close(fd_serv);??
		bytes = write(fd_client, "Bem-vindo Cliente!", sizeof("Bem-vindo Cliente!"));
		if (bytes == 0)
		{
		printf("[Erro]Nao conseguiu escrever nada no pipe.\n");
		}
		close(fd_client);
	}

	}while(1);
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

//Funcao responsavel pela duracao do campeonato
void *duracao_campeonato(void *dados /*int duracao, Arbitro *a*/)
{

	int dur = duracao;
	printf("Comecou campeonato\n");
	do
	{
		sleep(1);
		dur--;
	} while (dur > 0 && FLAG_TERMINA == 0);
	for (int i = 0; i < a.n_jogos; i++)
	{
		kill(a.jogos[i].pid_jogo, SIGUSR1);
	}

	for (int i = 0; i < a.nclientes; i++)
	{
		kill(a.clientes[i].pid, SIGUSR1);
	}
	printf("Terminou campeonato.\n");
	pthread_exit(NULL);

	//fechar apenas o jogo
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
	int fd_cli;
	int pont_exit;
	int status;
	int fd_serv;
	char fifo_name[20];
	int espera;
	int bytes = 0;
	char gamedir[TAM] = GAMEDIR;
	int maxplayers = MAXPLAYER;
	a.nclientes = 0;
	a.n_jogos = 0;
	char comando[TAM];
	int res;
	int pipe1[2];
	int pipe2[2];
	int filho;

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

	setbuf(stdout,NULL);
	
	if (access(SERV_PIPE, F_OK) == 0) //verificar se ja existe algum servidor a correr
	{
		fprintf(stderr, "[Erro] O servidor já existe.\n");
		exit(0);
	}
	
	if (mkfifo(SERV_PIPE, 0600) == -1) //verifica se conseguiu criar o named pipe
	{
		fprintf(stderr, "[Erro]na Criação do pipe do servidor.\n");
		exit(0);
	}


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

	if (getenv("GAMEDIR") != NULL)
		strcpy(gamedir, getenv("GAMEDIR"));
	if (getenv("MAXPLAYER") != NULL)
		maxplayers = atoi(getenv("MAXPLAYER"));

	printf("gamedir = %s;maxplayers = %d\n", gamedir, maxplayers);

    pthread_t *logins;
	logins = (pthread_t *)malloc(sizeof(pthread_t));

	pthread_create(logins,NULL,(void *) trata_logins,NULL);
	//pthread_t logins

	pthread_t *threads; //Thread que verifica a duracao do campeonato
	threads = (pthread_t *)malloc(sizeof(pthread_t));
/*
	if(a.nclientes>=2){
	pthread_create(threads, NULL, (void *)duracao_campeonato, NULL);

	}
*/	
	//pipe(pipe1);
	//pipe(pipe2);
	res = fork();
	char resp[200];
	char input[20];
	int tamanho=50;
	
	if(res == -1){
		printf("Fork falhou.\n");
		exit(0);
	}
	else if (res == 0)
	{
		// Processo filho
		//Falta gerar um nr aleatorio para escolher um jogo da diretoria
		//pipe1 -> write || pipe2 -> read
		close(pipe1[1]);												 //fechar parte escrita pipe1
		close(pipe2[0]);												 //fechar parte de leitura do pipe2
		dup2(pipe1[0], 0);												 //redirecionamos a escrita do pipe1
		dup2(pipe2[1], 1);												 //redirecionamos a leitra do pipe2
		execl("Jogo/G_004", "G_004", NULL);
		//exit(0);
		//free(threads);
	}
	else
	{
		//Processo Pai
		
		close(pipe1[0]); //pipe1 serve para comunicar escrita do arbitro -> jogo
		close(pipe2[1]); //pipe2 server para comunicar leitura do arbitro <- jogo
		//while(1){
			bytes = read(pipe2[0], resp, sizeof(resp));
			if(bytes == -1){
				fprintf(stderr,"O pipe nao conseguiu ler informacao.\n");
				//exit(0);
			}
		
			resp[bytes] = '\0';
	
			fprintf(stdout,"%s\t",resp);
		
		    strcpy(input,"1");
			//fprintf(stdin,"%d",input);
			//scanf("%d",&input);
			strcat(input,"\n");
			bytes = write(pipe1[1], &input, strlen(input));
			if(bytes == -1){
				fprintf(stderr,"O pipe nao conseguiu escrever informacao.\n");
				exit(0);
			}
			
			
		//}
		
	//Fd_serv trata de logins
/*	
	fd_serv = open(SERV_PIPE, O_RDONLY); //abertura do pipe (read only)
	bytes = read(fd_serv, &a.clientes[a.nclientes], sizeof(Cliente));
	if (bytes == 0)
	{
		printf("[Erro]Nao conseguiu ler nada do pipe.\n");
	}
	a.nclientes++; //vou adicionar um cliente
	printf("O jogador %s entrou no jogo.\n", a.clientes[a.nclientes - 1].nome);

	sprintf(fifo_name, CLIENT_PIPE, a.clientes[a.nclientes - 1].pid);
	fd_cli = open(fifo_name, O_WRONLY); //fd_cli trata de enviar info
	
	//>>falta ir buscar a pontuacao ao jogos dar a pontuacao c.pontuacao = x;
	//bytes=write(fd_serv,&c,sizeof(Cliente));
	bytes = write(fd_cli, "Bem-vindo Cliente!", sizeof("Bem-vindo Cliente!"));
	if (bytes == 0)
	{
		printf("[Erro]Nao conseguiu escrever nada no pipe.\n");
	}
*/
	//write()pontuacao
	
	comandosMenu();
	do
	{
		fflush(stdout);
		printf("\nIndroduza o comando: ");
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
			if (a.n_jogos == 0)
			{
				printf("\n Nenhum jogo disponivel!\n");
			}
			else
			{
				printf("\nJogos disponiveis: \n");
				for (int i = 0; i < a.n_jogos; i++)
				{
					printf("Jogo %d: %s\n", i + 1, a.jogos->identificacao);
				}
			}
		}
		else if (comando[0] == 'k')
		{
			//falta implementar
			printf("O comando inserido foi %s\n", comando);
			strcpy(comando, devolve_nome(comando));
			printf("O nome do jogador e: %s\n", comando);
		}
		else if (comando[0] == 's')
		{
			//falta implementar
			printf("O comando inserido foi %s\n", comando);
			strcpy(comando, devolve_nome(comando));
			printf("O nome do jogador e: %s\n", comando);
		}
		else if (comando[0] == 'r')
		{
			//falta implementar
			printf("O comando inserido foi %s\n", comando);
			strcpy(comando, devolve_nome(comando));
			printf("O nome do jogador e: %s\n", comando);
		}
		else if (strcasecmp(comando, "end") == 0)
		{
			//falta implementar
			printf("O comando inserido foi %s\n", comando);
		}
		else if (strcasecmp(comando, "exit") == 0)
		{
			FLAG_TERMINA = 1;
		}
	} while (FLAG_TERMINA == 0);
  }
  if( waitpid(res, &status, 0) == -1){
		perror("waitpid falhou!");
		return EXIT_FAILURE;
	}

	if(WIFEXITED(status) ){
		pont_exit = WEXITSTATUS(status);
		printf("A PONTUACAO FINAL FOI: %d\n",pont_exit);
	}

  	unlink(SERV_PIPE);
	close(fd_serv);
	return 0;
}
