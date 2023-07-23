#include "raylib.h"
#include "map.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define LARGURA 900
#define ALTURA 600

void menu(){  
        BeginDrawing();
        DrawText("Solecione uma opcao:", 250, 100, 40, BLACK);
        DrawText("N: novo jogo", 300, 250, 40, BLACK);
        DrawText("C: carregar jogo", 300, 300, 40, BLACK);
        DrawText("S: salvar jogo", 300, 350, 40, BLACK);
        DrawText("Q: sair jogo", 300, 400, 40, BLACK);
        DrawText("V: voltar jogo", 300, 450, 40, BLACK);

        if(IsKeyPressed(KEY_N)){

            novo_jogo();
            
        }

        if(IsKeyPressed(KEY_C)) {
            carregar_jogo();
            
        }
        if(IsKeyPressed(KEY_S)){
            
            salvar_jogo();
            
        }
        if(IsKeyPressed(KEY_Q)){
            sair_jogo();
            
        }
        if(IsKeyPressed(KEY_V)){
            voltar_jogo();
            
        }
  

}

  void novo_jogo(){
}

void carregar_jogo(){
}

void salvar_jogo(){
    ClearBackground(RAYWHITE);
    DrawText("Jogo salvo com sucesso!", ALTURA/2, LARGURA/2, 40, BLACK);
 
}

void sair_jogo(){
    
    ClearBackground(RAYWHITE);

    DrawText("Deseja salvar o jogo? (S/N)", LARGURA/2, ALTURA/2, 40, BLACK);

    if(IsKeyPressed(KEY_S)){
        salvar_jogo();
        CloseWindow();
    } else if (IsKeyPressed(KEY_N)){
       
        CloseWindow();
    }


}

void voltar_jogo(){
}  



int shouldMove(int i, int j, int dx, int dy, int width, int height) {
    int move = 1;
    if( (i == (height-2) && dy == 1) || (i == 1 && dy == -1))
        move = 0;
    if( (j == (width-2) && dx == 1) || (j == 1 && dx == -1))
        move = 0;
    return move;
}

int main(void){
    ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////GERACAO DO MAPA////////////////////////////////
    /////////////////////////////////////////////////////////////////////////

    {   //Esse bloco e pra eu poder minimixar esse texto todo
        int m[HEIGHT][WIDTH] = {0}; 
        int i, j, k, l; 
        int s = 0, dx = 0, dy = 0; 
        int s_ant = s; 
        int tunnels = 0, steps = 0, count_padding = PADDING; 
        srand(time(NULL));
        i = 1; //1 + (rand() % HEIGHT-3);
        j = WIDTH - 2; //1 + (rand() % WIDTH-3);
        m[i][j] = 1;
        while(tunnels < MAX_TUNNELS) {
            while(s == 0 || s == s_ant) {
                s = (rand() % 3) - 1;
            }
            if(s == 1) {
                while(dx == 0)
                    dx = (rand() % 3) - 1;
            } else{
                while(dy == 0)
                    dy = (rand() % 3) - 1;
            }
                while(steps == 0)
                    steps = (rand() % (WIDTH-2)) ;

            while(steps > 0 && shouldMove(i,j,dx,dy,WIDTH,HEIGHT)) {
                i += dy; 
                j += dx; 
                k = i; 
                l = j;
                m[i][j] = 1; 
                while(count_padding > 0) {
                    if(k + count_padding < (HEIGHT - 2)) {
                        k += count_padding;
                        m[k][j] = 1;
                        k -= count_padding;
                    }
                    else if (k - count_padding > 0) {
                        k -= count_padding;
                        m[k][j] = 1;
                        k += count_padding;
                    }
                    if(l + count_padding < (WIDTH - 2)) {
                        l += count_padding;
                        m[i][l] = 1;
                        l -= count_padding;
                    }
                    else if (l - count_padding > 0) {
                        l -= count_padding;
                        m[i][l] = 1;
                        l += count_padding;
                    }
                    count_padding--;
                }
                count_padding = PADDING;
                steps--;
            }
            s_ant = s;        
            steps = 0;
            s = 0;
            dx = 0;
            dy = 0;
            tunnels++;
        }
    }

   
   

    
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
