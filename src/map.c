
int shouldMove(int i, int j, int dx, int dy, int width, int height) {
    int move = 1;
    if( (i == (height-2) && dy == 1) || (i == 1 && dy == -1))
        move = 0;
    if( (j == (width-2) && dx == 1) || (j == 1 && dx == -1))
        move = 0;
    return move;
}

int generateMap(){

    int m[HEIGHT][WIDTH] = {0}; 
    int i, j, k, l; 
    int s = 0, dx = 0, dy = 0; 
    int s_ant = s; 
    int tunnels = 0, steps = 0, count_padding = PADDING; 
    srand(time(NULL));
    i = 1; //1 + (rand() % HEIGHT-3);
    j = WIDTH - 2; //1 + (rand() % WIDTH-3);
    m[i][j] = 1;
    while(tunnels < MAX_TUNNELS) {
        while(s == 0 || s == s_ant) {
            s = (rand() % 3) - 1;
        }
        if(s == 1) {
            while(dx == 0)
                dx = (rand() % 3) - 1;
        } else{
            while(dy == 0)
                dy = (rand() % 3) - 1;
        }
            while(steps == 0)
                steps = (rand() % (WIDTH-2)) ;

        while(steps > 0 && shouldMove(i,j,dx,dy,WIDTH,HEIGHT)) {
            i += dy; 
            j += dx; 
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
        s_ant = s;        
        steps = 0;
        s = 0;
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
