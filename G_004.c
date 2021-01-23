#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#define TAM 50

int PONTUACAO = 0;
int flag_termina = 0;

void termina_jogo()
{
    flag_termina = 1;
    exit(PONTUACAO);
}

int verifica(int p1, int p2)
{
    int ganha = 0;

    if (p1 == p2)
    {
        ganha = 0;
    }
    if (p1 == 1 && p2 == 2)
    {
        ganha = 2;
    }
    if (p1 == 2 && p2 == 1)
    {
        ganha = 1;
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

int main(int argc, char *argv[])
{
    int ronda = 0;
    int ganha = 0;
    int numero;
    int r;
    char objeto[50];
    setbuf(stdout, NULL);
    srand(time(NULL));
    if (signal(SIGUSR1, termina_jogo) == SIG_ERR)
    {
        printf("\n [ERRO] Nao foi possivel configurar o sinal SIGUSR1\n");
        exit(EXIT_FAILURE);
    }

    printf("\n---------------------BEM VINDO AO JOGO------------"
           "\n--------------------------------------Regras----------------------------------"
           "\nEscolha um objeto dos 3 possiveis: 1-Pedra 2-Papel 3-Tesoura"
           "\nO seu adversario irá escolher tambem 1 dos 3 possiveis"
           "\nPedra ganha Tesoura"
           "\nTesoura ganha Papel"
           "\nPapel ganha Pedra"
           "\nInsira o numero correspondente ao Objeto (1-Pedra;2-Papel;3-Tesoura): ");

    do
    {
        scanf("%s", objeto);

        numero = atoi(objeto);
        if (numero > 0 && numero < 4)
        {
            r = rand() % 3 + 1;
            ganha = verifica(numero, r);
            if (ganha == 1)
            {
                PONTUACAO += 3;
                ronda++;
                printf("\nO seu adversario jogou o objeto correspondente ao numero %d.Ronda %d Você ganhou!\nInsira o numero correspondente ao Objeto (1-Pedra;2-Papel;3-Tesoura): ", r, ronda);
            }
            if (ganha == 2)
            {
                ronda++;
                printf("\nO seu adversario jogou o objeto correspondente ao numero %d.Ronda %d Você perdeu!\nInsira o numero correspondente ao Objeto (1-Pedra;2-Papel;3-Tesoura): ", r, ronda);
            }
            if (ganha == 0)
            {
                ronda++;
                PONTUACAO += 1;
                printf("\nO seu adversario jogou o objeto correspondente ao numero %d.Ronda %d Empatada\nInsira o numero correspondente ao Objeto (1-Pedra;2-Papel;3-Tesoura): ", r, ronda); //Recebe 1 ponto por empate
            }
        }
        else
        {
            printf("\nNumero Invalido.\nInsira o numero correspondente ao Objeto (1-Pedra;2-Papel;3-Tesoura): ");
        }
    } while (flag_termina == 0);
    exit(PONTUACAO);
    return 0;
}
