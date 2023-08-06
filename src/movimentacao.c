/*
Esse programa movimenta um quadrado usando funcoes ate que o quadrado atinja a borda da tela
*/
#include "raylib.h"
#include "../include/game.h"
#include "map.c"
#include <stdlib.h>
#include <time.h>
#include <math.h>


int deveMover(int m[MAP_HEIGHT][MAP_WIDTH], int x, int y, int dx, int dy, int fatorx, int fatory, int ladox, int ladoy){
    int deve_mover = 1;

    //verifica as posicoes nos quatro vertices do quadrado:
    if(
        (m[(y)/fatory][(x+ladox)/fatorx] == 0 && dx == 1)||
        (m[(y)/fatory][((x-ladox/6)/fatorx)] == 0 && dx == -1)||
        (m[(y+ladoy-1)/fatory][(x+ladox)/fatorx] == 0 && dx == 1)||
        (m[(y+ladoy-1)/fatory][((x-ladox/6)/fatorx)] == 0 && dx == -1)) deve_mover = 0;


    if(
        (m[(y-ladoy/6)/fatory][(x)/fatorx] == 0 && dy == 1)||
        (m[(y+ladoy)/fatory][(x)/fatorx] == 0 && dy == -1)||
        (m[(y-ladoy/6)/fatory][(x+ladox-1)/fatorx] == 0 && dy == 1)||
        (m[(y+ladoy)/fatory][(x+ladox-1)/fatorx] == 0 && dy == -1)) deve_mover = 0;


    return deve_mover;
}

void move(int dx, int dy, int *x, int *y){
    if(dx == 1){
        *x += VELOCIDADE*FATORX/4; //Move para direita
    }
    if(dx == -1){
        *x -= VELOCIDADE*FATORX/4; //Move para esquerda
    }
    if(dy == 1){
        *y -= VELOCIDADE*FATORY/4; //Move para cima
    }
    if(dy == -1){
        *y += VELOCIDADE*FATORY/4; //Move para baixo
    }
}

void redefineDeslocamento(int *dx, int *dy){
    *dx = 0;
    *dy = 0;

    while (*dx == 0 && *dy == 0){
        *dx = 1 - (rand() % 3);
        *dy = 1 - (rand() % 3);
    }      
    ////
}

int movimentar(int *x, int *y, int *dx, int *dy, int map[MAP_HEIGHT][MAP_WIDTH])
{
    int moveu = 0; 
    
    if(deveMover(map, *x, *y, *dx, *dy, FATORX, FATORY, LADO_QUADRADOX, LADO_QUADRADOY)){
        move(*dx, *dy, x, y);
        moveu = 1;
    }
    //reseta as direcoes para que o movimento nao fique infinito e para que quando o quadrado encoste na parede com dx, ele ainda possa se mover com dy e vice versa:
    

    return moveu;
}
