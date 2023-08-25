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
#define MAX_INIMIGOS 14
#define MAX_TRAPS 20
#define MAX_BOMBS 5

//          Define o n maximo de mapas do modo normal:
#define MAX_MAPS 10 // Atualizar sempre que acrescentar ou remover um mapa, para que carregue adequadamente ou nao crashe na funcao de manipulacao de arquivo

Texture2D spritesheet;
int spritex = 0;
int spritey = 0;

typedef struct ENTIDADE {
    // Entidades means enemys or players
    int x, y;   // posicao x e y
    int dx, dy; // direcoes
    int health; // Saude
    int lives;
    int pontuacao;
    int bombs[MAX_BOMBS]; // Bombas
    int n_bombs;
    bool active; //Retorna se esta vivo ou morto
    bool collided; //Retorna se entidade esta em colisao
    bool canChase; //Retorna se entidade pode perseguir um jogador
} ENTIDADE; // Struct usada para criar novas "entidades" como jogadores e inimigos.
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
typedef struct ITEM {
    int x;
    int y;
    bool active;
} ITEM;
typedef struct GAME { // Objetos do jogo ---- Deixei tudo nessa struct para facilitar a manipulacao dos saves
    ITEM portal;
    ITEM trap[MAX_TRAPS];
    ITEM bomb[MAX_BOMBS];
    ENTIDADE player;
    ENTIDADE gaper[MAX_INIMIGOS/2]; //Inimigo que persegue o player quando esta no seu campo de visao
    ENTIDADE pooter[MAX_INIMIGOS/2]; //Inimigo que atira quando ve o player
    int map[MAP_HEIGHT][MAP_WIDTH];
    int current_map;
    int modo_jogo; // 1 e padrao, 2 e a geracao aleatoria
    int n_gaper;
    int n_pooter;
    int n_bombas;
    int n_traps;
    bool isLoaded;
} GAME;

GAME game = {0}; //Declaracao da struct game como global porque absolutamente tudo precia dessa struct e facilita muito os saves e loads
Texture2D isaac;

// Pre declaracao de funcoes (Pois como menu estava sendo declarado depois de novo_jogo, estava aparecendo warning no compilador)
void menu(char *state, int were_played);
int salvar_jogo(char *state, char filename[]);

void loadMap() //Carrega os mapas .txt
{
    FILE *maptxt;
    char c; // caracter buffer do mapa
    char filename[30];
    int i, j;
    sprintf(filename, "../mapas/mapa%02d.txt", game.current_map); // define o caminho do mapa e salva na variavel "filename", para que possamos alterar o nome do mapa dinamicamente (pois na funcao fopen nao pode passar paramentros para formatacao)

    if (!(maptxt = fopen(filename, "r"))) {
        printf("file open error with file ../mapas/map%02d.txt\n", game.current_map);
        WaitTime(1);
        return;
    } else {

        for (i = 0; i < MAP_HEIGHT; i++) {
            for (j = 0; j < MAP_WIDTH; j++) {

                if ((fread(&c, sizeof(char), 1, maptxt)) != 1) {
                    printf("map%02d.txt read failure!\n", game.current_map);
                } else {
                    c = toupper(c);
                    switch (c) {
                    case '#':
                        game.map[i][j] = 0; // Parede
                        break;

                    case '\n':
                        fseek(maptxt, 1, SEEK_CUR); // Pula o caracter de new line
                        break;
                    default:
                        game.map[i][j] = 1; // Espaco vazio
                    }
                    if (!game.isLoaded) {
                        switch (c) {
                        case 'J':
                            game.map[i][j] = 2; // jogador
                            game.player.x = j;
                            game.player.y = i;
                            break;
                        case 'I':
                            game.map[i][j] = 3; // Inimigo
                            game.gaper[game.n_gaper].x = j;
                            game.gaper[game.n_gaper].y = i;
                            game.n_gaper++; // incrementa game.n_gaper para verificar o proximo game.gaper
                            break;
                        case 'B':
                            game.map[i][j] = 4; // Bomba
                            game.bomb[game.n_bombas].x = j;
                            game.bomb[game.n_bombas].y = i;
                            game.n_bombas++;
                            break;
                        case 'X':
                            game.map[i][j] = 5; // Armadilha
                            game.trap[game.n_traps].x = j;
                            game.trap[game.n_traps].y = i;
                            game.n_traps++;
                            break;
                        case 'P':
                            game.map[i][j] = 6; // Portal
                            game.portal.x = j;
                            game.portal.y = i;
                            break;
                        }
                    }
                }
            }
        }

        fclose(maptxt);
    }
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void spawnEnemies(ENTIDADE inimigo[], int n_inimigos) //Spawna os inimigos (na geracao aleatoria)
{
    int i, x = 0, y = 0;
    for (i = 0; i < n_inimigos; i++) {
        while (!game.map[y][x]) {
            x = 3 + (rand() % MAP_WIDTH - 3);
            y = 3 + (rand() % (MAP_HEIGHT - 3));
        }
        inimigo[i].x = x;
        inimigo[i].y = y;
        x = 0;
        y = 0;
    }
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void spawnItems(ITEM item[], int n_items) //Spawna os itens (na geracao aleatoria)
{
    int i, x = 0, y = 0;
    for (i = 0; i < n_items; i++) {
        while (!game.map[y][x]) {
            x = 3 + (rand() % MAP_WIDTH - 3);
            y = 3 + (rand() % (MAP_HEIGHT - 3));
        }
        item[i].x = x;
        item[i].y = y;
        x = 0;
        y = 0;
    }
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void redefineDeslocamento(ENTIDADE *inimigo, int *steps) //Redefine o deslocamento quando o inimigo termina o movimento atual
{
    int s = 0;
    while (s == 0) {
        s = 1 - (rand() % 3);
    }

    inimigo->dx = 0;
    inimigo->dy = 0;
    if (s == 1) {
        while (inimigo->dx == 0)
            inimigo->dx = 1 - (rand() % 3);
    } else {
        while (inimigo->dy == 0)
            inimigo->dy = 1 - (rand() % 3);
    }
    if (*steps == 0)
        *steps = (int)GetFrameTime() * 500 + (rand() % (int)(GetFrameTime() * 700)); // Define um tempo para o game.gaper se movimentar
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void movimentar(ENTIDADE *entidade) //Movimenta uma entidade dentro do mapa, evitando mover atraves das paredes
{
    entidade->collided = false;
    if ((game.map[entidade->y][entidade->x + 1] == 0 && entidade->dx == 1) ||
        (game.map[entidade->y][entidade->x - 1] == 0 && entidade->dx == -1)) {
        entidade->collided = true;
    } else
        entidade->x += VELOCIDADE * entidade->dx;

    if ((game.map[entidade->y - 1][entidade->x] == 0 && entidade->dy == 1) ||
        (game.map[entidade->y + 1][entidade->x] == 0 && entidade->dy == -1)) {
        entidade->collided = true;
    } else
        entidade->y -= VELOCIDADE * entidade->dy;
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
int rayCast(ENTIDADE a, ENTIDADE b) //Funcao que esstabelece um vetor posicao relativa entre duas entidades. funciona como uma linha de visao para os inimigos
{
    int abx = b.x - a.x;                     // vetor posicao dos dois objetos no eixo x
    int aby = b.y - a.y;                     // vetor posicao dos dois objetos no eixo y
    int r = sqrt(pow(abx, 2) + pow(aby, 2)); // distancia entre os dois pontos
    int sight = 1;
    int seen_objects = 0;

    while (sight < r) {
        //
        if (abx != 0 && abs(b.x - a.x) > 0)
            b.x -= (abs(abx) / abx);
        if (aby != 0 && abs(b.y - a.y) > 0)
            b.y -= (abs(aby) / aby);

        int rx = a.x + (b.x - a.x); // posicao x dos quadrados da posicao relativa
        int ry = a.y + (b.y - a.y); // posicao y dos quadrados da posicao relativa

        // DrawRectangle(rx*FATORX + MARGIN_LEFT, ry*FATORY + MARGIN_TOP, 20, 20, BLUE);//Desenha os quadrados que representam a posicao relatica entre ent e object para fim de testes

        if (!game.map[ry][rx]) {
            seen_objects++;
        }
        sight++;
    }
    // Testa e printa na tela os objetos entre a posicao das duas entidades
    // char text[100];
    // sprintf(text, "N objetos na direcao atual: %d", seen_objects);
    // DrawText(text, LARGURA/2-200, ALTURA/2, FONT_SIZE, RED);

    return seen_objects;
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void checkCollision(){ //Checa as colisoes entre jogador, inimigos e itens (exceto o portal)
    int i;

    // Colisoes com os inimigos
    //Gaper:
        for (i = 0; i < game.n_gaper; i++) {
            if ((game.player.x + game.player.dx == game.gaper[i].x && game.player.y == game.gaper[i].y) ||
                (game.player.y - game.player.dy == game.gaper[i].y && game.player.x == game.gaper[i].x)) {
                game.player.health--;
                game.player.collided = true;
            }
            if ((game.gaper[i].x + game.gaper[i].dx == game.player.x && game.gaper[i].y == game.player.y) ||
                (game.gaper[i].y - game.gaper[i].dy == game.player.y && game.gaper[i].x == game.player.x)) {
                game.gaper[i].collided = true;
            }
        }
    //Pooter:
        for (i = 0; i < game.n_pooter; i++) {
            if ((game.player.x + game.player.dx == game.pooter[i].x && game.player.y == game.pooter[i].y) ||
                (game.player.y - game.player.dy == game.pooter[i].y && game.player.x == game.pooter[i].x)) {
                game.player.health--;
                game.player.collided = true;
            }
            if ((game.pooter[i].x + game.pooter[i].dx == game.player.x && game.pooter[i].y == game.player.y) ||
                (game.pooter[i].y - game.pooter[i].dy == game.player.y && game.pooter[i].x == game.player.x)) {
                game.pooter[i].collided = true;
            }
        }

        game.portal.active = true;

        // Colisoes com itens to mapa:
        // Bombas
        for (i = 0; i < game.n_bombas; i++) {
            if (game.player.x == game.bomb[i].x && game.player.y == game.bomb[i].y && game.bomb[i].active) {
                game.player.bombs[i]++;
                game.player.n_bombs++;
                game.bomb[i].active = false;
            }
        }
        // Traps
        for (i = 0; i < game.n_traps; i++) {
            if (game.player.x == game.trap[i].x && game.player.y == game.trap[i].y) {
                game.player.health -= 50;
            }
        }

}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void persegue(ENTIDADE player, ENTIDADE *gaper) //Funcao que implementa a perseguicao entre inimigo e jogador
{
    if (((game.player.x) > (gaper->x))) {
        gaper->dx = 1;
    }
    else if (((game.player.x) < (gaper->x))) {
        gaper->dx = -1;
    } else if (((game.player.y) < (gaper->y))) {
        gaper->dy = 1;
    }
    else if (((game.player.y) > (gaper->y))) {
        gaper->dy = -1;
    }
    movimentar(gaper);
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void DrawGame(){ //Funcao que desenha o jogo
    int i, j;
    BeginDrawing();            // Inicia o ambiente de desenho na tela
    ClearBackground(RAYWHITE); // Limpa a tela e define cor de fundo

    // Desenha o mapa na tela:
    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            if (game.map[i][j] == 0)
                DrawRectangle(j * FATORX + MARGIN_LEFT, i * FATORY + MARGIN_TOP, LADOX, LADOY, PURPLE);
        }
    }


    //Desenha as bombas
    for (i = 0; i < game.n_bombas; i++) {
        if (game.bomb[i].active)
            DrawRectangle(game.bomb[i].x * FATORX + MARGIN_LEFT, game.bomb[i].y * FATORY + MARGIN_TOP, LADO_QUADRADOX, LADO_QUADRADOY, BLACK);
    }

    //Deenha as traps
    for (i = 0; i < game.n_traps; i++) {
        DrawRectangle(game.trap[i].x * FATORX + MARGIN_LEFT, game.trap[i].y * FATORY + MARGIN_TOP, LADO_QUADRADOX, LADO_QUADRADOY, RED);
    }


    //Desenha o portal
    DrawRectangle(game.portal.x * FATORX + MARGIN_LEFT, game.portal.y * FATORY + MARGIN_TOP, LADO_QUADRADOX, LADO_QUADRADOY, GRAY); // Portal


     //Desenha o player
    DrawRectangle(game.player.x * FATORX + MARGIN_LEFT, game.player.y * FATORY + MARGIN_TOP, LADO_QUADRADOX, LADO_QUADRADOY, GREEN);
    Rectangle source;
    Rectangle dest;
    Vector2 origin;
    
    if(IsKeyDown(KEY_P)){
        spritex += 32;
        if(spritex > 320)
            spritex = 0;
    }
    if(IsKeyPressed(KEY_O)){
        spritey += 32;
        if(spritex > (32*5))
            spritex = 0;

    }
    source = (Rectangle){spritex, spritey,32,-32}; //Cordenadas da spritesheet: posicao x e y da sprite, largura e altura que vai ser mostrado (Ao usar laguras e alturas negativas, inverte a imagem)

    dest = (Rectangle){game.player.x*FATORX+MARGIN_LEFT, game.player.y*FATORY+MARGIN_TOP, LADO_QUADRADOX*2, LADO_QUADRADOY*2};
    //dest e o destino da sprite, ou seja, a posicao onde vai ser exibida na tela(como a posicao do jogador);

    origin = (Vector2){LADO_QUADRADOX/2, LADO_QUADRADOY/2}; //origin e pra centralizar melhor a sprite na posicao do jogador (usei para compensar a multiplicacao por 2 que eu fiz no dest pra sprite ficar maior)

    DrawTexturePro(isaac, source, dest, origin, 0.0, RAYWHITE );

    //Desenha os inimigos
    for (i = 0; i < game.n_gaper; i++) {
        if(game.gaper[i].active)
            DrawRectangle(game.gaper[i].x * FATORX + MARGIN_LEFT, game.gaper[i].y * FATORY + MARGIN_TOP, LADOX, LADOY, ORANGE);
    }
    for (i = 0; i < game.n_pooter; i++) {
        if(game.pooter[i].active)
            DrawRectangle(game.pooter[i].x * FATORX + MARGIN_LEFT, game.pooter[i].y * FATORY + MARGIN_TOP, LADOX, LADOY, BROWN);
    }


    EndDrawing();
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void novo_jogo(char *state) //Funcao que carrega um novo jogo (inclusive quando e feito o load do save e quando avanca de fase)
{
    *state = '\0';
    int enemy_steps = 0;

    int i, j;
    int x = 0, y = 0;
    puts("\nStarting new game...\n");

    if (!game.isLoaded) { //Reseta o numero de itens e inimigos caso o jogo NAO tenha sido carregado a partir de um save
        game.n_bombas = 0;
        game.n_gaper = 0;
        game.n_pooter = 0;
        game.n_traps = 0;
    }

    if (!game.modo_jogo) {
        // Se o modo de jogo for 1, gera o mapa e os spawns aleatoriamente, se nao, carrega os arquivos
        loadMap();
    } else {

        int difficulty = 1;
        game.n_gaper = (1 + (difficulty * game.current_map))/2;
        game.n_pooter = (1 + (difficulty * game.current_map))/4;

        if (game.n_gaper > MAX_INIMIGOS/2)
            game.n_gaper = MAX_INIMIGOS/2;
        if (game.n_pooter > MAX_INIMIGOS/2)
            game.n_pooter = MAX_INIMIGOS/2;

        game.n_bombas = 1 + (game.n_gaper * game.current_map) / (1 + 2 * pow(difficulty, 2));
        if (game.n_bombas > MAX_BOMBS)
            game.n_bombas = MAX_BOMBS;
        game.n_traps = 1 + (difficulty * game.current_map);
        if (game.n_traps > MAX_TRAPS)
            game.n_traps = MAX_TRAPS;

        for (i = 0; i < MAP_HEIGHT; i++) {
            for (j = 0; j < MAP_WIDTH; j++) {
                game.map[i][j] = 0;
            }
        }
        if (generateMap(game.map)) {
            puts("game generated sucessfully!");
        } else
            puts("game generation failure");

        // Spawn do jogador
        int count_avaible_cells = 0;
        while (game.map[y][x] == 0) {
            x = 3 + (rand() % MAP_WIDTH - 3);
            for (i = (int)(MAP_HEIGHT / 2); i < MAP_HEIGHT; i++) {
                for (j = 0; j < MAP_WIDTH; j++) {
                    if (game.map[i][j])
                        count_avaible_cells++;
                }
            }
            if (!count_avaible_cells)                    // Se nao houver espaco na metade debaixo do mapa:
                y = 3 + rand() % ((MAP_HEIGHT - 3) / 2); // ENTIDADE spawna em qualquer posicao livre do mapa
            else
                y = ((MAP_HEIGHT - 3) - (3 + rand() % ((MAP_HEIGHT - 3) / 2))); // ENTIDADE spawna na metade de baixo do mapa
        }
        game.player.x = x;
        game.player.y = y;
        puts("Setted player spawn");

        // Portal

        game.portal.x = MAP_WIDTH - 2;
        game.portal.y = 2 + (rand() % (MAP_HEIGHT - 5));
        while (!game.map[game.portal.y][game.portal.x - 1]) {
            game.portal.y = 2 + (rand() % (MAP_HEIGHT - 5));
        }
        game.map[game.portal.y][game.portal.x] = 6;
        puts("Portal spawned");

        spawnEnemies(game.gaper, game.n_gaper);
        spawnEnemies(game.pooter, game.n_pooter);

        puts("Setted enemies spawns");

        spawnItems(game.bomb, game.n_bombas);
        printf("Bombs spawns sucessfully setted: %d bombas in current map\n", game.n_bombas);

        spawnItems(game.trap, game.n_traps);
        printf("Traps spawns sucessfully setted: %d traps in current map\n", game.n_traps);
    }
    if (!game.isLoaded) {
        for (i = 0; i < game.n_gaper; i++) {
            redefineDeslocamento(&game.gaper[i], &enemy_steps);
        }
        puts("enemies directions setted");

        for (i = 0; i < game.n_bombas; i++) {
            game.bomb[i].active = true;
        }
        for (i = 0; i < game.n_gaper; i++) {
            game.gaper[i].active = true;
        }
        for (i = 0; i < game.n_bombas; i++) {
            game.pooter[i].active = true;
        }

        game.portal.active = false;
        game.player.health = 100;
        game.player.lives = 3;
        game.player.n_bombs = 0;
    }
    game.isLoaded = false;

    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    while (!WindowShouldClose() && *state != 'q' && *state != 'n' && *state != 'p') {
        //'q' = sair, 'n' = novo jogo, 'p' = passou de fase
        //                  JOGADOR:
        // Verifica se o usuario apertou ESC
        if (IsKeyPressed(KEY_ESCAPE)) {
            puts("Game paused");
            *state = 'e';
            menu(state, 1);
            if (*state == 'g') {
                return; // Remove um bug de o jogador iniciar um novo jogo pelo pause e morrer na primeira fase e nao exibir o game over
            }
            //  Time(0.2);
        }

        // Define as direcoes dx e dy do jogador:
        if (IsKeyDown(KEY_D))
            game.player.dx = 1;
        if (IsKeyDown(KEY_A))
            game.player.dx = -1;
        if (IsKeyDown(KEY_W))
            game.player.dy = 1;
        if (IsKeyDown(KEY_S))
            game.player.dy = -1;

        checkCollision();

        // Portal
        if (game.player.x == game.portal.x && game.player.y == game.portal.y && game.portal.active)
            *state = 'p';

        // Movimentacao do jogador
        if (!game.player.collided)
            movimentar(&game.player);
        game.player.collided = false;
        game.player.dx = 0;
        game.player.dy = 0;

        // Movimentacao dos inimigos

        for (i = 0; i < game.n_gaper; i++) {
            if(game.gaper[i].active){
                if (enemy_steps == 0)
                    redefineDeslocamento(&game.gaper[i], &enemy_steps);
                if (game.gaper[i].collided) {
                    redefineDeslocamento(&game.gaper[i], &enemy_steps);
                }
                if (rayCast(game.gaper[i], game.player) != 0) {
                    // Chama a funcao que verifica se tem paredes entre o game.gaper e o jogador
                    game.gaper[i].canChase = false;
                }
                if (game.gaper[i].canChase && !game.gaper[i].collided) {
                    persegue(game.player, &game.gaper[i]);
                } else {
                    if (enemy_steps > 0) {
                        movimentar(&game.gaper[i]);
                        enemy_steps--;
                    }
                }
                game.gaper[i].canChase = true;
                game.gaper[i].collided = false;
            }
        }

        for (i = 0; i < game.n_pooter; i++) {
            if(game.pooter[i].active){
                if (enemy_steps == 0)
                    redefineDeslocamento(&game.pooter[i], &enemy_steps);
                if (game.pooter[i].collided) {
                    redefineDeslocamento(&game.pooter[i], &enemy_steps);
                }
                if (rayCast(game.pooter[i], game.player) != 0) {
                    // Chama a funcao que verifica se tem paredes entre o game.gaper e o jogador
                    game.pooter[i].canChase = false;
                }
                if (game.pooter[i].canChase && !game.pooter[i].collided) {

                } else {
                    if (enemy_steps > 0) {
                        movimentar(&game.pooter[i]);
                        enemy_steps--;
                    }
                }
                game.pooter[i].canChase = true;
                game.pooter[i].collided = false;
            }

        }


        if (game.player.health < 1) {
            game.player.lives--;
            game.player.health = 100;
        }

        printf("\nplayer health: %d\n", game.player.health);
        printf("player lives: %d\n", game.player.lives);
        printf("player bombs: %d\n", game.player.n_bombs);

        if (IsKeyDown(KEY_SPACE)) {
            *state = 'p';
        }

        // Game Over:
        if (IsKeyPressed(KEY_MINUS))
            game.player.lives--;
        if (!game.player.lives) {
            char text[30];
            *state = 'g';
            strcpy(text, "Game Over!");
            DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE + 10) / 2, ALTURA / 2 - FONT_SIZE + 10, FONT_SIZE, RED);
            return;
        }

        //---------------------------------------------------------------------------
        //---------------------------------------------------------------------------
        //                          DESENHAR JOGO:
        DrawGame();
    }

    //------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------
    if (*state == 'p') {
        puts("Changing current map...\n");
        if (game.current_map < MAX_MAPS)
            game.current_map++;
        novo_jogo(state);
    }
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
int carregar_jogo(char *state, char filename[]) //Faz o load do save
{
    char text[50];
    int carregou = 0;
    FILE *savestate;
    if (!(savestate = fopen(filename, "rb"))) {
        puts("Erro ao abrir o arquivo de save!");
    } else {
        if ((fread(&game, sizeof(game), 1, savestate)) != 1) {
            puts("Erro ao carregar o jogo");
        } else {
            puts("Game loaded sucessfully!");
            carregou = 1;
        }
        fclose(savestate);
    }

    while (*state == 'c') {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        strcpy(text, "Jogo Carregado com sucesso!");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, ALTURA / 2 - FONT_SIZE, FONT_SIZE, BLACK);

        strcpy(text, "Aperte ESC para continuar");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE - 10) / 2, ALTURA / 2 - FONT_SIZE - 10 + 70, FONT_SIZE - 10, BLACK);

        if (IsKeyPressed(KEY_ESCAPE))
            *state = '\0';
        EndDrawing();
    }
    return carregou;
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
int salvar_jogo(char *state, char filename[]) //Salva a struct game em um arquivo binario
{
    char text[50];
    FILE *savestate;
    int salvou = 0;
    if ((!game.modo_jogo)) {
        if (!(savestate = fopen(filename, "wb"))) {
            puts("Erro ao abrir o arquivo de save!");
        } else {
            if ((fwrite(&game, sizeof(game), 1, savestate)) != 1) {
                puts("Erro ao salvar o jogo");
            } else {
                salvou = 1;
            }
            fclose(savestate);
        }
    }
    while (*state == 's') {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        if (game.modo_jogo) {
            strcpy(text, "Impossivel salvar no survival mode!");
            DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, ALTURA / 2 - FONT_SIZE, FONT_SIZE, BLACK);
        } else {
            strcpy(text, "Jogo salvo com sucesso!");
            DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, ALTURA / 2 - FONT_SIZE, FONT_SIZE, BLACK);
        }

        strcpy(text, "Aperte ESC para continuar");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE - 10) / 2, ALTURA / 2 - FONT_SIZE - 10 + 70, FONT_SIZE - 10, BLACK);
        if (IsKeyPressed(KEY_ESCAPE))
            *state = '\0';
        EndDrawing();
    }
    // menu(state, 1);
    return salvou;
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void sair_jogo(char *state) //Encerra o jogo apos perguntar se o usuario gostaria de salvar o jogo
{
    char text[50];
    char save_path[50] = "../saves/save.bin";
    while (*state != 'q') {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        strcpy(text, "Deseja salvar o jogo? (S/N)");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, ALTURA / 2 - FONT_SIZE, FONT_SIZE, BLACK);

        if (IsKeyPressed(KEY_S)) {
            salvar_jogo(state, save_path);
            *state = 'q';
        } else if (IsKeyPressed(KEY_N)) {
            *state = 'q';
        }
        EndDrawing();
    }
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void voltar_jogo(char *state) //Simplesmente retorna ao jogo
{
    *state = '\0';
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void menu(char *state, int were_played) //Menu principal do jogo
{
    // Menu do jogo
    char text[100] = "";
    char save_path[50] = "../saves/save.bin";

    while (*state == '\0' || *state == 'e' || *state == 'g') {
        if (*state == 'g') {
            WaitTime(2);
            *state = '\0';
        }

        if (IsKeyPressed(KEY_N)) {
            *state = 'a';
        }
        if (IsKeyPressed(KEY_C)) {
            *state = 'c';
            if (carregar_jogo(state, save_path)) {
                game.isLoaded = true;
                novo_jogo(state);
            }

            else {

                strcpy(text, "Nao foi possivel carregar o jogo!:");
                DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, 100, FONT_SIZE, RED);
            }
        }
        if (IsKeyPressed(KEY_S) && were_played) {
            *state = 's';
            if (salvar_jogo(state, save_path))
                puts("Game saved sucessfully");
        }
        if (IsKeyPressed(KEY_Q)) {
            puts("Exiting game...");
            *state = '0'; // atribui a '0' para poder sair do loop do menu.
            sair_jogo(state);
        }
        if (IsKeyPressed(KEY_V) && were_played) {
            puts("Game resumed");
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

        if (were_played) {
            strcpy(text, "S: salvar jogo");
            DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, 350, FONT_SIZE, BLACK);

            strcpy(text, "V: voltar ao jogo");
            DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, 450, FONT_SIZE, BLACK);
        }
        EndDrawing();
    }
    while (*state == 'a') {

        strcpy(text, "Selecione o Modo de jogo:");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, ALTURA / 4 - FONT_SIZE, FONT_SIZE, BLACK);
        strcpy(text, "1 - Padrao");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE - 10) / 2, ALTURA / 4 - FONT_SIZE + 100, FONT_SIZE - 10, BLACK);
        strcpy(text, "2 - Survival Mode (infinito)");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE - 10) / 2, ALTURA / 4 - FONT_SIZE + 150, FONT_SIZE - 10, BLACK);

        if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
            game.modo_jogo = 0;
            game.current_map = 1;
            *state = 'n';
            novo_jogo(state);
            were_played = 1;
        }
        if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) {
            game.modo_jogo = 1;
            game.current_map = 1;
            *state = 'n';
            novo_jogo(state);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }
}
//--------------------------------------------------------------------------------------
//---Funcao Principal-------------------------------------------------------------------
int main(void) //Funcao principal que apenas chama o menu
{
    //
    char state = '\0'; // Estados do jogo
    // char save_path[50] = "../saves/save.bin";

    InitWindow(LARGURA, ALTURA, "The Binding of Isaac");
    SetTargetFPS(24);
    SetExitKey(KEY_NULL); // remove a opcao de sair do jogo
    isaac = LoadTexture("../sprites/isaac.png");

    while (state == '\0' || state == 'e' || state == 'g' || state == 'p')
        menu(&state, 0);

    CloseWindow(); // Fecha a janela e o contexto OpenGL
    return 0;
}
