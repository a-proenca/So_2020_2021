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

int main(int argc, char *argv[])
{
    int contap1 = 0;
    int contap2 = 0;
    int ronda = 0;
    int ganha = 0;
    char resposta[TAM];
    int numero = 0;
    int r;

    if (signal(SIGUSR1, termina_jogo) == SIG_ERR)
    {
        printf("\n [ERRO] Nao foi possivel configurar o sinal SIGUSR1\n");
        exit(EXIT_FAILURE);
    }
    srand(time(NULL));

    printf("\n---------------------BEM VINDO AO JOGO------------");
    
    do
    {
        printf("\n--------------------------------------Regras----------------------------------");
        printf("\nEscolha um objeto dos 3 possiveis: 1-Pedra 2-Papel 3-Tesoura");
        printf("\nO seu adversario irá escolher tambem 1 dos 3 possiveis");
        printf("\nCaso não conheça este jogo escreva -> help <- para mais informações");
        printf("\nEscreva -> start <- assim que quiser começar a jogar: ");
        scanf("%s", resposta);
        if (strcasecmp(resposta, "HELP") == 0)
        {
            printf("\nPedra ganha Tesoura");
            printf("\nTesoura ganha Papel");
            printf("\nPapel ganha Pedra");

            printf("\nO jogo é a melhor de 3, ganha o jogador a vencer 2 vezes");
        }
        if (strcasecmp(resposta, "START") == 0)
        {
            contap1=0; 
            contap2=0;
            do
            {
                do
                {
                    printf("\nInsira o numero correspondente ao Objeto (1-Pedra;2-Papel;3-Tesoura): ");
                    scanf("%d", &numero);
                } while (numero < 1 || numero > 3);
                r = rand() % 3 + 1;
                printf("\nO seu adversario jogou o objeto correspondente ao numero %d", r);
                ganha = verifica(numero, r);
                if (ganha == 1)
                {
                    contap1++;
                    ronda++;
                    printf("\nRonda %d Você ganhou!", ronda);
                }
                if (ganha == 2)
                {
                    contap2++;
                    ronda++;
                    printf("\nRonda %d Você perdeu!", ronda);
                }
                if (ganha == 0)
                {

                    printf("\nRonda %d Empatada, a ronda será repetida", ronda + 1);
                }
            } while (((contap2 < 2 && contap1 < 2) || (contap1 < 2 && contap2 < 2)) && flag_termina == 0);
            if (contap1 > contap2)
            {
                printf("\n--------Resultado Final---------");
                printf("\nVocê Ganhou %d-%d\n", contap1, contap2);
                PONTUACAO+=1;
            }
            else
            {
                printf("\n--------Resultado Final----------");
                printf("\nVocê Perdeu %d-%d\n", contap1, contap2);
                if(PONTUACAO>0){
                PONTUACAO-=1;
                }
                else
                {
                PONTUACAO=0;
                }
            }
        }
    } while (strcasecmp(resposta, "exit") != 0 && flag_termina == 0);
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

//FALTA FAZER exit_status
// USAR O WAIT EXIT_STATUS
