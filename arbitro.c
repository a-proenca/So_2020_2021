#include "structs.h"
#define GAMEDIR "Jogo"  //caso nao exista v.ambiente fica com este valor
#define MAXPLAYER 30	   //caso nao exista v.ambiente fica com este valor


int main(int argc, char* argv[]){
int duracao;
int espera;
char gamedir[TAM] = GAMEDIR; 
int maxplayers = MAXPLAYER;

//verificacao
if(argc < 3){
 printf("Nao foi indicado o numero de argumentos necessarios.\n");
 printf("Por favor indique a duração do campeonato e o tempo de espera.\n");
 return 0;
}
else if(argc > 3){
	printf("Foram indicados argumentos em excesso.\n");
	printf("Por favor indique apenas a duração do campeonato e o tempo de espera.\n");
 return 0;
}
//leitura da linha de comandos
duracao = atoi(argv[1]);
espera = atoi(argv[2]);

printf("duracao = %d;espera = %d\n",duracao,espera);


if (getenv("GAMEDIR") != NULL)
	strcpy(gamedir,getenv("GAMEDIR"));
if (getenv("MAXPLAYER")!= NULL)
	maxplayers = atoi(getenv("MAXPLAYER"));

printf("gamedir = %s;maxplayers = %d\n", gamedir,maxplayers);
return 0;
}
