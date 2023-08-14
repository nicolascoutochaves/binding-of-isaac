#include "map.c"
#include "raylib.h"
#include <math.h>
#include <string.h>
#include <ctype.h>

#define LARGURA GetScreenWidth() // Largura da tela
#define ALTURA GetScreenHeight() // Altura da tela

#define FONT_SIZE 40

// Dimensoes que o mapa tera quando desenhado
#define LARGURA_MAPA 1200
#define ALTURA_MAPA 600
#define MARGIN_TOP 20                            // Margem em relacao ao topo da tela
#define MARGIN_LEFT (LARGURA - LARGURA_MAPA) / 2 // Margem em relacao a esquerda da tela

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
// Fatores de Conversao (Converte da matriz do mapa para a tela):

#define FATORX (LARGURA_MAPA / MAP_WIDTH) // Fator de conversao do eixo x
#define FATORY (ALTURA_MAPA / MAP_HEIGHT) // Fator de conversao do eixo y

#define LADOX (LARGURA_MAPA / MAP_WIDTH) // Espessura x das paredes do mapa
#define LADOY (ALTURA_MAPA / MAP_HEIGHT) // Espessura em y das paredes do mapa

//--------------------------------------------------------------------------------------
//              Personagens:

#define LADO_QUADRADOX LADOX
#define LADO_QUADRADOY LADOY
#define VELOCIDADE 1
#define MAX_INIMIGOS 15
#define MAX_TRAPS 20
#define MAX_BOMBS 20
//          Define o n maximo de mapas do modo normal:
#define MAX_MAPS 9 // Atualizar sempre que acrescentar ou remover um mapa, para que carregue adequadamente ou nao crashe na funcao de manipulacao de arquivo

Texture2D spritesheet;

typedef struct Entidades
{               // Entidades means enemys or players
    int x, y;   // posicao x e y
    int dx, dy; // direcoes
    int vida;
    bool active; // Retorna se esta vivo ou morto
    bool collided;
    bool canChase;
} Entidade; // Struct usada para criar novas "entidades" como jogadores e inimigos.
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
typedef struct Player
{
    Entidade ent;
    int pontuacao;
} Player;

typedef struct ITEM
{
    int x;
    int y;
    bool active;
} ITEM;

typedef struct Map
{
    ITEM portal;
    ITEM trap[MAX_TRAPS];
    ITEM bomb[MAX_BOMBS];
    Player player;
    Entidade inimigo[MAX_INIMIGOS];
} Map;

// Pre declaracao de funcoes (Pois como menu estava sendo declarado depois de novo_jogo, estava aparecendo warning no compilador)
void menu(char *state, int were_played);

void redefineDeslocamento(Entidade *inimigo, int *steps)
{   
    int s = 0;
    inimigo->dx = 0;
    inimigo->dy = 0;
    *steps = 0;
    while(*steps == 0)
        *steps = (int)GetFrameTime()*400 + (rand() % (int)(GetFrameTime()*700)); //Define um tempo para o inimigo se movimentar
    
    while(s == 0)
        s = 1 - (rand() % 3);
        
    if(s == 1)
        while (inimigo->dx == 0)
            inimigo->dx = 1 - (rand() % 3);
    else
        while(inimigo->dy == 0)
            inimigo->dy = 1 - (rand() % 3);
        
}


void movimentar(Entidade *entidade, int map[MAP_HEIGHT][MAP_WIDTH])
{
    entidade->collided = false;
    if ((map[entidade->y][entidade->x + 1] == 0 && entidade->dx == 1) ||
        (map[entidade->y][entidade->x - 1] == 0 && entidade->dx == -1))
    {
        entidade->collided = true;
    } else
        entidade->x += VELOCIDADE * entidade->dx;

    if ((map[entidade->y - 1][entidade->x] == 0 && entidade->dy == 1) ||
        (map[entidade->y + 1][entidade->x] == 0 && entidade->dy == -1))
    {
        entidade->collided = true;       
    } else
        entidade->y -= VELOCIDADE * entidade->dy;

}
//Funcao que recebe uma entidade e verifica quantas paredes existem na direcao que a entidade esta "olhando"
void rayCast(Entidade ent, int map[MAP_HEIGHT][MAP_WIDTH]){
    int lineofsight = 15;
    int sight = 0;
    int seen_objectsx = 0;
    int seen_objectsy = 0;
    int side = 1, sidey =1;
    char text[100];
    //Eixo X
    side = ent.dx;
    while(sight < lineofsight && ent.x+sight*side < MAP_WIDTH && ent.x+sight*side >= 0){
        if(!map[ent.y][ent.x+sight*side]){
            seen_objectsx++;
        }
        sight++;
    }
    sight = 0;
    //Eixo Y
    side = ent.dy;
    while(sight < lineofsight && ent.y-sight*side < MAP_HEIGHT && ent.y-sight*side >= 0){
        if(!map[ent.y-sight*side][ent.x]){
            seen_objectsy++;
        }
        sight++;
    }
    
    /* sprintf(text, "N objetos eixo x: %d", seen_objectsx);
    DrawText(text, LARGURA/2, ALTURA/2, FONT_SIZE, RED);
    sprintf(text, "N objetos eixo y: %d", seen_objectsy);
    DrawText(text, LARGURA/2, ALTURA/2+100, FONT_SIZE, RED); */

}
int persegue(Player player, Entidade *inimigo, int map[MAP_HEIGHT][MAP_WIDTH], int distance)
{       
        int distancex = sqrt(pow(player.ent.x - inimigo->x, 2));
        int distancey = sqrt(pow(player.ent.y - inimigo->y, 2));
        int count, countwall = 0;
        bool perseguiu = false;
        inimigo->canChase = false;
        
         if (((player.ent.x) > (inimigo->x)))
        {
            
            while(count < distancex){
                if(map[inimigo->y][inimigo->x+count] == 0)
                    countwall++;
                
                count++;
            }
            if(countwall == 0)
                    inimigo->canChase = true;
            else
                inimigo->canChase = false;

            if(inimigo->canChase){
                inimigo->dx = 1;
                movimentar(inimigo, map);
                perseguiu = true;
            }
        }

        else if (((player.ent.x) < (inimigo->x)))
        {
            
            while(count < distancex){
                if(map[inimigo->y][inimigo->x-count] == 0)
                    countwall++;
                
                count++;
            }
            if(countwall == 0)
                    inimigo->canChase = true;
            else
                inimigo->canChase = false;

            if(inimigo->canChase){
                inimigo->dx = -1;
                movimentar(inimigo, map);
                perseguiu = true;
            }
        } 

        else if (((player.ent.y) < (inimigo->y)))
        {
            while(count < distancey){
                if(map[inimigo->y-count][inimigo->x] == 0)
                    countwall++;
                count++;
            }
            if(countwall == 0)
                    inimigo->canChase = true;
            else 
                inimigo->canChase = false;
            
            if(inimigo->canChase){
                inimigo->dy = 1;
                movimentar(inimigo, map);
                perseguiu = true;
            }
        }

        else if (((player.ent.y) > (inimigo->y)))
        {
            while(count < distancey){
                if(map[inimigo->y+count][inimigo->x] == 0)
                    countwall++;
                count++;
            }
            if(countwall == 0)
                    inimigo->canChase = true;
            else
                inimigo->canChase = false;

            if(inimigo->canChase){
                inimigo->dy = -1;
                movimentar(inimigo, map);
                perseguiu = true;
            }
        }
        return perseguiu;
}
int modo_jogo = 1;   // 0 e padrao, 1 e a geracao aleatoria
int current_map = 1; // variavel global por enquanto, pra nao precisar passar o mapa atual por referencia para a funcao.
void novo_jogo(char *state)
{

    *state = '\0';
    int map[MAP_HEIGHT][MAP_WIDTH] = {0};
    FILE *maptxt;
    char filename[30];
    char c; // caracter do mapa
    int n_inimigos = 0;
    int enemy_steps;
    int i, j;
    int x = 0, y = 0;
    Player player = {0};
    Entidade inimigo[MAX_INIMIGOS];
    Map Map;
    Rectangle portal;

    srand(time(NULL)); //Gera a seed da funcao rand() usando o tempo do sistema
    if (!modo_jogo)
    {                                                          // Se o modo de jogo for 1, gera o mapa e os spawns aleatoriamente, se nao, carrega os arquivos
        sprintf(filename, "../mapas/mapa%d.txt", current_map); // define o caminho do mapa e salva na variavel "filename", para que possamos alterar o nome do mapa dinamicamente (pois na funcao fopen nao pode passar paramentros para formatacao)

        if (!(maptxt = fopen(filename, "r")))
        {
            DrawText("Erro ao carregar o mapa!", LARGURA / 2 - MeasureText("Erro ao carregar o mapa!", FONT_SIZE), ALTURA, FONT_SIZE, RED);
            WaitTime(1);
            return;
        }
        else
        {
            rewind(maptxt); // coloca o cursor no inicio do mapa
            for (i = 0; i < MAP_HEIGHT; i++)
            {
                for (j = 0; j < MAP_WIDTH; j++)
                {
                    fflush(maptxt);
                    if ((fread(&c, sizeof(char), 1, maptxt)) != 1)
                    {
                        DrawText("Erro ao ler o mapa!", LARGURA / 2 - MeasureText("Erro ao ler o mapa!", FONT_SIZE), ALTURA, FONT_SIZE, RED);
                    }
                    else
                    {
                        c = toupper(c);
                        switch (c)
                        {
                        case '#':
                            map[i][j] = 0; // Parede
                            break;
                        case 'J':
                            map[i][j] = 2; // jogador
                            player.ent.x = j;
                            player.ent.y = i;
                            break;
                        case 'I':
                            map[i][j] = 3; // Inimigo
                            inimigo[n_inimigos].x = j;
                            inimigo[n_inimigos].y = i;
                            n_inimigos++; // incrementa n_inimigos para verificar o proximo inimigo
                            break;
                        case 'B':
                            map[i][j] = 4; // Bomba
                            break;
                        case 'X':
                            map[i][j] = 5; // Armadilha
                            break;
                        case 'P':
                            map[i][j] = 6; // Portal
                            Map.portal.x = j;
                            Map.portal.y = i;
                            Map.portal.active = false;
                            break;
                        case '\n':
                            fseek(maptxt, 1, SEEK_CUR); // Pula o caracter de new line
                            break;
                        default:
                            map[i][j] = 1; // Espaco vazio
                        }
                    }
                }
            }

            fclose(maptxt);
        }
    }
    else
    {
        int difficulty = 1;
        if (n_inimigos > MAX_INIMIGOS)
            n_inimigos = MAX_INIMIGOS;
        n_inimigos = 1 + (difficulty * current_map);
        generateMap(map);

        for (i = 0; i < n_inimigos; i++)
        {
            // Spawn dos Inimigos
            while (map[y][x] == 0)
            {
                x = rand() % MAP_WIDTH - 1;
                y = rand() % (MAP_HEIGHT - 1) / 2;
            }
            inimigo[i].x = x;
            inimigo[i].y = y;
            x = 0;
            y = 0;
        }
        // Spawn do jogador
        while (map[y][x] == 0)
        {
            x = rand() % MAP_WIDTH - 1;
            y = (MAP_HEIGHT - 1) - rand() % (MAP_HEIGHT / 2); // Player spawna na metade de baixo do mapa
        }

        player.ent.x = x;
        player.ent.y = y;
    }
    for (i = 0; i < n_inimigos; i++)
        redefineDeslocamento(&inimigo[i], &enemy_steps);

    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    while (!WindowShouldClose() && *state != 'q' && *state != 'n' && *state != 'p')
    { //'q' = sair, 'n' = novo jogo, 'p' = passou de fase

        //                  JOGADOR:
        // Verifica se o usuario apertou ESC
        if (IsKeyPressed(KEY_ESCAPE))
        {
            *state = 'e';
            menu(state, 1);
            //  Time(0.2);
        }
        
        // Define as direcoes dx e dy do jogador:
        if (IsKeyDown(KEY_D))
            player.ent.dx = 1;
        if (IsKeyDown(KEY_A))
            player.ent.dx = -1;
        if (IsKeyDown(KEY_W))
            player.ent.dy = 1;
        if (IsKeyDown(KEY_S))
            player.ent.dy = -1;


        // Colisoes com os inimigos
        for (i = 0; i < n_inimigos; i++)
        {
            if ((player.ent.x + player.ent.dx == inimigo[i].x && player.ent.y == inimigo[i].y) ||
                (player.ent.y - player.ent.dy == inimigo[i].y && player.ent.x == inimigo[i].x))
            {
                player.ent.collided = true;
            }
            if ((inimigo[i].x + inimigo[i].dx == player.ent.x && inimigo[i].y == player.ent.y) ||
                (inimigo[i].y - inimigo[i].dy == player.ent.y && inimigo[i].x == player.ent.x))
            {
                inimigo[i].collided = true;
            }
        }

        Map.portal.active = true;
        // Colisoes com itens to mapa:
        if (player.ent.x == Map.portal.x && player.ent.y == Map.portal.y && Map.portal.active)
            *state = 'p';

        if (IsKeyPressed(KEY_R))
            current_map = 0; // Reseta o mapa para testes
        if (IsKeyPressed(KEY_SPACE))
        {
            *state = 'p';
        }
       

        if (!player.ent.collided)
            movimentar(&player.ent, map);
        player.ent.collided = false;
        rayCast(player.ent, map);
        player.ent.dx = 0;
        player.ent.dy = 0;

        for (i = 0; i < n_inimigos; i++)
        {
            if(enemy_steps == 0)
                redefineDeslocamento(&inimigo[i], &enemy_steps);

            if (inimigo[i].collided)
            {
                inimigo[i].canChase = false;
                redefineDeslocamento(&inimigo[i], &enemy_steps);   
            } else{
                int distancia = (sqrt(pow((player.ent.x - inimigo[i].x), 2) + pow((player.ent.y - inimigo[i].y), 2)));
                 //inimigo[i].canChase = true;
                
                //if (distancia < 10){
                    // DrawText("ESTA PERTO!", LARGURA/2, ALTURA/2, FONT_SIZE, PURPLE); //verifica se jogador esta perto de inimigo
                    /*  if(!persegue(player, &inimigo[i], map, distancia)){
                        if(enemy_steps > 0){
                            movimentar(&inimigo[i], map);
                            enemy_steps--;
                        }
                    }  */
                //}
                 /* else{
                    if(enemy_steps > 0){
                        movimentar(&inimigo[i], map);
                        enemy_steps--;
                    }
                }  */
            }
            inimigo[i].collided = false;            
        }
        
        //---------------------------------------------------------------------------
        //---------------------------------------------------------------------------
        //                          DESENHAR JOGO:
        BeginDrawing();            // Inicia o ambiente de desenho na tela
        ClearBackground(RAYWHITE); // Limpa a tela e define cor de fundo
        DrawRectangle(player.ent.x * FATORX + MARGIN_LEFT, player.ent.y * FATORY + MARGIN_TOP, LADO_QUADRADOX, LADO_QUADRADOY, GREEN);
        for (i = 0; i < n_inimigos; i++)
        {
            DrawRectangle(inimigo[i].x * FATORX + MARGIN_LEFT, inimigo[i].y * FATORY + MARGIN_TOP, LADOX, LADOY, ORANGE);
        }
        // Desenha o mapa na tela:
        for (i = 0; i < MAP_HEIGHT; i++)
        {
            for (j = 0; j < MAP_WIDTH; j++)
            {
                if (map[i][j] == 0)
                    DrawRectangle(j * FATORX + MARGIN_LEFT, i * FATORY + MARGIN_TOP, LADOX, LADOY, PURPLE);
            }
        }
        DrawRectangleRec(portal, YELLOW); // Portal
        EndDrawing();                     // Finaliza o ambiente de desenho na tela
    }

    //------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------
    if (*state == 'p')
    {
        if (current_map < MAX_MAPS)
            current_map++;
        novo_jogo(state);
    }

    *state = 'q';
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void carregar_jogo(char *state)
{
    *state = '\0';
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void salvar_jogo(char *state)
{
    char text[50];
    while (*state == 's')
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        strcpy(text, "Jogo salvo com sucesso!");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, ALTURA / 2 - FONT_SIZE, FONT_SIZE, BLACK);

        strcpy(text, "Aperte ESC para continuar");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, ALTURA / 2 - FONT_SIZE + 50, FONT_SIZE, BLACK);

        if (IsKeyPressed(KEY_ESCAPE))
            *state = '\0';
        EndDrawing();
    }
    menu(state, 1);
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void sair_jogo(char *state)
{
    char text[50];
    while (*state != 'q')
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        strcpy(text, "Deseja salvar o jogo? (S/N)");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, ALTURA / 2 - FONT_SIZE, FONT_SIZE, BLACK);

        if (IsKeyPressed(KEY_S))
        {
            salvar_jogo(state);
            *state = 'q';
        }
        else if (IsKeyPressed(KEY_N))
        {
            *state = 'q';
        }
        EndDrawing();
    }
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void voltar_jogo(char *state)
{
    *state = '\0';
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void menu(char *state, int were_played)
{ // Menu do jogo
    char text[100] = "";

    while (*state == '\0' || *state == 'e')
    {
        if (IsKeyPressed(KEY_N))
        {
            *state = 'a';
        }
        if (IsKeyPressed(KEY_C))
        {
            *state = 'c';
            carregar_jogo(state);
        }
        if (IsKeyPressed(KEY_S) && were_played)
        {
            *state = 's';
            salvar_jogo(state);
        }
        if (IsKeyPressed(KEY_Q))
        {
            *state = '0'; // atribui a '0' para poder sair do loop do menu.
            sair_jogo(state);
        }
        if (IsKeyPressed(KEY_V) && were_played)
        {
            *state = 'v';
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);

        strcpy(text, "Selecione uma opcao:");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, 100, FONT_SIZE, BLACK);

        strcpy(text, "N: novo jogo");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, 250, FONT_SIZE, BLACK);

        strcpy(text, "C: carregar jogo");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, 300, FONT_SIZE, BLACK);

        strcpy(text, "Q: sair do jogo");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, 400, FONT_SIZE, BLACK);

        if (were_played)
        {
            strcpy(text, "S: salvar jogo");
            DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, 350, FONT_SIZE, BLACK);

            strcpy(text, "V: voltar ao jogo");
            DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, 450, FONT_SIZE, BLACK);
        }
        EndDrawing();
    }
    while (*state == 'a')
    {

        strcpy(text, "Selecione o Modo de jogo:");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, ALTURA / 4 - FONT_SIZE, FONT_SIZE, BLACK);
        strcpy(text, "0 - padrao");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE - 10) / 2, ALTURA / 4 - FONT_SIZE + 100, FONT_SIZE - 10, BLACK);
        strcpy(text, "1 - geracao aleatoria");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE - 10) / 2, ALTURA / 4 - FONT_SIZE + 150, FONT_SIZE - 10, BLACK);

        if (IsKeyPressed(KEY_ZERO) || IsKeyPressed(KEY_KP_0))
        {
            modo_jogo = 0;
            current_map = 1;
            *state = 'n';
            novo_jogo(state);
            were_played = 1;
        }
        if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1))
        {
            modo_jogo = 1;
            current_map = 1;
            *state = 'n';
            novo_jogo(state);
            were_played = 1;
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }
}
//--------------------------------------------------------------------------------------
//---Funcao Principal-------------------------------------------------------------------
int main(void)
{                      //
    char state = '\0'; // Estados do jogo

    InitWindow(LARGURA, ALTURA, "The Binding of Isaac");
    SetTargetFPS(20);
    SetExitKey(KEY_NULL); // remove a opcao de sair do jogo

    while (state == '\0' || state == 'e')
        menu(&state, 0);

    CloseWindow(); // Fecha a janela e o contexto OpenGL
    return 0;
}
