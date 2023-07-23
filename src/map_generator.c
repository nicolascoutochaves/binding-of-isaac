/* Gerador de mapas 2 X 2 que se baseia no algoritmo da caminhada do bebado (Drunkard's Walk Algorithms).
Esse algoritmo pega um ponto aleatorio ou nao do mapa e comeca a gerar caminhos com direcoes, sentidos e quantidades de passos aleatorios, Sem exluir as paredes que compoem as 4 bordas do mapa ou criar caminhos desconectados.
*/
#include<stdio.h>
#include <stdlib.h>
#include <time.h>
#define WIDTH 60  //Largura do mapa
#define HEIGHT 30  //Altura do mapa
#define MAX_TUNNELS 50 //Quantidate de tuneis que vai gerar

//Define o espacamento entra os tuneis(0 e o espacamento padrao 1x1):
#define PADDING 3

//Funcao booleana que verifica se o tunel deve se mover em determinada direcao para evitar que as paredes sejam removidas ou ocorra problemas de acesso de posicoes inexistentes na matriz:
int shouldMove(int i, int j, int dx, int dy, int width, int height) {
    int move = 1;
    if( (i == (height-2) && dy == 1) || (i == 1 && dy == -1))
        move = 0;
    if( (j == (width-2) && dx == 1) || (j == 1 && dx == -1))
        move = 0;
    return move;
}

int main()
{
    int m[HEIGHT][WIDTH] = {0}; //Inicializa o mapa 100% preenchido
    int i, j, k, l; //Variaveis de controle da matriz
    int s = 0, dx = 0, dy = 0; //Variaveis que definem a direcao e os sentidos
    int s_ant = s; //Sentido anterior(s indica se esta movendo no eixo x ou no eixo y)
    int tunnels = 0, steps = 0, count_padding = PADDING; // Variaveis de controle dos loops

    srand(time(NULL)); //Gera a seed da funcao rand() usando o tempo do sistema

/*  Define a posicao i e j da matriz para que o tunel sempre comece a partir da posicao m[1][WIDTH - 2], que e a posicao do portal.
    Dessa forma garantimos que sempre havera um caminho livre ate o portal.
    Para iniciar em uma posicao aleatoria, utilize o trecho comentado abaixo:*/

    i = 1; //1 + (rand() % HEIGHT-3);
    j = WIDTH - 2; //1 + (rand() % WIDTH-3);
    m[i][j] = 1;

    //laco principal da geracao dos tuneis
    while(tunnels < MAX_TUNNELS) {

        //Aqui a direcao (x ou y) nunca sera 0 ou igual a anterior, assim o tunel sempre escolhe uma nova direcao, enriquecendo o mapa.
        while(s == 0 || s == s_ant) {
            s = (rand() % 3) - 1;
        }
        //quando s == 1, assume a direcao x.
        if(s == 1) {
            
            //Define o sentido do movimento no eixo x: este laco faz com que sempre seja adotado um sentido
            while(dx == 0)
                dx = (rand() % 3) - 1;
        } else{
            //Define o sentido do movimento no eixo y: sempre adota um sentido como no laco anterior.
            while(dy == 0)
                dy = (rand() % 3) - 1;
        }
        //Define a quantidade de passos que deve dar no sentido e direcao atuais:
            while(steps == 0)
                steps = (rand() % (WIDTH-2)) ;

        //laco que insere os tuneis na matriz enquanto a quantidade de passos sorteada e maior que 0
        while(steps > 0 && shouldMove(i,j,dx,dy,WIDTH,HEIGHT)) {

            i += dy; //acrescenta o sentido de y em i
            j += dx; //acrescenta o sentido de x em j
                //Note que cada iteracao do laco principal o sentido sera ou y ou x, nunca os dois ao mesmo tempo, evitando caminhos diagonais

            k = i; //k e uma variavel de controle do espacamento dos tuneis no sentido vertical
            l = j; //l e uma variavel de controle do espacamento dos tuneis no sentido horizontal

            m[i][j] = 1; //define a posicao do tunel

            //laco que usa os valores de k e l para acrescentar um espacamento no tunel (PADDING)
            while(count_padding > 0) {
                //Condicionais que verificam onde inserior os espacos extras, para evitar que seja inserido um espaco em locais proibidos:

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

                //aqui a variavel count_padding vai decrementando para que na proxima iteracao o padding seja adicionado numa posicao a mais ou a menos, dependendo da condicional
                count_padding--;

                /* Exemplo: se PADDING e igual a 2, pegamos alguns valores quaisquer de k ou l:

                      | 0 | 1 | 2 | 3 | 4 |...|
                    --|---|---|---|---|---|
                    0 | # | k | # |k+2| # |
                    --|---|---|---|---|---|
                    1 | # | # | # | # | # | 
                    --|---|---|---|---|---|
                    . |

                   -> '#' representa uma parede

                   -> k e incrementado no valor de padding pela variavel count_padding, ou seja, k+=2
                   -> em seguida, na nova posicao k, a matriz recebe um valor correspondente ao espaco em branco

                      | 0 | 1 | 2 | 3 | 4 |...|
                    --|---|---|---|---|---|
                    0 | # |k-2| # |k  | # |
                    --|---|---|---|---|---|
                    1 | # | # | # | # | # | 
                    --|---|---|---|---|---|
                    . |
                   -> k e decrementado pelo mesmo valor de count_padding e volta a posicao original
                   -> count_padding e decrementado e passa a ser 1: o loop repete enquanto count_padding for > 0
                   -> Agora k,que havia voltado ao valor original e incrementado pelo count_padding:
                      | 0 | 1 | 2 | 3 | 4 |...|
                    --|---|---|---|---|---|
                    0 | # |k  |k+1|   | # |
                    --|---|---|---|---|---|
                    1 | # | # | # | # | # | 
                    --|---|---|---|---|---|
                    . |
                    -> A martiz recebe o valor correspondente ao espaco em branco e k e decrementado para o valor original
                    -> count_padding e decrementado para 0 e assim encerra o laco com o seguinte resultado:
                      | 0 | 1 | 2 | 3 | 4 |...|
                    --|---|---|---|---|---|
                    0 | # |   |   |   | # |
                    --|---|---|---|---|---|
                    1 | # | # | # | # | # | 
                    --|---|---|---|---|---|
                    . |
                   -> Note que se PADDING for definido como 0, esse loop sera ignorado, gerando mapas com tuneis de tamanho padrao.
                 */
            }
            count_padding = PADDING; //Reatribui a variavel count_padding ao valor do padding.
            steps--; //decrementa a quantidade de passos para realizar uma nova operacao
        }
        s_ant = s; //define o sentido anterior
        
        //Reseta as variaveis de controle para que um outro loop possa ser realizado:
        steps = 0;
        s = 0;
        dx = 0;
        dy = 0;

        tunnels++;
    }

    //Imprime a matriz, substituindo os valores '1' para espacos em branco e '0' para '#'
    for (i=0; i<HEIGHT; i++) {
        for (j=0; j<WIDTH; j++) {
            if(m[i][j] == 1)
                printf(" ");
            else
                printf("#");
        }
        printf("\n");
    }




    return 0;
}
