#include "map.h"
#include <string.h>
#include <ncurses.h>
#include <stdlib.h>
#include "game_types.h"
#include "image.h"
#include "draw.h"
#include "game.h"
#include "engine_types.h"
#include "stdlib.h"
#include "player_pathfinding.h"
#include <time.h>

// Necessário para as funções que geram o mapa



 // Escolhe uma das salas do mapa, coloca nela um baú e cria uma porta.


void chest_room(Map** a, int r, int c) {
	for(int i = 1; i < r-1; i++) {  
		for(int j = 1; j < c-1; j++) {
			Vector2D v1 = {0, 0}, d1 = {0, 0}, d2 = {0, 0};
			int l1 = 1, l2 = 1, l3 = 1, l4 = 1, num_doors = 0, aux1 = 0, aux2 = 0;
			if(a[i][j].object == 1 && a[i][j+1].object == 1 && a[i+1][j].object == 1){ // encontra o canto superior esquerdo da sala
				v1.x = j;
				v1.y = i;
            	
				aux1 = i+1;
				while(a[aux1][j+1].object != 1 && aux1 < r-1){ // percorre a parede esquerda da sala (lado 1)
					l1 = l1+1;
					if(a[aux1][j].object == 0){ 
						num_doors++; // encontrou uma porta
						if(num_doors == 2){ // guarda a posição da porta
							d1.x = j;
							d1.y = aux1-1;
							d2.x = j;
							d2.y = aux1;
						}
					} 
					aux1++;	
				}
				aux2 = j+1;
				while(a[i+1][aux2].object != 1 && aux2 < c-1){ // percorre a parede superior da sala (lado 2)
					l2 = l2+1;
					if(a[i][aux2].object == 0){ 
						num_doors++; // encontrou uma porta
						if(num_doors == 2){ // guarda a posição da porta
							d1.x = aux2-1;
							d1.y = i;
							d2.x = aux2;
							d2.y = i;
						}
					} 
					aux2++;	
				}
				j++;
				while(a[aux1-1][j].object != 1 && j < c-1){ // percorre a parede inferior da sala (lado 3)
					l3 = l3+1;
					if(a[aux1][j].object == 0){ 
						num_doors++; // encontrou uma porta
						if(num_doors == 2){ // guarda a posição da porta
							d1.x = j-1;
							d1.y = aux1;
							d2.x = j;
							d2.y = aux1;
						}
					} 
					j++;	
				}
				aux1 = i+1;
				while(a[aux1][j-1].object != 1 && aux1 < r-1){ // percorre parede direita da sala (lado 4)
					l4 = l4+1;
					if(a[aux1][j].object == 0){ 
						num_doors++; // encontrou uma porta
						if(num_doors == 2){ // guarda a posição da porta
							d1.x = j;
							d1.y = aux1-1;
							d2.x = j;
							d2.y = aux1;
						}
					} 
					aux1++;
				}
				if(num_doors == 2 && l1 == l4 && l2 == l3){
					for(int k = v1.y + 1; k < v1.y + l1; k++) { // muda a cor do chão da sala fechada  
						for(int l = v1.x + 1 ; l < v1.x + l2; l++) {
							if (a[k][l].object != 2) a[k][l].object = 12; // garante que se a passagem de nível estiver dentro da sala não é removida
						}
					}
					a[v1.y+1][v1.x+1].object = 9; // posiciona o baú perto do canto superior esquerdo
					a[d1.y][d1.x].object = 10; // fecha a porta - duas posições
					a[d2.y][d2.x].object = 10; // fecha a porta - duas posições
                    
					// posiciona a chave
					int x = (random() % c);
        			int y = (random() % r);
					while(a[y][x].object != 0 || (x > v1.x && x < v1.x + l2) || (y > v1.y && y < v1.y + l1)){ // garante que a chave não fica na mesma sala do baú
						x = (random() % c); 	
        				y = (random() % r);
					}
					a[y][x].object = 11;
					return;
				}
			}
		}
	}
}



// Verifica se um elemento existe num array.


int elem(Vector2D e, Vector2D *v, int N) {
    int i, ans = 0;
    for(i = 0; i < N && !ans; i++)
        if(e.x == v[i].x && e.y == v[i].y) ans = 1;
    return ans;
}



// Remove o primeiro elemento de um array.


Vector2D* remove_elem(Vector2D* old, int old_size) {
    if (old == 0 || old_size <= 1) { // verifica se o array antigo tem pelo menos um elemento
	    return old;
    }
    Vector2D* new = (Vector2D*) malloc((old_size-1)*sizeof(Vector2D));
    for (int i = 1; i < old_size; i++) { // copia os elementos do array antigo a partir do segundo elemento para o array novo
        new[i - 1] = old[i];
    }
	if (new != NULL) free(new);    
    return &old[1]; // retorna o ponteiro para o array antigo sem o primeiro elemento
}



//  Verifica se um mapa é válido. Um mapa válido permite que o jogador acesse a todas as salas do mapa.


int valid_map(Map** a, int r, int c) {
	Vector2D fst, tmp;
	int i, count = 0, k = 0;

	fst.x = -1;
	fst.y = -1;
	for(i = 1; i < r-1; i++) { // encontra o primeiro ponto e guarda num array todas as posições onde o jogador pode andar
		for(int j = 1; j < c-1; j++) {
			if (a[i][j].object == 0){
				count++;
			    if (fst.x == -1) { // não volta a fazer esta condição depois de encontrar um primeiro ponto
					fst.x = j; 
					fst.y = i;
				}
			}
		}
	}
	i = 0;
	Vector2D* visit = malloc(2*count * sizeof(Vector2D));
	if (visit == NULL) {
		exit(EXIT_FAILURE);
	}
	Vector2D* walk = malloc(count * sizeof(Vector2D));
	if (walk == NULL) {
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < count; i++) {
        walk[i].x = 0;
        walk[i].y = 0;
    }
	visit[i].x = fst.x;
	visit[i].y = fst.y;
	while (i != -1) { 
		tmp.x = visit[0].x;
		tmp.y = visit[0].y+1;
		if (a[tmp.y][tmp.x].object == 0 && elem(tmp, walk, count) == 0) {
			walk[k].x = tmp.x;
			walk[k].y = tmp.y;
			i++;
			visit[i].x = tmp.x;
			visit[i].y = tmp.y;
			k++;
		}
		tmp.x = visit[0].x;
		tmp.y = visit[0].y-1; 
		if (a[tmp.y][tmp.x].object == 0 && elem(tmp, walk, count) == 0) {
			walk[k].x = tmp.x;
			walk[k].y = tmp.y;
			i++;
			visit[i].x = tmp.x;
			visit[i].y = tmp.y;
			k++;
		}
		tmp.x = visit[0].x+1;
		tmp.y = visit[0].y; 
		if (a[tmp.y][tmp.x].object == 0 && elem(tmp, walk, count) == 0) {
			walk[k].x = tmp.x;
			walk[k].y = tmp.y;
			i++;
			visit[i].x = tmp.x;
			visit[i].y = tmp.y;
			k++;
		}
		tmp.x = visit[0].x-1;
		tmp.y = visit[0].y; 
		if (a[tmp.y][tmp.x].object == 0 && elem(tmp, walk, count) == 0) {
			walk[k].x = tmp.x;
			walk[k].y = tmp.y;
			i++;
			visit[i].x = tmp.x;
			visit[i].y = tmp.y;
			k++;
		}
		visit = remove_elem (visit, i+1);
		i--;
	} 
	if (k >= count) return 0; // 0 - Mapa válido | 1 - Mapa inválido
	else return 1;
}



// Permite a geração de água no mapa. Existir água depende da profundidade e da aleatoriedade. A quantidade desta depende apenas da aleatoriedade. 


void gen_water(Map** a, int r, int c) {
	int x = 0, y = 0, water, ind, n = 3, prob_water = 0;
	if (LEVEL != 0) prob_water = (1/LEVEL)*100;
    while (n > 0) {
		water = 0;
		int random_num = rand() % 100;
   		if (prob_water <= random_num && LEVEL != 0) { // decidir se aparece água em um nível
			water = 1;
		}
		if (water == 1) {
			while(a[y][x].object != 0) {
				x = (random() % c);
        		y = (random() % r);
			}
			a[y][x].object = 7;
			if(a[y][x+1].object == 0) a[y][x+1].object = 7;
			if(a[y][x-1].object == 0) a[y][x-1].object = 7;
			if(a[y+1][x].object == 0) a[y+1][x].object = 7;
			if(a[y-1][x].object == 0) a[y-1][x].object = 7;
			for(int i = 1; i < r-1; i++) {    
	    		for(int j = 1; j < c-1; j++) {
					if (a[i][j].object == 7) {
						int conditions[] = {a[i][j+1].object, a[i][j-1].object, a[i+1][j].object, a[i-1][j].object};
						// dependendo da profundidade do nível é gerada menos água
						if (LEVEL >= 0.75*num_levels && LEVEL < num_levels-1){
							ind = (random() % 4);
					  	    if (conditions[ind] == 0) conditions[ind] = 7;
							ind = (random() % 4);
					 		if (conditions[ind] == 0) conditions[ind] = 7;
						} else if (LEVEL >= 0.5*num_levels && LEVEL < 0.75*num_levels){
							ind = (random() % 4);
					   		if (conditions[ind] == 0) conditions[ind] = 7;
							ind = (random() % 4);
					    	if (conditions[ind] == 0) conditions[ind] = 7;
							ind = (random() % 4);
					    	if (conditions[ind] == 0) conditions[ind] = 7;
						} else if (LEVEL >= 1 && LEVEL <= 0.5*num_levels) {
							ind = (random() % 4);
					    	if (conditions[ind] == 0) conditions[ind] = 7;
							ind = (random() % 4);
					    	if (conditions[ind] == 0) conditions[ind] = 7;
							ind = (random() % 4);
					    	if (conditions[ind] == 0) conditions[ind] = 7;
							ind = (random() % 4);
					    	if (conditions[ind] == 0) conditions[ind] = 7;
						}
						if (conditions[0] == 7) a[i][j+1].object = 7;
						if (conditions[1] == 7) a[i][j-1].object = 7;
						if (conditions[2] == 7) a[i+1][j].object = 7;
						if (conditions[3] == 7) a[i-1][j].object = 7;
					}
				}
			}
		}
		n--;
	}
	// coloca margens com um tipo diferente para se poder associar também uma cor diferente
	for (int i = 1; i < r-1; i++) {    
	    for (int j = 1; j < c-1; j++) {
			if (a[i][j].object == 7 && (a[i+1][j].object == 0 || a[i-1][j].object == 0 || a[i][j+1].object == 0 || a[i][j-1].object == 0 || a[i+1][j].object == 5 || a[i-1][j].object == 5 || a[i][j+1].object == 5 || a[i][j-1].object == 5 || a[i+1][j].object == 6 || a[i-1][j].object == 6 || a[i][j+1].object == 6 || a[i][j-1].object == 6)){
				a[i][j].object = 8;
			} 
		}
	}
}



// Permite a geração de relva no mapa. Existir relva depende da profundidade e da aleatoriedade. A quantidade desta depende apenas da aleatoriedade. 


void gen_grass(Map** a, int r, int c) {
	int x = 0, y = 0, grass = 0, ind, n = 3;
	int prob_grass = 90 - 10*LEVEL;
    while (n > 0) {
		grass = 0;
		int random_num = rand() % 100;
   		if (random_num <= prob_grass) { // decidir se aparece relva num nível
			grass = 1;
		}
		if (grass == 1) {
			while(a[y][x].object != 0) {
				x = (random() % c);
        		y = (random() % r);
			}
			a[y][x].object = 5;
			if(a[y][x+1].object == 0) a[y][x+1].object = 5;
			if(a[y][x-1].object == 0) a[y][x-1].object = 5;
			if(a[y+1][x].object == 0) a[y+1][x].object = 5;
			if(a[y-1][x].object == 0) a[y-1][x].object = 5;
			for(int i = 1; i < r-1; i++) {    
	    		for(int j = 1; j < c-1; j++) {
						if (a[i][j].object == 5) {
						int conditions[] = {a[i][j+1].object, a[i][j-1].object, a[i+1][j].object, a[i-1][j].object};
						// dependendo da profundidade do nível é gerada menos relva
						if (LEVEL >= 0.75*num_levels && LEVEL < num_levels-1){
							ind = (random() % 4);
					  	    if (conditions[ind] == 0) conditions[ind] = 5;
							ind = (random() % 4);
					 		if (conditions[ind] == 0) conditions[ind] = 5;
						} else if (LEVEL >= 0.5*num_levels && LEVEL < 0.75*num_levels){
							ind = (random() % 4);
					   		if (conditions[ind] == 0) conditions[ind] = 5;
							ind = (random() % 4);
					    	if (conditions[ind] == 0) conditions[ind] = 5;
							ind = (random() % 4);
					    	if (conditions[ind] == 0) conditions[ind] = 5;
						} else if (LEVEL >= 0 && LEVEL <= 0.5*num_levels) {
							ind = (random() % 4);
					    	if (conditions[ind] == 0) conditions[ind] = 5;
							ind = (random() % 4);
					    	if (conditions[ind] == 0) conditions[ind] = 5;
							ind = (random() % 4);
					    	if (conditions[ind] == 0) conditions[ind] = 5;
							ind = (random() % 4);
					    	if (conditions[ind] == 0) conditions[ind] = 5;
						}
						if (conditions[0] == 5) a[i][j+1].object = 5;
						if (conditions[1] == 5) a[i][j-1].object = 5;
						if (conditions[2] == 5) a[i+1][j].object = 5;
						if (conditions[3] == 5) a[i-1][j].object = 5;
					}
				}
			}
		}
		n--;
	}
	// coloca flores no meio da relva
	for (int i = 1; i < r-1; i++) {    
	    for (int j = 1; j < c-1; j++) {
			if (a[i][j].object == 5){
				int k = (random() % 10);
				if (k == 0) a[i][j].object = 6;
			} 
		}
	}
}


// Permite a geração de lava no mapa. Existir lava depende da profundidade e da aleatoriedade. A quantidade desta depende apenas da aleatoriedade. 


void gen_lava(Map** a, int r, int c) {
	int x = 0, y = 0, lava = 0, ind, n = 2;
	int prob_lava = 10 * LEVEL;
    int random_num = rand() % 100;
	while(n > 0) {
    	if (random_num <= prob_lava && LEVEL >= 2) { // decidir se aparece lava em um nível
			lava = 1;
		}
		if (lava == 1) { 
			while(a[y][x].object != 0) {
				x = (random() % c);
        		y = (random() % r);
			}
			a[y][x].object = 4;
			if(a[y][x+1].object == 0) a[y][x+1].object = 4;
			if(a[y][x-1].object == 0) a[y][x-1].object = 4;
			if(a[y+1][x].object == 0) a[y+1][x].object = 4;
			if(a[y-1][x].object == 0) a[y-1][x].object = 4;
			for(int i = 1; i < r-1; i++) {    	
	    		for(int j = 1; j < c-1; j++) {
					if (a[i][j].object == 4) {
						int conditions[] = {a[i][j+1].object, a[i][j-1].object, a[i+1][j].object, a[i-1][j].object};
						// dependendo da profundidade do nível é gerada mais lava
						if (LEVEL >= 2 && LEVEL < 0.25*num_levels){
							ind = (random() % 4);
						    if (conditions[ind] == 0) conditions[ind] = 4;
							ind = (random() % 4);
						    if (conditions[ind] == 0) conditions[ind] = 4;
						} else if (LEVEL >= 0.25*num_levels && LEVEL < 0.5*num_levels){
							ind = (random() % 4);
						    if (conditions[ind] == 0) conditions[ind] = 4;
							ind = (random() % 4);
						    if (conditions[ind] == 0) conditions[ind] = 4;
							ind = (random() % 4);
						    if (conditions[ind] == 0) conditions[ind] = 4;
						} else if (LEVEL >= 0.5*num_levels && LEVEL <= num_levels-1) {
							ind = (random() % 4);
						    if (conditions[ind] == 0) conditions[ind] = 4;
							ind = (random() % 4);
						    if (conditions[ind] == 0) conditions[ind] = 4;
							ind = (random() % 4);
						    if (conditions[ind] == 0) conditions[ind] = 4;
							ind = (random() % 4);
						    if (conditions[ind] == 0) conditions[ind] = 4;
						}
						if (conditions[0] == 4) a[i][j+1].object = 4;
						if (conditions[1] == 4) a[i][j-1].object = 4;
						if (conditions[2] == 4) a[i+1][j].object = 4;
						if (conditions[3] == 4) a[i-1][j].object = 4;
					}
				}
			}
		}
		n--;
	}
}



// Gera um número aleatório de salas, compreendido entre 15 a 25, num mapa. O tamanho das salas é variável.  Garante que a cada nova sala é aberta uma porta para as salas adjacentes já existentes.


void new_room_map (Map** a, int r, int c){
	srand(time(NULL)); // define a semente baseada no tempo atual
	int random_rooms = (random() % 11) + 15; // 15 a 25 salas
    int k = 0;

    while (k < random_rooms) {
        int width_room = (random() % 13) + 14; // largura da sala (14 a 26)
        int height_room = (random() % 13) + 10; // altura da sala (10 a 22)
        int roomX = (random() % c) + 1; // posição da sala no mapa
        int roomY = (random() % r) + 1;		
		while(roomX <= 9 && roomY <= 9){
			roomX = (random() % c) + 1; // posição da sala no mapa
        	roomY = (random() % r) + 1; 
		}
		// verifica se a sala fica situada dentro do mapa, senão gera uma nova sala
		if((roomX + width_room) < c-1 && (roomY + height_room < r-1)){  
            // coloca paredes na sala
            for(int j = roomX; j < roomX + width_room+1; j++){ // constroi a parede horizontal da sala
		      a[roomY][j].object = 1; 
		      a[roomY+height_room][j].object = 1;
	        } 
            for(int i = roomY; i < roomY + height_room; i++) { // constroi a parede vertical da sala
              a[i][roomX].object = 1; 
	          a[i][roomX+width_room].object = 1;
	        }
            // coloca o interior das salas como um tipo de mapa onde é possível andar
			for (int i = roomY + 1; i < roomY + height_room; i++) {
               for (int j = roomX + 1; j < roomX + width_room; j++) {
                   a[i][j].object = 0;
               }
            }
		    // abre portas entre salas adjacentes
			int init = roomY + height_room, end = 0;
            for (int j = roomY; j < roomY + height_room && j != 1; j++){ // esquerda
		      while (a[j][roomX].object == 1 && a[j][roomX-1].object != 3 && a[j][roomX+1].object != 3 && a[j][roomX-1].object != 1){ // tem de abrir porta
			 	  if (init > j) init = j;
				  if (end < j) end = j;
				  j++;
			   }
			   if (end - init == 1 && a[init][roomX+1].object != 1 && a[end][roomX+1].object != 1){ // para abrir uma porta é necessário de ter pelo menos duas posições disponíveis e no última não podemos ter uma parede
			      a[init][roomX].object = 0;
				  a[end][roomX].object = 0;
			   }
			   else if (end - init >= 2){
			      int door = (random() % (end-init))+1;
				  if (init+door+1 < end && a[init+door][roomX+1].object != 1 && a[init+door+1][roomX+1].object != 1 && a[init+door][roomX-1].object != 1 && a[init+door+1][roomX-1].object != 1){
				      a[init+door][roomX].object = 0;
					  a[init+door+1][roomX].object = 0;
				  }
				  else if (a[init+door-1][roomX+1].object != 1 && a[init+door-2][roomX+1].object != 1 && a[init+door-1][roomX-1].object != 1 && a[init+door-2][roomX-1].object != 1){ 
				     a[init+door-1][roomX].object = 0;
					 a[init+door-2][roomX].object = 0;
			      }
			   }
               end = 0;
			   init = j+1;
			}
		    init = roomY + height_room, end = 0;
            for (int j = roomY; j < roomY + height_room && j != r-1; j++){ // direita
		       while (a[j][roomX+width_room].object == 1 && a[j][roomX+width_room+1].object != 3 && a[j][roomX+width_room-1].object != 3 && a[j][roomX+width_room+1].object != 1){
			 	  if (init > j) init = j;
				  if (end < j) end = j;
				  j++;
			   }
			   if(end - init == 1 && a[init][roomX+width_room-1].object != 1 && a[end][roomX+width_room-1].object != 1){
			      a[init][roomX+width_room].object = 0;
				  a[end][roomX+width_room].object = 0;
			   }
			   else if(end - init >= 2){
			      int door = (random() % (end-init))+1;
				  if(init+door+1 < end && a[init+door][roomX+width_room+1].object != 1 && a[init+door+1][roomX+width_room+1].object != 1 && a[init+door][roomX+width_room-1].object != 1 && a[init+door+1][roomX+width_room-1].object != 1){ 
				     a[init+door][roomX+width_room].object = 0;
					 a[init+door+1][roomX+width_room].object = 0;
				  }
				  else if(a[init+door-1][roomX+width_room+1].object != 1 && a[init+door-2][roomX+width_room+1].object != 1 && a[init+door-1][roomX+width_room-1].object != 1 && a[init+door-2][roomX+width_room-1].object != 1){
				     a[init+door-1][roomX+width_room].object = 0;
					 a[init+door-2][roomX+width_room].object = 0;
				  }
			   }
			   end = 0;
			   init = j+1;
			}
		    init = roomX + width_room, end = 0;
            for(int j = roomX; j < roomX + width_room && j > 0; j++){ // cima
		       while (a[roomY][j].object == 1 && a[roomY-1][j].object != 3 && a[roomY+1][j].object != 3 && a[roomY-1][j].object != 1){
			 	  if (init > j) init = j;
				  if (end < j) end = j;
				  j++;
			   }
			   if(end - init == 1 && a[roomY+1][init].object != 1 && a[roomY+1][end].object != 1){
			      a[roomY][init].object = 0;
				  a[roomY][end].object = 0;
			   }
			   else if (end - init >= 2){
			      int door = (random() % (end-init))+1;
				  if(init+door+1 < end && a[roomY-1][init+door].object != 1 && a[roomY-1][init+door+1].object != 1 && a[roomY+1][init+door].object != 1 && a[roomY+1][init+door+1].object != 1){ 
				     a[roomY][init+door].object = 0;
					 a[roomY][init+door+1].object = 0;
				  }
				  else if(a[roomY-1][init+door-1].object != 1 && a[roomY-1][init+door-2].object != 1 && a[roomY+1][init+door-1].object != 1 && a[roomY+1][init+door-2].object != 1){ 
				     a[roomY][init+door-1].object = 0;
					 a[roomY][init+door-2].object = 0;
				  }
			  }
			  end = 0;
			  init = j+1;
			}
		    init = roomX + width_room, end = 0;
            for(int j = roomX; j < roomX + width_room && j != c; j++){ // baixo
		       while (a[roomY+height_room][j].object == 1 && a[roomY+height_room+1][j].object != 3 && a[roomY+height_room-1][j].object != 3 && a[roomY+height_room+1][j].object != 1){
			 	  if (init > j) init = j;
				  if (end < j) end = j;
				  j++;
			   }
			   if(end - init == 1 && a[roomY+height_room+1][init].object != 1 && a[roomY+height_room+1][end].object != 1){
			      a[roomY+height_room][init].object = 0;
				  a[roomY+height_room][end].object = 0;
			   }
			   else if (end - init >= 2){
			      int door = (random() % (end-init))+1;
				  if(init+door+1 < end && a[roomY+height_room+1][init+door].object != 1 && a[roomY+height_room+1][init+door+1].object != 1 && a[roomY+height_room-1][init+door].object != 1 && a[roomY+height_room-1][init+door+1].object != 1){ 
				     a[roomY+height_room][init+door].object = 0;
					 a[roomY+height_room][init+door+1].object = 0;
				  }
				  else if(a[roomY+height_room-1][init+door-1].object != 1 && a[roomY+height_room-1][init+door-2].object != 1 && a[roomY+height_room-1][init+door+1].object != 1 && a[roomY+height_room+1][init+door-2].object != 1){ 
				     a[roomY+height_room][init+door-1].object = 0;
					 a[roomY+height_room][init+door-2].object = 0;
				  }
			   }
			   end = 0;
			   init = j+1;
			}
        	k++;
		}
    }
}


// Gera diferentes tipos mobs em posições válidas de forma aleatória consoante o nível do mapa.


int gen_mobs(Mob **mobs, Map **map, int r, int c, int level){
	// Isto pode servir para fazer um modo dificíl mais tarde, subindo o valor
	int mobSpawnRate = 1;
	// Só spawna mobs a partir do segundo nível
	if(level > 0){
		int numMobs = (random() % (level * mobSpawnRate)) + 1;

		int numIntelligentMobs = 0;
		// Mobs inteligentes
		if(level > 2) {
			numIntelligentMobs = (random() % (level * mobSpawnRate));
		}

		*mobs = malloc(sizeof(Mob) * (numMobs + numIntelligentMobs));
		
		// Spawn de mobs brutos

		for(int i = 0; i < numMobs; i++){
			(*mobs)[i].position = get_random_floor_position(map, r, c);
			(*mobs)[i].targetPosition = (*mobs)[i].position;
			(*mobs)[i].health = 100;
			(*mobs)[i].mobBehavior = STUPID;
			(*mobs)[i].timeSinceLastUpdate = 0;
			(*mobs)[i].pathStep = 0;
			(*mobs)[i].path = NULL;
			(*mobs)[i].chasingPlayer = 0;
			(*mobs)[i].attackDamage = 5;
		}

		// Spawn de mobs inteligentes

		for(int i = 0; i < numIntelligentMobs; i++){
			(*mobs)[numMobs + i].position = get_random_floor_position(map, r, c);
			(*mobs)[numMobs + i].targetPosition = (*mobs)[numMobs + i].position;
			(*mobs)[numMobs + i].health = 50; // (mais fácil de matar que os brutos)
			(*mobs)[numMobs + i].mobBehavior = INTELLIGENT;
			(*mobs)[numMobs + i].timeSinceLastUpdate = 0;
			(*mobs)[numMobs + i].pathStep = 0;
			(*mobs)[numMobs + i].path = NULL;
			(*mobs)[numMobs + i].chasingPlayer = 0;
			(*mobs)[numMobs + i].attackDamage = 18;
		}

		return numMobs + numIntelligentMobs;
	}

	return 0;
}


 // Abre, entre 1 a 3, portais no mapa, para ser possível nagevar entre níveis. 


void new_level_map (Map** a, int r, int c){
	int random_num, count = 0, rc, rr;

	random_num = (random() % 3) + 1;
	while(count < random_num) {
		rc = (random() % c);
		rr = (random() % r);
		if (a[rr][rc].object == 0 && (a[rr-1][rc].object==0 || a[rr+1][rc].object==0 || a[rr][rc-1].object==0 || a[rr][rc+1].object==0)){
			a[rr][rc].object = 2;
			count++;
		}
	}
}

// Obtém uma posição válida no mapa para colocar entidades/objetos.


Vector2D get_random_floor_position(Map** map, int r, int c){
	int available = 0;
	Vector2D pos;
	while(!available){
		int randX = (random() % c);
		int randY = (random() % r);
		if(map[randY][randX].object == 0){
			pos.x = randX;
			pos.y = randY;
			available = 1;
		}
	}
	return pos;
}



// Gera um mapa. Caso este não seja válido, é gerado um novo. Um mundo é constituído por 20 mapas.


void gen_map(Map** a, int r, int c) {
    int count = 0;
	new_room_map(a,r,c);
	while (valid_map(a,r,c) == 1) {
		if (count > 8) { // faz refresh do mapa para evitar situações em que é impossível gerar um mapa válido
			for (int j = 0; j < r; j++) {
				for (int k = 0; k < c; k++) {
					a[j][k].object = 3; // inicializa o mapa como vazio
			   	}
	   		}
		count = 0;
		}
		count++;
		new_room_map(a,r,c);
	}
    new_level_map(a,r,c);
	chest_room(a,r,c);
}

// Desenha os mobs no mapa consoante o seu tipo.

void draw_mobs(Mob *mobs, int mobQuantity, Terminal *terminal){
	for(int i = 0; i < mobQuantity; i++){
		// Mob skins (depende do tipo de mob)
		switch (mobs[i].mobBehavior)
		{
		case STUPID:
			draw_custom_pixel(mobs[i].position, "><", 35, 0, terminal);
			break;
		case INTELLIGENT:
			draw_custom_pixel(mobs[i].position, "><", 35, 59, terminal);
			break;
		default:
			break;
		}

	}
}



// O mapa é uma matriz de tipos. Em função de cada tipo é feita a impressão de elementos específicos. Exemplo: 1 é parede e imprime "##" na cor escolhida.


void print_map(Map** a, int r, int c, GameState *gameState, Terminal *terminal) {
	//Image lava = load_image_from_file("assets/sprites/lava.sprite");
	//int k = 0, r_num = 0;
	for(int i = 0; i < r; i++){
      	for(int j = 0; j < c; j++){
			Vector2D pos = {j,i};
			if(gameState->pathSelection == 1 && is_cell_path_part(gameState, pos)){
				continue;
			}
			switch (a[i][j].object){
			case 0: // imprime um lugar onde o jogador pode andar
				draw_custom_pixel(pos, ". ", 79, 74, terminal);
				break;
			case 1: // imprime a parede
				draw_custom_pixel(pos, "##", 77, choose_color(66, 3, pos), terminal);
				break;
			case 2: // imprime uma porta para outro nível
				draw_custom_pixel(pos, "[]", 84, 83, terminal);
				break;
			case 4: // imprime lava
				draw_empty_pixel(pos, choose_color(44, 5, pos));
				break;
			case 5: // imprime relva
				if(((choose_color(59, 3, pos) + 5)) % 3 != 2){
					draw_custom_pixel(pos, "''", 78, choose_color(59, 3, pos), terminal);
				}
				else{
					draw_empty_pixel(pos, choose_color(59, 3, pos));
				}
				break;
			case 6: // imprime flores
				if(((choose_color(59, 3, pos) + 5)) % 3 != 0){
					draw_custom_pixel(pos, "**", 13, choose_color(59, 3, pos), terminal);
				}
				else{
					draw_empty_pixel(pos, choose_color(59, 3, pos));
				}
				break;
			case 7: // imprime água profunda
				draw_custom_pixel(pos, "~ ", choose_color(51, 2, pos)-1, choose_color(50, 3, pos), terminal);
				break;
			case 8: // imprime a margem da água
				draw_custom_pixel(pos, " .", 35, choose_color(54, 4, pos), terminal);
				break;
			case 9: // imprime baú
				draw_custom_pixel(pos, "??", 82, 80, terminal);
				break;
			case 10: // imprime porta 
				draw_empty_pixel(pos, 81);
				break;
			case 11: // imprime chave 
				draw_custom_pixel(pos, "-o", 82, 70, terminal);
				break;
			case 12: // imprime chão da sala, enquanto fechada, com um cor própria 
				draw_empty_pixel(pos, create_color_pattern(63, 2, pos));
				break;
			default:
				break;
			}	
   		}
    }
}



// Sorteia uma cor dentro dum conjunto de várias cores de forma a que certos tipos de terreno (relva, água, lava) tenham várias cores


int choose_color(int a, int dif, Vector2D pos){
	int seed = pos.x * 505 + pos.y * dif;
	srand(seed);
	return (a + (rand() % (dif + 1)));
}

// Nas salas secretas, para obter o padrão do chão, foi necessário implementar uma função que o criasse.


int create_color_pattern(int a, int dif, Vector2D pos){
	return (a + (((pos.x * pos.y) + (pos.x * dif) + (pos.x + dif)) % (dif + 1)));
}
