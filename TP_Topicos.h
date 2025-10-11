#ifndef TP_TOPICOS_H_INCLUDED
#define TP_TOPICOS_H_INCLUDED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <math.h>

///PIXELES (Esta asi en el TP)
#define TITULO "SIMON"
#define PIXELES_HORIZONTALES 800
#define PIXELES_VERTICALES 600

///FUENTE
#define TEXT_SIZE 40

///COLORES
#define brilloOscuro 150
#define brilloBrillante 255
///MACROS
#define OK_SALIDA 0
#define ERROR_SALIDA 1
#define DOBLE_CAPACIDAD 2
#define OPCIONES 3
#define DISTANCIA_OPCIONES 70
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
#define MENU_CONFIG 4
#define PIDIENDO_NOMBRE 5
#define DURACION_INICIAL 2000 ///Esto son MS

///MODOS
#define MODO_SCHONBERG 0
#define MODO_MOZART 1

typedef struct {
    int num_botones;           // 3..8
    int duracion_inicial_ms;   // MIN 2000 SEGUN TP
    int modo;                  // MODO_SCHONBERG / MODO_MOZART
    char ruta_melodia[256];    // RUTA DEL ARCHIVO PARA MODO Mozart
} tConfig;



///Estructuras
typedef struct
{
    SDL_Window *ventana;
    SDL_Renderer *renderizar;
    TTF_Font *texto_fuente;
    SDL_Color texto_color;
    SDL_Rect texto_rect;
    SDL_Texture *textura_imagen;
    SDL_Rect sprite_rect;
    SDL_Rect rect_btn_jugar;
    SDL_Rect rect_btn_config;
    SDL_Rect rect_btn_salir;
    Uint32 tiempo_ultimo_cambio;
    Mix_Chunk *sonidos[4];
    tConfig config;
    int nivel_actual; ///Nivel al que llego el jugador
    int paso_actual_jugador; /// Cuando esta realizando la secuencia
    int color_iluminado;
    int paso_secuencia;
    int *secuencia;
    size_t capacidad_secuencia; ///Cap. de niveles en memoria
    int estado_juego; ///Para chequear si se encuentra jugando

    char nombre_jugador[64];
    int partidas_jugadas;
    int partidas_ganadas;
    int partidas_perdidas;
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
void mostrarEstadisticas(tJuego *juego);
void manejarEventos(tJuego *juego,bool *corriendo);
//void iluminarBoton(tJuego *juego, int color);
int detectarBotonClick(int x, int y);
void agregar_nuevo_color_secuencia(tJuego *juego);
void mostrarPantallaPresentacion(tJuego *juego);
void mostrarMenuConfiguracion(tJuego *juego);
void pedirNombreJugador(tJuego *juego, bool *corriendo);
int cargarMelodiaDesdeArchivo(const char *ruta, tJuego *juego);
int calcularDuracionPorNota(int duracion_inicial_ms, int cantidad_notas);
#endif // TP_TOPICOS_H_INCLUDED
