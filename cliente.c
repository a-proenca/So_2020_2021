#include <stdio.h>
#include "structs.h"

int main(int argc, char argv[]){
char nome[TAM]=" ";

printf("\nIndique o seu nome: ");
scanf("%s",nome);

printf("Nome = %s\n",nome);

return 0;
}
