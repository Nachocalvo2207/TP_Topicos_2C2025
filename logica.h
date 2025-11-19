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
///Logica recuperatorio
int cmp_tono_ascendente(const int *a, const int *b);
void intercambio_generico(void *a, void *b, size_t tam);
void desordenar_secuencia(tJuego *juego);
void mostrar_secuencia(tJuego *juego, const char *titulo);
void ordenar_secuencia_por_seleccion(tJuego *juego);
void forzar_reproduccion_secuencia(tJuego *juego);
#endif // LOGICA_H_INCLUDED
