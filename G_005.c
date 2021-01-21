#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#define TAM 50

int PONTUACAO = 0;

int flag_termina = 0;

int verifica(int p1, int p2);

void termina_jogo()
{
    flag_termina = 1;
    exit(PONTUACAO);
}

int main(int argc, char *argv[])
{
    int ganha = 0;
    int numero;
    int r;
    char objeto[50];
    setbuf(stdout, NULL);

    if (signal(SIGUSR1, termina_jogo) == SIG_ERR)
    {
        printf("\n [ERRO] Nao foi possivel configurar o sinal SIGUSR1\n");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));

    printf("\n---------------------BEM VINDO AO JOGO------------"
           "\n--------------------------------------Regras----------------------------------"
           "\nEscolher um numero par ou impar (1,2)"
           "\n Adivinhar se o numero do adversario é para ou impar");

    do
    {
        do
        {
            printf("\nEscolha par ou impar (1,2)");
            scanf("%s", objeto);
            numero = atoi(objeto);
        } while ((numero < 1) || (numero > 2));

        /*if (numero2 == 0)
            { //Se o cliente puser #quit o arbitro manda 0
                flag_termina = 1;
            }*/
        r = rand() % 99 + 1;
        ganha = verifica(numero, r);
        if (ganha == 1)
        {
            printf("O numero era = %d", r);
            printf("Acertou! Ganhou 3 pontos.\n");
            PONTUACAO += 3;
        }
        else
        {
            printf("O numero era = %d", r);
            printf("Perdeu.\n");
        }
    } while (flag_termina == 0);

    printf("PONT = %d", PONTUACAO);

    exit(PONTUACAO);

    return 0;
}

int verifica(int p1, int p2)
{

    int ganha = 0;

    if (p2 % 2 == 0)
    { //nr aleatorio foi par
        if (p1 == 1)
        {
            ganha = 1;
        }
    }
    else
    { //se for impar
        if (p1 == 2)
        {
            ganha = 1;
        }
    }
    return ganha;
}
