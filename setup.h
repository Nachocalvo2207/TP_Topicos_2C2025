#ifndef SETUP_H_INCLUDED
#define SETUP_H_INCLUDED

#include "juego.h"

bool sdl_Iniciar(tJuego *juego);
void inicializarConfiguracion(tJuego *juego);
void inicializarColores(tJuego *juego);
bool crearTexto(tJuego *juego);
void limpieza_juego(tJuego *juego, int Estatus_Salida);
#endif // SETUP_H_INCLUDED
