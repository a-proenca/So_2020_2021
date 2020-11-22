#include "structs.h"
#define GAMEDIR "Jogo" //caso nao exista v.ambiente fica com este valor
#define MAXPLAYER 30   //caso nao exista v.ambiente fica com este valor


/*
void * Logins(){
	cliente ci;
	int fd_serv=open(SERV_PIPE,ORDONLY);
	read(fd_serv,&ci,sizeof(cliente));

}
*/

void comandosMenu()
{
	printf("==========Configuracoes==========\n");
	printf("Começar Campeonato\n");
	printf("Listar jogadores em jogo (players)\n");
	printf("Listar jogos disponiveis (games)\n");
	printf("Remover jogador do campeonato (k+nomejogador)\n");
	printf("Suspender a comunicacao entre jogador e jogo (s+nomejogador)\n");
	printf("Retomar a comunicacao entre jogador e jogo (r+nomejogador)\n");
	printf("Encerrar o campeonato (end)\n");
	printf("Sair (exit)\n");
	printf("=================================\n");
}

void comeca_campeonato(int duracao, Arbitro *a)
{
	sleep(duracao * 1000); //espera a duracao

	//kill(pid_jogo, SIGUSR1);

	for (int i = 0; i < a->nclientes; i++)
	{
		kill(a->clientes[i].pid, SIGUSR1);
	}
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
	int duracao;
	int espera;
	char gamedir[TAM] = GAMEDIR;
	int maxplayers = MAXPLAYER;
	Arbitro a;
	a.nclientes = 0;
	char comando[TAM];
	int res;
	int pipe1[2];
	int pipe2[2];

	if(pipe(pipe1)==-1){
		fprintf(stderr,"Erro na Criação do Pipe");
		exit(0);
	}
	if(pipe(pipe2)==-1){
		fprintf(stderr,"Erro na Criação do Pipe");
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

	res = fork();
	if(res== 0){ 
		// Processo filho
		//Falta gerar um nr aleatorio para escolher um jogo da diretoria
		//pipe1 -> write || pipe2 -> read
		// Cliente -> Jogo
		close(pipe1[1]);//fechar parte escrita pipe1
		close(pipe2[0]);//fechar parte de leitura do pipe2
		dup2(pipe1[0],0); //redirecionamos a escrita do pipe1
		dup2(pipe2[1],1); //redirecionamos a leitra do pipe2
		
		execl("/Jogo/G_004","G_004",NULL); 
	}
	else{ 
		//MADEIRAO
		close(pipe1[0]);
		close(pipe2[1]);
	    

	}
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
					printf(" Jogador %d: %s esta a jogar o jogo %s \n", i + 1, a.clientes[i].nome, a.clientes[i].jogo.identificacao);
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
	} while (strcasecmp(comando, "exit") != 0);
	return 0;
}
