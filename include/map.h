/* Gerador de mapas 2 X 2 que se baseia no algoritmo da caminhada do bebado (Drunkard's Walk Algorithms).
Esse algoritmo pega um ponto aleatorio ou nao do mapa e comeca a gerar caminhos com direcoes, sentidos e quantidades de passos aleatorios, Sem exluir as paredes que compoem as 4 bordas do mapa ou criar caminhos desconectados.
*/
#define WIDTH 60  //Largura do mapa
#define HEIGHT 30  //Altura do mapa
#define MAX_TUNNELS 50 //Quantidate de tuneis que vai gerar

//Define o espacamento entra os tuneis(0 e o espacamento padrao 1x1):
#define PADDING 3

//Funcao booleana que verifica se o tunel deve se mover em determinada direcao para evitar que as paredes sejam removidas ou ocorra problemas de acesso de posicoes inexistentes na matriz:
int shouldMove(int i, int j, int dx, int dy, int width, int height);

//Funcao que gera o mapa
int generateMap();
