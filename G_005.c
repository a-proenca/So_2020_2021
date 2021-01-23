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

    //Nr aleatorio foi par
    if (p2 % 2 == 0)
    {
        if (p1 == 1)
        {
            ganha = 1;
        }
    }
    //Se for impar
    else
    { 
        if (p1 == 2)
        {
            ganha = 1;
        }
    }
    return ganha;
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
           "\n Adivinhar se o numero do adversario é para ou impar"
           "\nEscolha par ou impar (1,2) ");

    do
    {
        scanf("%s", objeto);
        
        numero = atoi(objeto);
        if(numero == 1 || numero==2){
            r = rand() % 99 + 1;
            ganha = verifica(numero, r);
            if (ganha == 1)
            {
                printf("\nO numero era = %d Acertou! Ganhou 3 pontos.\nEscolha par ou impar (1,2) ", r);
                PONTUACAO += 3;
            }
            else
            {
                printf("\nO numero era = %d  Perdeu.\nEscolha par ou impar (1,2) ", r);
            }
        }
        else{
            printf("\nNumero Invalido.\nEscolha par ou impar (1,2) ");
        }
       
    } while (flag_termina == 0);

    exit(PONTUACAO);

    return 0;
}