all: clean map game

game: game.c
	gcc -o game.o game.c map.o -lraylib -lm -Wall
	

map: map.c
	gcc -o map.o -c map.c
	
clean:
	rm -f game.o; rm -f map.o;
