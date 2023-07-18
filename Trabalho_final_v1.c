#include "raylib.h"
#include <string.h>

#define LARGURA 1000
#define ALTURA 600

void menu(){
    DrawText("Selecione uma opcao:", 300, 200, 40, BLACK);

    DrawText("N: novo jogo", 300, 300, 40, BLACK);
    DrawText("C: carregar jogo", 300, 350, 40, BLACK);
    DrawText("S: salvar jogo", 300, 400, 40, BLACK);
    DrawText("Q: sair jogo", 300, 450, 40, BLACK);
    DrawText("V: voltar jogo", 300, 500, 40, BLACK);
}

void novo_jogo(){
}

void carregar_jogo(){
}

void salvar_jogo(){
}

void sair_jogo(){
}

void voltar_jogo(){
}

int main(void){

    InitWindow(LARGURA, ALTURA, "JOGO");
    SetTargetFPS(60);

    char op;

    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        //menu();
        if (IsKeyPressed(KEY_RIGHT)){
            menu();
            scanf(" %c", &op);
            switch(toupper(op)){
                case 'N': novo_jogo(); break;
                case 'C': carregar_jogo(); break;
                case 'S': salvar_jogo(); break;
                case 'Q': sair_jogo(); break;
                case 'V': voltar_jogo(); break;
                default: printf("Erro");
            }
        }

        BeginDrawing();//Inicia o ambiente de desenho na tela
        ClearBackground(RAYWHITE);//Limpa a tela e define cor de fundo
        EndDrawing();//Finaliza o ambiente de desenho na tela
    }
    CloseWindow();// Fecha a janela e o contexto OpenGL

    return 0;
}
