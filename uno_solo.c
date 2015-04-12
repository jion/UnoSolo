/**
 * Uno Solo - The SDL Senku Game
 * Copyright (C) 2009  Manuel Schnidrig
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdio.h>
#include <math.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
/*#define DEBUG*/
#define BX		0	/* Coordenada x del tablero */
#define BY		0	/* Coordenada y del tablero */
#define SZ		280	/* Tamanio del tablero */
#define SZ_P	60	/* Tamanio de la ficha */

int SDL_up(void);
int fill_board(void);
int out_of_board(int x, int y);
void s_refresh(void);

char board[7][7];
SDL_Surface *screen, *s_board, *s_piece;

int main(int argc, char *argv[]) {
	int i_x, i_y, e_x, e_y, drag=0; /* i: init (drag) -- e: end (drop) */
	SDL_Event event;
	
	if(SDL_up()) {
		printf("Error al iniciar la biblioteca SDL.\n");
		return 1;
	}

	fill_board();
	SDL_Flip(screen);

	/* main loop */
	while(1) {
		if( SDL_WaitEvent( &event ) ) 
			switch(event.type) {
				case SDL_MOUSEBUTTONDOWN:
					if(event.button.button == SDL_BUTTON_LEFT) {
						drag=1;
						i_x = event.button.x;
						i_y = event.button.y;
					}
					break;
				case SDL_MOUSEBUTTONUP: /*TODO: considerar tanto la posibilidad
										  de arrastre como la de click - click*/
					if(event.button.button == SDL_BUTTON_LEFT) {
						e_x = event.button.x;
						e_y = event.button.y;

						/***********************************************/
						/* Aca compruebo y muevo las fichas. Quizas
						 * deba estar fuera del control del mouse para
						 * aprovechar las coordenadas para el menu.
						 */
						if(drag==1) {
							#ifdef DEBUG
								printf("(%i, %i) --> (%i, %i) ||", i_x, i_y, e_x, e_y);
							#endif
							drag=0;
							i_x = i_x / SZ_P + BX;
							i_y = i_y / SZ_P + BY;
							e_x = e_x / SZ_P + BX;
							e_y = e_y / SZ_P + BY;

							#ifdef DEBUG
								printf("(%i, %i) [%i] --> (%i, %i) [%i]\n",i_x,i_y,board[i_y][i_x],e_x,e_y,board[e_y][e_x]);
							#endif

							if(out_of_board(i_x, i_y) || out_of_board(e_x, e_y)) break;
							#ifdef DEBUG
								printf("--> Movida dentro del tablero!\n");
							#endif
							if(board[i_y][i_x] == 1 && board[e_y][e_x] == 0) {
								if(abs(i_x - e_x)  == 2 || abs(i_y - e_y) == 2)
									if(i_x == e_x) {
										if(board[i_y+(i_y > e_y ? -1 : 1)][i_x]==1) {
											board[i_y][i_x] = 0;
											board[i_y+(i_y > e_y ? -1 : 1)][i_x] = 0;
											board[e_y][e_x] = 1;
										}
									}
									else if(i_y == e_y) {
										if(board[i_y][i_x+(i_x > e_x ? -1 : 1)]==1) {
											board[i_y][i_x] = 0;
											board[i_y][i_x+(i_x > e_x ? -1 : 1)] = 0;
											board[e_y][e_x] = 1;
											}
									}
							}
							s_refresh();
							SDL_Flip(screen);
						}
						/***********************************************/
					}
					break;

				case SDL_QUIT:
					return(0);
			}

	}
	return 0;
}

int SDL_up(void) {
	/* TODO: la funcion debe comprobar los tipos de video disponibles
	 * y demas bla bla y retornar 0 si todo anduvo bien o otro valor
	 * si hubo algun error. */
	SDL_Init(SDL_INIT_VIDEO);
	atexit(SDL_Quit);

	s_board = IMG_Load("board.jpg");
	s_piece = IMG_Load("piece.png");
	SDL_WM_SetCaption("Uno Solo v0.1 - 09/2008", NULL);
	screen = SDL_SetVideoMode(400, 400, 24, SDL_HWSURFACE | SDL_DOUBLEBUF);
	
	SDL_BlitSurface(s_board, NULL, screen, NULL);
	return 0;
}

int fill_board(void) {
	int x, y;
	SDL_Rect rect;
	
	SDL_BlitSurface(s_board, NULL, screen, NULL);
	for(y=0; y < 7; y++)
		for(x=0; x < 7; x++) {
			if(x == 3 && y == 3)  board[y][x] = 0;
			else if(out_of_board(x,y)) board[y][x] = 3;
			else board[y][x] = 1;

			if(board[y][x]==1) {
				rect.x = x*SZ_P; rect.y = y*SZ_P;
				SDL_BlitSurface(s_piece, NULL, screen, &rect);
				}
		}
}

void s_refresh(void) {
	int x, y;
	SDL_Rect rect;
	SDL_BlitSurface(s_board, NULL, screen, NULL);

	for(y=0; y < 7; y++)
		for(x=0; x < 7; x++) {
			if(board[y][x]==1) {
				rect.x = x*SZ_P; rect.y = y*SZ_P;
				SDL_BlitSurface(s_piece, NULL, screen, &rect);
				}
		}
}

int out_of_board(int x, int y) {
	/*TODO: Hacer que la funcion sirva tanto para el caso en que le
	 * pasamos un valor de x, y perteneciente al tablero como cuando
	 * le pasamos un valor fuera de los rangos de la matriz, para
	 * poder utilizar correctamente la funcion para comprobar si las
	 * coordenadas del mouse estan sobre una determinada ficha o fuera
	 * del rango del tablero (puede suceder en un futuro que el
	 * tablero no ocupe toda la ventana)
	 */
	if(x == 0 || x == 1 || x == 5 || x == 6)
		if(y == 0 || y == 1 || y == 5 || y == 6)
			return 1;
	if(x > (BX+SZ_P*7) || y > (BY+SZ_P*7)) return 1;
	
	return 0;
}

