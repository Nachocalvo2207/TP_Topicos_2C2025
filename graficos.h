#ifndef GRAFICOS_H_INCLUDED
#define GRAFICOS_H_INCLUDED

#include "juego.h"


void dibujarTablero(tJuego *juego);
void dibujar_texto_izquierda(tJuego* juego, const char* t, int x, int y, TTF_Font* f, SDL_Color c);
void dibujar_texto_centro(tJuego* juego, const char* t, int x, int y, TTF_Font* f, SDL_Color c);
void dibujar_texto_derecha(tJuego* juego, const char* t, int x, int y, TTF_Font* f, SDL_Color c);
void dibujar_juego(tJuego *juego);
void mostrarMenuConfiguracion(tJuego *juego);
void mostrarPantallaPresentacion(tJuego *juego);
void mostrarPantallaErrorMozart(tJuego *juego);
void mostrarPantallaAyuda(tJuego *juego);
void mostrarPantallaVictoria(tJuego *juego);
void mostrarPantallaError(tJuego *juego);
void pedirNombreJugador(tJuego *juego);
void _dibujar_rectangulo_boton(tJuego *juego, int x, int y, int w, int h, const char *texto,
                               SDL_Color fondo_color, SDL_Color texto_color);
void dibujar_botones_control(tJuego *juego);

#endif // GRAFICOS_H_INCLUDED
