#include "movimentacao.c"
#include <ctype.h>

typedef struct Entidades{
    int x; //posicao x
    int y; //posicao y
    int vida;
    int pontuacao;
} Entidade; //Struct usada para criar novas "entidades" como jogadores e inimigos.


typedef struct Player{
    struct Entidades;
} Player;

void novo_jogo(char *state){
    *state = '\0';
    WaitTime(0.2);
    int map[MAP_HEIGHT][MAP_WIDTH] = {0};

    generateMap(map);
    Player player;

    while(*state != 'n' && *state != 'q'){
        int difficulty = 2;
        int i, j;

        player.x = (MAP_WIDTH * FATORX -(3*FATORX) )- LADOX;
        player.y = 3*FATORY;

        while (!WindowShouldClose() && *state != 'q'){
            //Desenha o mapa na tela:
            for(i = 0; i < (MAP_HEIGHT); i++){
                for(j = 0; j < MAP_WIDTH; j++){
                    if(map[i][j] == 0){
                        DrawRectangle(j*FATORX, i*FATORY, LADOX, LADOY, RED);
                    }
                }
             }

            //Verifica se o jogador nao apertou ESC
            if(IsKeyPressed(KEY_ESCAPE)){
                *state = 'e';
                menu(state, 1);
                WaitTime(0.2);
            }
            movimentar(&player.x, &player.y, map);
            DrawRectangle(player.x, player.y, LADO_QUADRADO, LADO_QUADRADO, GREEN);
            //novo_jogo(&x, &y, map);
            BeginDrawing();//Inicia o ambiente de desenho na tela
            ClearBackground(RAYWHITE);//Limpa a tela e define cor de fundo
            EndDrawing();//Finaliza o ambiente de desenho na tela
        }
        CloseWindow();// Fecha a janela e o contexto OpenGL
    }

}


void carregar_jogo(char *state){
    *state = '\0';
}

void salvar_jogo(char *state){
    while(*state == 's'){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Jogo salvo com sucesso!", LARGURA/4, ALTURA/2, 40, BLACK);
        DrawText("Aperte ESC para continuar", LARGURA/4 + 20, ALTURA/2 + 100, 30, BLACK);
        if(IsKeyPressed(KEY_ESCAPE)) *state = '\0';
        EndDrawing();
    }
    menu(state, 1);
}

void sair_jogo(char *state){
    while(*state != 'q'){
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Deseja salvar o jogo? (S/N)", LARGURA/4, ALTURA/2, 40, BLACK);

        if(IsKeyPressed(KEY_S)){
            salvar_jogo(state);
            *state = 'q';
        } else if (IsKeyPressed(KEY_N)){
            *state = 'q';
        }
        EndDrawing();
    }
}

void voltar_jogo(char *state){
    *state = '\0';
}//


void menu(char *state, int were_played){ //Menu do jogo
        while(*state == '\0'||*state == 'e'){
            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("Solecione \numa opcao:", LARGURA/4, 100, 40, BLACK);
            DrawText("N: novo jogo", LARGURA/4, 250, 40, BLACK);
            DrawText("C: carregar jogo", LARGURA/4, 300, 40, BLACK);
            if(were_played) DrawText("S: salvar jogo", LARGURA/4, 350, 40, BLACK);
            DrawText("Q: sair do jogo", LARGURA/4, 400, 40, BLACK);
            if(were_played) DrawText("V: voltar ao jogo", LARGURA/4, 450, 40, BLACK);

            if(IsKeyPressed(KEY_N)){
                were_played = 1;
                *state = 'n';
                novo_jogo(state);
            }
            if(IsKeyPressed(KEY_C)){
                *state = 'c';
                carregar_jogo(state);
            }
            if(IsKeyPressed(KEY_S) && were_played){
                *state = 's';
                salvar_jogo(state);
            }
            if(IsKeyPressed(KEY_Q)){
                *state = '0'; //atribui a '0' para poder sair do loop do menu.
                sair_jogo(state);
            }
            if(IsKeyPressed(KEY_V) && were_played){
                *state = 'v';
            }

            EndDrawing();
        }
}


int main(void){ //
    char state = '\0'; //Estados do jogo


    InitWindow(LARGURA, ALTURA, "JOGO");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);//remove a opcao de sair do jogo

        while(state == '\0' || state == 'e')
        menu(&state, 0);


    return 0;
}
