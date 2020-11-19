#include <stdio.h>
#include "structs.h"
#include <sys/wait.h>

void acabou_campeonato()
{
  printf("Acabou campeonato.\n");
  //falta a parte da pontuacao
}


int main(int argc, char argv[])
{
  char nome[TAM] = " ";

  if (signal(SIGUSR1, acabou_campeonato) == SIG_ERR)
  {
    printf("\n [ERRO] Nao foi possivel configurar o sinal SIGUSR1\n");
    exit(EXIT_FAILURE);
  }

  printf("\nIndique o seu nome: ");
  scanf("%s", nome);

  printf("Nome = %s\n", nome);

  return 0;
}
