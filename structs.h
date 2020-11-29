#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#define TAM 50
#define SERV_PIPE "pipe_servidor"
#define CLIENT_PIPE "client_%d"


//estrutura jogo
typedef struct jogo{
 char identificacao[TAM]; //Tem de ser unico
 int pid_jogo; 
}Jogo; 

//estrutura cliente
typedef struct cliente{
 char nome[TAM]; //Tem de ser unico
 int pontuacao;
 int pid; 
 Jogo jogo;
 }Cliente; 

//estrutura arbitro
typedef struct arbitro{
 int nclientes; //numero total de clientes
 Cliente clientes[TAM]; // vetor de clientes 
 int n_jogos; //nr total de jogos
 Jogo jogos[TAM];// vetor de jogos
}Arbitro;

