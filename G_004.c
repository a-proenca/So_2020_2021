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
}

//O JOGO AGORA NAO E POR RONDAS, PARA FACILITAR OS NAMED PIPES

int main(int argc, char *argv[])
{
    int ronda = 0;
    int ganha = 0;
    int numero;
    int r;
    char objeto[50];
    setbuf(stdout,NULL);

    

    if (signal(SIGUSR1, termina_jogo) == SIG_ERR)
    {
        printf("\n [ERRO] Nao foi possivel configurar o sinal SIGUSR1\n");
        exit(EXIT_FAILURE);
    }
    
    srand(time(NULL));

    printf("\n---------------------BEM VINDO AO JOGO------------"
           "\n--------------------------------------Regras----------------------------------"
           "\nEscolha um objeto dos 3 possiveis: 1-Pedra 2-Papel 3-Tesoura"
           "\nO seu adversario irá escolher tambem 1 dos 3 possiveis"
           "\nPedra ganha Tesoura"
           "\nTesoura ganha Papel"
           "\nPapel ganha Pedra");
    
   
    do
    {
        do
        {
            printf("\nInsira o numero correspondente ao Objeto (1-Pedra;2-Papel;3-Tesoura): ");
            scanf("%s", objeto);
            
            /*if (numero2 == 0)
            { //Se o cliente puser #quit o arbitro manda 0
                flag_termina = 1;
            }*/
            numero = atoi(objeto);

        } while (numero < 0 || numero > 3);
        if (numero != 0)
        {
            r = rand() % 3 + 1;
            ganha = verifica(numero, r);
            if (ganha == 1)
            {
                PONTUACAO += 3; //Recebe 3 pontos por vitoria
                ronda++;
                printf("\nO seu adversario jogou o objeto correspondente ao numero %d.Ronda %d Você ganhou!",r, ronda);
            }
            if (ganha == 2)
            {
                ronda++;
                printf("\nO seu adversario jogou o objeto correspondente ao numero %d.Ronda %d Você perdeu!",r,ronda);
            }
            if (ganha == 0)
            {
                ronda++;
                PONTUACAO += 1;
                printf("\nO seu adversario jogou o objeto correspondente ao numero %d.Ronda %d Empatada",r, ronda); //Recebe 1 ponto por empate
            }
        }
    } while (flag_termina == 0) ;

    
    //itoa(PONTUACAO,pontuacao2,10);
    printf("PONT = %d", PONTUACAO);

    exit(PONTUACAO);

    return 0;
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
