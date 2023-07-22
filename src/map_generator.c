#include<stdio.h>
#include <stdlib.h>
#include <time.h>
#define WIDTH 60
#define HEIGHT 30
#define MAX_TUNNELS 100

/*How large are the tunnels (0 for tunnels 1x1)
Realize that if the PADDING is too high (close to HEIGHT or WIDTH values) it will generate a hollow square*/
#define PADDING 2

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
    int m[HEIGHT][WIDTH] = {0};
    int i, j, k, l;
    int d=0, dx=0, dy=0;
    int dant = 0;
    int tunnels = 0;
    int steps = 0;
    int count_padding = PADDING;


    srand(time(NULL));

    i = 1;//1 + (rand() % HEIGHT-2);
    j = WIDTH - 2;//1 + (rand() % WIDTH-2);

    m[i][j] = 1;


    while(tunnels < MAX_TUNNELS) {

        while(d == 0 || d == dant) {
            d = (rand() % 3) - 1;
        }

        if(d == 1) {
            while(dx == 0)
                dx = (rand() % 3) - 1;

            while(steps == 0)
                steps = (rand() % (WIDTH-2)) ;
        }

        else if(d == -1) {

            while(dy == 0)
                dy = (rand() % 3) - 1;

            while(steps == 0)
                steps = (rand() % (HEIGHT-2));
        }




        while(steps > 0 && shouldMove(i,j,dx,dy,WIDTH,HEIGHT)) {
            i+=dy;
            j+=dx;
            k = i;
            l = j;
            m[i][j] = 1;

            while(count_padding > 0) {
            
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
                count_padding--;
            }
            count_padding = PADDING;
            steps--;
        }

        dant = d;
        steps = 0;
        d = 0;
        dx = 0;
        dy = 0;
        tunnels++;
    }

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