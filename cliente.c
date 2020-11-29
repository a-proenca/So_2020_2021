#include <stdio.h>
#include "structs.h"
#include <sys/wait.h>
Cliente c;
int servpid;

void acabou_campeonato()
{
  printf("Acabou campeonato.\n");
  //falta a parte da pontuacao
}

int main(int argc, char argv[])
{
  char fifo_name[20];
  int fd_serv;
  int nb;
  int fd_cli;
  char teste[50];
  int bytes;
  char instrucao[TAM];

  if (access(SERV_PIPE, F_OK) != 0)
  { //verifica se existe um servidor a correr
    printf("[Erro]Nao existe nenhum servidor ativo.\n");
    exit(EXIT_FAILURE);
  }
  sprintf(fifo_name, CLIENT_PIPE, getpid());

  if (access(fifo_name, F_OK) == 0)
  { // verifica se o pipe ja esta aberto
    printf("\n[ERRO] Cliente ja existe.\n");
    exit(EXIT_FAILURE);
  }

  if (mkfifo(fifo_name, 0600) == -1)
  {
    printf("\n[ERRO] Erro ao criar o pipe do cliente (mkfifo)");
    exit(EXIT_FAILURE);
  }
  c.pontuacao = 0;
  c.pid = getpid();
  strcpy(c.nome_jogo,"");
  c.sair = 0;

  printf("\nIndique o seu nome: ");
  scanf("%s", c.nome);

  printf("Nome = %s\n", c.nome);

  if (signal(SIGUSR1, acabou_campeonato) == SIG_ERR)
  {
    printf("\n [ERRO] Nao foi possivel configurar o sinal SIGUSR1\n");
    exit(EXIT_FAILURE);
  }

  fd_serv = open(SERV_PIPE, O_WRONLY); //enviar login ao arbitro
  bytes = write(fd_serv, &c, sizeof(Cliente));
  if (bytes == 0)
  {
    printf("[Erro]Nao conseguiu escrever nada no pipe.\n");
  }

  fd_cli = open(fifo_name, O_RDONLY); // Recebe info do servidor
  bytes = read(fd_cli, &teste, sizeof(teste));
  if (bytes == 0)
  {
    printf("[Erro]Nao conseguiu ler nada do pipe.\n");
  }
  printf("Li do servidor: %s\n", teste);

  do
  {
    fflush(stdout);
    fgets(instrucao, TAM, stdin);
    instrucao[strlen(instrucao) - 1] = '\0';
    if (strcasecmp(instrucao, "#mygame") == 0)
    {
      if (strcasecmp(c.nome_jogo, "") == 0)
      {
        printf("Nao esta nenhum jogo a decorrer.\n");
      }
      else
      {
        printf("O jogo que esta a jogar e o %s\n", c.nome_jogo);
      }
    }
    else if (strcasecmp(instrucao, "#quit") == 0)
    {
      c.sair = 1;
    }
  } while (c.sair != 1);
  close(fd_cli);
  unlink(CLIENT_PIPE);
  return 0;
}
