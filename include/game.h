#define LARGURA 1200
#define ALTURA 600


//////////////////////////////////////////////////////
//              Geracao do mapa:

#define MAP_WIDTH 60  //Largura do mapa
#define MAP_HEIGHT 30  //Altura do mapa
#define MAX_TUNNELS 100 //Quantidate de tuneis que vai gerar
#define PADDING 4 //Define o espacamento entra os tuneis(0 e o espacamento padrao 1x1):


/////////////////////////////////////////////////////////
//              Fatores de Conversao:

#define FATORX  (LARGURA / MAP_WIDTH)
#define FATORY  (ALTURA / MAP_HEIGHT)
#define LADOX FATORX //Espessura das paredes desenhadas pela raylib
#define LADOY FATORY


///////////////////////////////////////////////////////////
//              Personagens:

#define LADO_QUADRADO 30
#define VELOCIDADE 1

extern void menu();
