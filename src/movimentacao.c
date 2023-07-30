/*
Esse programa movimenta um quadrado usando funcoes ate que o quadrado atinja a borda da tela
*/
#include "raylib.h"
#include "../include/game.h"
#include "map.c"
#include <stdlib.h>
#include <time.h>
#include <math.h>


int deveMover(int m[MAP_HEIGHT][MAP_WIDTH], int x, int y, int dx, int dy, int fatorx, int fatory, int lado){
    int deve_mover = 1;

    //verifica as posicoes nos quatro vertices do quadrado:
    if(
        (m[(y)/FATORY][(x+LADO_QUADRADO)/FATORX] == 0 && dx == 1)||
        (m[(y)/FATORY][((x-LADO_QUADRADO/6)/FATORX)] == 0 && dx == -1)||
        (m[(y+LADO_QUADRADO-1)/FATORY][(x+LADO_QUADRADO)/FATORX] == 0 && dx == 1)||
        (m[(y+LADO_QUADRADO-1)/FATORY][((x-LADO_QUADRADO/6)/FATORX)] == 0 && dx == -1)) deve_mover = 0;


    if(
        (m[(y-LADO_QUADRADO /6)/FATORY][(x)/FATORX] == 0 && dy == 1)||
        (m[(y+LADO_QUADRADO)/FATORY][(x)/FATORX] == 0 && dy == -1)||
        (m[(y-LADO_QUADRADO /6)/FATORY][(x+LADO_QUADRADO-1)/FATORX] == 0 && dy == 1)||
        (m[(y+LADO_QUADRADO)/FATORY][(x+LADO_QUADRADO-1)/FATORX] == 0 && dy == -1)) deve_mover = 0;


    return deve_mover;
}

void move(int dx, int dy, int *x, int *y){
    if(dx > 0){
        *x += VELOCIDADE*FATORX/4; //Move para direita
    }
    if(dx < 0){
        *x -= VELOCIDADE*FATORX/4; //Move para esquerda
    }
    if(dy > 0){
        *y -= VELOCIDADE*FATORY/4; //Move para cima
    }
    if(dy < 0){
        *y += VELOCIDADE*FATORY/4; //Move para baixo
    }
}

void movimentar(int *x, int *y, int map[MAP_HEIGHT][MAP_WIDTH])
{
    int dx = 0, dy = 0;

    //Define as direcoes dx e dy:
    if (IsKeyDown(KEY_D))
        dx = 1;
    if (IsKeyDown(KEY_A))
        dx = -1;
    if (IsKeyDown(KEY_W))
        dy = 1;
    if (IsKeyDown(KEY_S))
        dy = -1;

    if(deveMover(map, *x, *y, dx, dy, FATORX, FATORY, LADO_QUADRADO))
        move(dx, dy, x, y);

    //reseta as direcoes para que o movimento nao fique infinito e para que quando o quadrado encoste na parede em dx ele ainda possa se mover em dy e vice versa:
    dx = 0;
    dy = 0;
}
