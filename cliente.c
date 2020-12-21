#include <stdio.h>
#include "structs.h"
#include <sys/wait.h>
Cliente c;
int servpid;
int fd_serv;
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
  int bytes;
  c.sair = 1;
  printf("\nO programa foi interrompido!\n");
  bytes = write(fd_serv, &c, sizeof(Cliente));
  if (bytes == 0)
  {
    printf("[Erro]Nao conseguiu escrever nada no pipe.\n");
  }
  unlink(fifo_name);
  unlink(fifo_name_serv);
  exit(EXIT_FAILURE);
}
void interrupcao_ar()
{
  printf("\nO jogador foi encerrado pelo arbitro!\n");
  unlink(fifo_name);
  unlink(fifo_name_serv);
  exit(EXIT_FAILURE);
}
//Vou escrever para o arbitro no pipe -> SERV_PIPE_WR
//Vou ler do servidor pelo CLIENT_PIPE

int main(int argc, char argv[])
{
  int fd_cli;
  char mensagem_serv[100];
  int bytes;
  char instrucao[TAM];
  int fd_servidor;
  char resp[500];
  char msg[500]="";

  setbuf(stdout, NULL);
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
  //GUARDAR NA ESTRUTURA CLIENTE O NOME DO PIPE DE ESCRITA(ARB->CLI)
  strcpy(c.nome_pipe_escrita, fifo_name_serv);
  //GUARDAR NA ESTRUTURA CLIENTE O NOME DO PIPE DE LEITURA(ARB <- CLI)
  strcpy(c.nome_pipe_leitura, fifo_name);

  if (access(fifo_name, F_OK) == 0)
  { /// verifica se o pipe ja esta aberto
    printf("\n[ERRO] Cliente ja existe.\n");
    exit(EXIT_FAILURE);
  }
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
  if (mkfifo(fifo_name, 0600) == -1)
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

  fd_cli = open(fifo_name_serv, O_RDONLY);
  fd_servidor = open(c.nome_pipe_leitura, O_WRONLY);

  fd_set fontes;
  while (c.sair != 1)
  {

    fflush(stdout);
    FD_ZERO(&fontes);
    FD_SET(0, &fontes);      //preparar para receber do teclado
    FD_SET(fd_cli, &fontes); //preparar para receber do pipe
    int res = select(fd_cli + 1, &fontes, NULL, NULL, NULL);
    if (res > 0 && FD_ISSET(0, &fontes))
    { //se tiver a receber algo do teclado
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
    else if (res > 0 && FD_ISSET(fd_cli, &fontes))
    { //se receber algo do pipe
      bytes = read(fd_cli, &resp, sizeof(resp));
      if (bytes == -1)
      {
        fprintf(stderr, "O pipe nao conseguiu ler informacao proveniente do arbitro.\n");
      }

      printf("Jogo: %d %s\n", bytes, resp);
      fflush(stdin);
      scanf("%s",msg);
      msg[strlen(msg)] = '\0';

      //enviar a info ao arbitro

      bytes = write(fd_servidor, msg, sizeof(msg));
      if (bytes == -1)
      {
        fprintf(stderr, "O pipe nao conseguiu escrever a informacao para o arbitro.\n");
      }
    }
  }
  //mandar ao servidor para dar quit!
  bytes = write(fd_serv, &c, sizeof(Cliente));
  if (bytes == 0)
  {
    printf("[Erro]Nao conseguiu escrever nada no pipe.\n");
  }
  close(fd_cli);
  unlink(fifo_name_serv);
  return 0;
}
