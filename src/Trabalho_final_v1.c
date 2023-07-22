#include "raylib.h"
#include <string.h>

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

int main(void){

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
