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

/*
int main(void){
    char input[1024]={""};

    fprintf(stdout,"STDOUT\n");
    fprintf(stderr,"STDERR\n");
    scanf("%1023s",input);

    fprintf(stdout,"STDIN=[%s]\n",input);

    return 0;
}
*/


//O JOGO AGORA NAO E POR RONDAS, PARA FACILITAR OS NAMED PIPES
int main(int argc, char *argv[])
{
    int contap1 = 0;
    int contap2 = 0;
    int ronda = 0;
    int ganha = 0;
    //char resposta[TAM];
    int numero = 0;
    int r;
    setbuf(stdout,NULL);
   // printf("CHEGUEI AO 1 PRINT.\n");

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
    sleep(10);
    /*
    do
    {
        do
        {
            printf("\nInsira o numero correspondente ao Objeto (1-Pedra;2-Papel;3-Tesoura): ");
            scanf("%d", &numero);

            if (numero == 0)
            { //Se o cliente puser #quit o arbitro manda 0
                flag_termina = 1;
            }
        } while (numero < 0 || numero > 3);
        if (numero != 0)
        {
            r = rand() % 3 + 1;
            printf("\nO seu adversario jogou o objeto correspondente ao numero %d", r);
            ganha = verifica(numero, r);
            if (ganha == 1)
            {
                PONTUACAO += 3; //Recebe 3 pontos por vitoria
                ronda++;
                printf("\nRonda %d Você ganhou!", ronda);
            }
            if (ganha == 2)
            {
                ronda++;
                printf("\nRonda %d Você perdeu!", ronda);
            }
            if (ganha == 0)
            {
                ronda++;
                PONTUACAO += 1;
                printf("\nRonda %d Empatada", ronda);
            }
        }
    } while (flag_termina == 0);
    printf("PONT = %d", PONTUACAO);
    exit(PONTUACAO);
     */
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
