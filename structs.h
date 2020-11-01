#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TAM 50


//estrutura cliente
typedef struct cliente{
 char nome[TAM]; //Tem de ser unico
 int pontuacao;
}Cliente; 

//estrutura arbitro
typedef struct arbitro{
 int nclientes; //numero total de clientes
 Cliente clientes[TAM]; // vetor de clientes 
}Arbitro;


