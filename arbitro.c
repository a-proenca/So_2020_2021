#include "structs.h"
char gamedir[TAM] = "/Jogo/"; 
int maxplayers = 30;
#define GAMEDIR gamedir
#define MAXPLAYER maxplayers

int main(int argc, char* argv[]){

int duracao;
int espera;

//verificacao
if(argc != 3){
 printf("Nao foi indicado o numero de argumentos necessarios.\n");
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
