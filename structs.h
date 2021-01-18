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
#define SERV_PIPE "pipe_servidor" //Trata de logins (arbitro <- cliente)
#define SERV_PIPE_WR "server_c%d"  //Trata de info do arbitro -> cliente
#define CLIENT_PIPE "client_%d"

//estrutura jogo
typedef struct jogo
{
    char identificacao[TAM]; //Tem de ser unico
    int pid_jogo;
} Jogo;

typedef struct jogosAdecorrer
{
    pthread_t thread;
    char nomejogo[TAM];
    char nomecliente[TAM];
}JogosAdecorrer;

//estrutura cliente
typedef struct cliente
{
    int pontuacao;
    int pid;
    int atendido; 
    int suspenso;
    char nome[TAM]; //Tem de ser unico
    char nome_pipe_escrita[TAM];
    char nome_pipe_leitura[TAM];
    char nome_jogo[TAM]; //Nome do jogo    
    int sair; //1- sai do jogo 
} Cliente;

//estrutura arbitro
typedef struct arbitro
{
    int nclientes;         //numero total de clientes
    Cliente clientes[TAM]; // vetor de clientes
    int n_jogos;           //nr total de jogos
    Jogo jogos[TAM];       // vetor de jogos
    int maxplayers;
    char gamedir[TAM];
    JogosAdecorrer jogosAdecorrer[TAM]; //vetor de jogos a decorrer
    int n_jogosAdecorrer;
} Arbitro;

