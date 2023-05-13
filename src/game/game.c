#include "game.h"
#include <ncurses.h>
#include <stdlib.h>
#include "map.h"
#include "time.h"
#include "game_types.h"
#include "engine_types.h"
#include "player.h"
#include "image.h"
#include "draw.h"
#include "mobs_ai.h"
#include <unistd.h>
#include "sys/time.h"
#include "components.h"

int LEVEL = 0;

GameState *init_game_state(){
	GameState *state = malloc(sizeof(GameState));
	if (state == NULL) {
        fprintf(stderr, "ERRO: Nao foi possivel alocar memoria\n");
        exit(EXIT_FAILURE);
    }
	Vector2D pos = {20,20};
	char name[15] = "NOME";
	state->player = *init_player(name, pos);
	return state;
}


void execute_input(GameState *state, World *w, int r, int c){
	int key = getch();
    
	switch (key)
	{
		case KEY_UP:
		case '8':
			apply_movement(state, NORTH, w[LEVEL].map, r, c);
			break;
		case KEY_DOWN:
		case '2':
			apply_movement(state, SOUTH, w[LEVEL].map, r, c);
			break;
		case KEY_RIGHT:
		case '6':
			apply_movement(state, EAST, w[LEVEL].map, r, c);
			break;
		case KEY_LEFT:
		case '4':
			apply_movement(state, WEST, w[LEVEL].map, r, c);
			break;
		case KEY_A1:
		case '7':
			apply_movement(state, NORTH, w[LEVEL].map, r, c);
			apply_movement(state, WEST, w[LEVEL].map, r, c);
			break;
		case KEY_A3:
		case '9':
			apply_movement(state, NORTH, w[LEVEL].map, r, c);
			apply_movement(state, EAST, w[LEVEL].map, r, c);
			break;
		case KEY_C1:
		case '1':
			apply_movement(state, SOUTH, w[LEVEL].map, r, c);
			apply_movement(state, WEST, w[LEVEL].map, r, c);
			break;
		case KEY_C3:
		case '3':
			apply_movement(state, SOUTH, w[LEVEL].map, r, c);
			apply_movement(state, EAST, w[LEVEL].map, r, c);
			break;
		case 'q':
			endwin();
			exit(0);
			break;
		case 'u':
			check_for_portal(state, w, r, c, -1);
			break;
		case 'd':
			check_for_portal(state, w, r, c, 1);
			break;
		default:
			break;
	}
}

void check_for_portal(GameState *state, World *w, int r, int c, int dir){
	if (w[LEVEL].map[state->player.position.y][state->player.position.x].object == 2) { //encontrou um porta, muda de nível e gera novo mapa
		if (dir == -1 && LEVEL > 0) {
			LEVEL--;
		}
		if (dir == 1 && LEVEL < 9) {
			LEVEL++;
		}
		if (w[LEVEL].created == 0) {
			gen_map(w[LEVEL].map,r,c);
			gen_lava(w[LEVEL].map,r,c);
			gen_grass(w[LEVEL].map,r,c);
			gen_water(w[LEVEL].map,r,c);
			w[LEVEL].mobQuantity = gen_mobs(w[LEVEL].mobs, w[LEVEL].map, r,c, LEVEL);
			w[LEVEL].created = 1;
		}
		clear();
		while (w[LEVEL].map[state->player.position.y][state->player.position.x].object != 0 && (!(w[LEVEL].map[state->player.position.y][state->player.position.x].object == 2 && (LEVEL == 0 || LEVEL == 9)))) {
			state->player.position.x = (random() % c);
			state->player.position.y = (random() % r);

		}
	} 
}

void update(GameState *state, World *worlds, int r, int c, struct timeval currentTime) {
	execute_input(state, worlds, r, c);
	
	for(int i = 0; i < worlds[LEVEL].mobQuantity; i++){
		wander_ai(&worlds[LEVEL].mobs[i], &state->player, worlds[LEVEL].map, r, c);
	}

	struct timeval endTime;
	gettimeofday(&endTime, NULL);

	unsigned long elapsedMicroseconds = (endTime.tv_sec - currentTime.tv_sec) * 1000000 + (endTime.tv_usec - currentTime.tv_usec);
	for(int i = 0; i < worlds[LEVEL].mobQuantity; i++){
		update_timer(&worlds[LEVEL].mobs[i], elapsedMicroseconds);
	}

	update_drowning(worlds[LEVEL].map, state, elapsedMicroseconds);

	if(state->player.timeSinceDrownStart > 10000000){
		state->player.health = 0;
		state->gameover = 1;
	}
	
	refresh();
}


int game(Terminal *terminal) {
	GameState *gameState = init_game_state();

	int ncols, nrows;
    ncols = terminal->xMax;
	ncols = ncols/2;
    nrows = terminal->yMax;
    
	// Criação e inicialização do mapa 
	//NUM_COLUMNS = ncols;

	short buttonGradient[4] = {16,17,18,19};

	int num_levels = 10;
    World* worlds = (World*)malloc(num_levels * sizeof(World));
	if (worlds == NULL) {
		exit(EXIT_FAILURE);
	}
    for (int i = 0; i < num_levels; i++) {
		worlds[i].created = 0;
		worlds[i].map = (MAP**)malloc(nrows * sizeof(MAP*));
		worlds[i].mobs = (Mob*)malloc(i * sizeof(Mob) * 2);
	    if (worlds[i].map == NULL) {
		   exit(EXIT_FAILURE);
	   }
	   for (int j = 0; j < nrows; j++) {
		   worlds[i].map[j] = (MAP*)malloc(ncols * sizeof(MAP));
		   if (worlds[i].map[j] == NULL) {
			   exit(EXIT_FAILURE);
		   }
		   for (int k = 0; k < ncols; k++) {
			   worlds[i].map[j][k].object = 3; // Inicializando o valor do campo object como 3 (vazio)
		   }
	   }

	}

	srandom(time(NULL));

	intrflush(stdscr, false);
    
    // Gera e imprime o primeiro mapa/nível do mundo
	gen_map(worlds[LEVEL].map,nrows,ncols);
	worlds[LEVEL].created = 1;
	gen_grass(worlds[LEVEL].map,nrows,ncols); // no nível 0 é possível existir relva
	worlds[LEVEL].mobQuantity = 0;
	print_map(worlds[LEVEL].map,nrows,ncols);

	// Inicializa a posição do jogador num lugar válido dentro do mapa
	while(worlds[LEVEL].map[gameState->player.position.y][gameState->player.position.x].object != 0 && worlds[LEVEL].map[gameState->player.position.y][gameState->player.position.x].object != 4 && worlds[LEVEL].map[gameState->player.position.y][gameState->player.position.x].object != 5) {
		gameState->player.position.x = (random() % ncols);
		gameState->player.position.y = (random() % nrows);
	}
	gameState->gameover = 0;

	endwin(); 

	Image characterSprite = load_image_from_file("assets/sprites/characters/player1.sprite");
    
	struct timeval currentTime;
	while(1) {
		gettimeofday(&currentTime, NULL);
		move(nrows - 1, 0);
		attron(COLOR_PAIR(COLOR_WHITE));
		printw("(%d, %d) %d %d", gameState->player.position.x, gameState->player.position.y, ncols, nrows);
		attroff(COLOR_PAIR(COLOR_WHITE));
		print_map(worlds[LEVEL].map, nrows, ncols);
		draw_mobs(worlds[LEVEL].mobs, nrows, ncols, worlds[LEVEL].mobQuantity);
		Image gate = load_image_from_file("assets/sprites/gate.sprite"); //Não apagar estas 3 linhas, usadas p/ testes
	    draw_to_screen(gate, gameState->player.position);
		draw_light(gameState, nrows, ncols, worlds[LEVEL].map);

		//draw_to_screen(characterSprite, gameState->player.position);
		//move(st.playerX, st.playerY);

		// Botões (temporário)
		int buttonToolbarX = (terminal->xMax / 2) - (73 / 2);
		Vector2D buttonExplorePos = {buttonToolbarX,terminal->yMax-1};
		button(buttonGradient, "Explore", buttonExplorePos);
		Vector2D buttonRestPos = {buttonToolbarX+13+4,terminal->yMax-1};
		button(buttonGradient, "Rest", buttonRestPos);
		Vector2D buttonSearchPos = {buttonToolbarX+13+4+14,terminal->yMax-1};
		button(buttonGradient, "Search", buttonSearchPos);
		Vector2D buttonMenuPos = {buttonToolbarX+12+4+13+4+14,terminal->yMax-1};
		button(buttonGradient, "Menu", buttonMenuPos);
		Vector2D buttonInvPos = {buttonToolbarX+14+12+4+13+4+14,terminal->yMax-1};
		button(buttonGradient, "Inventory", buttonInvPos);

		Vector2D healthBarPos = {0,1};
		progress_bar(gameState->player.health, 100, 20, 20, 21, "Health", healthBarPos);

		int timeToDrownSecs = 10 - floor(gameState->player.timeSinceDrownStart * 0.000001);

		Vector2D oxygenBarPos = {0,3};
		progress_bar(timeToDrownSecs, 10, 20, 22, 23, "Oxygen", oxygenBarPos);
    
		if (gameState->gameover == 1){
			move(0,150);
			printw("** PERDEU O JOGO PRIMA c para continuar**");
			refresh();
        	int c;
        	do {
            	c = getchar();
        	} while (c != 'c');
        	endwin();
        	return(0);
		}

		mvprintw(0, 180, "Level: %d", LEVEL);

		update(gameState, worlds, nrows, ncols, currentTime);
	}

	return 0;
}