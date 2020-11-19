#include <stdio.h>
#include "structs.h"
#include <sys/wait.h>
cliente c;
int servpid;


void acabou_campeonato()
{
  printf("Acabou campeonato.\n");
  //falta a parte da pontuacao
}


int main(int argc, char argv[])
{
  c.pid=getpid();
  if (signal(SIGUSR1, acabou_campeonato) == SIG_ERR)
  {
    printf("\n [ERRO] Nao foi possivel configurar o sinal SIGUSR1\n");
    exit(EXIT_FAILURE);
  }

  printf("\nIndique o seu nome: ");
  scanf("%s", c.nome);
 

  printf("Nome = %s\n", c.nome);

  return 0;
}
