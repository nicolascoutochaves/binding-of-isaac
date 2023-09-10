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

#define LADOX 20 // Espessura x das paredes do mapa
#define LADOY 20 // Espessura em y das paredes do mapa

//--------------------------------------------------------------------------------------
//              Personagens:

#define LADO_QUADRADOX 20
#define LADO_QUADRADOY 20
#define VELOCIDADE 1
#define MAX_INIMIGOS 14
#define MAX_TRAPS (MAP_HEIGHT*MAP_WIDTH)/4 // ate 1/4 do mapa pode ser trap
#define MAX_BOMBS 100
#define EXP_RADIUS 6
#define EXP_AREA EXP_RADIUS*EXP_RADIUS*400
#define DEATH_DELAY 2 //Tempo que o personagem fica morto
#define MAX_TIROS 10
#define PLAYER_SPEED 0.10
#define ENEMY_SPEED 0.11
#define BOSS_SPEED 0.18
#define SHOT_SPEED 0.065
#define BOSS_DELAY 30 //Tempo para o boss chamar reforcos
#define SPECIAL_DELAY 10 //delay para o player usar o especial
#define MAX_SCORES 10




//          Define o n maximo de mapas do modo normal:
#define MAX_MAPS 10 // Atualizar sempre que acrescentar ou remover um mapa, para que carregue adequadamente ou nao crashe na funcao de manipulacao de arquivo
typedef struct TIRO{
    Texture2D texture;
    Color color;
    int x, y;
    int dx, dy;
    float delay;
    float speed;
    bool active; //Retorna se esta ativo ou nao
    bool collided; //Retorna se entidade esta em colisao
    bool isPlayer;
}TIRO;
typedef struct ENTIDADE {
    // Entidade means enemys or players
    TIRO tiro[MAX_TIROS];
    Texture2D texture;
    Color color;
    int x, y;   // posicao x e y
    int dx, dy; // direcoes
    int health; // Saude
    float delay; // delay do movimento
    float special_delay; //delay da habilidade especial
    int lives;
    int pontuacao;
    int n_bombs;
    int n_tiros;
    int spritex; //posicao x e y das sprites
    int spritey;
    bool active; //Retorna se esta vivo ou morto
    bool collided; //Retorna se entidade esta em colisao
    bool canChase; //Retorna se entidade pode perseguir um jogador
    bool isPlayer;
    bool isBoss;
} ENTIDADE; // Struct usada para criar novas "entidades" como jogadores e inimigos.
//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
typedef struct ITEM {
    Texture2D texture;
    Color color;
    int x;
    int y;
    int spritex;
    int spritey;
    float alive_time;
    bool active;
    bool colectable;
} ITEM;

typedef struct TIMER{
    float death;
    float moviment;
    float invecible;
    float enemy_shot;
} TIMER;

typedef struct GAME { // Objetos do jogo ---- Deixei tudo nessa struct para facilitar a manipulacao dos saves
    ITEM portal;
    ITEM trap[MAX_TRAPS];
    ITEM bomb[MAX_BOMBS];
    ITEM explosion[EXP_AREA];
    ENTIDADE player;
    ENTIDADE gaper[MAX_INIMIGOS/2]; //Inimigo que persegue o player quando esta no seu campo de visao
    ENTIDADE pooter[MAX_INIMIGOS/2]; //Inimigo que atira quando ve o player
    ENTIDADE boss;
    ENTIDADE tiro[MAX_TIROS];
    int map[MAP_HEIGHT][MAP_WIDTH];
    int current_map;
    int modo_jogo; // 1 e padrao, 2 e a geracao aleatoria
    int n_gaper;
    int n_pooter;
    int n_bombas;
    int n_traps;
    int n_explosion;
    char state;
    bool isLoaded;
    int frameCount;
    int n_tiro;
} GAME;

GAME game = {0}; //Declaracao da struct game como global porque absolutamente tudo precia dessa struct e facilita muito os saves e loads
TIMER timer;

Texture2D background;
Texture2D blocks;
Texture2D door;
//Acessa outras linhas do arquivo de sprite para realizar as animacoes:
int s_sizex = 32, s_sizey = 32; //tamanhos das sprites
int s_front = 0; //animacao frontal
int s_back = 32; //animacao traseira
int s_right = 64; //animacao direita
int s_left = 96; //animacao esquerda

// Pre declaracao de funcoes (Pois como menu estava sendo declarado depois de novo_jogo, estava aparecendo warning no compilador)
void menu(int were_played);
int salvar_jogo(char filename[]);

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
                            
                            game.player.x = j;
                            game.player.y = i;
                            break;
                        case 'I':
                            
                            game.gaper[game.n_gaper].x = j;
                            game.gaper[game.n_gaper].y = i;
                            game.n_gaper++; // incrementa game.n_gaper para verificar o proximo game.gaper
                            break;
                        case 'Y':
                            game.pooter[game.n_pooter].x = j;
                            game.pooter[game.n_pooter].y = i;
                            game.n_pooter++; // incrementa game.n_gaper para verificar o proximo game.gaper
                            break;
                        case 'F':
                            game.boss.x = j;
                            game.boss.y = i;
                            break;
                        case 'B':
                            
                            game.bomb[game.n_bombas].x = j;
                            game.bomb[game.n_bombas].y = i;
                            game.n_bombas++;
                            break;
                        case 'X':
                            
                            game.trap[game.n_traps].x = j;
                            game.trap[game.n_traps].y = i;
                            game.n_traps++;
                            break;
                        case 'P':
                            
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
        inimigo[i].active = true;
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
        *steps = 10 + rand() % 60; // Define um tempo para o game.gaper se movimentar
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void movimentar(ENTIDADE *entidade) //Movimenta uma entidade dentro do mapa, evitando mover atraves das paredes
{
    float move_delay;
    if(entidade->isPlayer)
        move_delay = PLAYER_SPEED;
    else move_delay = ENEMY_SPEED;

    entidade->delay += GetFrameTime();

    if(entidade->delay > move_delay && !entidade->collided){
        if (!(game.map[entidade->y][entidade->x + entidade->dx])) {
            entidade->collided = true;
        } else
            entidade->x += VELOCIDADE * entidade->dx;

        if (!(game.map[entidade->y - entidade->dy][entidade->x])) {
            entidade->collided = true;
        } else
            entidade->y -= VELOCIDADE * entidade->dy;
        entidade->delay = 0;
    } 
    
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void movimentarBoss(ENTIDADE *entidade){ //Movimenta entidades maiores
    int i, j;
    entidade->collided = false;
    entidade->delay += GetFrameTime();
    for(i = 0; i < MAP_HEIGHT; i++){
        for(j = 0; j < MAP_WIDTH; j++){
            if(!game.map[i][j]){
                if(CheckCollisionRecs((Rectangle){(entidade->x+entidade->dx), (entidade->y - entidade->dy), 7, 7},(Rectangle){j, i, 1, 1})){
                    entidade->collided = true;
                } 
            }
        }
    }
    if(entidade->delay > BOSS_SPEED){
        if(!entidade->collided){
            entidade->x += VELOCIDADE * entidade->dx;
            entidade->y -= VELOCIDADE * entidade->dy;
        }
        entidade->delay = 0;
    }


}


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
void checkCollision(ENTIDADE *ent){ //Checa as colisoes entre jogador, inimigos e itens (exceto o portal)
    int i, j;

    // Colisoes com os inimigos
    //Gaper:
        for (i = 0; i < game.n_gaper; i++) {
            if(game.gaper[i].active && game.player.active){
                if ((game.player.x + game.player.dx == game.gaper[i].x && game.player.y == game.gaper[i].y) ||
                    (game.player.y - game.player.dy == game.gaper[i].y && game.player.x == game.gaper[i].x)) {
                    game.player.health -= 60;
                    game.player.active = false; //Player fica invulneravel apos o primeiro dano
                }
            }
        }
    //Pooter:
        for (i = 0; i < game.n_pooter; i++) {
            if(game.pooter[i].active && game.player.active){
                if ((game.player.x + game.player.dx == game.pooter[i].x && game.player.y == game.pooter[i].y) ||
                    (game.player.y - game.player.dy == game.pooter[i].y && game.player.x == game.pooter[i].x)) {
                    game.player.health -= 60;
                    game.player.active = false;//Player fica invulneravel apos o primeiro dano
                }
            }            
        }

        //Colisao com o boss
        if( ((game.player.x - game.boss.x) >= 0 && (game.player.x - game.boss.x) < 7) && ((game.player.y - game.boss.y) >= 0 && (game.player.y - game.boss.y) < 7) && game.boss.active && game.player.active){
            //printf("Entrou no boss! %d, %d\n", game.player.x - game.boss.x, game.player.y - game.boss.y);
            game.player.health -= 90;
        }



        // Colisoes com itens to mapa:
        // Bombas
        for (i = 0; i < game.n_bombas; i++) {
            if ((ent->x + ent->dx == game.bomb[i].x && ent->y == game.bomb[i].y) ||
                (ent->y - ent->dy == game.bomb[i].y && ent->x == game.bomb[i].x)){
                if (!game.bomb[i].colectable && game.bomb[i].active){
                    ent->collided = true;
                }

                if(game.bomb[i].colectable  && game.bomb[i].active && ent->active && ent->isPlayer){
                    ent->n_bombs++;
                    ent->pontuacao += 50;
                    game.bomb[i].active = false;
                }
            }
        }
        if(ent->x == game.bomb[game.n_bombas-1].x && ent->y == game.bomb[game.n_bombas-1].y)
            ent->collided = false; //Corrige o bug de ficar preso dentro da bomba (principalmente quando player bota a bomba)



        // Traps
        for (i = 0; i < game.n_traps; i++) {
            if (ent->x == game.trap[i].x && ent->y == game.trap[i].y && ent->isPlayer) {
                game.player.health -= 80;
            }
        }

        //Tiros do Jogador
        for (i = 0; i < MAX_TIROS; i++) {
            if((ent->x == game.player.tiro[i].x && ent->y == game.player.tiro[i].y) && game.player.tiro[i].active){
                if(!ent->isPlayer){//Player nao morre com proprio tiro
                    ent->health -= 60;
                    game.player.pontuacao += 150;
                    game.player.tiro[i].collided = true;
                }
            }
            if((game.player.tiro[i].x - game.boss.x) >= 0 && (game.player.tiro[i].x - game.boss.x) < 7 && (game.player.tiro[i].y - game.boss.y) >= 0 && (game.player.tiro[i].y - game.boss.y < 7) && game.boss.active && game.player.tiro[i].active){
                game.boss.health -= 60;
                game.player.pontuacao += 50;
                game.player.tiro[i].collided = true;
            }
        }
        
        //Tiros do inimigo
        for (i = 0; i < game.n_pooter; i++) {
            for(j = 0; j < MAX_TIROS; j++){
                if((ent->x == game.pooter[i].tiro[j].x && ent->y == game.pooter[i].tiro[j].y) && game.pooter[i].tiro[j].active){
                    if(ent->isPlayer){
                        ent->health -= 60;
                        game.player.active = false; //Player fica invulneravel apos o primeiro tiro
                        game.pooter[i].tiro[j].collided = true;
                    }
                }
            }
            
        }

        //Explosoes:
        for(i = 0; i < game.n_explosion; i++){
        if(ent->x == game.explosion[i].x && ent->y == game.explosion[i].y) {
            ent->health -= 100;
            game.player.pontuacao += 500;
        }
    }

    if(ent->health <= 0){
        ent->active = false;
    }

    if(game.boss.health <= 0 && game.boss.active){
        game.boss.active = false;
        game.player.pontuacao += 5000;
    }

}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void persegue(ENTIDADE player, ENTIDADE *gaper) //Funcao que implementa a perseguicao entre inimigo e jogador
{
    int steps; //Passos aleatorios que gaper vai dar caso colida com algum objeto
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

    if(gaper->collided){
        gaper->dx *= -1;
        gaper->dy *= -1;
    }
    if(gaper->isBoss == true){
        movimentarBoss(gaper);
    }
    else movimentar(gaper);
}
//------------------------------------------------------------------------------------------------
void checkBombs(){
    int tmp = 0;
    int i, j, exp_range;
    int count, shape = 2; //Contador para modelar as explosoes -> shape significa quanto arredondado vai ser as explosoes
    for(i = 0; i < game.n_bombas; i++){
        if(game.bomb[i].active && !game.bomb[i].colectable){
            game.bomb[i].alive_time += GetFrameTime();
            char text[40];
            sprintf(text, "bomb %d alive_time: %f", i, game.bomb[i].alive_time);
            puts(text);
            for(j = 0; j < game.n_explosion; j++){
                if(game.bomb[i].x == game.explosion[j].x && game.bomb[i].y == game.explosion[j].y){
                    game.bomb[i].alive_time = 4;
                }
            }
        }
        if(game.bomb[i].alive_time > 3){
            game.bomb[i].active = false;
            game.bomb[i].alive_time = 0;
            tmp = 0;
            count = -shape;
            for(j = 0; j < EXP_RADIUS; j++){
                exp_range = 0;
                if(count <= 0) tmp = 0;
                while(exp_range < EXP_RADIUS-tmp){
                    game.explosion[game.n_explosion].y = game.bomb[i].y-exp_range;
                    game.explosion[game.n_explosion].x = game.bomb[i].x+j;
                    if(!game.map[game.explosion[game.n_explosion].y][game.bomb[i].x]){
                        exp_range = EXP_RADIUS;
                    }
                    if(!game.map[game.bomb[i].y][game.explosion[game.n_explosion].x]){
                        j = EXP_RADIUS;
                    }
                    if(exp_range != EXP_RADIUS && j != EXP_RADIUS){
                        game.explosion[game.n_explosion].active = true;
                        game.n_explosion++;
                    }
                    exp_range++;
                }
                tmp++;
                count++;
            }
            tmp = 0;
            count = -shape;
            for(j = 0; j < EXP_RADIUS; j++){
                exp_range = 0;
                if(count <= 0) tmp = 0;
                while(exp_range < EXP_RADIUS-tmp){
                    game.explosion[game.n_explosion].y = game.bomb[i].y-exp_range;
                    game.explosion[game.n_explosion].x = game.bomb[i].x-j;
                    if(!game.map[game.explosion[game.n_explosion].y][game.bomb[i].x]){
                        exp_range = EXP_RADIUS;
                    }
                    if(!game.map[game.bomb[i].y][game.explosion[game.n_explosion].x]){
                        j = EXP_RADIUS;
                    }
                    if(exp_range != EXP_RADIUS && j != EXP_RADIUS){
                        game.explosion[game.n_explosion].active = true;
                        game.n_explosion++;
                    }
                    exp_range++;
                }
                tmp++;
                count++;
            }

            tmp = 0;
            count = -shape;
            for(j = 0; j < EXP_RADIUS; j++){
                exp_range = 0;
                if(count <= 0) tmp = 0;
                while(exp_range < EXP_RADIUS-tmp){
                    game.explosion[game.n_explosion].y = game.bomb[i].y+exp_range;
                    game.explosion[game.n_explosion].x = game.bomb[i].x+j;
                    if(!game.map[game.explosion[game.n_explosion].y][game.bomb[i].x]){
                        exp_range = EXP_RADIUS;
                    }
                    if(!game.map[game.bomb[i].y][game.explosion[game.n_explosion].x]){
                        j = EXP_RADIUS;
                    }
                    if(exp_range != EXP_RADIUS && j != EXP_RADIUS){
                        game.explosion[game.n_explosion].active = true;
                        game.n_explosion++;
                    }
                    exp_range++;
                }
                tmp++;
                count++;
            }
            tmp = 0;
            count = -shape;
            for(j = 0; j < EXP_RADIUS; j++){
                exp_range = 0;
                if(count <= 0) tmp = 0;
                while(exp_range < EXP_RADIUS-tmp){
                    game.explosion[game.n_explosion].y = game.bomb[i].y+exp_range;
                    game.explosion[game.n_explosion].x = game.bomb[i].x-j;
                    if(!game.map[game.explosion[game.n_explosion].y][game.bomb[i].x]){
                        exp_range = EXP_RADIUS;
                    }
                    if(!game.map[game.bomb[i].y][game.explosion[game.n_explosion].x]){
                        j = EXP_RADIUS;
                    }
                    if(exp_range != EXP_RADIUS && j != EXP_RADIUS){
                        game.explosion[game.n_explosion].active = true;
                        game.n_explosion++;
                    }
                    exp_range++;
                }
                tmp++;
                count++;
            }
                  
        }

        if(!game.bomb[i].active && !game.bomb[i].colectable){
            game.bomb[i].alive_time += GetFrameTime();
            if(game.bomb[i].alive_time > 1.5){
                game.bomb[i].alive_time = 0;
                game.bomb[i].colectable = true;
                puts("explosao acabou!");
                game.n_explosion = 0;
            }
            
        }
        
    }
    
}
//------------------------------------------------------------------------------------------------
void DrawGame(){ //Funcao que desenha o jogo
    int i, j;
    int orientation;
    char text[50];
    Rectangle source;
    Rectangle dest;
    Vector2 origin;

    BeginDrawing();            // Inicia o ambiente de desenho na tela
    ClearBackground(BLACK); // Limpa a tela e define cor de fundo

    origin = (Vector2){-MARGIN_LEFT, -MARGIN_TOP}; //Dar a margem do mapa

    source = (Rectangle){0, 0, 1200, 600}; //Cordenadas da spritesheet: posicao x e y da sprite, largura e altura que vai ser mostrado (Ao usar laguras e alturas negativas, inverte a imagem)
    dest = (Rectangle){0, 0, LARGURA_MAPA, ALTURA_MAPA};
    //dest e o destino da sprite, ou seja, a posicao onde vai ser exibida na tela(como a posicao do jogador);
    DrawTexturePro(background, source, dest, origin, 0.0, RAYWHITE);

    

    // Desenha os blocos do mapa:
    for (i = 2; i < MAP_HEIGHT-2; i++) {
        for (j = 2; j < MAP_WIDTH-2; j++) { 
            if (game.map[i][j] == 0){
                DrawRectangle(j * FATORX + MARGIN_LEFT+1, i * FATORY + MARGIN_TOP+1, LADOX-2, LADOY-2, PURPLE);

                source = (Rectangle){0, 0, LADOX, LADOY}; //Cordenadas da spritesheet: posicao x e y da sprite, largura e altura que vai ser mostrado (Ao usar laguras e alturas negativas, inverte a imagem)
                dest = (Rectangle){j * FATORX, i * FATORY, LADOX, LADOY};
                //dest e o destino da sprite, ou seja, a posicao onde vai ser exibida na tela(como a posicao do jogador);
                DrawTexturePro(blocks, source, dest, origin, 0.0, RAYWHITE);
            }
        }
    }

    
    origin = (Vector2){LADO_QUADRADOX/2-MARGIN_LEFT, LADO_QUADRADOY-MARGIN_TOP}; //origin e pra centralizar melhor a sprite na posicao do jogador (usei para compensar a multiplicacao por 2 que eu fiz no dest pra sprite ficar maior)
    
    //Deenha as traps
    for (i = 0; i < game.n_traps; i++) {
        game.trap[i].spritex += 31;
        if(game.trap[i].spritex >= 31*50) game.trap[i].spritex = 0;

        source = (Rectangle){game.trap[i].spritex, 0, 31, 32}; //Cordenadas da spritesheet: posicao x e y da sprite, largura e altura que vai ser mostrado (Ao usar laguras e alturas negativas, inverte a imagem)
        dest = (Rectangle){game.trap[i].x*FATORX, game.trap[i].y*FATORY+10, LADO_QUADRADOX*2, LADO_QUADRADOY*2};
        //dest e o destino da sprite, ou seja, a posicao onde vai ser exibida na tela(como a posicao do jogador);
        DrawTexturePro(game.trap[0].texture, source, dest, origin, 0.0, RAYWHITE);
    }

    //Desenha as bombas
    for (i = 0; i < game.n_bombas; i++) {
        if (game.bomb[i].active){
            source = (Rectangle){game.bomb[i].spritex, 0, s_sizex, s_sizey}; //Cordenadas da spritesheet: posicao x e y da sprite, largura e altura que vai ser mostrado (Ao usar laguras e alturas negativas, inverte a imagem)
            dest = (Rectangle){game.bomb[i].x*FATORX+10, game.bomb[i].y*FATORY+20, LADO_QUADRADOX+5, LADO_QUADRADOY+5};
            //dest e o destino da sprite, ou seja, a posicao onde vai ser exibida na tela(como a posicao do jogador);

            if(!game.bomb[i].colectable){
                game.bomb[i].texture = LoadTexture("../sprites/bomb_active.png");
                game.bomb[i].spritex += 32;
                if(game.bomb[i].spritex >= 32*(60*3)) game.bomb[i].spritex = 0;
                game.bomb[i].color = (Color){255, 200, 150, 255};
                
            }
            DrawTexturePro(game.bomb[i].texture, source, dest, origin, 0.0, game.bomb[i].color);
        }
    }

    

    //Explosao da bomba
    for (i = 0; i < game.n_explosion; i++) {
        
        game.explosion[i].spritex += 32;
        if(game.explosion[i].spritex >= 32*(60*2)) game.explosion[i].spritex = 0;

        source = (Rectangle){game.explosion[i].spritex, 0, s_sizex, s_sizey}; //Cordenadas da spritesheet: posicao x e y da sprite, largura e altura que vai ser mostrado (Ao usar laguras e alturas negativas, inverte a imagem)
        dest = (Rectangle){game.explosion[i].x*FATORX+10, game.explosion[i].y*FATORY+20, LADO_QUADRADOX, LADO_QUADRADOY};
        //dest e o destino da sprite, ou seja, a posicao onde vai ser exibida na tela(como a posicao do jogador);
        DrawTexturePro(game.explosion[0].texture, source, dest, origin, 0.0, ORANGE);
    }




    //DESENHA A HUB
        DrawText(TextFormat("FASE ATUAL: %02d", game.current_map), 10, (MAP_HEIGHT*FATORY)+30, 20, WHITE);
        DrawText(TextFormat("TIME: %03.2f", (float)game.frameCount/60), 10, (MAP_HEIGHT*FATORY)+50, 20, WHITE);
        DrawText(TextFormat("SAUDE: %03d", game.player.health), 10, (MAP_HEIGHT*FATORY)+70, 20, WHITE);
        DrawText(TextFormat("VIDAS: %02d", game.player.lives), 10,(MAP_HEIGHT*FATORY)+90, 20, WHITE);
        DrawText(TextFormat("BOMBAS: %02d", game.player.n_bombs), 10,(MAP_HEIGHT*FATORY)+110, 20, WHITE);
        DrawText(TextFormat("PONTUACAO: %07d", game.player.pontuacao), 10,(MAP_HEIGHT*FATORY)+130, 20, WHITE);
        DrawText(TextFormat("ESPECIAL COOLDOWN: %.1f", game.player.special_delay), 10,(MAP_HEIGHT*FATORY)+150, 20, WHITE);



    //DESENHA OS TIROS do Jogador
        for(i = 0; i < MAX_TIROS; i++){
            if(game.player.tiro[i].active){
                if(game.player.tiro[i].dx == 1)
                    orientation = s_sizex;
                else
                    orientation = -s_sizex;
                
                source = (Rectangle){0, 0, orientation, s_sizey}; //Cordenadas da spritesheet: posicao x e y da sprite, largura e altura que vai ser mostrado (Ao usar laguras e alturas negativas, inverte a imagem)
                dest = (Rectangle){game.player.tiro[i].x*FATORX, game.player.tiro[i].y*FATORY, LADO_QUADRADOX*3, LADO_QUADRADOY*3};
                //dest e o destino da sprite, ou seja, a posicao onde vai ser exibida na tela(como a posicao do jogador);
                DrawTexturePro(game.player.tiro[i].texture, source, dest, origin, 0.0, game.player.tiro[i].color);
            }

            for(j = 0; j < game.n_pooter; j++){
                if(game.pooter[j].tiro[i].active){
                    if(game.pooter[j].tiro[i].dx == 1)
                        orientation = s_sizex;
                    else
                        orientation = -s_sizex;
                    
                    source = (Rectangle){0, 0, orientation, s_sizey}; //Cordenadas da spritesheet: posicao x e y da sprite, largura e altura que vai ser mostrado (Ao usar laguras e alturas negativas, inverte a imagem)
                    dest = (Rectangle){game.pooter[j].tiro[i].x*FATORX, game.pooter[j].tiro[i].y*FATORY, LADO_QUADRADOX*3, LADO_QUADRADOY*3};
                    //dest e o destino da sprite, ou seja, a posicao onde vai ser exibida na tela(como a posicao do jogador);
                    DrawTexturePro(game.pooter[j].tiro[i].texture, source, dest, origin, 0.0, game.pooter[j].tiro[i].color);
                }
            }
        }


    
    if(game.portal.x > MAP_WIDTH/2)
        orientation = s_sizex;
    else
        orientation = -s_sizex;

    source = (Rectangle){game.portal.spritex, 0, orientation, s_sizey}; //Cordenadas da spritesheet: posicao x e y da sprite, largura e altura que vai ser mostrado (Ao usar laguras e alturas negativas, inverte a imagem)
    dest = (Rectangle){game.portal.x*FATORX, game.portal.y*FATORY+12, LADO_QUADRADOX*2, LADO_QUADRADOY*2};
    //dest e o destino da sprite, ou seja, a posicao onde vai ser exibida na tela(como a posicao do jogador);

    //Desenha a Porta
    if(game.portal.active){
        DrawTexturePro(door, source, dest, origin, 0.0, (Color){100,100,255,255});
    }
    //Desenha o portal
    DrawTexturePro(game.portal.texture, source, dest, origin, 0.0, RAYWHITE);





    //Desenha o player e inimigos

    if(game.player.health <= 0){
        game.player.texture = LoadTexture("../sprites/isaac_dead.png");
        game.player.spritex = 0;
        game.player.spritey = 0;
        if (timer.death >= DEATH_DELAY - 0.1){
            game.player.texture = LoadTexture("../sprites/isaac.png");
        }
    }
    
    if(game.player.dx == 0 && game.player.dy == 0)
        game.player.spritex = 0;
    if(game.player.dx == 1)
        game.player.spritey = s_right;
    else if(game.player.dx == -1)
        game.player.spritey = s_left;
    else if(game.player.dy == 1)
        game.player.spritey = s_back;
    else if(game.player.dy == -1)
        game.player.spritey = s_front;
    game.player.spritex += 32; //Realiza a animacao mudando a posicao da sprite
    if(game.player.spritex > 32*10)
        game.player.spritex = 0;
    
    
    //DrawRectangle(game.player.x*FATORX+MARGIN_LEFT, game.player.y*FATORY+MARGIN_TOP, 20, 20, GREEN);
    source = (Rectangle){game.player.spritex, game.player.spritey, s_sizex, s_sizey}; //Cordenadas da spritesheet: posicao x e y da sprite, largura e altura que vai ser mostrado (Ao usar laguras e alturas negativas, inverte a imagem)
    dest = (Rectangle){game.player.x*FATORX, game.player.y*FATORY, LADO_QUADRADOX*2, LADO_QUADRADOY*2};
    //dest e o destino da sprite, ou seja, a posicao onde vai ser exibida na tela(como a posicao do jogador);
    DrawTexturePro(game.player.texture, source, dest, origin, 0.0, game.player.color);

    //Desenha os inimigos
    for (i = 0; i < game.n_gaper; i++) {
        if(game.gaper[i].active){
            if(game.gaper[i].dx == 0 && game.gaper[i].dy == 0)
                game.gaper[i].spritex = 0;
            else{
                game.gaper[i].spritex += 32;
                if(game.gaper[i].spritex > 32*8){
                    game.gaper[i].spritex = 0;
                }
                game.gaper[i].spritey = 0;
            }
                
            
            source = (Rectangle){game.gaper[i].spritex, game.gaper[i].spritey, s_sizex, s_sizey}; //Cordenadas da spritesheet: posicao x e y da sprite, largura e altura que vai ser mostrado (Ao usar laguras e alturas negativas, inverte a imagem)
            dest = (Rectangle){game.gaper[i].x*FATORX, game.gaper[i].y*FATORY, LADO_QUADRADOX*2, LADO_QUADRADOY*2};
            //dest e o destino da sprite, ou seja, a posicao onde vai ser exibida na tela(como a posicao do jogador);
            DrawTexturePro(game.gaper[0].texture, source, dest, origin, 0.0, RAYWHITE);
        }
    }
    for (i = 0; i < game.n_pooter; i++) {
        if(game.pooter[i].active){
            source = (Rectangle){game.pooter[i].spritex, game.pooter[i].spritey, s_sizex, s_sizey}; //Cordenadas da spritesheet: posicao x e y da sprite, largura e altura que vai ser mostrado (Ao usar laguras e alturas negativas, inverte a imagem)
            dest = (Rectangle){game.pooter[i].x*FATORX, game.pooter[i].y*FATORY, LADO_QUADRADOX*2, LADO_QUADRADOY*2};
            //dest e o destino da sprite, ou seja, a posicao onde vai ser exibida na tela(como a posicao do jogador);
            DrawTexturePro(game.pooter[0].texture, source, dest, origin, 0.0, RAYWHITE);
        }
    }
    if(game.boss.active){
            source = (Rectangle){game.boss.spritex, game.boss.spritey, s_sizex*4, s_sizey*4}; //Cordenadas da spritesheet: posicao x e y da sprite, largura e altura que vai ser mostrado (Ao usar laguras e alturas negativas, inverte a imagem)
            dest = (Rectangle){(game.boss.x)*FATORX, (game.boss.y)*FATORY, LADO_QUADRADOX*2*4, LADO_QUADRADOY*2*4};
            //dest e o destino da sprite, ou seja, a posicao onde vai ser exibida na tela(como a posicao do jogador);
            DrawTexturePro(game.boss.texture, source, dest, origin, 0.0, RAYWHITE);

            sprintf(text, "Chamar aliados: %.1f", game.boss.special_delay);
            DrawText(text, LARGURA/2 - MeasureText(text, FONT_SIZE) + MARGIN_LEFT, 100 + MARGIN_TOP, FONT_SIZE, RED);
        }
    


    EndDrawing();
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------

void moverProjeteis(ENTIDADE *ent){
    int i;
    if(ent->n_tiros == MAX_TIROS) ent->n_tiros = 0;
    if(!ent->isPlayer){
        
        if (((game.player.x) > (ent->x))) {
        ent->tiro[ent->n_tiros].dx = 1;
        ent->tiro[ent->n_tiros].dy = 0;
        }
        else if (((game.player.x) < (ent->x))) {
            ent->tiro[ent->n_tiros].dx = -1;
            ent->tiro[ent->n_tiros].dy = 0;
        }
        else if (((game.player.y) < (ent->y))) {
            ent->tiro[ent->n_tiros].dx = 0;
            ent->tiro[ent->n_tiros].dy = 1;
        }
        else if (((game.player.y) > (ent->y))) {
            ent->tiro[ent->n_tiros].dx = 0;
            ent->tiro[ent->n_tiros].dy = -1;
        }
        
    }
    
    for(i = 0; i < MAX_TIROS; i++){
        if(ent->tiro[i].active){
            ent->tiro[i].speed += GetFrameTime();
            if(ent->tiro[i].speed > SHOT_SPEED){
                if (!(game.map[ent->tiro[i].y][ent->tiro[i].x + ent->tiro[i].dx])) {
                ent->tiro[i].collided = true;
                } else
                    ent->tiro[i].x += VELOCIDADE * ent->tiro[i].dx;

                if (!(game.map[ent->tiro[i].y - ent->tiro[i].dy][ent->tiro[i].x])) {
                    ent->tiro[i].collided = true;
                } else
                    ent->tiro[i].y -= VELOCIDADE * ent->tiro[i].dy;
                    
                ent->tiro[i].speed = 0;
            }
        }
        if(ent->tiro[i].collided){
            ent->tiro[i].active = false;
            ent->tiro[i].collided = false;
        }

    }
    
}

void highscores(){
    FILE *highscores;
    char filename[50] = "../highscores.txt";

    highscores = fopen(filename, "r");
    if (!highscores){
        puts("Erro ao ler os placares");
    }
}

void finishGame(){FILE *highscorebin, *highscoretxt;
    int i = 0, j, tmp;
    int indice_pontuacao;
    int scores[MAX_SCORES] = {0}; //pontuacoes exibidas
    int buffer[MAX_SCORES] = {0};
    char filename[50] = "../highscores.bin";
    char playername[21] = "jogador"; //nome do jogador do jogo atual


    game.player.pontuacao += (game.player.pontuacao / (0.1 + game.frameCount/500)) * game.player.lives;
    printf("Pontuacao calculada: %d\n", game.player.pontuacao);
    
    highscorebin = fopen(filename, "ab+");
    if(!highscorebin){
        puts("Erro ao abrir highcores.bin");
    } else{
        i = 0;
        while(i < MAX_SCORES && !feof(highscorebin)){
            if((fread(&buffer[i], sizeof(int), 1, highscorebin)) != 1){
                puts("Erro ao obter as pontuacoes do arquivo highscores.bin");
            }else{
                scores[i] = buffer[i];
            }
            i++;
        }
        /* puts("Pontuacao contida em highscores.bin:");
        for(i = 0; i < MAX_SCORES; i++){
            printf("%5d", scores[i]);
        }
        printf("\n"); */

        for(i = 0; i < MAX_SCORES; i++){
            for(j = 0; j < MAX_SCORES; j++){
                if(scores[i] > scores[j]){
                    tmp = scores[j];
                    scores[j] = scores[i];
                    scores[i] = tmp;
                }
            }
        }
        /* puts("Pontuacao ordenada:");
        for(i = 0; i < MAX_SCORES; i++){
            printf("%5d", scores[i]);
        }
        printf("\n"); */

        for(i = 0; i < MAX_SCORES; i++){
            if(game.player.pontuacao > scores[i]){
                printf("Voce bateu um recorde!\nDigite seu nome: ");
                scanf(" %s", playername);
                tmp = scores[i];
                scores[i] = game.player.pontuacao;
                indice_pontuacao = i;
                scores[MAX_SCORES-1] = tmp;
                i = 10;
            }
        }
        for(i = 0; i < MAX_SCORES; i++){
            for(j = 0; j < MAX_SCORES; j++){
                if(scores[i] > scores[j]){
                    tmp = scores[j];
                    scores[j] = scores[i];
                    scores[i] = tmp;
                }
            }
        }
        puts("Pontuacao Inserida:");
        for(i = 0; i < MAX_SCORES; i++){
            printf("%5d", scores[i]);
        }
        printf("\n");
        fclose(highscorebin);
    }

    highscorebin = fopen(filename, "wb");
    if(!highscorebin){
        puts("Erro ao abrir arquivo highscores.bin para insercao de scores");
    } else{
        if((fwrite(scores, sizeof(int), MAX_SCORES, highscorebin)) != MAX_SCORES){
            puts("Erro ao gravar a pontuacao do jogador");
        }

        fclose(highscorebin);
    }

    strcpy(filename, "../highscores.txt");
    highscoretxt = fopen(filename, "a");
    if(!highscoretxt){
        puts("Erro ao abrir highscores.txt");
    }else{
        
        fprintf(highscoretxt, "%s - %d\n", playername, scores[indice_pontuacao]);

        fclose(highscoretxt);
    }
}


void novo_jogo() //Funcao que carrega um novo jogo (inclusive quando e feito o load do save e quando avanca de fase)
{
    if(game.state == 'n'){
        game.current_map = 1;
        game.player.health = 100;
        game.player.lives = 3;
        game.player.n_bombs = 0;
        game.frameCount = 0;
        game.player.pontuacao = 0;
    }
    game.state = '\0';
    int enemy_steps = 0;
    int i, j;
    int x = 0, y = 0;

    puts("\nStarting new game...\n");

    if (!game.isLoaded) { //Reseta o numero de itens e inimigos caso o jogo NAO tenha sido carregado a partir de um save
        game.n_bombas = 0;
        game.n_gaper = 0;
        game.n_pooter = 0;  
        game.n_explosion = 0;
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
        game.player.active = true;
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
        if(!(game.current_map % 10)){ //spawna o boss em mapas multiplos de 10
            game.boss.health = 10000;
            game.boss.active = true;
            game.boss.isBoss = true;
            game.boss.delay = 0;          
            game.boss.special_delay = BOSS_DELAY;

        }

        for (i = 0; i < game.n_gaper; i++) {
            redefineDeslocamento(&game.gaper[i], &enemy_steps);
        }
        puts("enemies directions setted");

        for (i = 0; i < game.n_bombas; i++) {
            game.bomb[i].active = true;
            game.bomb[i].colectable = true;
            game.bomb[i].alive_time = 0;
            game.bomb[i].texture = LoadTexture("../sprites/bomb_idle.png");
            game.bomb[i].color = RAYWHITE;
        }
        for (i = 0; i < game.n_gaper; i++) {
            game.gaper[i].health = 50;
            game.gaper[i].active = true;
        }
        for (i = 0; i < game.n_bombas; i++) {
            game.pooter[i].health = 50;
            game.pooter[i].active = true;
        }
        for(i = 0; i < MAX_TIROS; i++){
            game.player.tiro[i].active = false;
            game.player.tiro[i].texture = LoadTexture("../sprites/tears.png");
            game.player.tiro[i].color = RAYWHITE;
            for(j = 0; j < game.n_pooter; j++){
                game.pooter[j].tiro[i].active = false;
                game.pooter[j].tiro[i].texture = LoadTexture("../sprites/tears.png");
                game.pooter[j].tiro[i].color = RED;
                game.pooter[j].tiro[i].delay = 0;
                
            }
        }

        game.n_explosion = 0;
        game.portal.active = false;
        timer.death = 0;
        timer.invecible = 0; 
        game.player.delay = 0;
        game.player.n_tiros = 0;
        game.player.texture = LoadTexture("../sprites/isaac.png");
        game.player.isPlayer = true;
        game.player.active = false;
        game.player.special_delay = 0;
    }
    game.isLoaded = false;
    if(game.modo_jogo)
            game.boss.active = false;

    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    while (!WindowShouldClose() && game.state != 'q' && game.state != 'n' && game.state != 'p' && game.state != 'g') {
        //'q' = sair, 'n' = novo jogo, 'p' = passou de fase
        
        if(IsKeyPressed(KEY_Y)){ //Especial do personagem
            if(game.player.active)
                game.player.active = false;
            else
                game.player.active = true;
        }
        if(!game.player.active){
            game.player.color = (Color){255, 255, 255, 100};
        }
        // Verifica se o usuario apertou ESC
        if (IsKeyPressed(KEY_ESCAPE)) {
            puts("Game paused");
            game.state = 'e';
            menu(1);
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

        if (game.player.health <= 0) { //Muda as animacoes e ativa o modo invencivel quando o player morre
            game.player.dx = 0;
            game.player.dy = 0;
            game.player.active = false;
            timer.death += GetFrameTime();
            game.player.color = (Color){255, 255, 255, 100};
            if(timer.death > DEATH_DELAY){
                game.player.lives--;
                game.player.health = 100;
                timer.death = 0;
                game.player.delay = 0;
            }
        }
        if(!game.player.active && !timer.death){
            timer.invecible += GetFrameTime();
            if(timer.invecible > 3){
                game.player.active = true;
                timer.invecible = 0;
                game.player.color = RAYWHITE;
            }
        }
        if(game.player.active){
            checkCollision(&game.player);
       
            if(IsKeyPressed(KEY_RIGHT)){
                game.player.tiro[game.player.n_tiros].x = game.player.x;
                game.player.tiro[game.player.n_tiros].y = game.player.y;
                game.player.tiro[game.player.n_tiros].dx = 1;
                game.player.tiro[game.player.n_tiros].dy = 0;
                game.player.tiro[game.player.n_tiros].active = true;
                game.player.n_tiros++;
            }
            if(IsKeyPressed(KEY_LEFT)){
                game.player.tiro[game.player.n_tiros].x = game.player.x;
                game.player.tiro[game.player.n_tiros].y = game.player.y;
                game.player.tiro[game.player.n_tiros].dx = -1;
                game.player.tiro[game.player.n_tiros].dy = 0;
                game.player.tiro[game.player.n_tiros].active = true;
                game.player.n_tiros++;
            }
            if(IsKeyPressed(KEY_UP)){
                game.player.tiro[game.player.n_tiros].x = game.player.x;
                game.player.tiro[game.player.n_tiros].y = game.player.y;
                game.player.tiro[game.player.n_tiros].dx = 0;
                game.player.tiro[game.player.n_tiros].dy = 1;
                game.player.tiro[game.player.n_tiros].active = true;
                game.player.n_tiros++;
            }
            if(IsKeyPressed(KEY_DOWN)){
                game.player.tiro[game.player.n_tiros].x = game.player.x;
                game.player.tiro[game.player.n_tiros].y = game.player.y;
                game.player.tiro[game.player.n_tiros].dx = 0;
                game.player.tiro[game.player.n_tiros].dy = -1;
                game.player.tiro[game.player.n_tiros].active = true;
                game.player.n_tiros++;
            }
        }
        moverProjeteis(&game.player);
     

        // Movimentacao do jogador
        if (!game.player.collided)
            movimentar(&game.player);
        game.player.collided = false;

        // Movimentacao dos inimigos

        for (i = 0; i < game.n_gaper; i++) {
            if(game.gaper[i].active){
                checkCollision(&game.gaper[i]);
                
                if (rayCast(game.gaper[i], game.player)) {
                    // Chama a funcao que verifica se tem paredes entre o gaper e o jogador
                    game.gaper[i].canChase = false;
                }
                if (game.gaper[i].canChase && game.player.active) {
                    persegue(game.player, &game.gaper[i]);
                } else {
                    if (enemy_steps == 0)
                        redefineDeslocamento(&game.gaper[i], &enemy_steps);
                    if (enemy_steps > 0) {
                        movimentar(&game.gaper[i]);
                        enemy_steps--;
                    }
                    if (game.gaper[i].collided) {
                        redefineDeslocamento(&game.gaper[i], &enemy_steps);
                    }
                }
                game.gaper[i].collided = false;
                game.gaper[i].canChase = true;
            
            }
        }

        for (i = 0; i < game.n_pooter; i++) {
            if(game.pooter[i].active){
                checkCollision(&game.pooter[i]);

                if (enemy_steps == 0)
                    redefineDeslocamento(&game.pooter[i], &enemy_steps);
                if (game.pooter[i].collided) {
                    redefineDeslocamento(&game.pooter[i], &enemy_steps);
                }
                if (rayCast(game.pooter[i], game.player)) {
                    // Chama a funcao que verifica se tem paredes entre o game.gaper e o jogador
                    game.pooter[i].canChase = false;
                }
                    if (game.pooter[i].canChase && game.player.active) {
                        //for(j = 0; j < MAX_TIROS; j++){
                        game.pooter[j].tiro[i].delay -= GetFrameTime();

                        if(game.pooter[j].tiro[i].delay  <= 0){
                                game.pooter[i].tiro[game.pooter[i].n_tiros].x = game.pooter[i].x;
                                game.pooter[i].tiro[game.pooter[i].n_tiros].y = game.pooter[i].y;
                                game.pooter[i].tiro[game.pooter[i].n_tiros].active = true;
                                game.pooter[j].tiro[i].delay = 2;
                                game.pooter[i].n_tiros++;
                        }
                            
                            
                        //}
                    }
                    if (enemy_steps > 0) {
                            movimentar(&game.pooter[i]);
                            enemy_steps--;
                        }
                    
                game.pooter[i].collided = false;
                game.pooter[i].canChase = true;
            }
            moverProjeteis(&game.pooter[i]);

        }

        //PORTAL ATIVO OU NAO
        int count_enemies = 0;

        for(i = 0; i < game.n_gaper; i++){
            if(game.gaper[i].active)
                count_enemies++;
        }
        for(i = 0; i < game.n_pooter; i++){
            if(game.pooter[i].active)
                count_enemies++;
        }
        
        if(!(game.current_map % 10)){
            if(game.boss.active){
                count_enemies++;
                game.boss.special_delay -= GetFrameTime();
                //printf("boss delay: %.1f\n", game.boss.special_delay);
                if(game.boss.special_delay <= 0){
                    game.boss.active = false;
                    game.n_gaper = 3;
                    game.n_pooter = 3;
                    
                    spawnEnemies(game.gaper, game.n_gaper);
                    spawnEnemies(game.pooter, game.n_pooter);
                    
                }
                
                if (rayCast(game.boss, game.player)) {
                    // Chama a funcao que verifica se tem paredes entre o gaper e o jogador
                    game.boss.canChase = false;
                }
                if (game.boss.canChase && game.player.active) {
                    persegue(game.player, &game.boss);
                } else {
                    if (enemy_steps <= 0)
                        redefineDeslocamento(&game.boss, &enemy_steps);
                    else{
                        movimentarBoss(&game.boss);
                        enemy_steps--;
                    }
                    if (game.boss.collided) {
                        redefineDeslocamento(&game.boss, &enemy_steps);
                    }
                }
                checkCollision(&game.boss);
                game.boss.canChase = true;
                
            }
            if(!count_enemies && game.boss.health > 0){
                game.n_gaper = 0;
                game.n_pooter = 0;
                game.boss.active = true;
                game.boss.special_delay = BOSS_DELAY;
            }

        }
        
        
    
        if(!count_enemies)
            game.portal.active = true;
        else game.portal.active = false;

        

        if((IsKeyPressed(KEY_E) && !timer.death)){
            if(game.player.n_bombs > 0){
                game.n_bombas++;
                game.bomb[game.n_bombas-1].x = game.player.x;
                game.bomb[game.n_bombas-1].y = game.player.y;
                game.bomb[game.n_bombas-1].active = true;
                game.bomb[game.n_bombas-1].colectable = false;
                game.player.n_bombs--;
            }

        }

        checkBombs();

        
       

	    //ATUALIZA O TEMPO
        game.frameCount++;
        
        /* printf("\nplayer health: %d\n", game.player.health);
        printf("player lives: %d\n", game.player.lives);*/
        //printf("player bombs: %d\n", game.player.n_bombs);
        
        if(game.player.special_delay > 0){
            game.player.special_delay -= GetFrameTime();
        }
        if (IsKeyPressed(KEY_SPACE)) {
            if(game.player.special_delay <= 0){
                game.player.active = false;
                game.player.special_delay = SPECIAL_DELAY;
            }
        }

        // Portal
        if ((game.player.x == game.portal.x && game.player.y == game.portal.y && game.portal.active) && game.player.active){
            game.player.pontuacao += 300;
            game.state = 'p';
        }
        // Game Over
        if (!game.player.lives) {
            game.state = 'g';
            
        }

        
        //---------------------------------------------------------------------------
        //---------------------------------------------------------------------------
        //                          DESENHAR JOGO:
        DrawGame();

        //Reseta as direcoes do jogador pra ele nao ficar deslizando
        game.player.dx = 0;
        game.player.dy = 0;
        if(IsKeyPressed(KEY_MINUS)){
            game.player.lives --;
        }
    }

    //------------------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------------------
    if (game.state == 'p') {
        puts("Changing current map...\n");
        if (game.current_map < MAX_MAPS)
            game.current_map++;
        else{            
            //finishGame();
        }
        novo_jogo();
    }


    if(game.state == 'g'){
        char text[30];
        finishGame();
        strcpy(text, "Game Over!");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE + 10) / 2, ALTURA / 2 - FONT_SIZE + 10, FONT_SIZE, RED);
        return;
    }

}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
int carregar_jogo(char filename[]) //Faz o load do save
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

    while (game.state == 'c') {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        strcpy(text, "Jogo Carregado com sucesso!");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, ALTURA / 2 - FONT_SIZE, FONT_SIZE, BLACK);

        strcpy(text, "Aperte ESC para continuar");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE - 10) / 2, ALTURA / 2 - FONT_SIZE - 10 + 70, FONT_SIZE - 10, BLACK);

        if (IsKeyPressed(KEY_ESCAPE))
            game.state = '\0';
        EndDrawing();
    }
    return carregou;
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
int salvar_jogo(char filename[]) //Salva a struct game em um arquivo binario
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
    while (game.state == 's') {
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
            game.state = '\0';
        EndDrawing();
    }
    // menu( 1);
    return salvou;
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void sair_jogo() //Encerra o jogo apos perguntar se o usuario gostaria de salvar o jogo
{
    char text[50];
    char save_path[50] = "../saves/save.bin";
    while (game.state != 'q') {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        strcpy(text, "Deseja salvar o jogo? (S/N)");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, ALTURA / 2 - FONT_SIZE, FONT_SIZE, BLACK);

        if (IsKeyPressed(KEY_S)) {
            salvar_jogo( save_path);
            game.state = 'q';
        } else if (IsKeyPressed(KEY_N)) {
            game.state = 'q';
        }
        EndDrawing();
    }
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void voltar_jogo() //Simplesmente retorna ao jogo
{
    game.state = '\0';
}
//------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void menu(int were_played) //Menu principal do jogo
{
    // Menu do jogo
    char text[100] = "";
    char save_path[50] = "../saves/save.bin";

    while (game.state == '\0' || game.state == 'e' || game.state == 'g') {
        if (game.state == 'g') {
            WaitTime(2);
            game.state = '\0';
        }

        if (IsKeyPressed(KEY_N)) {
            game.state = 'a';
        }
        if (IsKeyPressed(KEY_C)) {
            game.state = 'c';
            if (carregar_jogo(save_path)) {
                game.isLoaded = true;
                novo_jogo();
            }

            else {

                strcpy(text, "Nao foi possivel carregar o jogo!:");
                DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, 100, FONT_SIZE, RED);
            }
        }
        if (IsKeyPressed(KEY_S) && were_played) {
            game.state = 's';
            if (salvar_jogo( save_path))
                puts("Game saved sucessfully");
        }
        if (IsKeyPressed(KEY_Q)) {
            puts("Exiting game...");
            game.state = '0'; // atribui a '0' para poder sair do loop do menu.
            sair_jogo();
        }
        if (IsKeyPressed(KEY_V) && were_played) {
            puts("Game resumed");
            game.state = 'v';
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
    while (game.state == 'a') {

        strcpy(text, "Selecione o Modo de jogo:");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE) / 2, ALTURA / 4 - FONT_SIZE, FONT_SIZE, BLACK);
        strcpy(text, "1 - Padrao");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE - 10) / 2, ALTURA / 4 - FONT_SIZE + 100, FONT_SIZE - 10, BLACK);
        strcpy(text, "2 - Survival Mode (infinito)");
        DrawText(text, LARGURA / 2 - MeasureText(text, FONT_SIZE - 10) / 2, ALTURA / 4 - FONT_SIZE + 150, FONT_SIZE - 10, BLACK);

        if (IsKeyPressed(KEY_ONE) || IsKeyPressed(KEY_KP_1)) {
            game.modo_jogo = 0;
            game.current_map = 1;
            game.state = 'n';
            novo_jogo();
            were_played = 1;
        }
        if (IsKeyPressed(KEY_TWO) || IsKeyPressed(KEY_KP_2)) {
            game.modo_jogo = 1;
            game.current_map = 1;
            game.state = 'n';
            novo_jogo();
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
    game.state = '\0'; // Estados do jogo
    // char save_path[50] = "../saves/save.bin";
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(LARGURA, ALTURA, "The Binding of Isaac");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL); // remove a opcao de sair do jogo

    background = LoadTexture("../sprites/basement1.png");
    blocks = LoadTexture("../sprites/block1.png");
    door = LoadTexture("../sprites/door1.png");

    game.player.texture = LoadTexture("../sprites/isaac.png");
    game.player.color = RAYWHITE;
    game.boss.texture = LoadTexture("../sprites/monstro.png");
    game.gaper[0].texture = LoadTexture("../sprites/gaper_front.png");
    game.pooter[0].texture = LoadTexture("../sprites/pooter_front.png");
    game.trap[0].texture = LoadTexture("../sprites/trap.png");
    game.explosion[0].texture = LoadTexture("../sprites/explosion.png");
    
    game.portal.texture = LoadTexture("../sprites/portal1.png");
    
    

    while (game.state == '\0' || game.state == 'e' || game.state == 'g' || game.state == 'p')
        menu(0);

    CloseWindow(); // Fecha a janela e o contexto OpenGL
    return 0;
}