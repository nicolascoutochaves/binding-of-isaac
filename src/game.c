#include "movimentacao.c"
#include <ctype.h>
typedef struct Entidades{
    int x, y; //posicao x e y
    int dx, dy; //direcoes
    int vida;
}Entidade; //Struct usada para criar novas "entidades" como jogadores e inimigos.


typedef struct Player{
    Entidade ent;
    int pontuacao;

} Player;

void novo_jogo(char *state){
    *state = '\0';
    //WaitTime(0.2);
    int map[MAP_HEIGHT][MAP_WIDTH] = {0};
    int difficulty = 1;
    int atual_map = 1;
    int n_inimigos = 1 + (difficulty * atual_map);
    int temporizador = 0;
    if (n_inimigos > MAX_INIMIGOS) n_inimigos = MAX_INIMIGOS;

    generateMap(map);
    Player player = {0};
    Entidade inimigo[n_inimigos];

    while(*state != 'n' && *state != 'q'){
        
        int i, j;
        int x = 2, y = 2;
        for(i = 0; i < n_inimigos; i++){
            //Spawn dos Inimigos
            while(map[y/FATORY][x/FATORX] == 0 ||
                  map[y/FATORY][(x+LADOX)/FATORX] == 0 ||
                  map[(y+LADOY)/FATORY][(x)/FATORX] == 0 ||
                  map[(y+LADOY)/FATORY][(x+LADOX)/FATORX] == 0){
                x = rand() % ((LARGURA-LADOX)/FATORX)*LADOX;
                y = rand() % ((ALTURA-LADOY)/FATORY/2)*LADOY;
            }
            inimigo[i].x = x;
            inimigo[i].y = y;

            //Aqui poderiamos futuramente implementar uma opçao para os inimigos nao spawnarem juntos
            /* while(x == inimigo[i-1].x || x+LADOX+1 == inimigo[i-1].x){
                x = rand() % ((LARGURA-LADOX)/FATORX)*LADOX;
                inimigo[i].x = x;
            }
            while(y == inimigo[i-1].y || y+LADOY+1 == inimigo[i-1].y){
                y = rand() % ((ALTURA-LADOY)/FATORX/2)*LADOY;
                inimigo[i].y = y;
            } */

            x = 2;
            y = 2;
            //redefineDeslocamento(&inimigo[i].dx, &inimigo[i].dy);
            inimigo[i].dx = 1;
            inimigo[i].dy = 0;
        }
            //Spawn do jogador
         
        while(
                map[y/FATORY][x/FATORX] == 0 ||
                map[y/FATORY][(x+LADOX)/FATORX] == 0 ||
                map[(y+LADOY)/FATORY][(x)/FATORX] == 0 ||
                map[(y+LADOY)/FATORY][(x+LADOX)/FATORX] == 0
             ){
                x = rand() % ((LARGURA-LADOX)/FATORX)*LADOX;
                y = (ALTURA - LADOY) - rand() % ((ALTURA/2/FATORY))*LADOY;
            }

             player.ent.x = x;
             player.ent.y = y;

        while (!WindowShouldClose() && *state != 'q'){
            //Desenha o mapa na tela:
            for(i = 0; i < (MAP_HEIGHT); i++){
                for(j = 0; j < MAP_WIDTH; j++){
                    if(map[i][j] == 0){
                        DrawRectangle(j*FATORX, i*FATORY, LADOX, LADOY, RED);
                    }
                }
             }
            //                  JOGADOR:
            //Verifica se o usuario apertou ESC
            if(IsKeyPressed(KEY_ESCAPE)){
                *state = 'e';
                menu(state, 1);
                WaitTime(0.2);
            }
            //Define as direcoes dx e dy do jogador:
            if (IsKeyDown(KEY_D))
                player.ent.dx = 1;
            if (IsKeyDown(KEY_A))
                player.ent.dx = -1;
            if (IsKeyDown(KEY_W))
                player.ent.dy = 1;
            if (IsKeyDown(KEY_S))
                player.ent.dy = -1;

            DrawRectangle(player.ent.x, player.ent.y, LADO_QUADRADOX, LADO_QUADRADOY, GREEN);
            movimentar(&player.ent.x, &player.ent.y, &player.ent.dx, &player.ent.dy, map);
            player.ent.dx = 0;
            player.ent.dy = 0;

            //              Inimigos
            
            for(i = 0; i < n_inimigos; i++) {
            DrawRectangle(inimigo[i].x, inimigo[i].y, LADOX, LADOY, BLUE);
            }

            temporizador++;
            if(temporizador > GetFrameTime()*45){
                for(i = 0; i < n_inimigos; i++){
                    if(!movimentar(&inimigo[i].x, &inimigo[i].y, &inimigo[i].dx, &inimigo[i].dy, map))
                        redefineDeslocamento(&inimigo[i].dx, &inimigo[i].dy);
                        
                }
                temporizador = 0;
            }


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
}


void menu(char *state, int were_played){ //Menu do jogo
        while(*state == '\0'||*state == 'e'){
            BeginDrawing();
            ClearBackground(RAYWHITE);


            DrawText("Selecione uma opcao:", LARGURA/4, 100, 40, BLACK);

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
