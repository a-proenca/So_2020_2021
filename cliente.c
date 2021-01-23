#include "structs.h"

Cliente c;

//Funcao que trata de identificar o cliente
int identificacao()
{
  char mensagem_serv[50];
  int bytes;

  //verifica se o servidor ainda esta ativo
  if (access(SERV_PIPE, F_OK) != 0)
  {
    printf("[Erro]Nao existe nenhum servidor ativo.\n");
    exit(EXIT_FAILURE);
  }
  int fd_serv = open(SERV_PIPE, O_WRONLY); //enviar login ao arbitro
  c.pontuacao = 0;
  c.atendido = 0;
  c.suspenso = 0;
  c.vencedor = 0;
  strcpy(c.comando, "");
  c.pid = getpid();
  strcpy(c.nome_jogo, "");
  c.sair = 0;

  printf("\nIndique o seu nome:");
  scanf("%s", c.nome);

  //Enviar a estrutura cliente ao arbitro
  bytes = write(fd_serv, &c, sizeof(Cliente));
  if (bytes == 0)
  {
    printf("[Erro]Nao conseguiu escrever nada no pipe.\n");
  }
  close(fd_serv);

  //Recebe info do servidor de boas vindas caso a autenticação tenha sido bem sucedida
  int fd_cli = open(c.nome_pipe_escrita, O_RDONLY);
  bytes = read(fd_cli, &mensagem_serv, sizeof(mensagem_serv));
  if (bytes == 0)
  {
    printf("[Erro]Nao conseguiu ler nada do pipe.\n");
  }
  printf("servidor: %s\n", mensagem_serv);

  if (strcasecmp(mensagem_serv, "Bem-vindo Cliente!") != 0) //Por exemplo quando ha 1 cliente com o mesmo nome
  {
    c.sair = 1;
    close(fd_cli);
    unlink(c.nome_pipe_escrita);
    unlink(c.nome_pipe_leitura);
    return 0;
  }
  close(fd_cli);
  return 1;
}

//Quando acaba o campeonato mostra a pontuacao e pergunta se quer continuar a jogar
void acabou_campeonato()
{
  char resp[700];
  char letra;

  printf("Acabou campeonato.\n");
  //Ler a pontuacao e o jogador vencedor
  int fd_cli = open(c.nome_pipe_escrita, O_RDONLY);
  int bytes = read(fd_cli, &resp, sizeof(resp));
  if (bytes == -1)
  {
    fprintf(stderr, "O pipe nao conseguiu ler informacao proveniente do arbitro.\n");
  }
  //Mostro a pontucao e o vencedor
  printf(" %s\n", resp);
  close(fd_cli);
  do
  {
    printf("Quer voltar a jogar? (s/n)\n");
    scanf(" %c", &letra);
  } while (letra != 's' && letra != 'n');

  if (letra == 'n')
  {
    c.sair = 1;
  }
  else
  {
    if (identificacao() == 0)
      exit(0);
  }
}

//Fazer unlink caso o programa seja interrompido ctrl+c;
void interrupcao_c()
{
  int bytes;
  c.sair = 1;
  printf("\nO programa foi interrompido!\n");
  int fd_serv = open(SERV_PIPE, O_WRONLY);
  bytes = write(fd_serv, &c, sizeof(Cliente));
  if (bytes == 0)
  {
    printf("[Erro]Nao conseguiu escrever nada no pipe.\n");
  }
  close(fd_serv);
  unlink(c.nome_pipe_leitura);
  unlink(c.nome_pipe_escrita);
  exit(EXIT_FAILURE);
}
void interrupcao_ar()
{
  printf("\nO jogador foi encerrado pelo arbitro!\n");
  unlink(c.nome_pipe_leitura);
  unlink(c.nome_pipe_escrita);
  exit(EXIT_FAILURE);
}

void TrataSinais()
{
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
  if (signal(SIGUSR1, acabou_campeonato) == SIG_ERR)
  {
    printf("\n [ERRO] Nao foi possivel configurar o sinal SIGUSR1\n");
    exit(EXIT_FAILURE);
  }
}

void TrataNamedPipes()
{
  //GUARDAR NA ESTRUTURA CLIENTE O NOME DO PIPE DE ESCRITA(ARB->CLI)
  sprintf(c.nome_pipe_escrita, SERV_PIPE_WR, getpid());
  //GUARDAR NA ESTRUTURA CLIENTE O NOME DO PIPE DE LEITURA(ARB <- CLI)
  sprintf(c.nome_pipe_leitura, CLIENT_PIPE, getpid());

  //verifica se existe um servidor a correr
  if (access(SERV_PIPE, F_OK) != 0)
  {
    printf("[Erro]Nao existe nenhum servidor ativo.\n");
    exit(EXIT_FAILURE);
  }
  // verifica se o pipe ja esta aberto
  if (access(c.nome_pipe_leitura, F_OK) == 0)
  {
    printf("\n[ERRO] Cliente ja existe.\n");
    exit(EXIT_FAILURE);
  }
  if (access(c.nome_pipe_escrita, F_OK) == 0)
  {
    printf("\n[ERRO] Cliente ja existe.\n");
    exit(EXIT_FAILURE);
  }
  //Trata da abertura dos pipes
  if (mkfifo(c.nome_pipe_escrita, 0600) == -1)
  {
    printf("\n[ERRO] Erro ao criar o pipe do cliente (mkfifo)");
    exit(EXIT_FAILURE);
  }
  if (mkfifo(c.nome_pipe_leitura, 0600) == -1)
  {
    printf("\n[ERRO] Erro ao criar o pipe do cliente (mkfifo)");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char argv[])
{
  int bytes;
  char instrucao[TAM];
  int fd_serv, fd;

  setbuf(stdout, NULL);

  TrataNamedPipes();
  TrataSinais();

  if (identificacao() == 0)
    return 0;

  //Abertura do pipe para podermos receber a info do arbitro
  int fd_cli = open(c.nome_pipe_escrita, O_RDWR);
  fd_set fontes;
  while (c.sair != 1)
  {
    fflush(stdout);
    printf(">>");
    fflush(stdout);

    FD_ZERO(&fontes);
    FD_SET(0, &fontes);      //preparar para receber do teclado
    FD_SET(fd_cli, &fontes); //preparar para receber do pipe
    int res = select(fd_cli + 1, &fontes, NULL, NULL, NULL);

    //se tiver a receber algo do teclado
    if (res > 0 && FD_ISSET(0, &fontes))
    {
      fgets(instrucao, TAM, stdin);
      instrucao[strlen(instrucao) - 1] = '\0';

      //Se for um digito significa que e info para o jogo
      if (isdigit(instrucao[0]))
      {
        fd = open(c.nome_pipe_leitura, O_WRONLY);
        bytes = write(fd, instrucao, sizeof(instrucao));
        if (bytes == -1)
        {
          fprintf(stderr, "O pipe nao conseguiu escrever a informacao para o arbitro.\n");
        }
        close(fd);
      }

      // Comando #mygame ou #quit
      if (strcasecmp(instrucao, "#mygame") == 0 || strcasecmp(instrucao, "#quit") == 0)
      {
        strcpy(c.comando, instrucao);
        fd_serv = open(SERV_PIPE, O_WRONLY);
        bytes = write(fd_serv, &c, sizeof(c));
        if (bytes == -1)
        {
          fprintf(stderr, "O pipe nao conseguiu escrever a informacao para o arbitro.\n");
        }
        close(fd_serv);
        //Apaga o comando
        strcpy(c.comando, "");
      }
    }
    //se receber algo do pipe
    else if (res > 0 && FD_ISSET(fd_cli, &fontes))
    {
      char resp[700];
      memset(resp, 0, sizeof(resp));
      //Vou ler a info proveniente do jogo, ou dos comandos
      bytes = read(fd_cli, &resp, sizeof(resp));
      if (bytes == -1)
      {
        fprintf(stderr, "O pipe nao conseguiu ler informacao proveniente do arbitro.\n");
      }

      printf(" %s\n", resp);
      if (strcasecmp(resp, "Nao esta a decorrer nenhum jogo. Adeus!") == 0)
        c.sair = 1;
    }
  }
  close(fd_cli);
  unlink(c.nome_pipe_escrita);
  unlink(c.nome_pipe_leitura);
  return 0;
}
