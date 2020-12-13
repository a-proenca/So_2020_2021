#include <stdio.h>
#include "structs.h"
#include <sys/wait.h>
Cliente c;
int servpid;
char fifo_name[50];
char fifo_name_serv[50];

void acabou_campeonato()
{
  printf("Acabou campeonato.\n");
  //falta a parte da pontuacao
}
//fazer unlink caso o programa seja interrompido ctrl+c;
void interrupcao_c()
{
  printf("\nO programa foi Interrompido!\n");
  unlink(fifo_name);
  unlink(fifo_name_serv);
  exit(EXIT_FAILURE);
}
void interrupcao_ar()
{
  printf("\nO arbitro foi fechado!\n");
  unlink(fifo_name);
  unlink(fifo_name_serv);
  exit(EXIT_FAILURE);
}

int main(int argc, char argv[])
{
  int fd_serv;
  int fd_cli;
  char mensagem_serv[100];
  int bytes;
  char instrucao[TAM];

  if (signal(SIGINT, interrupcao_c) == SIG_ERR)
  {
    printf("\n [ERRO] Nao foi possivel configurar o sinal SIGINT\n");
    exit(EXIT_FAILURE);
  }

  if (signal(SIGQUIT, interrupcao_c) == SIG_ERR)
  {
    printf("\n [ERRO] Nao foi possivel configurar o sinal SIGQUIT\n");
    exit(EXIT_FAILURE);
  }

  if (signal(SIGUSR2, interrupcao_ar) == SIG_ERR)
  {
    printf("\n [ERRO] Nao foi possivel configurar o sinal SIGUSR2\n");
    exit(EXIT_FAILURE);
  }

  if (access(SERV_PIPE, F_OK) != 0)
  { //verifica se existe um servidor a correr
    printf("[Erro]Nao existe nenhum servidor ativo.\n");
    exit(EXIT_FAILURE);
  }
  sprintf(fifo_name_serv, SERV_PIPE_WR, getpid());
  sprintf(fifo_name, CLIENT_PIPE, getpid());

  if (access(fifo_name_serv, F_OK) == 0)
  { // verifica se o pipe ja esta aberto
    printf("\n[ERRO] Cliente ja existe.\n");
    exit(EXIT_FAILURE);
  }

  if (mkfifo(fifo_name_serv, 0600) == -1)
  {
    printf("\n[ERRO] Erro ao criar o pipe do cliente (mkfifo)");
    exit(EXIT_FAILURE);
  }

  if (signal(SIGUSR1, acabou_campeonato) == SIG_ERR)
  {
    printf("\n [ERRO] Nao foi possivel configurar o sinal SIGUSR1\n");
    exit(EXIT_FAILURE);
  }

  fd_serv = open(SERV_PIPE, O_WRONLY); //enviar login ao arbitro
  c.pontuacao = 0;
  c.atendido = 0;
  c.pid = getpid();
  strcpy(c.nome_jogo, "");
  c.sair = 0;

  printf("\nIndique o seu nome:");
  scanf("%s", c.nome);
  bytes = write(fd_serv, &c, sizeof(Cliente));
  if (bytes == 0)
  {
    printf("[Erro]Nao conseguiu escrever nada no pipe.\n");
  }
	
  fd_cli = open(fifo_name_serv, O_RDONLY); // Recebe info do servidor de boas vindas caso a autenticação tenha sido bem sucedida
  bytes = read(fd_cli, &mensagem_serv, sizeof(mensagem_serv));
  if (bytes == 0)
  {
    printf("[Erro]Nao conseguiu ler nada do pipe.\n");
  }
  printf("servidor: %s\n", mensagem_serv);
  
  if (strcasecmp(mensagem_serv, "Bem-vindo Cliente!") != 0)
  {
    c.sair = 1;
  }
  

  close(fd_cli);

  while (c.sair != 1)
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
  }
  //mandar ao servidor para dar quit!

  close(fd_cli);
  unlink(fifo_name_serv);
  return 0;
}
