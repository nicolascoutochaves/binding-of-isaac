#include <stdio.h>
#include <string.h>
#define MAX 10


int main(){

    FILE *highscorebin;//, *highscoretxt;
    int i = 0, j, tmp;
    int scores[MAX] = {0}; //pontuacoes exibidas
    int buffer[MAX] = {0};
    int pontuacao;
    char filename[50] = "highscores.bin";
    //char currentscore[10]; //pontuacao do jogo atual
    //char playername[50] = "jogador"; //nome do jogador do jogo atual
    

    highscorebin = fopen(filename, "ab+");
    if(!highscorebin){
        puts("Erro ao abrir highcores.bin");
    } else{
        i = 0;
        while(i < MAX && !feof(highscorebin)){
            if((fread(&buffer[i], sizeof(int), 1, highscorebin)) != 1){
                puts("Erro ao obter as pontuacoes do arquivo highscores.bin");
            }else{
                scores[i] = buffer[i];
            }
            i++;
        }
        puts("Pontuacao contida em highscores.bin:");
        for(i = 0; i < MAX; i++){
            printf("%5d", scores[i]);
        }
        printf("\n");

        for(i = 0; i < MAX; i++){
            for(j = 0; j < MAX; j++){
                if(scores[i] > scores[j]){
                    tmp = scores[j];
                    scores[j] = scores[i];
                    scores[i] = tmp;
                }
            }
        }
        puts("Pontuacao ordenada:");
        for(i = 0; i < MAX; i++){
            printf("%5d", scores[i]);
        }
        printf("\n");

        puts("Insira  pontuacao:");
        scanf("%d", &pontuacao);

        for(i = 0; i < MAX; i++){
            if(pontuacao > scores[i]){
                tmp = scores[i];
                scores[i] = pontuacao;
                scores[MAX-1] = tmp;
                i = 10;
            }
        }
        for(i = 0; i < MAX; i++){
            for(j = 0; j < MAX; j++){
                if(scores[i] > scores[j]){
                    tmp = scores[j];
                    scores[j] = scores[i];
                    scores[i] = tmp;
                }
            }
        }
        puts("Pontuacao Inserida:");
        for(i = 0; i < MAX; i++){
            printf("%5d", scores[i]);
        }
        printf("\n");
        fclose(highscorebin);
    }

    highscorebin = fopen(filename, "wb");
    if(!highscorebin){
        puts("Erro ao abrir arquivo highscores.bin para insercao de scores");
    } else{
        if((fwrite(scores, sizeof(int), MAX, highscorebin)) != MAX){
            puts("Erro ao gravar a pontuacao do jogador");
        }

        fclose(highscorebin);
    }
}