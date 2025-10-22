#ifndef ESTADISTICAS_H_INCLUDED
#define ESTADISTICAS_H_INCLUDED

#include "juego.h"


void cargar_estadisticas(tJuego *juego);
void guardar_estadisticas(tJuego *juego);
void actualizar_TOP(tJuego *juego);
void ordenar_top(tEstadistica *top_jugadores, int cantidad);
void mostrar_estadisticas(tJuego *juego);


#endif // ESTADISTICAS_H_INCLUDED
