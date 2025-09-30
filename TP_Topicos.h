#ifndef TP_TOPICOS_H_INCLUDED
#define TP_TOPICOS_H_INCLUDED

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

///PIXELES (Esta asi en el TP)
#define TITULO "SIMON_DICE"
#define PIXELES_HORIZONTALES 1366
#define PIXELES_VERTICALES 768

///Estructuras
typedef struct{
    SDL_Window *ventana;
    SDL_Renderer *renderizar;
}tJuego;

///MACROS
#define OK_SALIDA 0
#define ERROR_SALIDA 1

///Botones
#define ROJO 0
#define AZUL 1
#define AMARILLO 2
#define VERDE 3
#define MAX_SEQ 100

///Funciones
bool sdl_Iniciar(tJuego *juego);
void limpieza_juego(tJuego *juego, int Estatus_Salida);

// Funciones del Simon
void dibujarTablero(tJuego *juego);
void iluminarBoton(tJuego *juego, int color);
int detectarBotonClick(int x, int y);
#endif // TP_TOPICOS_H_INCLUDED
