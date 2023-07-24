/* Codigo que testa se o mapa esta sendo gerado corretamente.
A ideia aqui e incluir o map.c pelo #include para reduzir bastante a quantidade de codigo misturado
Para fazer isso basta que o arquivo map.c tenha so funcoes, sem a main. dai se tu da o #include ele reconhece direitinho as funcoes dentro do arquivo principal do jogo, so precisa passar o mapa pelo parametro (lembrando que como o mapa e uma matriz, ao passar na funcao ele e passado por referencia, entao nao precisamos nos preocupar com ponteiros e etc, pois o mapa ja e um ponteiro de ponteiro) */
#include <stdio.h>
#include "./map.c"

int main(){
    int map[MAP_HEIGHT][MAP_WIDTH] = {0};
    generateMap(map);
    printMap(map);
}