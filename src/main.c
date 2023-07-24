#include "raylib.h"
#include "map.c"
#include "menu.c"

#define LARGURA 900
#define ALTURA 600






int main(void){

    int map[MAP_HEIGHT][MAP_WIDTH] = {0};
    generateMap(map);
   
    
    InitWindow(LARGURA, ALTURA, "JOGO");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);//remove a opcao de sair do jogo

    while (!WindowShouldClose()) // Detect window close button or exit key
    {
        if (IsKeyPressed(KEY_M)) menu();

        BeginDrawing();//Inicia o ambiente de desenho na tela
        if (IsKeyPressed(KEY_M)) menu();
        ClearBackground(RAYWHITE);//Limpa a tela e define cor de fundo
        EndDrawing();//Finaliza o ambiente de desenho na tela
    }
    CloseWindow();// Fecha a janela e o contexto OpenGL

    return 0;
}
