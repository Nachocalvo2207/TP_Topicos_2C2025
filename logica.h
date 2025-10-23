#ifndef LOGICA_H_INCLUDED
#define LOGICA_H_INCLUDED

#include "juego.h"
#include "estadisticas.h"

void reiniciarJuego(tJuego *juego);
void actualizarJuego(tJuego *juego);
void agregar_nuevo_color_secuencia(tJuego *juego);
int generar_tono(int limite);
int cargarMelodiaDesdeArchivo(const char *ruta, tJuego *juego);
void guardar_melodia_desafio(tJuego *juego);
int calcularDuracionPorNota(int duracion_inicial_ms, int cantidad_notas);
void palabra_mayus(char *palabra);
int detectarBotonClick(int x, int y, int N);



#endif // LOGICA_H_INCLUDED
