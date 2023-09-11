#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
//////////////////////////////////////////////////////
//              Geracao do mapa:

//Dimensoes logicas do mapa (referente a matriz do mapa, nao ao mapa desenhado pela raylib)
#define MAP_WIDTH 60  //Largura do mapa
#define MAP_HEIGHT 30  //Altura do mapa
#define MAX_TUNNELS 60//Quantidate de tuneis que vai gerar
#define PADDING 3 //Define o espacamento entra os tuneis(0 e o espacamento padrao 1x1):

//Funcao booleana que verifica se o tunel deve se mover em determinada direcao para evitar que as paredes sejam removidas ou ocorra problemas de acesso de posicoes inexistentes na matriz:
extern int canGenerate(int i, int j, int dx, int dy, int width, int height);

//Gera o Mapa aleatoriamente usando uma implementacao propria do Drunkard's Walk Algorithms
extern int generateMap(int m[MAP_HEIGHT][MAP_WIDTH]);

//Pega o tempo atual em ms, para que a seed do rand() mude mais rapido
extern long long current_timestamp();
