#include "raylib.h"
#define ALTURA 600
#define LARGURA 600

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
