#ifndef TP_TOPICOS_H_INCLUDED
#define TP_TOPICOS_H_INCLUDED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

///PIXELES (Esta asi en el TP)
#define TITULO "SIMON_DICE"
#define PIXELES_HORIZONTALES 800
#define PIXELES_VERTICALES 600

///FUENTE
#define TEXT_SIZE 40

///MACROS
#define OK_SALIDA 0
#define ERROR_SALIDA 1
#define MAX_SEQ 5

///Botones
#define SIN_COLOR -1
#define VERDE 0
#define ROJO 1
#define AMARILLO 2
#define AZUL 3


///ESTADOS
#define INICIO 0
#define SECUENCIA 1
#define JUGANDO 2
#define FINALIZADO 3


///Estructuras
typedef struct
{
    SDL_Window *ventana;
    SDL_Renderer *renderizar;
    TTF_Font *texto_fuente;
    SDL_Color texto_color;
    SDL_Rect texto_rect; ///No me se la traduccion
    SDL_Texture *textura_imagen;
    SDL_Rect sprite_rect;
    int secuencia[MAX_SEQ]; ///Secuencia de colores generada
    int nivel_actual; ///Nivel al que llego el jugador
    int paso_actual_jugador; /// Cuando esta realizando la secuencia
    int color_iluminado;
    Uint32 tiempo_ultimo_cambio;
    int paso_secuencia;
    int estado_juego; ///Para chequear si se encuentra jugando
} tJuego;


///Funciones
bool sdl_Iniciar(tJuego *juego);
void limpieza_juego(tJuego *juego, int Estatus_Salida);
bool crearTexto(tJuego *juego);
void actualizarJuego(tJuego *juego);

///Funciones del Simon
void reiniciarJuego(tJuego *juego);
void dibujarTablero(tJuego *juego);
void dibujar_juego(tJuego *juego);

void manejarEventos(tJuego *juego,bool *corriendo);
//void iluminarBoton(tJuego *juego, int color);
int detectarBotonClick(int x, int y);
void agregar_nuevo_color_secuencia(tJuego *juego);
#endif // TP_TOPICOS_H_INCLUDED
