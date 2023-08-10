#include "map.c"
#include "raylib.h"
#include <math.h>
#include <string.h>
#include <ctype.h>

#define LARGURA GetScreenWidth() //Largura da tela
#define ALTURA GetScreenHeight() //Altura da tela

#define FONT_SIZE 40


//Dimensoes que o mapa tera quando desenhado
#define LARGURA_MAPA 1200
#define ALTURA_MAPA 600
#define MARGIN_TOP 20 //Margem em relacao ao topo da tela
#define MARGIN_LEFT (LARGURA-LARGURA_MAPA)/2 //Margem em relacao a esquerda da tela

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Fatores de Conversao (Converte da matriz do mapa para a tela):

#define FATORX  (LARGURA_MAPA / MAP_WIDTH) //Fator de conversao do eixo x
#define FATORY  (ALTURA_MAPA / MAP_HEIGHT) //Fator de conversao do eixo y

#define LADOX FATORX //Espessura x das paredes do mapa
#define LADOY FATORY //Espessura em y das paredes do mapa

//--------------------------------------------------------------------------------------
//              Personagens:

#define LADO_QUADRADOX FATORX
#define LADO_QUADRADOY FATORY
#define VELOCIDADE FATORX/4
#define MAX_INIMIGOS 15

Texture2D spritesheet;



typedef struct Entidades{ // Entidades means enemys or players
    int x, y; //posicao x e y
    int dx, dy; //direcoes
    int vida;
    bool active; //Retorna se esta vivo ou morto
}Entidade; //Struct usada para criar novas "entidades" como jogadores e inimigos.
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
typedef struct Player{
    Entidade ent;
    int pontuacao;
} Player;



//Pre declaracao de funcoes (Pois como menu estava sendo declarado depois de novo_jogo, estava aparecendo warning no compilador)
void menu(char *state, int were_played);


int deveMover(int m[MAP_HEIGHT][MAP_WIDTH], int x, int y, int dx, int dy, int fatorx, int fatory, int ladox, int ladoy){
    int deve_mover = 1;

    //Desconta de x e y as margens para nao bugar as colisoes
    y -= MARGIN_TOP;
    x -= MARGIN_LEFT;


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
        *x += VELOCIDADE; //Move para direita
    }
    if(dx == -1){
        *x -= VELOCIDADE; //Move para esquerda
    }
    if(dy == 1){
        *y -= VELOCIDADE; //Move para cima
    }
    if(dy == -1){
        *y += VELOCIDADE; //Move para baixo
    }
}

void redefineDeslocamento(int *dx, int *dy){
    *dx = 0;
    *dy = 0;

    while (*dx == 0 && *dy == 0){
        *dx = 1 - (rand() % 3);
        *dy = 1 - (rand() % 3);
    }

}

int movimentar(int *x, int *y, int *dx, int *dy, int map[MAP_HEIGHT][MAP_WIDTH])
{

    int moveu = 0;

    if(deveMover(map, *x, *y, *dx, *dy, FATORX, FATORY, LADO_QUADRADOX, LADO_QUADRADOY)){
        move(*dx, *dy, x, y);
        moveu = 1;

    }

    return moveu;
}

void persegue(Player player, Entidade *inimigo, int map[MAP_HEIGHT][MAP_WIDTH]){

    if (deveMover(map, inimigo->x, inimigo->y, inimigo->dx, inimigo->dy,FATORX, FATORY, LADO_QUADRADOX, LADO_QUADRADOY)){
        if( ((player.ent.x) > (inimigo->x))){
            inimigo->x += VELOCIDADE;
        }

        if( ((player.ent.x + LADO_QUADRADOX ) < (inimigo->x)) ){
            inimigo->x -= VELOCIDADE;
        }

        if( ((player.ent.y + LADO_QUADRADOY) < (inimigo->y)) ){
            inimigo->y -= VELOCIDADE;
        }

        if( ((player.ent.y) > (inimigo->y)) ){
            inimigo->y += VELOCIDADE;
        }
    } else{
        redefineDeslocamento(&inimigo->dx, &inimigo->dy);
        inimigo->x += VELOCIDADE * inimigo->dx;
        inimigo->y -= VELOCIDADE * inimigo->dy;
    }


}

int current_map = 2; //variavel global por enquanto, pra nao precisar passar o mapa atual por referencia para a funcao.
void novo_jogo(char *state){

    *state = '\0';
    int map[MAP_HEIGHT][MAP_WIDTH] = {0};
    FILE *maptxt;
    char filename[30];
    char c; //caracter do mapa
    int modo_jogo = 0; //0 e padrao, 1 e a geracao aleatoria
    int n_inimigos = 0;
    int i, j;
    Player player = {0};
    Entidade inimigo[MAX_INIMIGOS];
    
    if(modo_jogo) generateMap(map); //Se o modo de jogo for 1, gera o mapa e os spawns aleatoriamente, se nao, carrega os arquivos
    else{
        sprintf(filename, "../mapas/mapa%d.txt", current_map); //define o caminho do mapa e salva na variavel "filename", para que possamos alterar o nome do mapa dinamicamente (pois na funcao fopen nao pode passar paramentros para formatacao)

        if(!(maptxt = fopen(filename, "r"))){
            DrawText("Erro ao carregar o mapa!", LARGURA/2-MeasureText("Erro ao carregar o mapa!", FONT_SIZE), ALTURA, FONT_SIZE, RED);
            WaitTime(1);
            return;
        } else{
            rewind(maptxt); //coloca o cursor no inicio do mapa
            for(i = 0; i < MAP_HEIGHT; i++){
                for(j = 0; j < MAP_WIDTH; j++){
                    if((fread(&c, sizeof(char), 1, maptxt))!= 1){
                        DrawText("Erro ao ler o mapa!", LARGURA/2-MeasureText("Erro ao ler o mapa!", FONT_SIZE), ALTURA, FONT_SIZE, RED);
                    } else{
                        c = toupper(c);

                        switch(c){
                            case '#': map[i][j] = 0; //Parede
                                break;
                            case 'J': map[i][j] = 2; //jogador
                                player.ent.x = j*FATORX;
                                player.ent.y = i*FATORY;
                                break;
                            case 'I': map[i][j] = 3; //Inimigo
                                    inimigo[n_inimigos].x = j*FATORX;
                                    inimigo[n_inimigos].y = i*FATORY;
                                    n_inimigos++;//incrementa n_inimigos para verificar o proximo inimigo            
                                break;
                            case 'B': map[i][j] = 4; //Bomba
                                break; 
                            case 'X': map[i][j] = 5; //Armadilha
                                break;
                            case 'P': map[i][j] = 6; //Portal
                                break;
                            case '\n': fseek(maptxt, 1, SEEK_CUR); //Pula o caracter de new line
                                break;
                            default: map[i][j] = 1; //Espaco vazio

                        }
                        
                        fflush(maptxt);
                    }
                }
            }

            fclose(maptxt);
                
        }
    }

    int x = 2, y = 2; //inicializa x e y em posicoes invalidas para que seja sempre sorteado uma posicao valida no loop abaixo

    if(modo_jogo){

        int difficulty = 1;
        if (n_inimigos > MAX_INIMIGOS) n_inimigos = MAX_INIMIGOS;
        n_inimigos = 1 + (difficulty * current_map);

            for(i = 0; i < n_inimigos; i++){
        //Spawn dos Inimigos
        while(map[y/FATORY][x/FATORX] == 0 ||
                map[y/FATORY][(x+LADOX)/FATORX] == 0 ||
                map[(y+LADOY)/FATORY][(x)/FATORX] == 0 ||
                map[(y+LADOY)/FATORY][(x+LADOX)/FATORX] == 0){
            x = rand() % ((LARGURA_MAPA-LADOX)/FATORX)*LADOX;
            y = rand() % ((ALTURA_MAPA-LADOY)/FATORX/2)*LADOY;
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
    }
        //Spawn do jogador

    while(map[y/FATORY][x/FATORX] == 0 ||
                map[y/FATORY][(x+LADOX)/FATORX] == 0 ||
                map[(y+LADOY)/FATORY][(x)/FATORX] == 0 ||
                map[(y+LADOY)/FATORY][(x+LADOX)/FATORX] == 0){
            x = rand() % ((LARGURA_MAPA-LADOX)/FATORX)*LADOX;
            y = (ALTURA_MAPA - LADO_QUADRADOY) - rand() % ((ALTURA_MAPA-LADOY)/FATORX/2)*LADOY; // Player spawna na metade de baixo do mapa
        }

            player.ent.x = x;
            player.ent.y = y;
    
} 

    redefineDeslocamento(&inimigo[i].dx, &inimigo[i].dy);



        
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
        while (!WindowShouldClose() && *state != 'q' && *state != 'n'){

            //Desenha o mapa na tela:
            for(i = 0; i < (MAP_HEIGHT); i++){
                for(j = 0; j < MAP_WIDTH; j++){
                    if(map[i][j] == 0){
                        DrawRectangle(j*FATORX+MARGIN_LEFT, i*FATORY+MARGIN_TOP, LADOX, LADOY, PURPLE);
                    }
                }
             }
            //                  JOGADOR:
            //Verifica se o usuario apertou ESC
            if(IsKeyPressed(KEY_ESCAPE)){
                *state = 'e';
                menu(state, 1);
                //  Time(0.2);
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



            movimentar(&player.ent.x, &player.ent.y, &player.ent.dx, &player.ent.dy, map);
            DrawRectangle(player.ent.x, player.ent.y, LADO_QUADRADOX, LADO_QUADRADOY, GREEN);


            //              Inimigos

            for(i = 0; i < n_inimigos; i++) {
            DrawRectangle(inimigo[i].x, inimigo[i].y, LADOX, LADOY, ORANGE);
            }

 
             for(i = 0; i < n_inimigos; i++){

                

                if((sqrt(pow((player.ent.x - inimigo[i].x), 2) + pow( (player.ent.y - inimigo[i].y), 2)) ) < 10*FATORX){
                    DrawText("ESTA PERTO!", LARGURA/2, ALTURA/2, FONT_SIZE, PURPLE); //verifica se jogador esta perto de inimigo
                    persegue(player, &inimigo[i], map);

                } else if(!movimentar(&inimigo[i].x, &inimigo[i].y, &inimigo[i].dx, &inimigo[i].dy, map))
                     redefineDeslocamento(&inimigo[i].dx, &inimigo[i].dy);
             }  



            //Colisao com o Jogador:
            for(i = 0; i < n_inimigos; i++){
                if(CheckCollisionRecs(
                    (Rectangle){player.ent.x, player.ent.y, LADOX, LADOY},
                    (Rectangle){inimigo[i].x, inimigo[i].y, LADOX, LADOY})) {

                        /* player.ent.x -= VELOCIDADE*player.ent.dx;
                        player.ent.y += VELOCIDADE*player.ent.dy; */
 

                        
 
                }
            }

            
            if(map[player.ent.y/FATORY][player.ent.x/FATORX] == 6){
                *state = 'n';
                current_map++;
                novo_jogo(state);
            }

    
            
            player.ent.dx = 0;
            player.ent.dy = 0;

            BeginDrawing();//Inicia o ambiente de desenho na tela
            ClearBackground(RAYWHITE);//Limpa a tela e define cor de fundo
            EndDrawing();//Finaliza o ambiente de desenho na tela
        }
        CloseWindow();// Fecha a janela e o contexto OpenGL
    
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void carregar_jogo(char *state){
    *state = '\0';
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void salvar_jogo(char *state){
    char text[50];
    while(*state == 's'){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        strcpy(text, "Jogo salvo com sucesso!");
        DrawText(text, LARGURA/2 - MeasureText(text, FONT_SIZE)/2, ALTURA/2 - FONT_SIZE, FONT_SIZE, BLACK);

        strcpy(text, "Aperte ESC para continuar");
        DrawText(text, LARGURA/2 - MeasureText(text, FONT_SIZE)/2, ALTURA/2 - FONT_SIZE + 50, FONT_SIZE, BLACK);


        if(IsKeyPressed(KEY_ESCAPE)) *state = '\0';
        EndDrawing();
    }
    menu(state, 1);
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void sair_jogo(char *state){
    char text[50];
    while(*state != 'q'){
        BeginDrawing();
        ClearBackground(RAYWHITE);


        strcpy(text, "Deseja salvar o jogo? (S/N)");
        DrawText(text, LARGURA/2 - MeasureText(text, FONT_SIZE)/2, ALTURA/2 - FONT_SIZE, FONT_SIZE, BLACK);


        if(IsKeyPressed(KEY_S)){
            salvar_jogo(state);
            *state = 'q';
        } else if (IsKeyPressed(KEY_N)){
            *state = 'q';
        }
        EndDrawing();
    }
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void voltar_jogo(char *state){
    *state = '\0';
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void menu(char *state, int were_played){ //Menu do jogo
        char text[50] = "";
        while(*state == '\0'||*state == 'e'){
            BeginDrawing();
            ClearBackground(RAYWHITE);

            strcpy(text, "Selecione uma opcao:");
            DrawText(text, LARGURA/2 - MeasureText(text, FONT_SIZE)/2, 100, FONT_SIZE, BLACK);

            strcpy(text, "N: novo jogo");
            DrawText(text, LARGURA/2 - MeasureText(text, FONT_SIZE)/2, 250, FONT_SIZE, BLACK);

            strcpy(text, "C: carregar jogo");
            DrawText(text, LARGURA/2 - MeasureText(text, FONT_SIZE)/2, 300, FONT_SIZE, BLACK);

            strcpy(text, "Q: sair do jogo");
            DrawText(text, LARGURA/2 - MeasureText(text, FONT_SIZE)/2, 400, FONT_SIZE, BLACK);

            if(were_played) {
                strcpy(text,  "S: salvar jogo");
                DrawText(text, LARGURA/2 - MeasureText(text, FONT_SIZE)/2, 350, FONT_SIZE, BLACK);

                strcpy(text, "V: voltar ao jogo");
                DrawText(text, LARGURA/2 - MeasureText(text, FONT_SIZE)/2, 450, FONT_SIZE, BLACK);
            }

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
//--------------------------------------------------------------------------------------
//---Funcao Principal-------------------------------------------------------------------
int main(void){ //
    char state = '\0'; //Estados do jogo

    InitWindow(LARGURA, ALTURA, "The Binding of Isaac");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);//remove a opcao de sair do jogo

        while(state == '\0' || state == 'e')
        menu(&state, 0);

    return 0;
}
