#ifndef JUEGO_H_INCLUDED
#define JUEGO_H_INCLUDED


///LIBRERIAS
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <ctype.h>


///PIXELES (Esta asi en el TP)
#define TITULO "SIMON"
#define PIXELES_HORIZONTALES 800
#define PIXELES_VERTICALES 600

///FUENTE (DINAMICA SEGUN TAMANIO)
#define TEXT_SIZE (PIXELES_VERTICALES / 15)
#define TEXT_CONFIG_SIZE (PIXELES_VERTICALES / 25)
#define TEXT_AYUDA_SIZE (PIXELES_VERTICALES / 35)

///COLORES
#define brilloOscuro 150
#define brilloBrillante 255

///MACROS
#define OK_SALIDA 0
#define ERROR_SALIDA 1
#define ERROR_MELODIA -1
#define DOBLE_CAPACIDAD 2
#define OPCIONES 5
#define DISTANCIA_OPCIONES 70
#define MAX_SEQ 5
#define CANT_BOTONES 8
#define ERROR_MOZART 9
#define AYUDA 10

#define TOP_JUGADORES 5
#define DURACION_INICIAL 2000 ///Esto son MS
#define PAUSA_ENTRE_NOTAS 75
#define DURACION_FLASH_JUGADOR 75
#define PALABRA_CHEAT "DISLEXIA"

///Botones
#define SIN_COLOR -1
#define VERDE 0
#define ROJO 1
#define AMARILLO 2
#define AZUL 3
#define NARANJA 4
#define ROSA 5
#define VIOLETA 6
#define AQUAMARINO 7

///SONIDOS
#define SND_AMARILLO "snd/AMARILLO.wav"
#define SND_AQUAMARINO "snd/AQUAMARINO.wav"
#define SND_AZUL "snd/AZUL.wav"
#define SND_NARANJA "snd/NARANJA.wav"
#define SND_ROJO "snd/ROJO.wav"
#define SND_ROSA "snd/ROSA.wav"
#define SND_VERDE "snd/VERDE.wav"
#define SND_VIOLETA "snd/VIOLETA.wav"


///ESTADOS
#define INICIO 0
#define SECUENCIA 1
#define JUGANDO 2
#define FINALIZADO 3
#define NIVEL_COMPLETADO 4
#define MENU_CONFIG 5
#define PIDIENDO_NOMBRE 6
#define VICTORIA 7 ///MODO MOZART
#define MODO_DESAFIO 8

///CARPETAS
#define ESTADISTICAS "estadisticas.bin"
#define RUTA_MOZART "Melodia.txt"
#define RUTA_DESAFIO "MelodiaDesafio.txt"
#define RUTA_FUENTE "fnt/freesansbold.ttf"

///MODOS
#define MODO_SCHONBERG 0
#define MODO_MOZART 1
#define MODO_CHEAT 2 ///DISLEXIA


///ESTRUCTURAS
typedef struct
{
    int num_botones;           // 3..8
    int duracion_inicial_ms;   // MIN 2000 SEGUN TP
    int modo;                  // MODO_SCHONBERG / MODO_MOZART
    char ruta_melodia[256];    // RUTA DEL ARCHIVO PARA MODO Mozart
} tConfig;

typedef struct
{
    char nombre[50];
    int nivel_alcanzado;
} tEstadistica;

typedef struct
{
    int r_brillante;
    int g_brillante;
    int b_brillante;
    int r_oscuro;
    int g_oscuro;
    int b_oscuro;
} tColorData;

///Estructuras
typedef struct
{
    SDL_Window *ventana;
    SDL_Renderer *renderizar;
    TTF_Font *texto_fuente;
    TTF_Font *texto_config;
    TTF_Font *texto_ayuda_fuente;
    SDL_Color texto_color;
    SDL_Rect texto_rect;
    SDL_Texture *textura_imagen;
    SDL_Rect sprite_rect;
    SDL_Rect rect_btn_jugar;
    SDL_Rect rect_btn_config;
    SDL_Rect rect_btn_salir;
    Uint32 tiempo_ultimo_cambio;
    Uint32 tiempo_encendido;
    Uint32 tiempo_flash_jugador;
    Mix_Chunk *sonidos[CANT_BOTONES];
    tConfig config;
    tColorData lista_colores[CANT_BOTONES];
    int nivel_actual; ///Nivel al que llego el jugador
    int nivel_maximo; ///Maximo nivel alcanzado (Estadisticas)
    int paso_actual_jugador; /// Cuando esta realizando la secuencia
    int color_iluminado;
    int paso_secuencia;
    int *secuencia;
    size_t capacidad_secuencia; ///Cap. de niveles en memoria
    int estado_juego; ///Para chequear si se encuentra jugando
    int proximo_estado; ///Para
    char nombre_jugador[64];
    int partidas_jugadas;
    int long_melodia_mozart; ///
    tEstadistica top_jugadores[TOP_JUGADORES]; ///POR EL MOMENTO TOP 5
    int cant_top_jugadores;
} tJuego;




#endif // JUEGO_H_INCLUDED
