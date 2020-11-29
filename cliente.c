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

  
  sprintf(fifo_name, CLIENT_PIPE, getpid());
/*
  if(acess(fifo_name,F_OK)==0){
    printf("\n[ERRO] Cli ja existe.\n");
    exit(EXIT_FAILURE);
  }

  if(mkfifo(fifo_name, 0600) == -1){
    printf("\n[ERRO] mkfif do CLiente deu erro");
    exit(EXIT_FAILURE);
  }
*/
  if (signal(SIGUSR1, acabou_campeonato) == SIG_ERR)
  {
    printf("\n [ERRO] Nao foi possivel configurar o sinal SIGUSR1\n");
    exit(EXIT_FAILURE);
  }

  if(access(SERV_PIPE,F_OK)!=0){
        perror("[Erro] O servidor nao esta a correr.\n");
        exit(0);
  }
  c.pid=getpid();
  do{

  
  fd_serv = open(SERV_PIPE, O_RDONLY);
  nb=read(fd_serv,&c,sizeof(Cliente));
/*
  fd_cli = open(fifo_name, O_RDONLY);
  //read();
*/
  }while (nb<0);//fazer até ao sinal de interromper
 
  close(fd_serv);
 
  //unlink(CLINT_PIPE);
  /*
  printf("\nIndique o seu nome: ");
  scanf("%s", c.nome);
 

  printf("Nome = %s\n", c.nome);
*/
  return 0;
}
