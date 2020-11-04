#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define TAM 50

int verifica(int p1, int p2);

int main(int argc, char *argv[])
{

    char resposta[TAM];
    int numero = 0;
    int r;

    srand(time(NULL));

    printf("\n---------------------BEM VINDO AO JOGO------------");

    printf("\n--------------------------------------Regras----------------------------------");
    printf("\nEscolha um objeto dos 3 possiveis (Pedra/Papel/Tesoura)");
    printf("O seu adversario irá escolher tambem 1 dos 3 possiveis");
    printf("Caso não conheça este jogo escreva help para mais informações");
    printf("Escreva -> start <- assim que quiser começar a jogar: ");
    scanf("%s", resposta);
    if (strcasecmp(resposta, "HELP") == 0)
    {
        printf("Pedra ganha Tesoura\n");
        printf("Tesoura ganha Papel\n");
        printf("Papel ganha Pedra\n");

        printf("O jogo é a melhor de 3, ganha o jogador a vencer 2 vezes");
    }
    if (strcasecmp(resposta, "START") == 0)
    {

        //EU JA SEI O PROBLEMA EU DPS JA DOU FIX NISTO, BASICAMENTE ESTAMOS A PEDIR 1 NUMERO SO 1 VEZ E PRECISAMOS DE 2 OU 3 DEPENDENDO DO RESULTADO 
        do{
        printf("Insira 1 Numero(1-3):");
        scanf(" %d", &numero);
        }while(numero!=1);
        int contap1 = 0;
        int contap2 = 0;
        int ganha;
        while (contap1 <= 2 || contap2 <= 2){
            r = rand() % 3 + 1;
            ganha = verifica(r, numero);
            if (ganha == 1)
                contap1++;
            if (ganha == 2)
                contap2++;
        }
        if (contap1 > contap2)
        {
            printf("\nComputador Ganhou");
            
        }
        else
        {
            printf("\nVocê Ganhou");
            
        }
    }
}

int verifica(int p1, int p2)
{

    int ganha;

    if (p1 == p2)
    {
        ganha = 0;
    }
    if (p1 == 1 && p2 == 2)
    {
        ganha = 1;
    }
    if (p1 == 2 && p2 == 1)
    {
        ganha = 2;
    }
    if (p1 == 1 && p2 == 3)
    {
        ganha = 1;
    }
    if (p1 == 3 && p2 == 1)
    {
        ganha = 2;
    }
    if (p1 == 2 && p2 == 3)
    {
        ganha = 2;
    }
    if (p1 == 3 && p2 == 2)
    {
        ganha = 1;
    }

    return ganha;
}