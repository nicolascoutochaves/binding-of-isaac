/*
Esse programa movimenta um quadrado usando funcoes ate que o quadrado atinja a borda da tela
*/
#include "raylib.h"
#include "map.c"
#include <stdlib.h>
#include <time.h>
#define ALTURA 600
#define LARGURA 600
#define LADO 30
#define LADO_QUADRADO 15
#define VELOCIDADE 5


int deveMover(int m[MAP_HEIGHT][MAP_WIDTH], int x, int y, int dx, int dy, int larg, int alt, int lado){
    int deve_mover = 1;
    int fatorx = LARGURA / MAP_WIDTH, fatory = ALTURA / MAP_HEIGHT;

    if((m[(y+LADO_QUADRADO/2)/fatory][(x+LADO_QUADRADO/2)/fatorx+1] == 0 && dx == 1)||(m[(y-LADO_QUADRADO/4)/fatory][(x-LADO_QUADRADO)/fatorx-1] == 0 && dx == -1)) deve_mover = 0;
    if((m[(y+LADO_QUADRADO/2)/fatory-1][(x+LADO_QUADRADO/2)/fatorx] == 0 && dy == 1)||(m[(y-LADO_QUADRADO/4)/fatory+1][(x-LADO_QUADRADO/2)/fatorx] == 0 && dy == -1)) deve_mover = 0;


    return deve_mover;
}

void move(int dx, int dy, int *x, int *y){
    if(dx > 0){
        *x += VELOCIDADE; //Move para direita
    }
    if(dx < 0){
        *x -= VELOCIDADE; //Move para esquerda
    }
    if(dy > 0){
        *y -= VELOCIDADE; //Move para cima
    }
    if(dy < 0){
        *y += VELOCIDADE; //Move para baixo
    }
}

void movimentar(int *x, int *y, int map[MAP_HEIGHT][MAP_WIDTH])
{
    int dx = 0, dy = 0;
    int i, j;
    int fatorx = LARGURA / MAP_WIDTH, fatory = ALTURA / MAP_HEIGHT;

    for(i = 0; i < MAP_HEIGHT; i++){
        for(j = 0; j < MAP_WIDTH; j++){
            if(map[i][j] == 0){
                DrawRectangle(j*fatorx, i*fatory, LADO, LADO, RED);
            }
        }
    }
    srand(time(NULL));

    

    //Define as direcoes dx e dy:
    if (IsKeyDown(KEY_D))
        dx = 1;

    if (IsKeyDown(KEY_A))
        dx = -1;

    if (IsKeyDown(KEY_W))
        dy = 1;

    if (IsKeyDown(KEY_S))
        dy = -1;
     
    if(deveMover(map, *x, *y, dx, dy, LARGURA, ALTURA, LADO_QUADRADO))
        move(dx, dy, x, y); 
    
    
    

    //reseta as direcoes para que o movimento nao fique infinito e para que quando o quadrado encoste na parede em dx ele ainda possa se mover em dy e vice versa:
    dy = 0;
    dx = 0;
    
   
}
 