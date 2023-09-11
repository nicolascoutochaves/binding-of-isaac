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

extern int canGenerate(int i, int j, int dx, int dy, int width, int height);
extern int generateMap(int m[MAP_HEIGHT][MAP_WIDTH]);
extern long long current_timestamp();