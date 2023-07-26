#include "raylib.h"

#include "movimentacao.c"

#define LARGURA 600
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

  void novo_jogo(int *x, int *y, int map[MAP_HEIGHT][MAP_WIDTH]){
    int difficulty = 2;
    movimentar(x, y, map);
    DrawRectangle(*x, *y, LADO_QUADRADO, LADO_QUADRADO, GREEN);
        
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





int main(void){
    
   
    int map[MAP_HEIGHT][MAP_WIDTH] = {0};
    generateMap(map);

    InitWindow(LARGURA, ALTURA, "JOGO");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);//remove a opcao de sair do jogo
    int fatorx = LARGURA / MAP_WIDTH, fatory = ALTURA / MAP_HEIGHT;
    int x, y;

    x = (MAP_WIDTH * fatorx -(3*fatorx) )- LADO; //Pois lado sempre sera divisor da largura ou altura.
    y = 3*fatory; //Pois lado sempre sera 

    while (!WindowShouldClose()) // Detect window close button or exit key
    {
        //if (IsKeyPressed(KEY_M)) menu();
        novo_jogo(&x, &y, map);
        BeginDrawing();//Inicia o ambiente de desenho na tela
        ClearBackground(RAYWHITE);//Limpa a tela e define cor de fundo
        EndDrawing();//Finaliza o ambiente de desenho na tela
    }
    CloseWindow();// Fecha a janela e o contexto OpenGL

    return 0;
}
