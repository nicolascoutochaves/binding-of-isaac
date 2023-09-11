/* Gerador de mapas 2 X 2 que se baseia no algoritmo da caminhada do bebado (Drunkard's Walk Algorithms).
Esse algoritmo pega um ponto aleatorio ou nao do mapa e comeca a gerar caminhos com direcoes, sentidos e quantidades de passos aleatorios, Sem exluir as paredes que compoem as 4 bordas do mapa ou criar caminhos desconectados.
*/

#include "map.h"



int canGenerate(int i, int j, int dx, int dy, int width, int height) {
    int generate = 1;
    if( (i == (height-3) && dy == 1) || (i == 2 && dy == -1))
        generate = 0;
    if( (j == (width-3) && dx == 1) || (j == 2 && dx == -1))
        generate = 0;
    return generate;
}

long long current_timestamp() { 
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

int generateMap(int m[MAP_HEIGHT][MAP_WIDTH])
{
    int i, j, k, l; //Variaveis de controle da matriz
    int s = 0, dx = 0, dy = 0; //Variaveis que definem a direcao e os sentidos
    int s_ant = s; //Sentido anterior(s indica se esta movendo no eixo x ou no eixo y)
    int tunnels = 0, steps = 0, count_padding = PADDING; // Variaveis de controle dos loops
    long long seed = current_timestamp();
    printf("Generating map...\n");
    srand(seed); //Gera a seed da funcao rand() usando o tempo do sistema
    printf("Map seed: %lld\n", seed);

    for (i = 0; i < MAP_HEIGHT; i++) {
        for (j = 0; j < MAP_WIDTH; j++) {
            m[i][j] = 0;
        }
    }

/*  Define a posicao i e j da matriz para que o tunel sempre comece a partir da posicao m[1][MAP_WIDTH - 2], que e a posicao do portal.
    Dessa forma garantimos que sempre havera um caminho livre ate o portal.
    Para iniciar em uma posicao aleatoria, utilize o trecho comentado abaixo:*/

    i = 2; //1 + (rand() % MAP_HEIGHT-3);
    j = MAP_WIDTH - 3; //1 + (rand() % MAP_WIDTH-3);
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
                steps = (rand() % (MAP_WIDTH-3)) ;

        //laco que insere os tuneis na matriz enquanto a quantidade de passos sorteada e maior que 0
        while(steps > 0 && canGenerate(i,j,dx,dy,MAP_WIDTH,MAP_HEIGHT)) {

            i += dy; //acrescenta o sentido de y em i
            j += dx; //acrescenta o sentido de x em j
                //Note que cada iteracao do laco principal o sentido sera ou y ou x, nunca os dois ao mesmo tempo, evitando caminhos diagonais

            k = i; //k e uma variavel de controle do espacamento dos tuneis no sentido vertical
            l = j; //l e uma variavel de controle do espacamento dos tuneis no sentido horizontal

            m[i][j] = 1; //define a posicao do tunel

            //laco que usa os valores de k e l para acrescentar um espacamento no tunel (PADDING)
            while(count_padding > 0) {
                //Condicionais que verificam onde inserior os espacos extras, para evitar que seja inserido um espaco em locais proibidos:
                if (s == 1){
                    if(k + count_padding < (MAP_HEIGHT - 3)) {
                        k += count_padding;
                        m[k][j] = 1;
                        k -= count_padding;
                    }

                    else if (k - count_padding > 1) {
                        k -= count_padding;
                        m[k][j] = 1;
                        k += count_padding;
                    }
                }
                else{
                    if(l + count_padding < (MAP_WIDTH - 3)) {
                        l += count_padding;
                        m[i][l] = 1;
                        l -= count_padding;
                    }

                    else if (l - count_padding > 1) {
                        l -= count_padding;
                        m[i][l] = 1;
                        l += count_padding;
                    }
                }

                //aqui a variavel count_padding vai decrementando para que na proxima iteracao o padding seja adicionado numa posicao a mais ou a menos, dependendo da condicional
                count_padding--;

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
    
    return 1;
}

void printMap(int m[MAP_HEIGHT][MAP_WIDTH]){
    int i, j;
    //Imprime a matriz, substituindo os valores '1' para espacos em branco e '0' para '#'
    
    for (i=0; i<MAP_HEIGHT; i++) {
        for (j=0; j<MAP_WIDTH; j++) {
            if(m[i][j] == 1)
                printf(" ");
            else
                printf("#");
        }
        printf("\n");
    }
    printf("\n");


}
